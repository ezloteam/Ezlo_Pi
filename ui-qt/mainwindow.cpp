#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QDebug>

#include <QTimer>
#include <QMessageBox>

#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>

#include <QProcess>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QUuid>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include "ezuuid.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ezlogic_flag_serial_port_open(false),
    ezlogic_flag_is_ezlopi(false),
    ezlogic_device_counter(0),
    ezlogic_flag_registered(false),
    ezlogic_flag_fimware_present(false),
    ezlogic_log_level(EZPI_LOG_INFO)
{
    // Create and Init UI
    ui = new Ui::MainWindow;
    // Set goemetry of the UI
    setFixedSize(810, 628);
    ui->setupUi(this);

    ezlogic_serial_port = new QSerialPort;
    ezlogic_serial_port->setBaudRate(115200);

    EzloPi = new EzPi();

    ezlogic_form_login = new login(this);
    ezlogic_form_WiFi = new Dialog_WiFi(this, ezlogic_serial_port);
    ezlogic_form_devadd = new Dialog_devadd(this);
    ezlogic_status = new QLabel(this);

    connect(ezlogic_form_devadd, SIGNAL(ezpi_send_dev_type_selected(EZPI_UINT8)), this, SLOT(ezlogic_receive_dev_type_selected(EZPI_UINT8)));

    connect(ezlogic_serial_port, &QSerialPort::readyRead, this, &MainWindow::ezlogic_serial_receive);

    connect(ezlogic_form_WiFi, SIGNAL(ezpi_signal_serial_rx_wifi(ezpi_cmd)), this, SLOT(ezlogic_serial_receive_wif(ezpi_cmd)));

    ezlogic_prov_data_user_token.clear();

    // Deactive buttons
    // Open the selected serial port
    ui->pushButton_connect_uart->setEnabled(false);

    // Add and remove config
    ui->pushButton_add_device->setEnabled(false);
    ui->pushButton_remove_device->setEnabled(false);

    // Set WiFi
    ui->pushButton_set_wifi->setEnabled(false);

    // Get and set configs
    ui->pushButton_get_ezpi_config->setEnabled(false);
    ui->pushButton_set_ezpi_config->setEnabled(false);

    // Erase and flash the chip
    ui->pushButton_flash_ezpi_bins->setEnabled(false);
    ui->pushButton_erase_flash->setEnabled(false);

    ui->tableWidget_device_table->clearContents();
    ui->tableWidget_device_table->setEnabled(false);

    // Log
    ui->pushButton_clear_uart_direct_log->setEnabled(false);

    // Register
    ui->actionRegister->setEnabled(false);

    // Registered deviecs list
    ui->comboBox_registered_devices->setEnabled(false);

    ui->comboBox_esp32_board_type->setEnabled(false);
    ui->pushButton_device_restart->setEnabled(false);
    ui->pushButton_device_factory_reset->setEnabled(false);

    connect(ui->comboBox_uart_list,SIGNAL(currentIndexChanged(const QString&)),
            this,SLOT(on_comboBox_uart_list_currentIndexChanged(const QString&)));

    // Timer initialization
    ezlogic_timer_ask_info.callOnTimeout(this, &MainWindow::ezlogic_message_info_no_firmware_detected);
    ezlogic_timer_serial_complete.callOnTimeout(this, &MainWindow::ezlogic_serial_process);

    ezlogic_read_data_serial = new QByteArray;

    // scan and list registered devices
    ezpi_update_dev_list();

    ui->statusBar->addWidget(ezlogic_status);

    ezpi_show_status_message("EzloPi V1.3.4");
    // Build 0 : Release
    // Build 1 : S3 Support
    // Build 2 : Other device support
    EZPI_UINT8 ezlogic_selected_board = ui->comboBox_esp32_board_type->currentIndex() + 1;
    EzloPi->EZPI_SET_BOARD_TYPE((ezpi_board_type)ezlogic_selected_board);
    EzloPi->EZPI_INIT_BOARD();
}

MainWindow::~MainWindow() {
    ezlogic_serial_port->close();
    delete ezlogic_read_data_serial;
    delete ui;
}

// UI generated slots
void MainWindow::on_pushButton_connect_uart_clicked() {

    ezlogic_serial_port->setPort(ezlogic_serial_port_info);

    if(ezlogic_flag_serial_port_open == false) {
        if(!ezlogic_serial_port_info.isNull()) {

            if(ezlogic_serial_port->open(QIODevice::ReadWrite)) {
                ezlogic_flag_serial_port_open = true;

                //Modify UI elements:
                ui->pushButton_connect_uart->setText("Close");
                ui->pushButton_scan_uart_ports->setDisabled(true);
                ui->comboBox_uart_list->setDisabled(true);

                ui->actionRegister->setEnabled(true);

                // Display message on console
                qDebug() << ezlogic_serial_port_info.portName() << " serial port is open.";
                if(ezlogic_log_level == EZPI_LOG_INFO) ui->textBrowser_console_log->append(ezlogic_serial_port_info.portName() + " serial port is open.");

                // Check firmware
                ezlogic_check_firmware(); // Send get info json

                ezpi_show_status_message("Connected to " + \
                                         ezlogic_serial_port_info.portName() + " " + \
                                         QString::number(ezlogic_serial_port->baudRate()) + " No Parity 1 Stop bit.");

            } else {
                qDebug() << "Failed opeaning serial port: " << ui->comboBox_uart_list->currentText();
                if(ezlogic_log_level == EZPI_LOG_INFO) ui->textBrowser_console_log->append("Failed opeaning serial port: " + ui->comboBox_uart_list->currentText());
            }
        }
//        else {
//            qDebug() << ezlogic_serial_port_info.portName() <<  ": port is busy !";
//            if(ezlogic_log_level == EZPI_LOG_INFO) ui->textBrowser_console_log->append(ezlogic_serial_port_info.portName() + QString::fromLocal8Bit(": port is busy !"));
//        }
    } else {
        ezlogic_serial_port->close();
        ui->tableWidget_device_table->clearContents();
        ezlogic_flag_serial_port_open = false;

        ui->pushButton_connect_uart->setText("Open");
        ui->pushButton_scan_uart_ports->setDisabled(false);
        ui->comboBox_uart_list->setDisabled(false);

        ui->actionRegister->setEnabled(false);

        qDebug() << ezlogic_serial_port_info.portName() << " serial port is close.";
        if(ezlogic_log_level == EZPI_LOG_INFO) ui->textBrowser_console_log->append(ezlogic_serial_port_info.portName() + " serial port is close.");

        ezpi_show_status_message("Disconnected to " + ezlogic_serial_port_info.portName());

        // Deactivate all the buttons
        ui->pushButton_set_wifi->setEnabled(false);
        ui->pushButton_add_device->setEnabled(false);

        ui->pushButton_get_ezpi_config->setEnabled(false);
        ui->pushButton_set_ezpi_config->setEnabled(false);

        ui->pushButton_erase_flash->setEnabled(false);
        ui->pushButton_flash_ezpi_bins->setEnabled(false);

        ui->pushButton_clear_uart_direct_log->setEnabled(false);

        ui->pushButton_remove_device->setEnabled(false);

        ui->tableWidget_device_table->clearContents();

        ui->tableWidget_device_table->setEnabled(false);

        ui->comboBox_registered_devices->setEnabled(false);

        ui->pushButton_device_restart->setEnabled(false);

        ui->comboBox_esp32_board_type->setEnabled(false);
    }
}
void MainWindow::on_comboBox_uart_list_currentIndexChanged() {

    for(QSerialPortInfo info : ezlogic_serial_ports_info_list) {
        if(ui->comboBox_uart_list->currentText() == info.portName()) {
            ezlogic_serial_port_info = info;
        }
    }
    qDebug() << "New selected port: " << ezlogic_serial_port_info.portName();
    if(ezlogic_log_level == EZPI_LOG_INFO) {
        ui->textBrowser_console_log->append("New selected port: " + ezlogic_serial_port_info.portName() + "\n");
    }
}
void MainWindow::on_pushButton_set_wifi_clicked() {
    ezlogic_form_WiFi->setFixedSize(315, 165);
    ezlogic_form_WiFi->setModal(true);
    ezlogic_form_WiFi->show();
}
void MainWindow::on_pushButton_erase_flash_clicked() {

    ezlogic_process_erase_flash = new QProcess(this);

    connect(ezlogic_process_erase_flash, &QProcess::readyReadStandardOutput, this, &MainWindow::ezlogic_log_erase_flash);
    connect(ezlogic_process_erase_flash, &QProcess::readyReadStandardError, this, &MainWindow::ezlogic_log_erase_flash);

    ezlogic_serial_port->close();
    ui->tableWidget_device_table->clearContents();
    ui->pushButton_connect_uart->setEnabled(false);


    #ifdef __linux__
            QString ser_port = "/dev/" + ezlogic_serial_port_info.portName();
    #elif _WIN32
        QString ser_port = ezlogic_serial_port_info.portName();
    #else

    #endif


    #ifdef __linux__
        ezlogic_process_erase_flash->setProgram("esptool");
    #elif _WIN32
        ezlogic_process_erase_flash->setProgram("esptool.exe");
    #else

    #endif

    QStringList arguments;
    arguments.append("-p");
    arguments.append(ser_port);
    arguments.append("erase_flash");

    QString command = "";
    for(auto args : arguments) {
        command += " " + args;
    }

    qDebug() << "Erase flash command arguments: " << command;

    ezlogic_process_erase_flash->setArguments(arguments);
    ezlogic_process_erase_flash->start();
}

void MainWindow::on_pushButton_flash_ezpi_bins_clicked() {

    ui->textBrowser_console_log->clear();

    ezlogic_process_write_flash = new QProcess(this);

    connect(ezlogic_process_write_flash, &QProcess::readyReadStandardOutput, this, &MainWindow::ezlogic_log_write_flash);
    connect(ezlogic_process_write_flash, &QProcess::readyReadStandardError, this, &MainWindow::ezlogic_log_write_flash);

    #ifdef __linux__
            QString ser_port = "/dev/" + ezlogic_serial_port_info.portName();
    #elif _WIN32
        QString ser_port = ezlogic_serial_port_info.portName();
    #else

    #endif

    ezlogic_serial_port->close();
    ui->tableWidget_device_table->clearContents();
    ui->pushButton_connect_uart->setEnabled(false);


    #ifdef __linux__
        ezlogic_process_write_flash->setProgram("esptool");
    #elif _WIN32
        ezlogic_process_write_flash->setProgram("esptool.exe");
    #else

    #endif

    QStringList arguments;

    arguments.append("-p");
    arguments.append(ser_port);
    arguments.append("--chip");

    switch(EzloPi->EZPI_GET_BOARD_TYPE()) {

        case EZPI_BOARD_TYPE_NONE: {
            break;
        }

        case EZPI_BOARD_TYPE_ESP32_GENERIC: {
            arguments.append("esp32");
            arguments.append("-b 460800");
            arguments.append("--before");
            arguments.append("default_reset");
            arguments.append("--after");
            arguments.append("hard_reset");
            arguments.append("write_flash");
            arguments.append("-z");
            arguments.append("--flash_mode");
            arguments.append("dio");
            arguments.append("--flash_size");
            arguments.append("detect");
            arguments.append("--flash_freq");
            arguments.append("40m");
            arguments.append("0x1000");
            arguments.append("ezpibins/esp32/0x1000.bin");
            arguments.append("0x8000");
            arguments.append("ezpibins/esp32/0x8000.bin");
            arguments.append("0x10000");
            arguments.append("ezpibins/esp32/0x10000.bin");
            arguments.append("0xD000");
            arguments.append("ezpibins/esp32/0xd000.bin");
            break;
        }

        case EZPI_BOARD_TYPE_ESP32_S3: {
            arguments.append("esp32s3");
            arguments.append("-b 460800");
            arguments.append("--before");
            arguments.append("default_reset");
            arguments.append("--after");
            arguments.append("hard_reset");
            arguments.append("write_flash");
            arguments.append("-z");
            arguments.append("--flash_mode");
            arguments.append("dio");
            arguments.append("--flash_size");
            arguments.append("detect");
            arguments.append("--flash_freq");
            arguments.append("40m");
            arguments.append("0x0");
            arguments.append("ezpibins/esp32s3/0x0000.bin");
            arguments.append("0x8000");
            arguments.append("ezpibins/esp32s3/0x8000.bin");
            arguments.append("0x10000");
            arguments.append("ezpibins/esp32s3/0x10000.bin");
            arguments.append("0xD000");
            arguments.append("ezpibins/esp32s3/0xd000.bin");
            break;
        }

        default: {
            break;
        }
    }

    QString ezpi_selected_registered_device = ui->comboBox_registered_devices->currentText();
//    QString message_user_flash = "You are now about to flash the firmware "
//                                 "for the device, which is regisrered with the serial: ";
//    message_user_flash += ezpi_selected_registered_device;
//    message_user_flash += " in ezlo cloud.";

    QMessageBox::information(this, "Flashing info!", "You are now about to flash the firmware "
                                                      "for the device, which is regisrered with the serial: " +
                                                      ezpi_selected_registered_device +
                                                      " in ezlo cloud.");

    ezpi_selected_registered_device += ".bin";
    arguments.append("0x3B0000");
    arguments.append("devs/"+ ezpi_selected_registered_device);

    ezlogic_process_write_flash->setArguments(arguments);
    ezlogic_process_write_flash->start();
}

void MainWindow::on_pushButton_clear_uart_direct_log_clicked() {
    ui->textBrowser_console_log->clear();
}

void MainWindow::on_comboBox_esp32_board_type_currentIndexChanged(int index) {
    EZPI_UINT8 board_index = index + 1;
    switch(board_index) {
        case EZPI_BOARD_TYPE_NONE:
            break;
        case EZPI_BOARD_TYPE_ESP32_GENERIC:
            EzloPi->EZPI_SET_BOARD_TYPE(EZPI_BOARD_TYPE_ESP32_GENERIC);
            EzloPi->EZPI_INIT_BOARD();
            ezlogic_clear_table_data();
            ui->textBrowser_console_log->append("Warning : Data for previously selected device has been deleted.");
            qDebug() << "New Selected board : ESP32 Generic.";
            break;
         case EZPI_BOARD_TYPE_ESP32_S3:
            EzloPi->EZPI_SET_BOARD_TYPE(EZPI_BOARD_TYPE_ESP32_S3);
            EzloPi->EZPI_INIT_BOARD();
            ezlogic_clear_table_data();
            ui->textBrowser_console_log->append("Warning : Data for previously selected device has been deleted.");
            qDebug() << "New Selected board : ESP32 S3.";
            break;
        case EZPI_BOARD_TYPE_ESP32_C3:
            EzloPi->EZPI_SET_BOARD_TYPE(EZPI_BOARD_TYPE_ESP32_C3);
            EzloPi->EZPI_INIT_BOARD();
            ezlogic_clear_table_data();
            ui->textBrowser_console_log->append("Warning : Data for previously selected device has been deleted.");
            qDebug() << "New Selected board : ESP32 C3.";
            break;
        default:
            break;
    }
}

void MainWindow::on_pushButton_add_device_clicked() {

    if (ezlogic_device_counter >= EZPI_MAX_DEVICES) {
        QMessageBox::information(this,"Device full","Devices reaches top limit, no more devices can be added");
        return;
    }
    ezlogic_form_devadd->setFixedSize(250, 120);
    ezlogic_form_devadd->setModal(true);
    ezlogic_form_devadd->show();
}

void MainWindow::on_pushButton_remove_device_clicked() {

    EZPI_UINT8 last_row = ui->tableWidget_device_table->rowCount();
    qDebug() << "Row count: " << QString::number(last_row);
    ui->tableWidget_device_table->removeRow(last_row - 1);

    switch(ezlogic_table_row_device_map.at(last_row - 1)) {

        case EZPI_DEV_TYPE_DIGITAL_OP:
            EzloPi->EZPI_DELETE_OUTPUT_DEVICE();
            ezlogic_table_row_device_map.pop_back();
            break;
        case EZPI_DEV_TYPE_DIGITAL_IP:
            EzloPi->EZPI_DELETE_INPUT_DEVICE();
            ezlogic_table_row_device_map.pop_back();
            break;
        case EZPI_DEV_TYPE_ANALOG_IP:
            EzloPi->EZPI_DELETE_AINPUT_DEVICE();
            ezlogic_table_row_device_map.pop_back();
            break;

        case EZPI_DEV_TYPE_PWM:
            EzloPi->EZPI_DELETE_PWM_DEVICE();
            ezlogic_table_row_device_map.pop_back();
            break;

        case EZPI_DEV_TYPE_UART:
            EzloPi->EZPI_DELETE_UART_DEVICE();
            ezlogic_table_row_device_map.pop_back();
            break;

        case EZPI_DEV_TYPE_ONE_WIRE:
            EzloPi->EZPI_DELETE_ONEWIRE_DEVICE();
            ezlogic_table_row_device_map.pop_back();
            break;
        case EZPI_DEV_TYPE_I2C:
            EzloPi->EZPI_DELETE_I2C_DEVICE();
            ezlogic_table_row_device_map.pop_back();
            break;
        case EZPI_DEV_TYPE_SPI:
            EzloPi->EZPI_DELETE_SPI_DEVICE();
            ezlogic_table_row_device_map.pop_back();
            break;
    case EZPI_DEV_TYPE_OTHER:
        EzloPi->EZPI_DELETE_OTHER_DEVICE();
        ezlogic_table_row_device_map.pop_back();
        break;
        default:
            break;
    }

    if(last_row == 1) {
        ui->pushButton_remove_device->setEnabled(false);
    }
}

void MainWindow::on_pushButton_get_ezpi_config_clicked() {

    QString response_data;

    QString json_send_get_config = "{\"cmd\":4}";

    ezlogic_cmd_state = CMD_ACTION_GET_CONFIG;
    ezlogic_serial_transfer(json_send_get_config.toLocal8Bit());

}

void MainWindow::on_pushButton_set_ezpi_config_clicked() {

    QJsonObject object_root_set_device;
    QJsonDocument document_root_set_device;
    QJsonArray array_device_detail;

    std::vector <ezpi_device_digital_op_t> device_digital_op = EzloPi->EZPI_GET_OUTPUT_DEVICES();
    std::vector <ezpi_device_digital_ip_t> device_digital_ip = EzloPi->EZPI_GET_INPUT_DEVICES();
    std::vector <ezpi_device_analog_ip_t> device_analog_ip = EzloPi->EZPI_GET_AINPUT_DEVICES();
    std::vector <ezpi_device_pwm_t> device_pwm = EzloPi->EZPI_GET_PWM_DEVICES();
    std::vector <ezpi_device_uart_t> device_uart = EzloPi->EZPI_GET_UART_DEVICES();
    std::vector <ezpi_device_one_wire_t> device_onewire = EzloPi->EZPI_GET_ONEWIRE_DEVICES();
    std::vector <ezpi_device_I2C_t> device_i2c = EzloPi->EZPI_GET_I2C_DEVICES();
    std::vector <ezpi_device_SPI_t> device_spi = EzloPi->EZPI_GET_SPI_DEVICES();
    std::vector <ezpi_device_other_t> device_other = EzloPi->EZPI_GET_OTHER_DEVICES();

    object_root_set_device.insert("cmd", CMD_ACTION_SET_CONFIG);
    object_root_set_device.insert("dev_total", EzloPi->EZPI_GET_DEVICE_COUNT());

    for(EZPI_UINT8 i = 0; i < device_digital_op.size(); i++) {

        QJsonObject object_device_digital_output;

        object_device_digital_output.insert("dev_type", device_digital_op[i].dev_type);
        object_device_digital_output.insert("dev_name", device_digital_op[i].dev_name);
        object_device_digital_output.insert("id_room", device_digital_op[i].id_room);
        object_device_digital_output.insert("id_item", device_digital_op[i].id_item);
        object_device_digital_output.insert("val_ip", device_digital_op[i].val_ip);
        object_device_digital_output.insert("val_op", device_digital_op[i].val_op);
        object_device_digital_output.insert("gpio_in", device_digital_op[i].gpio_in);
        object_device_digital_output.insert("gpio_out", device_digital_op[i].gpio_out);
        object_device_digital_output.insert("is_ip", device_digital_op[i].is_ip);
        object_device_digital_output.insert("pullup_ip", device_digital_op[i].pullup_ip);
        object_device_digital_output.insert("pullup_op", device_digital_op[i].pullup_op);
        object_device_digital_output.insert("ip_inv", device_digital_op[i].ip_inv);
        object_device_digital_output.insert("op_inv", device_digital_op[i].op_inv);

        array_device_detail.push_back(object_device_digital_output);
    }

    for(EZPI_UINT8 i = 0; i < device_digital_ip.size(); i++) {
        QJsonObject object_device_digital_input;

        object_device_digital_input.insert("dev_type", device_digital_ip[i].dev_type);
        object_device_digital_input.insert("dev_name", device_digital_ip[i].dev_name);
        object_device_digital_input.insert("id_room", device_digital_ip[i].id_room);
        object_device_digital_input.insert("id_item", device_digital_ip[i].id_item);
        object_device_digital_input.insert("val_ip", device_digital_ip[i].val_ip);
        object_device_digital_input.insert("gpio", device_digital_ip[i].gpio);
        object_device_digital_input.insert("pull_up", device_digital_ip[i].pull_up);
        object_device_digital_input.insert("logic_inv", device_digital_ip[i].logic_inv);

        array_device_detail.push_back(object_device_digital_input);
    }

    for(EZPI_UINT8 i = 0; i < device_analog_ip.size(); i++) {
        QJsonObject object_device_analog_input;

        object_device_analog_input.insert("dev_type", device_analog_ip[i].dev_type);
        object_device_analog_input.insert("dev_name", device_analog_ip[i].dev_name);
        object_device_analog_input.insert("id_room", device_analog_ip[i].id_room);
        object_device_analog_input.insert("id_item", device_analog_ip[i].id_item);
        object_device_analog_input.insert("gpio", device_analog_ip[i].gpio);

        if(device_analog_ip[i].resln_bit == 1) {
            object_device_analog_input.insert("resln_bit", 8);
        } else if(device_analog_ip[i].resln_bit == 2) {
            object_device_analog_input.insert("resln_bit", 10);
        } else if (device_analog_ip[i].resln_bit == 3) {
            object_device_analog_input.insert("resln_bit", 12);
        } else {

        }

        array_device_detail.push_back(object_device_analog_input);
    }


    for(EZPI_UINT8 i = 0; i < device_pwm.size(); i++) {
        QJsonObject object_device_pwm;

        object_device_pwm.insert("dev_type", device_pwm[i].dev_type);
        object_device_pwm.insert("dev_name", device_pwm[i].dev_name);
        object_device_pwm.insert("id_room", device_pwm[i].id_room);
        object_device_pwm.insert("id_item", device_pwm[i].id_item);
        object_device_pwm.insert("gpio", device_pwm[i].gpio);
        object_device_pwm.insert("freq_hz", device_pwm[i].freq_hz);
        object_device_pwm.insert("pwm_resln", device_pwm[i].pwm_resln);
        object_device_pwm.insert("duty_cycle", device_pwm[i].duty_cycle);

        array_device_detail.push_back(object_device_pwm);
    }


    for(EZPI_UINT8 i = 0; i < device_uart.size(); i++) {
        QJsonObject object_device_uart;

        object_device_uart.insert("dev_type", device_uart[i].dev_type);
        object_device_uart.insert("dev_name", device_uart[i].dev_name);
        object_device_uart.insert("id_room", device_uart[i].id_room);
        object_device_uart.insert("id_item", device_uart[i].id_item);
        object_device_uart.insert("gpio_rx", device_uart[i].gpio_rx);
        object_device_uart.insert("gpio_tx", device_uart[i].gpio_tx);
        object_device_uart.insert("baud_rate", device_uart[i].baud_rate);

        array_device_detail.push_back(object_device_uart);
    }


    for(EZPI_UINT8 i = 0; i < device_onewire.size(); i++) {
        QJsonObject object_device_onewire;

        object_device_onewire.insert("dev_type", device_onewire[i].dev_type);
        object_device_onewire.insert("dev_name", device_onewire[i].dev_name);
        object_device_onewire.insert("id_room", device_onewire[i].id_room);
        object_device_onewire.insert("id_item", device_onewire[i].id_item);
        object_device_onewire.insert("gpio", device_onewire[i].gpio);

        array_device_detail.push_back(object_device_onewire);
    }

    for(EZPI_UINT8 i = 0; i < device_i2c.size(); i++) {
        QJsonObject object_device_i2c;

        object_device_i2c.insert("dev_type", device_i2c[i].dev_type);
        object_device_i2c.insert("dev_name", device_i2c[i].dev_name);
        object_device_i2c.insert("id_room", device_i2c[i].id_room);
        object_device_i2c.insert("id_item", device_i2c[i].id_item);
        object_device_i2c.insert("gpio_sda", device_i2c[i].gpio_sda);
        object_device_i2c.insert("gpio_scl", device_i2c[i].gpio_scl);
        object_device_i2c.insert("pullup_scl", device_i2c[i].pullup_scl);
        object_device_i2c.insert("pullup_sda", device_i2c[i].pullup_sda);
        object_device_i2c.insert("slave_addr", device_i2c[i].slave_addr);

        array_device_detail.push_back(object_device_i2c);
    }

    for(EZPI_UINT8 i = 0; i < device_spi.size(); i++) {
        QJsonObject object_device_spi;

        object_device_spi.insert("dev_type", device_spi[i].dev_type);
        object_device_spi.insert("dev_name", device_spi[i].dev_name);
        object_device_spi.insert("id_room", device_spi[i].id_room);
        object_device_spi.insert("id_item", device_spi[i].id_item);
        object_device_spi.insert("gpio_miso", device_spi[i].gpio_miso);
        object_device_spi.insert("gpio_mosi", device_spi[i].gpio_mosi);
        object_device_spi.insert("gpio_sck", device_spi[i].gpio_sck);
        object_device_spi.insert("gpio_cs", device_spi[i].gpio_cs);

        array_device_detail.push_back(object_device_spi);
    }


    for(EZPI_UINT8 i = 0; i < device_other.size(); i++) {
        QJsonObject object_device_other;

        object_device_other.insert("dev_type", device_other[i].dev_type);
        object_device_other.insert("dev_name", device_other[i].dev_name);
        object_device_other.insert("id_room", device_other[i].id_room);
        object_device_other.insert("id_item", device_other[i].id_item);
        object_device_other.insert("en_gpio1", device_other[i].en_gpio1);
        object_device_other.insert("gpio1", device_other[i].gpio1);
        object_device_other.insert("en_gpio2", device_other[i].en_gpio2);
        object_device_other.insert("gpio2", device_other[i].gpio2);
        object_device_other.insert("en_gpio3", device_other[i].en_gpio3);
        object_device_other.insert("gpio3", device_other[i].gpio3);

        array_device_detail.push_back(object_device_other);
    }

    object_root_set_device.insert("dev_detail", array_device_detail);
    document_root_set_device.setObject(object_root_set_device);
    QString json_string = document_root_set_device.toJson(QJsonDocument::Indented);
    ui->textBrowser_console_log->append(json_string);
    qDebug() << json_string;
    ezlogic_cmd_state = CMD_ACTION_SET_CONFIG;
    ezlogic_serial_transfer(document_root_set_device.toJson(QJsonDocument::Compact));
}

void MainWindow::on_pushButton_device_restart_clicked() {
    ezlogic_cmd_state = CMD_ACTION_RESET;
    QString ezlogic_reset_command = "{\"cmd\":0}";
    ezlogic_serial_transfer(ezlogic_reset_command.toUtf8());
    QMessageBox::warning(this, "Device ready to restart", "Connected hardware device will now restart!");
}


// Custom Slots
void MainWindow::ezlogic_log_write_flash() {

    QMessageBox ezpi_message_box_failed_erase;
    QByteArray byteArray = ezlogic_process_write_flash->readAllStandardOutput();
    QString str_StandardOutput = QString::fromLocal8Bit(byteArray);
//    qDebug() << str_StandardOutput;

    ui->textBrowser_console_log->append(str_StandardOutput);

    byteArray = ezlogic_process_write_flash->readAllStandardError();
    QString str_StandardError = QString::fromLocal8Bit(byteArray);
//    qDebug() << str_StandardError;

    ui->textBrowser_console_log->append(str_StandardError);

    if(str_StandardOutput.contains("error") || str_StandardError.contains("error")) {
        ezpi_message_box_failed_erase.information(this, "Failed flashing the device!", "Flashing ezlopi firmware to the device was not successful, close the app, disconnect device, reconnect and try again!");
        ezlogic_serial_port->open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    } else if(str_StandardError.contains("Hard resetting") || str_StandardOutput.contains("Hard resetting")) {
        ezpi_message_box_failed_erase.information(this, "Success flashing the device!", "Flashing ezlopi firmware to the device was successful! You can now gracefully disconnect the device and move further!");
        ezlogic_serial_port->open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    }
}

void MainWindow::ezlogic_log_erase_flash() {

    QMessageBox ezpi_message_box_failed_erase;
    QByteArray byteArray = ezlogic_process_erase_flash->readAllStandardOutput();
    QString str_StandardOutput = QString::fromLocal8Bit(byteArray);

    ui->textBrowser_console_log->append(str_StandardOutput);

    byteArray = ezlogic_process_erase_flash->readAllStandardError();
    QString str_StandardError = QString::fromLocal8Bit(byteArray);

    ui->textBrowser_console_log->append(str_StandardError);

    if(str_StandardOutput.contains("error") || str_StandardError.contains("error")) {
        ezpi_message_box_failed_erase.information(this, "Failed erasing the device!", "Erasing the flas of the device was not successful, close the app, disconnect device, reconnect and try again!");
        ezlogic_serial_port->open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    } else if(str_StandardOutput.contains("Hard resetting") || str_StandardError.contains("Hard resetting")) {
        ezpi_message_box_failed_erase.information(this, "Success erasing the device!", "Erasing the device was successful! You can now proceed forward flashing the ezlopi firmware!");
        ezlogic_serial_port->open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    }
}

EZPI_BOOL MainWindow::ezlogic_check_firmware() {

#if 0
    QString test_json = "{\"cmd\":1,\"status\":1,\"v_sw\":3625,\"v_type\":1,\"build\":17,\"v_idf\":8456,\"uptime"
                        "\":4856,\"build_date\":1657623331,\"boot_count\":15,\"boot_reason\":2,\"mac\":1577079727,"
                        "\"uuid\":\"65261d76-e584-4d35-aff1-d84bd043\",\"serial\":100004032,\"ssid\":\"ssid\",\"dev_type\":1,"
                        "\"dev_flash\":64256,\"dev_free_flash\":300,\"dev_name\":\"My Device\"}";
#endif

    QString json_send_get_info = "{\"cmd\":1}";

    ezlogic_cmd_state = CMD_ACTION_GET_INFO;
    ezlogic_serial_transfer(json_send_get_info.toUtf8());
    ezlogic_timer_ask_info.start(EZPI_FIRMWARE_CHECK_TIMEOUT);
    return ezlogic_flag_fimware_present;
}

void MainWindow::ezlogic_message_info_no_firmware_detected() {
    QMessageBox ezpi_messagebox_ezpi_firmware_not_detected;
    ezpi_messagebox_ezpi_firmware_not_detected.information(this, "No firmware detected!", "No ezlo pi firmware has been detected. Please flash firmware and try again!");
    ezlogic_timer_ask_info.stop();

    // Enable flash and erase buttons
    ui->pushButton_erase_flash->setEnabled(true);
    ui->pushButton_flash_ezpi_bins->setEnabled(true);
    ui->pushButton_clear_uart_direct_log->setEnabled(true);
    ui->comboBox_esp32_board_type->setEnabled(true);

    ezlogic_clear_table_data();
    ui->tableWidget_device_table->setEnabled(false);

    ui->comboBox_registered_devices->setEnabled(true);
}

void MainWindow::ezlogic_receive_dev_type_selected(EZPI_UINT8 dev_type_index) {

    qDebug() << "Device type Index: " << dev_type_index;

    ezlogic_form_configdev_digitalio = new Dialog_configdev_digitalio(this, EzloPi);
    ezlogic_form_config_digital_ip = new Dialog_config_input(this, EzloPi);
    ezlogic_form_config_analog_ip = new Dialog_config_adc(this, EzloPi);
    ezlogic_form_config_pwm = new Dialog_config_pwm(this, EzloPi);
    ezlogic_form_config_uart = new Dialog_config_uart(this, EzloPi);
    ezlogic_form_config_onewire = new Dialog_config_onewire(this, EzloPi);
    ezlogic_form_config_i2c = new Dialog_config_i2c(this, EzloPi);
    ezlogic_form_config_spi = new Dialog_config_spi(this, EzloPi);
    ezlogic_form_config_other = new Dialog_config_other(this, EzloPi);

    connect(ezlogic_form_configdev_digitalio, SIGNAL(ezpi_signal_dev_op_added(ezpi_dev_type)), this, SLOT(ezlogic_receive_added_dev(ezpi_dev_type)));
    connect(ezlogic_form_config_digital_ip, SIGNAL(ezpi_signal_dev_ip_added(ezpi_dev_type)), this, SLOT(ezlogic_receive_added_dev(ezpi_dev_type)));
    connect(ezlogic_form_config_analog_ip, SIGNAL(ezpi_signal_dev_adc_added(ezpi_dev_type)), this, SLOT(ezlogic_receive_added_dev(ezpi_dev_type)));
    connect(ezlogic_form_config_pwm, SIGNAL(ezpi_signal_dev_pwm_added(ezpi_dev_type)), this, SLOT(ezlogic_receive_added_dev(ezpi_dev_type)));
    connect(ezlogic_form_config_uart, SIGNAL(ezpi_signal_dev_uart_added(ezpi_dev_type)), this, SLOT(ezlogic_receive_added_dev(ezpi_dev_type)));
    connect(ezlogic_form_config_onewire, SIGNAL(ezpi_signal_dev_onewire_added(ezpi_dev_type)), this, SLOT(ezlogic_receive_added_dev(ezpi_dev_type)));
    connect(ezlogic_form_config_i2c, SIGNAL(ezpi_signal_dev_i2c_added(ezpi_dev_type)), this, SLOT(ezlogic_receive_added_dev(ezpi_dev_type)));
    connect(ezlogic_form_config_spi, SIGNAL(ezpi_signal_dev_spi_added(ezpi_dev_type)), this, SLOT(ezlogic_receive_added_dev(ezpi_dev_type)));
    connect(ezlogic_form_config_other, SIGNAL(ezpi_signal_dev_other_added(ezpi_dev_type)), this, SLOT(ezlogic_receive_added_dev(ezpi_dev_type)));

    switch(dev_type_index) {
        case EZPI_DEV_TYPE_DIGITAL_OP: {
            ezlogic_form_configdev_digitalio->setFixedSize(275, 380);
            ezlogic_form_configdev_digitalio->setModal(true);
            ezlogic_form_configdev_digitalio->show();
            break;
        }
        case EZPI_DEV_TYPE_DIGITAL_IP: {
            ezlogic_form_config_digital_ip->setFixedSize(290, 260);
            ezlogic_form_config_digital_ip->setModal(true);
            ezlogic_form_config_digital_ip->show();
            break;
        }
        case EZPI_DEV_TYPE_ANALOG_IP: {
            ezlogic_form_config_analog_ip->setFixedSize(170, 265);
            ezlogic_form_config_analog_ip->setModal(true);
            ezlogic_form_config_analog_ip->show();
           break;
        }
        case EZPI_DEV_TYPE_ANALOG_OP: {
            QMessageBox::information(this, "Unsupported", "This feature is not supported yet!");
            break;
        }
        case EZPI_DEV_TYPE_PWM: {
            ezlogic_form_config_pwm->setFixedSize(220, 315);
            ezlogic_form_config_pwm->setModal(true);
            ezlogic_form_config_pwm->show();
            break;
        }
        case EZPI_DEV_TYPE_UART: {
            ezlogic_form_config_uart->setFixedSize(185, 280);
            ezlogic_form_config_uart->setModal(true);
            ezlogic_form_config_uart->show();
            break;
        }
        case EZPI_DEV_TYPE_ONE_WIRE: {
            ezlogic_form_config_onewire->setFixedSize(190, 230);
            ezlogic_form_config_onewire->setModal(true);
            ezlogic_form_config_onewire->show();
            break;
        }
        case EZPI_DEV_TYPE_I2C: {
            ezlogic_form_config_i2c->setFixedSize(325, 240);
            ezlogic_form_config_i2c->setModal(true);
            ezlogic_form_config_i2c->show();
            break;
        }
        case EZPI_DEV_TYPE_SPI: {
            ezlogic_form_config_spi->setFixedHeight(296);
            ezlogic_form_config_spi->setFixedWidth(190);
            ezlogic_form_config_spi->setModal(true);
            ezlogic_form_config_spi->show();
            break;
        }
        case EZPI_DEV_TYPE_OTHER: {
            if(EzloPi->EZPI_GET_BOARD_TYPE() == EZPI_BOARD_TYPE_ESP32_C3) {
                QMessageBox::information(this, "Unsupported", "Not supported for board ESP32C3.");
                break;
            }
            ezlogic_form_config_other->setFixedHeight(270);
            ezlogic_form_config_other->setFixedWidth(185);
            ezlogic_form_config_other->setModal(true);
            ezlogic_form_config_other->show();
            break;
        }

        default: {
            QMessageBox::information(this, "Unknown", "Unknown type.");
        }
    }
}

void MainWindow::ezlogic_receive_added_dev(ezpi_dev_type ezpi_added_dev_type) {

    qDebug() << "Added device type: " << QString::number(ezpi_added_dev_type);

    ui->pushButton_remove_device->setEnabled(true);
//    ui->pushButton_set_ezpi_config->setEnabled(true);

    switch(ezpi_added_dev_type) {
        case EZPI_DEV_TYPE_DIGITAL_OP: {

            std::vector <ezpi_device_digital_op_t> output_devices = EzloPi->EZPI_GET_OUTPUT_DEVICES();
            EZPI_UINT8 output_devices_total = (EZPI_UINT8)output_devices.size();
            ezpi_device_digital_op_t output_device = output_devices[output_devices_total - 1];
            ezlogic_table_adddev_digital_op(output_device);
            break;
        }
        case EZPI_DEV_TYPE_DIGITAL_IP: {

            std::vector <ezpi_device_digital_ip_t> input_devices = EzloPi->EZPI_GET_INPUT_DEVICES();
            EZPI_UINT8 input_devices_total = (EZPI_UINT8)input_devices.size();
            ezpi_device_digital_ip_t input_device = input_devices[input_devices_total - 1];
            ezlogic_table_adddev_digital_ip(input_device);
            break;
        }
        case EZPI_DEV_TYPE_ANALOG_IP: {

            std::vector <ezpi_device_analog_ip_t> adc_devices = EzloPi->EZPI_GET_AINPUT_DEVICES();
            EZPI_UINT8 adc_devices_total = (EZPI_UINT8)adc_devices.size();
            ezpi_device_analog_ip_t adc_device = adc_devices[adc_devices_total - 1];
            ezlogic_table_adddev_analog_ip(adc_device);
            break;
        }
        case EZPI_DEV_TYPE_PWM: {
            std::vector <ezpi_device_pwm_t> pwm_devices = EzloPi->EZPI_GET_PWM_DEVICES();
            EZPI_UINT8 pwm_devices_total = (EZPI_UINT8)pwm_devices.size();
            ezpi_device_pwm_t pwm_device = pwm_devices[pwm_devices_total - 1];
            ezlogic_table_adddev_pwm(pwm_device);
            break;
        }
        case EZPI_DEV_TYPE_UART: {
            std::vector <ezpi_device_uart_t> uart_devices = EzloPi->EZPI_GET_UART_DEVICES();
            EZPI_UINT8 uart_devices_total = (EZPI_UINT8)uart_devices.size();
            ezpi_device_uart_t uart_device = uart_devices[uart_devices_total - 1];
            ezlogic_table_adddev_uart(uart_device);
            break;
        }
        case EZPI_DEV_TYPE_ONE_WIRE: {
            std::vector <ezpi_device_one_wire_t> onewire_devices = EzloPi->EZPI_GET_ONEWIRE_DEVICES();
            EZPI_UINT8 onewire_devices_total = (EZPI_UINT8)onewire_devices.size();
            ezpi_device_one_wire_t onewire_device = onewire_devices[onewire_devices_total - 1];
            ezlogic_table_adddev_onewire(onewire_device);
            break;
        }
        case EZPI_DEV_TYPE_I2C: {
            std::vector <ezpi_device_I2C_t> i2c_devices = EzloPi->EZPI_GET_I2C_DEVICES();
            EZPI_UINT8 i2c_devices_total = (EZPI_UINT8)i2c_devices.size();
            ezpi_device_I2C_t i2c_device = i2c_devices[i2c_devices_total - 1];
            ezlogic_table_adddev_i2c(i2c_device);
            break;
        }
        case EZPI_DEV_TYPE_SPI: {
            std::vector <ezpi_device_SPI_t> spi_devices = EzloPi->EZPI_GET_SPI_DEVICES();
            EZPI_UINT8 spi_devices_total = (EZPI_UINT8)spi_devices.size();
            ezpi_device_SPI_t spi_device = spi_devices[spi_devices_total - 1];
            ezlogic_table_adddev_spi(spi_device);
            break;
        }
        case EZPI_DEV_TYPE_OTHER: {
            std::vector <ezpi_device_other_t> other_devices = EzloPi->EZPI_GET_OTHER_DEVICES();
            EZPI_UINT8 other_devices_total = (EZPI_UINT8)other_devices.size();
            ezpi_device_other_t other_device = other_devices[other_devices_total - 1];
            ezlogic_table_adddev_other(other_device);
            break;
        }
        default:
            break;
    }
}

void MainWindow::ezlogic_serial_receive_wif(ezpi_cmd cmd) {
    ezlogic_cmd_state = cmd;
    ezlogic_timer_serial_complete.start(EZPI_SERIAL_READ_TIMEOUT);
}

void MainWindow::ezlogic_clear_table_data(void) {

    while(ui->tableWidget_device_table->rowCount()) {
        ui->tableWidget_device_table->removeRow(ui->tableWidget_device_table->rowCount()-1);
    }

    ui->pushButton_remove_device->setEnabled(false);
//    ui->pushButton_set_ezpi_config->setEnabled(false);

    // Clear data
    EzloPi->EZPI_CLEAR_OUTPUT_DEVICES();
    EzloPi->EZPI_CLEAR_INPUT_DEVICES();
    EzloPi->EZPI_CLEAR_AINPUT_DEVICES();
    EzloPi->EZPI_CLEAR_PWM_DEVICES();
    EzloPi->EZPI_CLEAR_UART_DEVICES();
    EzloPi->EZPI_CLEAR_ONEWIRE_DEVICES();    
    EzloPi->EZPI_CLEAR_I2C_DEVICES();
    EzloPi->EZPI_CLEAR_SPI_DEVICES();
    ezlogic_table_row_device_map.clear();
}


// UI generated SLOTS

void MainWindow::on_pushButton_scan_uart_ports_clicked() {

    QMessageBox messageBoxNoUart;
    QSerialPortInfo ports;

    ui->comboBox_uart_list->clear(); // Clear the existing list

    ezlogic_serial_ports_info_list.clear(); // Clear the existing list of serial port info

    if(ports.availablePorts().size() <= 0) {
        messageBoxNoUart.information(this, "No device found!", "We did not find any device connected, please check your connection and try again.");
    } else {
        qDebug() << "Available UART Ports";
        if(ezlogic_log_level == EZPI_LOG_INFO) ui->textBrowser_console_log->append("Available UART Ports:\n");

        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            qDebug().noquote() << info.portName() << info.description() << info.manufacturer();
            if(ezlogic_log_level == EZPI_LOG_INFO) ui->textBrowser_console_log->append(info.portName() + " " + info.description() + " " + info.manufacturer() + "\n");

            if(info.description() != "") {
                ui->comboBox_uart_list->addItem(info.portName());
                ezlogic_serial_ports_info_list.push_back(info);
            }
        }

        for(QSerialPortInfo info : ezlogic_serial_ports_info_list) {
            if(ui->comboBox_uart_list->currentText() == info.portName()) {
                ezlogic_serial_port_info = info;
            }
        }
        ui->pushButton_connect_uart-> setEnabled(true);
    }
}
void MainWindow::ezlogic_success_prov_dat(QNetworkReply *d) {

    qDebug() << "Added new device";
    ui->textBrowser_console_log->append("Added new device!");

    QByteArray response_bytes = d->readAll();
//    qDebug() << "\r\n\r\n";
//    qDebug().noquote() << QString(response_bytes);
//    qDebug() << "\r\n\r\n";
    QJsonParseError jerror;
    QJsonDocument jdoc= QJsonDocument::fromJson(response_bytes, &jerror);

    if(jerror.error != QJsonParseError::NoError) {

        qDebug() << "Message failed parcing json";
        if(ezlogic_log_level == EZPI_LOG_INFO) ui->textBrowser_console_log->append("Error: Failed parcing json");
        return;
    }

    QJsonObject jobj_main = jdoc.object();

     uint8_t status = jobj_main["status"].toInt();
     uint8_t complete = jobj_main["complete"].toInt();

    if( (status == 1) && (complete == 1) ) {

        if(ezlogic_log_level == EZPI_LOG_INFO) ui->textBrowser_console_log->append("Success: New device has been registered, you can login and find the device added.");

        QJsonObject jobj_data = jobj_main["data"].toObject();
        QString uuid = jobj_data["uuid"].toString();

        QJsonObject jobj_get_prov_json;
        QJsonObject jobj_param;

        ezlogic_uuid_self_prov = uuid;
        jobj_param["uuid"] = uuid;
        jobj_get_prov_json["params"] = jobj_param;
        jobj_get_prov_json["call"] = QString("self_provision_get");

        QJsonDocument jdoc_get_prov_json(jobj_get_prov_json);

        QByteArray get_prov_json = jdoc_get_prov_json.toJson(QJsonDocument::Compact);


        // Make POST request
        QUrl url("https://api-cloud.ezlo.com/v1/request");
        QNetworkRequest request(url);
        request.setRawHeader("Authorization", "Bearer " + ezlogic_prov_data_user_token.toLocal8Bit());

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);

        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(ezlogic_success_get_prov_jsons(QNetworkReply*)));        

        // FIXME for debug
        qDebug() << "Sync" << QString::fromUtf8(get_prov_json.data(), get_prov_json.size());

        manager->post(request, get_prov_json);
    } else {
        QMessageBox::warning(this, "Communication failed", "Registration process aborted, try again closing and reopeaning the app!");
        return;
    }
}

void MainWindow::ezlogic_success_get_prov_jsons(QNetworkReply *d) {

     conv_u16_array_t ser_ver;
     conv_64_array_t id;

    struct uuid _uuid;

    QByteArray response_bytes = d->readAll();
    qDebug() << "\r\n\r\n";
    qDebug().noquote() << QString(response_bytes);
    qDebug() << "\r\n\r\n";
    QJsonParseError jerror;

    QJsonDocument jdoc_prov_data= QJsonDocument::fromJson(response_bytes, &jerror);

    if(jerror.error != QJsonParseError::NoError) {
        qDebug() << "Message failed parcing json";
        return;
    }

    QJsonObject jobj_root_prov_data = jdoc_prov_data.object();

    if((jobj_root_prov_data["status"] == 1) && (jobj_root_prov_data["complete"] == 1)) {

        QJsonObject jobj_prov_data_data = jobj_root_prov_data["data"].toObject();

        QString prov_data_provision_data = jobj_prov_data_data["provision_data"].toString();

        QJsonDocument jdoc_prov_data_prov_data = QJsonDocument::fromJson(prov_data_provision_data.toUtf8());
        QJsonObject jobj_prov_data_prov_data = jdoc_prov_data_prov_data.object();

        QByteArray ld_binary_array;


        uuid_from_string(jobj_prov_data_data["controller_uuid"].toString().toStdString().c_str(), &_uuid);
        QString dev_uuid = QString::fromLocal8Bit(QByteArray::fromRawData((const char *)_uuid.bytes, 16));

        qDebug() << "provision_server: " << jobj_prov_data_prov_data["provision_server"].toString();
        qDebug() << "cloud_server: " << jobj_prov_data_prov_data["cloud_server"].toString();
        qDebug() << "provision_token: " << jobj_prov_data_prov_data["provision_token"].toString();
        qDebug() << "default_wifi_ssid: " << jobj_prov_data_prov_data["default_wifi_ssid"].toString();
        qDebug() << "default_wifi_password: " << jobj_prov_data_prov_data["default_wifi_password"].toString();
        qDebug() << "id: " << jobj_prov_data_prov_data["id"].toInt();
        qDebug() << "uuid: " << jobj_prov_data_data["controller_uuid"].toString();

        ui->textBrowser_console_log->append( QString::fromStdString("provision_server: ") + jobj_prov_data_prov_data["provision_server"].toString());
        ui->textBrowser_console_log->append( QString::fromStdString("cloud_server: ") + jobj_prov_data_prov_data["cloud_server"].toString());
        ui->textBrowser_console_log->append( QString::fromStdString("default_wifi_ssid: ") + jobj_prov_data_prov_data["default_wifi_ssid"].toString());
        ui->textBrowser_console_log->append( QString::fromStdString("default_wifi_password: ") + jobj_prov_data_prov_data["default_wifi_password"].toString());
        ui->textBrowser_console_log->append( QString::fromStdString("controller_uuid: ") + jobj_prov_data_data["controller_uuid"].toString());
        ui->textBrowser_console_log->append( QString::fromStdString("id or Serial: ") + QString::number(jobj_prov_data_prov_data["id"].toInt()));

        ser_ver.data = 0;
        // For Connection-ID-0
        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_0 + EZPI_PROV_CONN_ID_PARAM_SN,
                               QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Serial

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_0 + EZPI_PROV_CONN_ID_PARAM_VERSION,
                               QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Version

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_0 + EZPI_PROV_CONN_ID_PARAM_RESERVE_I,
                               QString::fromStdString("").toLocal8Bit());

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_0 + EZPI_PROV_CONN_ID_PARAM_PROV_SERVER,
                               jobj_prov_data_prov_data["provision_server"].toString().toLocal8Bit());
        ld_binary_array.append('\0');


        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_0 + EZPI_PROV_CONN_ID_PARAM_PROV_TOKEN,
                               jobj_prov_data_prov_data["provision_token"].toString().toLocal8Bit());
        ld_binary_array.append('\0');


        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_0 + EZPI_PROV_CONN_ID_PARAM_CLOUD_SERVER,
                               jobj_prov_data_prov_data["cloud_server"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_0 + EZPI_PROV_CONN_ID_PARAM_PROV_UUID,
                               ezlogic_uuid_self_prov.toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_0 + EZPI_PROV_CONN_ID_PARAM_CA_CERT,
                               jobj_prov_data_prov_data["signing_ca_certificate"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_0 + EZPI_PROV_CONN_ID_PARAM_SSL_PRIVATE_KEY,
                               jobj_prov_data_prov_data["ssl_private_key"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_0 + EZPI_PROV_CONN_ID_PARAM_SSL_SHARED_KEY,
                               jobj_prov_data_prov_data["ssl_shared_key"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        // For Connection ID 1
        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + EZPI_PROV_CONN_ID_PARAM_SN,
                               QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Serial

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + EZPI_PROV_CONN_ID_PARAM_VERSION,
                               QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Version

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + EZPI_PROV_CONN_ID_PARAM_RESERVE_I,
                               QString::fromStdString("").toLocal8Bit());

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + EZPI_PROV_CONN_ID_PARAM_PROV_SERVER,
                               jobj_prov_data_prov_data["provision_server"].toString().toLocal8Bit());
        ld_binary_array.append('\0');


        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + EZPI_PROV_CONN_ID_PARAM_PROV_TOKEN,
                               jobj_prov_data_prov_data["provision_token"].toString().toLocal8Bit());
        ld_binary_array.append('\0');


        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + EZPI_PROV_CONN_ID_PARAM_CLOUD_SERVER,
                               jobj_prov_data_prov_data["cloud_server"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + EZPI_PROV_CONN_ID_PARAM_PROV_UUID,
                               ezlogic_uuid_self_prov.toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + EZPI_PROV_CONN_ID_PARAM_CA_CERT,
                               jobj_prov_data_prov_data["signing_ca_certificate"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + EZPI_PROV_CONN_ID_PARAM_SSL_PRIVATE_KEY,
                               jobj_prov_data_prov_data["ssl_private_key"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + EZPI_PROV_CONN_ID_PARAM_SSL_SHARED_KEY,
                               jobj_prov_data_prov_data["ssl_shared_key"].toString().toLocal8Bit());
        ld_binary_array.append('\0');


        ser_ver.data = 0;
        // For HUB-ID-0
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_SN,
                               QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Serial

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_VERSION,
                               QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Version

        id.data = jobj_prov_data_prov_data["id"].toInt();
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_ID,
                               QByteArray::fromRawData((const char *)id.data_bytes, 8));

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_ZWAVE_REGION,
                               QString::fromStdString("US").toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_WIFI_SSID,
                               jobj_prov_data_prov_data["default_wifi_ssid"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_WIFI_PASS,
                               jobj_prov_data_prov_data["default_wifi_password"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_NAME,
                               QString::fromStdString("EzloPi").toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_MANUFACTURER,
                               QString::fromStdString("EzloPi Manufacturer").toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_BRAND,
                               QString::fromStdString("EzloPi Brand").toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_MODEL,
                               QString::fromStdString("EzloPi Model").toLocal8Bit());
        ld_binary_array.append('\0');


        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_EZLOPI_DEV_TYPE,
                               QString::fromStdString("generic").toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + EZPI_PROV_HUB_ID_PARAM_UUID,
                               jobj_prov_data_data["controller_uuid"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        // For HUB-ID-1
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_SN,
                               QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Serial

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_VERSION,
                               QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Version

        id.data = jobj_prov_data_prov_data["id"].toInt();
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_ID,
                               QByteArray::fromRawData((const char *)id.data_bytes, 8));

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_ZWAVE_REGION,
                               QString::fromStdString("US").toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_WIFI_SSID,
                               jobj_prov_data_prov_data["default_wifi_ssid"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_WIFI_PASS,
                               jobj_prov_data_prov_data["default_wifi_password"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_NAME,
                               QString::fromStdString("EzloPi").toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_MANUFACTURER,
                               QString::fromStdString("EzloPi Manufacturer").toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_BRAND,
                               QString::fromStdString("EzloPi Brand").toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_MODEL,
                               QString::fromStdString("EzloPi Model").toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_EZLOPI_DEV_TYPE,
                               QString::fromStdString("generic").toLocal8Bit());
        ld_binary_array.append('\0');

        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + EZPI_PROV_HUB_ID_PARAM_UUID,
                               jobj_prov_data_data["controller_uuid"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        qDebug() << "Current dir: " << QDir::currentPath();
        QString ld_file_name =  "devs/";
        ld_file_name += QString::number(jobj_prov_data_prov_data["id"].toInt());
        ld_file_name += ".bin";
        qDebug() << "File name: " << ld_file_name;
        QFile out(ld_file_name);
        out.open(QIODevice::WriteOnly);
        out.write(ld_binary_array);
        out.close();

        QMessageBox::information(this, "Registration complete!", "Adding new device has been successful.\n Please note the ID from console below.\n" \
                                                                 "now flash the firmware!");

        // scan and list registered devices
        ui->comboBox_registered_devices->clear();
        QDir directory("devs");
        QStringList registered_devices = directory.entryList(QStringList() << "*.bin",QDir::Files);
        foreach(QString filename, registered_devices) {
            qDebug() << "Registered devices: " << filename.remove(".bin");
            ui->comboBox_registered_devices->addItem(filename.remove(".bin"));
        }

        ezlogic_flag_registered = true;
    } else {
        QMessageBox::information(this, "Registration failed!", "Failed adding new device into the cloud platform, try again closing and reopeaning the app!");
    }
}

// ACTION list
void MainWindow::on_actionLogin_triggered() {
    ezlogic_form_login->setFixedSize(350, 225);
    ezlogic_form_login->setModal(true);
    ezlogic_form_login->show();
}

void MainWindow::on_actionRegister_triggered() {

    bool flag_login = ezlogic_form_login->ezpi_get_flag_user_login();
    uint64_t login_expires = ezlogic_form_login->ezpi_get_token_expiry_time();
    ezlogic_prov_data_user_token = ezlogic_form_login->ezpi_get_user_access_token();

    if(ezlogic_flag_registered) {
        QMessageBox::information(this, "Device has been registered.", "A device has already been registered in your account.");
        return;
    }

    if(flag_login == true) {

        if((uint64_t)QDateTime::currentSecsSinceEpoch() < login_expires) {

//            qDebug() << "Token: " << ezlogic_prov_data_user_token;

            QJsonObject jobj_get_uuid_root;
            QJsonObject jobj_param;
            QJsonObject nothing;

            jobj_param["description"] = QString("EzloPi Dev x.x.x");
            jobj_param["meta"] = nothing;
            jobj_get_uuid_root["call"] = QString("self_provision_request");
            jobj_get_uuid_root["params"] = jobj_param;

            QJsonDocument get_uuid(jobj_get_uuid_root);

            QByteArray getUUID_JSON = get_uuid.toJson(QJsonDocument::Compact);


            // Make POST request
            QUrl url("https://api-cloud.ezlo.com/v1/request");
            QNetworkRequest request(url);
            request.setRawHeader("Authorization", "Bearer " + ezlogic_prov_data_user_token.toLocal8Bit());

            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

            QNetworkAccessManager *manager = new QNetworkAccessManager(this);

            connect(manager, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(ezlogic_success_prov_dat(QNetworkReply*)));

            qDebug() << "Sync" << QString::fromUtf8(getUUID_JSON.data(), getUUID_JSON.size());

            manager->post(request, getUUID_JSON);

        } else {
          // Token expired
            qDebug() << "Token expired";
             ui->textBrowser_console_log->append("Error: user token expired, aborting the operation.");
        }

    } else {
        ui->textBrowser_console_log->append("Please login first to register.");
    }
}

void MainWindow::on_actionClear_Table_triggered() {
    ezlogic_clear_table_data();
}

void MainWindow::on_actionDisable_triggered() {
    ezlogic_log_level = EZPI_LOG_NONE;
    ui->actionDisable->setChecked(true);
    ui->actionInfo->setChecked(false);
    ui->actionDebug->setChecked(false);
    ui->textBrowser_console_log->append("Disabled log \n");
}


void MainWindow::on_actionInfo_triggered() {
    ezlogic_log_level = EZPI_LOG_INFO;
    ui->actionDisable->setChecked(false);
    ui->actionInfo->setChecked(true);
    ui->actionDebug->setChecked(false);
    ui->textBrowser_console_log->append("Enabled log : Info only \n");
}


void MainWindow::on_actionDebug_triggered() {
    ezlogic_log_level = EZPI_LOG_DEBUG;
    ui->actionDisable->setChecked(false);
    ui->actionInfo->setChecked(false);
    ui->actionDebug->setChecked(true);
    ui->textBrowser_console_log->append("Enabled log : debug only \n");
}

void MainWindow::on_actionAbout_EzloPi_triggered() {
    QMessageBox::about(this, "EzloPi V1.3.4", \
                       "EzloPi is an open-source project contributed by Ezlo Innovation "
                       "to extend the capabilities of ESP32 chipset-based devices "
                       "and platforms. It provides unparalleled capabilities to configure and "
                       "control your ESP-based devices and bring any of your automation ideas to life."
                       "\nEzloPi UI Version 1.3.4\n"
                       "EzloPi Firmware Version 2.0.7\n"
                       "Build type: Development\r\n"
                       "Web: https://www.ezlopi.com/\n"
                       "Project: https://github.com/ezloteam/Ezlo_Pi\n"
                       "Licence: EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT");
}

void MainWindow::on_actionExit_triggered() {
    QApplication::quit();
}

void MainWindow::ezlogic_table_adddev_digital_op(ezpi_device_digital_op_t output_device) {

    QTableWidgetItem * table_item_ezpi_devices;
    EZPI_UINT8 count_row =  ui->tableWidget_device_table->rowCount();
    ui->tableWidget_device_table->setRowCount(count_row + 1);

    table_item_ezpi_devices = new QTableWidgetItem(output_device.dev_name);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_NAME, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_DEV_TYPE(output_device.dev_type));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_ITEM_TYPE(output_device.id_item));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_ITEM_TYPE, table_item_ezpi_devices);

    EZPI_STRING GPIOs;
    GPIOs = QString::number(output_device.gpio_out);
    if(output_device.is_ip) {
        GPIOs += ", " + QString::number(output_device.gpio_in);
    }
    table_item_ezpi_devices = new QTableWidgetItem(GPIOs);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_GPIOS, table_item_ezpi_devices);
    ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_DIGITAL_OP);
}

void MainWindow::ezlogic_table_adddev_digital_ip(ezpi_device_digital_ip_t input_device) {

    QTableWidgetItem * table_item_ezpi_devices;
    EZPI_UINT8 count_row =  ui->tableWidget_device_table->rowCount();
    ui->tableWidget_device_table->setRowCount(count_row + 1);

    table_item_ezpi_devices = new QTableWidgetItem(input_device.dev_name);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_NAME, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_DEV_TYPE(input_device.dev_type));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_ITEM_TYPE(input_device.id_item));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_ITEM_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(QString::number(input_device.gpio));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_GPIOS, table_item_ezpi_devices);
    ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_DIGITAL_IP);

}

void MainWindow::ezlogic_table_adddev_analog_ip(ezpi_device_analog_ip_t adc_device) {

    QTableWidgetItem * table_item_ezpi_devices;
    EZPI_UINT8 count_row =  ui->tableWidget_device_table->rowCount();
    ui->tableWidget_device_table->setRowCount(count_row + 1);

    table_item_ezpi_devices = new QTableWidgetItem(adc_device.dev_name);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_NAME, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_DEV_TYPE(adc_device.dev_type));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_ITEM_TYPE(adc_device.id_item));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_ITEM_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(QString::number(adc_device.gpio));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_GPIOS, table_item_ezpi_devices);
    ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_ANALOG_IP);

}

void MainWindow::ezlogic_table_adddev_pwm(ezpi_device_pwm_t pwm_device) {

    QTableWidgetItem * table_item_ezpi_devices;
    EZPI_UINT8 count_row =  ui->tableWidget_device_table->rowCount();
    ui->tableWidget_device_table->setRowCount(count_row + 1);

    table_item_ezpi_devices = new QTableWidgetItem(pwm_device.dev_name);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_NAME, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_DEV_TYPE(pwm_device.dev_type));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_ITEM_TYPE(pwm_device.id_item));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_ITEM_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(QString::number(pwm_device.gpio));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_GPIOS, table_item_ezpi_devices);
    ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_PWM);

}

void MainWindow::ezlogic_table_adddev_uart(ezpi_device_uart_t uart_device) {

    QTableWidgetItem * table_item_ezpi_devices;
    EZPI_UINT8 count_row =  ui->tableWidget_device_table->rowCount();
    ui->tableWidget_device_table->setRowCount(count_row + 1);

    table_item_ezpi_devices = new QTableWidgetItem(uart_device.dev_name);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_NAME, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_DEV_TYPE(uart_device.dev_type));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_ITEM_TYPE(uart_device.id_item));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_ITEM_TYPE, table_item_ezpi_devices);

    EZPI_STRING GPIOs;
    GPIOs = "RX: " + QString::number(uart_device.gpio_rx) + ", TX: " + QString::number(uart_device.gpio_tx) + ", Baud:" + QString::number(uart_device.baud_rate);
    table_item_ezpi_devices = new QTableWidgetItem(GPIOs);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_GPIOS, table_item_ezpi_devices);
    ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_UART);

}

void MainWindow::ezlogic_table_adddev_onewire(ezpi_device_one_wire_t onewire_device) {

    QTableWidgetItem * table_item_ezpi_devices;
    EZPI_UINT8 count_row =  ui->tableWidget_device_table->rowCount();
    ui->tableWidget_device_table->setRowCount(count_row + 1);

    table_item_ezpi_devices = new QTableWidgetItem(onewire_device.dev_name);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_NAME, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_DEV_TYPE(onewire_device.dev_type));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_ITEM_TYPE(onewire_device.id_item));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_ITEM_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(QString::number(onewire_device.gpio));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_GPIOS, table_item_ezpi_devices);
    ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_ONE_WIRE);

}
void MainWindow::ezlogic_table_adddev_i2c(ezpi_device_I2C_t i2c_device) {
    QTableWidgetItem * table_item_ezpi_devices;
    EZPI_UINT8 count_row =  ui->tableWidget_device_table->rowCount();
    ui->tableWidget_device_table->setRowCount(count_row + 1);

    table_item_ezpi_devices = new QTableWidgetItem(i2c_device.dev_name);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_NAME, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_DEV_TYPE(i2c_device.dev_type));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_ITEM_TYPE(i2c_device.id_item));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_ITEM_TYPE, table_item_ezpi_devices);

    EZPI_STRING GPIOs;
    GPIOs = "SDA: " + QString::number(i2c_device.gpio_sda) + ", SCL: " + QString::number(i2c_device.gpio_scl);
    table_item_ezpi_devices = new QTableWidgetItem(GPIOs);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_GPIOS, table_item_ezpi_devices);
    ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_I2C);
}
void MainWindow::ezlogic_table_adddev_spi(ezpi_device_SPI_t spi_device) {

    QTableWidgetItem * table_item_ezpi_devices;
    EZPI_UINT8 count_row =  ui->tableWidget_device_table->rowCount();
    ui->tableWidget_device_table->setRowCount(count_row + 1);

    table_item_ezpi_devices = new QTableWidgetItem(spi_device.dev_name);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_NAME, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_DEV_TYPE(spi_device.dev_type));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_ITEM_TYPE(spi_device.id_item));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_ITEM_TYPE, table_item_ezpi_devices);

    EZPI_STRING GPIOs;
    GPIOs = "MOSI: " + QString::number(spi_device.gpio_mosi) + \
            ", MISO: " + QString::number(spi_device.gpio_miso) + \
            ", SCK: " + QString::number(spi_device.gpio_sck) + \
            ", CS: " + QString::number(spi_device.gpio_cs);
    table_item_ezpi_devices = new QTableWidgetItem(GPIOs);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_GPIOS, table_item_ezpi_devices);
    ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_SPI);
}

void MainWindow::ezlogic_table_adddev_other(ezpi_device_other_t other_device) {

    QTableWidgetItem * table_item_ezpi_devices;
    EZPI_UINT8 count_row =  ui->tableWidget_device_table->rowCount();
    ui->tableWidget_device_table->setRowCount(count_row + 1);

    table_item_ezpi_devices = new QTableWidgetItem(other_device.dev_name);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_NAME, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_DEV_TYPE(other_device.dev_type));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_DEV_TYPE, table_item_ezpi_devices);
    table_item_ezpi_devices = new QTableWidgetItem(EzloPi->EZPI_GET_ITEM_TYPE(other_device.id_item));
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_ITEM_TYPE, table_item_ezpi_devices);


    EZPI_STRING GPIOs = "";
    if(other_device.en_gpio1) GPIOs += "GPIO1: " + QString::number(other_device.gpio1);
    if(other_device.en_gpio2) GPIOs += " GPIO2: " + QString::number(other_device.gpio2);
    if(other_device.en_gpio3) GPIOs += " GPIO3: " + QString::number(other_device.gpio3);
    table_item_ezpi_devices = new QTableWidgetItem(GPIOs);
    ui->tableWidget_device_table->setItem(count_row, EZLOZIC_TABLE_COLUMN_GPIOS, table_item_ezpi_devices);
    ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_OTHER);

}

void MainWindow::ezlogic_serial_receive(void) {
    QByteArray serial_read_temp =  ezlogic_serial_port->readAll();
    *ezlogic_read_data_serial += serial_read_temp;
//    qDebug() << "Serial read on ready : " << QString::fromLocal8Bit(*ezpi_read_data_serial);
    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("dd.MM.yyyy hh:mm:ss");
//    QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();

//    qDebug() << "Date:"+formattedTime;
    if(ezlogic_log_level == EZPI_LOG_DEBUG) ui->textBrowser_console_log->append(formattedTime + ": " + QString::fromLocal8Bit(serial_read_temp));
//    qDebug() << formattedTime + ": " + QString::fromLocal8Bit(serial_read_temp);
    serial_read_temp.clear();
}

void MainWindow::ezlogic_serial_process(void) {

    ezlogic_timer_serial_complete.stop();

    int rx_size = ezlogic_read_data_serial->count();

    if(rx_size == 0) {
        ezlogic_flag_fimware_present = false;
        QMessageBox::warning(this, "Timeout!", "Serial receive timeout!");
        return;
    }

    int idx = 0;    
//    int found_start_bytes = 0;

    // while (idx != (rx_size - 8)) {
    //     if( (ezlogic_read_data_serial->at(idx) == '\200') &&
    //         (ezlogic_read_data_serial->at(idx+1) == '\r') &&
    //         (ezlogic_read_data_serial->at(idx+2) == '\n') &&
    //         (ezlogic_read_data_serial->at(idx+3) == '{') )
    //     {
    //         found_start_bytes = 1;
    //         ezlogic_read_data_serial->remove(0, idx+3);
    //         break;
    //     }

    //     idx++;
    // }

    // idx = 0;
    rx_size = ezlogic_read_data_serial->count();
    int opening_count = 0;
    int closing_count = 0;

    while (idx != rx_size) {
        if ('{' == ezlogic_read_data_serial->at(idx))
        {
            opening_count++;
        }

        if ('}' == ezlogic_read_data_serial->at(idx))
        {
            closing_count++;
        }

        if ( opening_count == closing_count)
        {
            ezlogic_read_data_serial->remove(idx+1, rx_size-idx-1);
            break;
        }
        idx++;
    }

    qDebug().noquote() << "\r\n\r\nOpening count: " << opening_count << "Closing count: " << closing_count;
    qDebug().noquote() << "\r\nFound start_byte: "<< found_start_bytes << "\r\n**************** Json data:\r\n" << QString::fromLocal8Bit(*ezlogic_read_data_serial);

//    if( 0 == found_start_bytes ) {
//        return;
//    }


    ui->textBrowser_console_log->append("Json Data: " + QString::fromLocal8Bit(*ezlogic_read_data_serial));


    switch (ezlogic_cmd_state) {
        case CMD_ACTION_RESET:
            ezlogic_action_restart(*ezlogic_read_data_serial);
            break;
        case CMD_ACTION_SET_WIFI:
            ezlogic_action_set_wifi(*ezlogic_read_data_serial);
            break;
        case CMD_ACTION_GET_INFO:
            ezlogic_action_check_info(*ezlogic_read_data_serial);
            if(ezlogic_flag_fimware_present == true) {
                ui->pushButton_set_wifi->setEnabled(true);
                ui->pushButton_add_device->setEnabled(true);

                ui->pushButton_get_ezpi_config->setEnabled(true);
                ui->pushButton_set_ezpi_config->setEnabled(true);

                ui->pushButton_erase_flash->setEnabled(true);
                ui->pushButton_flash_ezpi_bins->setEnabled(true);

                ui->pushButton_remove_device->setEnabled(false);

                ui->pushButton_clear_uart_direct_log->setEnabled(true);

                ui->tableWidget_device_table->setEnabled(true);

                ui->comboBox_registered_devices->setEnabled(true);

                ui->pushButton_device_restart->setEnabled(true);

                ui->comboBox_esp32_board_type->setEnabled(true);

                ezlogic_timer_ask_info.stop();
            }
            break;
        case CMD_ACTION_SET_CONFIG:
            ezlogic_action_set_config_process(*ezlogic_read_data_serial);
            break;
        case CMD_ACTION_GET_CONFIG:
            ezlogic_action_get_config_process(*ezlogic_read_data_serial);
            break;
        default:
            qDebug() << "Unknown CMD from UI !";
            ui->textBrowser_console_log->append("Unknown CMD from UI !");
            break;
    }
}

void MainWindow::ezlogic_serial_transfer(QByteArray d) {
    ezlogic_serial_port->flush();
    ezlogic_serial_port->write(d.constData());
    ezlogic_read_data_serial->clear();
    ezlogic_timer_serial_complete.start(EZPI_SERIAL_READ_TIMEOUT);
}

void MainWindow::ezlogic_action_check_info(QByteArray serial_read) {

    ezpi_info_t get_info_fmw_info;
    ezlogic_flag_fimware_present = false;
    QJsonParseError jsonError;
    QJsonDocument doc_get_info = QJsonDocument::fromJson(serial_read, &jsonError);

#if 1
    if (jsonError.error != QJsonParseError::NoError){
        qDebug() << jsonError.errorString();
        QMessageBox::warning(this, "Error!", "Incorrect data format received!");
        return;
    }
#endif

    QJsonObject obj_data_root_get_info = doc_get_info.object();
    QVariantMap json_map_root_get_info = obj_data_root_get_info.toVariantMap();

#if 1
    if(json_map_root_get_info["cmd"].toUInt() != CMD_ACTION_GET_INFO) {
        QMessageBox::warning(this, "Error!", "Invalid command received!");
        return;
    }
#endif
    if(json_map_root_get_info["status"].toUInt() == 1) {

//        get_info_fmw_info.v_sw = json_map_root_get_info["v_fmw"].toUInt();
        get_info_fmw_info.v_type = json_map_root_get_info["v_type"].toUInt();
        get_info_fmw_info.build = json_map_root_get_info["build"].toUInt();
        get_info_fmw_info.v_idf = json_map_root_get_info["v_idf"].toUInt();
        get_info_fmw_info.uptime = json_map_root_get_info["uptime"].toUInt();
        get_info_fmw_info.build_date = json_map_root_get_info["build_date"].toUInt();

        EzloPi->EZPI_SET_FMW_INFO(get_info_fmw_info);
        ezlogic_flag_fimware_present = true;
//        if(get_info_fmw_info.v_sw > 1)    ezlogic_flag_fimware_present = true;
//        else ezlogic_flag_fimware_present = false;
    }
}

void MainWindow::ezlogic_action_get_config_process(QByteArray serial_read) {

    QJsonParseError jsonError;
    QJsonDocument doc_get_config = QJsonDocument::fromJson(QString::fromLocal8Bit(serial_read).toUtf8(), &jsonError);

    if (jsonError.error != QJsonParseError::NoError){
        qDebug() << jsonError.errorString();
        QMessageBox::warning(this, "Error!", "Incorrect data format received!");
        return;
    }

    QJsonObject obj_data_root_get_config = doc_get_config.object();
    QVariantMap json_map_root_get_config = obj_data_root_get_config.toVariantMap();

    if(json_map_root_get_config["cmd"].toUInt() != CMD_ACTION_GET_CONFIG) {
        QMessageBox::warning(this, "Error!", "Incorrect command received!");
        return;
    }

    QVariantList list_get_config_device_detail = json_map_root_get_config["dev_detail"].toList();

    ezpi_device_digital_op_t device_digital_op;
    ezpi_device_digital_ip_t device_digital_ip;
    ezpi_device_analog_ip_t device_analog_ip;
    ezpi_device_pwm_t device_pwm;
    ezpi_device_uart_t device_uart;
    ezpi_device_one_wire_t device_onewire;
    ezpi_device_I2C_t device_i2c;
    ezpi_device_SPI_t device_spi;
    ezpi_device_other_t device_other;

    EZPI_UINT8 dev_count_get_config = 0;

    // Clear table contents
    ezlogic_clear_table_data();

    // Clear internal device storage
    EzloPi->EZPI_CLEAR_OUTPUT_DEVICES();
    EzloPi->EZPI_CLEAR_INPUT_DEVICES();
    EzloPi->EZPI_CLEAR_AINPUT_DEVICES();
    EzloPi->EZPI_CLEAR_PWM_DEVICES();
    EzloPi->EZPI_CLEAR_UART_DEVICES();
    EzloPi->EZPI_CLEAR_ONEWIRE_DEVICES();
    EzloPi->EZPI_CLEAR_I2C_DEVICES();
    EzloPi->EZPI_CLEAR_SPI_DEVICES();
    EzloPi->EZPI_CLEAR_OTHER_DEVICES();

    for(EZPI_UINT8 i = 0; i < list_get_config_device_detail.size(); i++) {

        dev_count_get_config++;

        QVariantMap get_config_device = list_get_config_device_detail[i].toMap();

        switch(get_config_device["dev_type"].toUInt()) {
            case EZPI_DEV_TYPE_DIGITAL_OP:
                device_digital_op.dev_name = get_config_device["dev_name"].toString();
                device_digital_op.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
                device_digital_op.id_room = get_config_device["id_room"].toString();
                device_digital_op.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
                device_digital_op.val_ip = get_config_device["val_ip"].toBool();
                device_digital_op.val_op = get_config_device["val_op"].toBool();
                device_digital_op.gpio_in = get_config_device["gpio_in"].toUInt();
                device_digital_op.gpio_out = get_config_device["gpio_out"].toUInt();
                device_digital_op.is_ip = get_config_device["is_ip"].toBool();
                device_digital_op.ip_inv = get_config_device["ip_inv"].toBool();
                device_digital_op.pullup_ip = get_config_device["pullup_ip"].toBool();
                device_digital_op.pullup_op = get_config_device["pullup_op"].toBool();
                device_digital_op.op_inv = get_config_device["op_inv"].toBool();

                EzloPi->EZPI_ADD_OUTPUT_DEVICE(device_digital_op);

                ezlogic_table_adddev_digital_op(device_digital_op);

                break;
            case EZPI_DEV_TYPE_DIGITAL_IP:
                device_digital_ip.dev_name = get_config_device["dev_name"].toString();
                device_digital_ip.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
                device_digital_ip.id_room = get_config_device["id_room"].toString();
                device_digital_ip.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
                device_digital_ip.val_ip = get_config_device["val_ip"].toBool();
                device_digital_ip.gpio = get_config_device["gpio"].toUInt();
                device_digital_ip.pull_up = get_config_device["pull_up"].toBool();
                device_digital_ip.logic_inv = get_config_device["logic_inv"].toBool();

                EzloPi->EZPI_ADD_INPUT_DEVICE(device_digital_ip);

                ezlogic_table_adddev_digital_ip(device_digital_ip);
                break;


        case EZPI_DEV_TYPE_ANALOG_IP:
            device_analog_ip.dev_name = get_config_device["dev_name"].toString();
            device_analog_ip.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
            device_analog_ip.id_room = get_config_device["id_room"].toString();
            device_analog_ip.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
            device_analog_ip.gpio = get_config_device["gpio"].toUInt();
            device_analog_ip.resln_bit = (ezpi_adc_resln)get_config_device["resln_bit"].toUInt();

            EzloPi->EZPI_ADD_AINPUT_DEVICE(device_analog_ip);

            ezlogic_table_adddev_analog_ip(device_analog_ip);
            break;

            case EZPI_DEV_TYPE_PWM:
                device_pwm.dev_name = get_config_device["dev_name"].toString();
                device_pwm.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
                device_pwm.id_room = get_config_device["id_room"].toString();
                device_pwm.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
                device_pwm.gpio = get_config_device["gpio"].toUInt();
                device_pwm.pwm_resln = (ezpi_pwm_resln)get_config_device["pwm_resln"].toUInt();
                device_pwm.duty_cycle = get_config_device["duty_cycle"].toUInt();
                device_pwm.freq_hz = get_config_device["freq_hz"].toUInt();

                EzloPi->EZPI_ADD_PWM_DEVICE(device_pwm);

                ezlogic_table_adddev_pwm(device_pwm);
                break;

            case EZPI_DEV_TYPE_UART:
                device_uart.dev_name = get_config_device["dev_name"].toString();
                device_uart.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
                device_uart.id_room = get_config_device["id_room"].toString();
                device_uart.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
                device_uart.gpio_rx = get_config_device["gpio_rx"].toUInt();
                device_uart.gpio_tx = get_config_device["gpio_tx"].toUInt();
                device_uart.baud_rate = get_config_device["baud_rate"].toUInt();

                EzloPi->EZPI_ADD_UART_DEVICE(device_uart);

                ezlogic_table_adddev_uart(device_uart);
                break;

            case EZPI_DEV_TYPE_ONE_WIRE:
                device_onewire.dev_name = get_config_device["dev_name"].toString();
                device_onewire.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
                device_onewire.id_room = get_config_device["id_room"].toString();
                device_onewire.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
                device_onewire.gpio = get_config_device["gpio"].toUInt();

                EzloPi->EZPI_ADD_ONEWIRE_DEVICE(device_onewire);

                ezlogic_table_adddev_onewire(device_onewire);
                break;

            case EZPI_DEV_TYPE_I2C:
                device_i2c.dev_name = get_config_device["dev_name"].toString();
                device_i2c.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
                device_i2c.id_room = get_config_device["id_room"].toString();
                device_i2c.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
                device_i2c.gpio_sda = get_config_device["gpio_sda"].toUInt();
                device_i2c.gpio_scl = get_config_device["gpio_scl"].toUInt();
                device_i2c.pullup_scl = get_config_device["pullup_scl"].toBool();
                device_i2c.pullup_sda = get_config_device["pullup_sda"].toBool();
                device_i2c.slave_addr = get_config_device["slave_addr"].toUInt();

                EzloPi->EZPI_ADD_I2C_DEVICE(device_i2c);

                ezlogic_table_adddev_i2c(device_i2c);
                break;

            case EZPI_DEV_TYPE_SPI:
                device_spi.dev_name = get_config_device["dev_name"].toString();
                device_spi.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
                device_spi.id_room = get_config_device["id_room"].toString();
                device_spi.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
                device_spi.gpio_miso = get_config_device["gpio_miso"].toUInt();
                device_spi.gpio_mosi = (ezpi_dev_type)get_config_device["gpio_mosi"].toUInt();
                device_spi.gpio_sck = get_config_device["gpio_sck"].toUInt();
                device_spi.gpio_cs = (ezpi_item_type)get_config_device["gpio_cs"].toUInt();

                EzloPi->EZPI_ADD_SPI_DEVICE(device_spi);

                ezlogic_table_adddev_spi(device_spi);
                break;

            case EZPI_DEV_TYPE_OTHER:
                device_other.dev_name = get_config_device["dev_name"].toString();
                device_other.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
                device_other.id_room = get_config_device["id_room"].toString();
                device_other.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
                device_other.en_gpio1 = get_config_device["en_gpio1"].toBool();
                device_other.gpio1 = get_config_device["gpio1"].toUInt();
                device_other.en_gpio2 = get_config_device["en_gpio2"].toBool();
                device_other.gpio2 = get_config_device["gpio2"].toUInt();
                device_other.en_gpio3 = get_config_device["en_gpio3"].toBool();
                device_other.gpio3 = get_config_device["gpio3"].toUInt();

                EzloPi->EZPI_ADD_OTHER_DEVICE(device_other);

                ezlogic_table_adddev_other(device_other);
                break;
            default:
                break;
        }
    }

    if(dev_count_get_config < 1) {
        QMessageBox::information(this, "No device!", "Device configurations not found !");
    } else {
        ui->pushButton_remove_device->setEnabled(true);
    }
}

void MainWindow::ezlogic_action_set_wifi(QByteArray wifi_response) {

    QString response_data = QString::fromLocal8Bit(wifi_response);

    QJsonParseError jsonError;
    QJsonDocument doc_set_wifi_response = QJsonDocument::fromJson(response_data.toUtf8(), &jsonError);

#if 0
    if (jsonError.error != QJsonParseError::NoError){
        qDebug() << jsonError.errorString();
        QMessageBox::warning(this, "Error!", "Incorrect data format received!");
        return;
    }
#endif

    QJsonObject obj_root_set_wifi_response = doc_set_wifi_response.object();
    QVariantMap json_map_root_set_wifi_response = obj_root_set_wifi_response.toVariantMap();

    if(json_map_root_set_wifi_response["cmd"].toUInt() == CMD_ACTION_SET_WIFI) {
        if((json_map_root_set_wifi_response["status_connect"].toUInt() == 1) &&
                (json_map_root_set_wifi_response["status_write"].toUInt() == 1)) {
            QMessageBox::information(this, "Success!", "WiFi write and connection successful.");
        } else if((json_map_root_set_wifi_response["status_connect"].toUInt() == 0) &&
                  (json_map_root_set_wifi_response["status_write"].toUInt() == 1)) {
            QMessageBox::warning(this, "Connection failed!", "WiFi connection failed, but write success !");
        } else {
            QMessageBox::warning(this, "Connection failed!", "WiFi connection failed.");
        }
    } else {
        QMessageBox::warning(this, "Error!", "Unknown command received, WiFi: unknown status!");
    }
}

void MainWindow::ezlogic_action_set_config_process(QByteArray serial_read) {
    QJsonParseError jsonError;
    QJsonDocument doc_set_config_response = QJsonDocument::fromJson(serial_read, &jsonError);

#if 0
    if (jsonError.error != QJsonParseError::NoError){
        qDebug() << jsonError.errorString();
        QMessageBox::warning(this, "Error!", "Incorrect data format received!");
        return;
    }
#endif

    QJsonObject obj_root_set_config_response = doc_set_config_response.object();
    QVariantMap json_map_root_set_config_response = obj_root_set_config_response.toVariantMap();

    if(json_map_root_set_config_response["cmd"].toUInt() == CMD_ACTION_SET_CONFIG) {
        if(json_map_root_set_config_response["status_write"].toUInt() == 1) {
            QMessageBox::information(this, "Success", "Writing configuration to ESP32 is successful!");
        } else {
            QMessageBox::warning(this, "Failed!", "Failed writing configuration to ESP device!");
        }
    } else {
        QMessageBox::warning(this, "Error!", "Unknown command received, writing configuration to ESP device: unknown status!");
    }
}

void MainWindow::ezlogic_action_restart(QByteArray reset_response) {


    QJsonParseError jsonError;
    QJsonDocument doc_reset_response = QJsonDocument::fromJson(reset_response, &jsonError);

#if 1
    if (jsonError.error != QJsonParseError::NoError){
        qDebug() << jsonError.errorString();
        QMessageBox::warning(this, "Error!", "Incorrect data format received!");
        return;
    }
#endif

    QJsonObject obj_root_reset_response = doc_reset_response.object();
    QVariantMap json_map_root_reset_response = obj_root_reset_response.toVariantMap();

    if(json_map_root_reset_response["cmd"].toUInt() == CMD_ACTION_RESET) {
        if(json_map_root_reset_response["status"].toUInt() == 1) {
            QMessageBox::information(this, "Success!", "Successfully issued restart command !");
        } else {
            QMessageBox::warning(this, "Restart failed!", "Restart attempt failed!");
        }
    } else {
        QMessageBox::warning(this, "Error!", "Unknown command received, Restart: unknown status!");
    }
}

void MainWindow::ezpi_show_status_message(const QString &message) {
    ezlogic_status->setText(message);
}

void MainWindow::ezpi_update_dev_list(void) {
    QDir directory("devs");
    QStringList registered_devices = directory.entryList(QStringList() << "*.bin",QDir::Files);
    if (registered_devices.size() == 0) {
        ui->comboBox_registered_devices->setEnabled(false);
    }

    foreach(QString filename, registered_devices) {
        qDebug() << "Registered devices: " << filename.remove(".bin");
        ui->comboBox_registered_devices->addItem(filename.remove(".bin"));
    }
}
