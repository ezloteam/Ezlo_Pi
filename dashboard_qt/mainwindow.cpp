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
#include<iostream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),    
    ezpi_flag_serial_port_open(false),
    ezpi_flag_is_ezlopi(false),
    ezpi_device_counter(0),
    ezpi_flag_enable_log(true),
    ezpi_flag_registered(false),
    ezpi_fimware_present(false)
{
    // Create and Init UI
    ui = new Ui::MainWindow;
    // Set goemetry of the UI
    setFixedSize(810, 628);
    ui->setupUi(this);

    ezpi_serial_port = new QSerialPort;
    ezpi_serial_port->setBaudRate(115200);

    EzloPi = new EzPi();
    EzloPi->EZPI_SET_BOARD_TYPE(EZPI_BOARD_TYPE_ESP32_GENERIC);
    EzloPi->EZPI_INIT_BOARD();

    ezpi_form_login = new login(this);
    ezpi_form_WiFi = new Dialog_WiFi(this, ezpi_serial_port);
    ezpi_form_devadd = new Dialog_devadd(this);

    connect(ezpi_form_devadd, SIGNAL(ezpi_send_dev_type_selected(EZPI_UINT8)), this, SLOT(ezpi_receive_dev_type_selected(EZPI_UINT8)));

    connect(ezpi_serial_port, &QSerialPort::readyRead, this, &MainWindow::ezpi_serial_receive);

    connect(ezpi_form_WiFi, SIGNAL(ezpi_signal_serial_rx_wifi(ezpi_cmd)), this, SLOT(ezpi_serial_receive_wif(ezpi_cmd)));

    user_token.clear();

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

    ui->comboBox_esp32_board_type->setEnabled(false);

    connect(ui->comboBox_uart_list,SIGNAL(currentIndexChanged(const QString&)),
            this,SLOT(on_comboBox_uart_list_currentIndexChanged(const QString&)));

    // Timer initialization
    ezpi_timer_ask_info.callOnTimeout(this, &MainWindow::ezpi_message_info_no_firmware_detected);
    ezpi_timer_serial_complete.callOnTimeout(this, &MainWindow::ezpi_serial_process);

    ezpi_read_data_serial = new QByteArray;


}

MainWindow::~MainWindow() {
    ezpi_serial_port->close();
    delete ezpi_read_data_serial;
    delete ui;    
}

// UI generated slots
void MainWindow::on_pushButton_connect_uart_clicked() {

    ezpi_serial_port->setPort(ezpi_serial_port_info);

    if(ezpi_flag_serial_port_open == false) {
        if(!ezpi_serial_port_info.isBusy()) {

            if(ezpi_serial_port->open(QIODevice::ReadWrite)) {
                ezpi_flag_serial_port_open = true;

                //Modify UI elements:
                ui->pushButton_connect_uart->setText("Close");
                ui->pushButton_scan_uart_ports->setDisabled(true);
                ui->comboBox_uart_list->setDisabled(true);

                ui->actionRegister->setEnabled(true);

                // Display message on console
                qDebug() << ezpi_serial_port_info.portName() << " serial port is open.";
                if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(ezpi_serial_port_info.portName() + " serial port is open.");

                // Check firmware
                if(ezpi_check_firmware()) {
                    ui->pushButton_set_wifi->setEnabled(true);
                    ui->pushButton_add_device->setEnabled(true);

                    ui->pushButton_get_ezpi_config->setEnabled(true);
                    ui->pushButton_set_ezpi_config->setEnabled(true);

                    ui->pushButton_erase_flash->setEnabled(true);
                    ui->pushButton_flash_ezpi_bins->setEnabled(true);

                    ui->pushButton_remove_device->setEnabled(true);

                    ui->pushButton_clear_uart_direct_log->setEnabled(true);

                    ui->tableWidget_device_table->setEnabled(true);

                    ezpi_timer_ask_info.stop();

                } else {
                    ezpi_timer_ask_info.start(5000);
                }

            } else {
                qDebug() << "Failed opeaning serial port: " << ui->comboBox_uart_list->currentText();
                if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("Failed opeaning serial port: " + ui->comboBox_uart_list->currentText());
            }
        } else {
            qDebug() << ezpi_serial_port_info.portName() <<  ": port is busy !";
            if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(ezpi_serial_port_info.portName() + QString::fromLocal8Bit(": port is busy !"));
        }
    } else {
        ezpi_serial_port->close();
        ui->tableWidget_device_table->clearContents();
        ezpi_flag_serial_port_open = false;

        ui->pushButton_connect_uart->setText("Open");
        ui->pushButton_scan_uart_ports->setDisabled(false);
        ui->comboBox_uart_list->setDisabled(false);

        ui->actionRegister->setEnabled(false);

        qDebug() << ezpi_serial_port_info.portName() << " serial port is close.";
        if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(ezpi_serial_port_info.portName() + " serial port is close.");

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
    }
}
void MainWindow::on_comboBox_uart_list_currentIndexChanged() {

    for(QSerialPortInfo info : ezpi_serial_ports_info_list) {
        if(ui->comboBox_uart_list->currentText() == info.portName()) {
            ezpi_serial_port_info = info;
        }
    }
    qDebug() << "New selected port: " << ezpi_serial_port_info.portName();
    if(ezpi_flag_enable_log) {
        ui->textBrowser_console_log->append("New selected port: " + ezpi_serial_port_info.portName() + "\n");
    }
}
void MainWindow::on_pushButton_set_wifi_clicked() {
    ezpi_form_WiFi->setFixedSize(315, 165);
    ezpi_form_WiFi->setModal(true);
    ezpi_form_WiFi->show();
}
void MainWindow::on_pushButton_erase_flash_clicked() {

    ezpi_process_erase_flash = new QProcess(this);

    connect(ezpi_process_erase_flash, &QProcess::readyReadStandardOutput, this, &MainWindow::ezpi_log_erase_flash);
    connect(ezpi_process_erase_flash, &QProcess::readyReadStandardError, this, &MainWindow::ezpi_log_erase_flash);

    ezpi_serial_port->close();
    ui->tableWidget_device_table->clearContents();
    ui->pushButton_connect_uart->setEnabled(false);


    #ifdef __linux__
            QString ser_port = "/dev/" + ezpi_serial_port_info.portName();
    #elif _WIN32
        QString ser_port = ezpi_serial_port_info.portName();
    #else

    #endif

    ezpi_process_erase_flash->setProgram("esptool/esptool.exe");
    QStringList arguments;
    arguments.append("-p");
    arguments.append(ser_port);
    arguments.append("erase_flash");

    QString command = "";
    for(auto args : arguments) {
        command += args;
    }

    qDebug() << command;

    ezpi_process_erase_flash->setArguments(arguments);
    ezpi_process_erase_flash->start();
}

void MainWindow::on_pushButton_flash_ezpi_bins_clicked() {

    ui->textBrowser_console_log->clear();

    ezpi_process_write_flash = new QProcess(this);

    connect(ezpi_process_write_flash, &QProcess::readyReadStandardOutput, this, &MainWindow::ezpi_log_write_flash);
    connect(ezpi_process_write_flash, &QProcess::readyReadStandardError, this, &MainWindow::ezpi_log_write_flash);

    #ifdef __linux__
            QString ser_port = "/dev/" + ezpi_serial_port_info.portName();
    #elif _WIN32
        QString ser_port = ezpi_serial_port_info.portName();
    #else

    #endif

    ezpi_serial_port->close();
    ui->tableWidget_device_table->clearContents();
    ui->pushButton_connect_uart->setEnabled(false);

    ezpi_process_write_flash->setProgram("esptool/esptool.exe");

    QStringList arguments;
    arguments.append("-p");
    arguments.append(ser_port);
    arguments.append("-b 460800");
//    arguments.append("--before default_reset");
//    arguments.append("--after hard_reset");
//    arguments.append("--chip esp32");
//    arguments.append("--flash_mode dio");
//    arguments.append("--flash_size detect");
    arguments.append("write_flash");
//    arguments.append("write_flash");
//    arguments.append("--flash_freq 40m");
    arguments.append("0x1000 ezpibins/bootloader.bin");
    arguments.append("0x8000 ezpibins/partition-table.bin");
    arguments.append("0x10000 ezpibins/esp_configs.bin");
    arguments.append("0x2E0000 ezpibins/ld.bin");

    QString argument_string = "-p " + ser_port + " -b 460800 " + \
                                " --before default_reset " + \
                                "--after hard_reset " + \
                                "--chip esp32 " + \
                                "write_flash " + \
                                "--flash_mode dio " + \
                                "--flash_size detect " + \
                                "--flash_freq 40m " + \
                                "0x1000 ezpibins/bootloader.bin " + \
                                "0x8000 ezpibins/partition-table.bin " + \
                                "0xD000 ezpibins/ota_data_initial.bin " + \
                                "0x10000 ezpibins/esp_configs.bin " + \
                                "0x3B0000 ezpibins/ld.bin";


    qDebug() << "Current dir : " << QDir::currentPath();
    ui->textBrowser_console_log->append("Current dir: " + QDir::currentPath());

    qDebug() << "Arguments : " << argument_string;

    ezpi_process_write_flash->setNativeArguments(argument_string);
    ezpi_process_write_flash->start();
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
            qDebug() << "New Selected board : ESP32 Generic.";
            break;
        case EZPI_BOARD_TYPE_ESP32_C3:
            EzloPi->EZPI_SET_BOARD_TYPE(EZPI_BOARD_TYPE_ESP32_C3);
            qDebug() << "New Selected board : ESP32 C3.";
            break;
         case EZPI_BOARD_TYPE_ESP32_S3:
            EzloPi->EZPI_SET_BOARD_TYPE(EZPI_BOARD_TYPE_ESP32_S3);
            qDebug() << "New Selected board : ESP32 S3.";
            break;
        default:
            break;
    }
}

void MainWindow::on_pushButton_add_device_clicked() {

    if (ezpi_device_counter >= EZPI_MAX_DEVICES) {
        QMessageBox::information(this,"Device full","Devices reaches top limit, no more devices can be added");
        return;
    }
    ezpi_form_devadd->setFixedSize(250, 120);
    ezpi_form_devadd->setModal(true);
    ezpi_form_devadd->show();
}

void MainWindow::on_pushButton_remove_device_clicked() {

    QList<QTableWidgetItem*> ezlogic_table_selected_item = ui->tableWidget_device_table->selectedItems();

    EZPI_UINT8 ezlogic_table_selected_row = ezlogic_table_selected_item[0]->row();

//    if(ezlogic_table_selected_row < ui->tableWidget_device_table->rowCount()) {
//        ui->pushButton_remove_device->setEnabled("false");
//    } else {

//    }

    qDebug() << "Selected row: " << ezlogic_table_selected_row;
    ui->tableWidget_device_table->removeRow(ezlogic_table_selected_row);

    switch(ezlogic_table_row_device_map.at(ezlogic_table_selected_row)) {

        case EZPI_DEV_TYPE_DIGITAL_OP:
            EzloPi->EZPI_DELETE_OUTPUT_DEVICE();
            break;
        case EZPI_DEV_TYPE_DIGITAL_IP:
            EzloPi->EZPI_DELETE_INPUT_DEVICE();
            break;
        case EZPI_DEV_TYPE_ANALOG_IP:
            EzloPi->EZPI_DELETE_AINPUT_DEVICE();
            break;
        case EZPI_DEV_TYPE_ONE_WIRE:
            EzloPi->EZPI_DELETE_ONEWIRE_DEVICE();
            break;
        case EZPI_DEV_TYPE_I2C:
            EzloPi->EZPI_DELETE_I2C_DEVICE();
            break;
        case EZPI_DEV_TYPE_SPI:
            EzloPi->EZPI_DELETE_SPI_DEVICE();
            break;
        default:
            break;
    }
}

void MainWindow::on_pushButton_get_ezpi_config_clicked() {

#if 0
    QString test_json = "{\"cmd\":4,\"dev_detail\":[{\"dev_name\":\"Digital Out 1\",\"dev_type\":1,\"gpio_in\":4,"
                        "\"gpio_out\":2,\"id_item\":1,\"id_room\":0,\"ip_inv\":false,\"is_ip\":true,\"op_inv\":false,"
                        "\"pullup_ip\":true,\"pullup_op\":true,\"val_ip\":false,\"val_op\":false},{\"dev_name\":\"Digital In 1"
                        "\",\"dev_type\":2,\"gpio\":5,\"id_item\":4,\"id_room\":0,\"logic_inv\":false,\"pull_up\":true,\"val_ip"
                        "\":false},{\"dev_name\":\"Onewire 1\",\"dev_type\":7,\"gpio\":13,\"id_item\":15,\"id_room\":0,\"pull_up"
                        "\":true,\"val_ip\":false},{\"dev_name\":\"I2C 1\",\"gpio_scl\":15,\"gpio_sda\":14,\"id_item\":5,\"id_room"
                        "\":0,\"pullup_scl\":false,\"pullup_sda\":false,\"slave_addr\":0},{\"dev_name\":\"SPI 1\",\"dev_type\":9,"
                        "\"gpio_cs\":19,\"gpio_miso\":17,\"gpio_mosi\":16,\"gpio_sck\":18,\"id_item\":13,\"id_room\":0}],\"dev_total\":5}";
#endif

    QString response_data;

    QString json_send_get_config = "{\"cmd\":4}";

    ezpi_cmd_state = CMD_ACTION_GET_CONFIG;
    ezpi_serial_transfer(json_send_get_config.toLocal8Bit());

#if 0

    ezpi_serial_port->write(json_send_get_config);

    if (ezpi_serial_port->waitForBytesWritten()) {
        // read response
        if (ezpi_serial_port->waitForReadyRead(5000)) {
            QByteArray responseData = ezpi_serial_port->readAll();
            while (ezpi_serial_port->waitForReadyRead(10))
                responseData += ezpi_serial_port->readAll();

            const EZPI_STRING response = QString::fromUtf8(responseData);
            qDebug() << "Response : " << response;
            response_data = response;
        } else {
            QMessageBox::warning(this, "Request time out!", "No response from the device !\n Connection status unknown !");
        }
    } else {

    }
#endif

}

void MainWindow::on_pushButton_set_ezpi_config_clicked() {

    QJsonObject object_root_set_device;
    QJsonDocument document_root_set_device;
    QJsonArray array_device_detail;

    std::vector <ezlogic_device_digital_op_t> device_digital_op = EzloPi->EZPI_GET_OUTPUT_DEVICES();
    std::vector <ezlogic_device_digital_ip_t> device_digital_ip = EzloPi->EZPI_GET_INPUT_DEVICES();
    std::vector <ezlogic_device_analog_ip_t> device_analog_ip = EzloPi->EZPI_GET_AINPUT_DEVICES();
    std::vector <ezlogic_device_one_wire_t> device_onewire = EzloPi->EZPI_GET_ONEWIRE_DEVICES();
    std::vector <ezlogic_device_I2C_t> device_i2c = EzloPi->EZPI_GET_I2C_DEVICES();
    std::vector <ezlogic_device_SPI_t> device_spi = EzloPi->EZPI_GET_SPI_DEVICES();

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

    for(EZPI_UINT8 i = 0; i < device_onewire.size(); i++) {
        QJsonObject object_device_onewire;

        object_device_onewire.insert("dev_type", device_onewire[i].dev_type);
        object_device_onewire.insert("dev_name", device_onewire[i].dev_name);
        object_device_onewire.insert("id_room", device_onewire[i].id_room);
        object_device_onewire.insert("id_item", device_onewire[i].id_item);
        object_device_onewire.insert("val_ip", device_onewire[i].val_ip);
        object_device_onewire.insert("pull_up", device_onewire[i].pull_up);
        object_device_onewire.insert("gpio", device_onewire[i].gpio);

        array_device_detail.push_back(object_device_onewire);
    }

    for(EZPI_UINT8 i = 0; i < device_i2c.size(); i++) {
        QJsonObject object_device_i2c;

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

    object_root_set_device.insert("dev_detail", array_device_detail);
    document_root_set_device.setObject(object_root_set_device);

    ezpi_cmd_state = CMD_ACTION_SET_CONFIG;
    ezpi_serial_transfer(document_root_set_device.toJson(QJsonDocument::Compact));
}

// Custom Slots
void MainWindow::ezpi_log_write_flash() {

    QMessageBox ezpi_message_box_failed_erase;
    QByteArray byteArray = ezpi_process_write_flash->readAllStandardOutput();
    QString str_StandardOutput = QString::fromLocal8Bit(byteArray);
//    qDebug() << str_StandardOutput;

    ui->textBrowser_console_log->append(str_StandardOutput);

    byteArray = ezpi_process_write_flash->readAllStandardError();
    QString str_StandardError = QString::fromLocal8Bit(byteArray);
//    qDebug() << str_StandardError;

    ui->textBrowser_console_log->append(str_StandardError);

    if(str_StandardOutput.contains("error") || str_StandardError.contains("error")) {
        ezpi_message_box_failed_erase.information(this, "Failed flashing the device!", "Flashing ezlopi firmware to the device was not successful, close the app, disconnect device, reconnect and try again!");
        ezpi_serial_port->open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    } else if(str_StandardError.contains("Hard resetting") || str_StandardOutput.contains("Hard resetting")) {
        ezpi_message_box_failed_erase.information(this, "Success flashing the device!", "Flashing ezlopi firmware to the device was successful! You can now gracefully disconnect the device and move further!");
        ezpi_serial_port->open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    }
}

void MainWindow::ezpi_log_erase_flash() {

    QMessageBox ezpi_message_box_failed_erase;
    QByteArray byteArray = ezpi_process_erase_flash->readAllStandardOutput();
    QString str_StandardOutput = QString::fromLocal8Bit(byteArray);
//    qDebug() << str_StandardOutput;

    ui->textBrowser_console_log->append(str_StandardOutput);

    byteArray = ezpi_process_erase_flash->readAllStandardError();
    QString str_StandardError = QString::fromLocal8Bit(byteArray);
//    qDebug() << str_StandardError;

    ui->textBrowser_console_log->append(str_StandardError);

    if(str_StandardOutput.contains("error") || str_StandardError.contains("error")) {
        ezpi_message_box_failed_erase.information(this, "Failed erasing the device!", "Erasing the flas of the device was not successful, close the app, disconnect device, reconnect and try again!");
        ezpi_serial_port->open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    } else if(str_StandardOutput.contains("Hard resetting") || str_StandardError.contains("Hard resetting")) {
        ezpi_message_box_failed_erase.information(this, "Success erasing the device!", "Erasing the device was successful! You can now proceed forward flashing the ezlopi firmware!");
        ezpi_serial_port->open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    }
}

EZPI_BOOL MainWindow::ezpi_check_firmware() {

#if 0
    QString test_json = "{\"cmd\":1,\"status\":1,\"v_sw\":3625,\"v_type\":1,\"build\":17,\"v_idf\":8456,\"uptime"
                        "\":4856,\"build_date\":1657623331,\"boot_count\":15,\"boot_reason\":2,\"mac\":1577079727,"
                        "\"uuid\":\"65261d76-e584-4d35-aff1-d84bd043\",\"serial\":100004032,\"ssid\":\"ssid\",\"dev_type\":1,"
                        "\"dev_flash\":64256,\"dev_free_flash\":300,\"dev_name\":\"My Device\"}";
#endif

    QString json_send_get_info = "{\"cmd\":1}";

    ezpi_cmd_state = CMD_ACTION_GET_INFO;
    ezpi_serial_transfer(json_send_get_info.toUtf8());

    return ezpi_fimware_present;
}

void MainWindow::ezpi_message_info_no_firmware_detected() {
    QMessageBox ezpi_messagebox_ezpi_firmware_not_detected;
    ezpi_messagebox_ezpi_firmware_not_detected.information(this, "No firmware detected!", "No ezlo pi firmware has been detected. Please flash firmware and try again!");
    ezpi_timer_ask_info.stop();

    // Enable flash and erase buttons
    ui->pushButton_erase_flash->setEnabled(true);
    ui->pushButton_flash_ezpi_bins->setEnabled(true);
    ui->pushButton_clear_uart_direct_log->setEnabled(true);

    ui->tableWidget_device_table->clearContents();
    ui->tableWidget_device_table->setEnabled(false);
}

void MainWindow::ezpi_receive_dev_type_selected(EZPI_UINT8 dev_type_index) {

    qDebug() << "Device type Index: " << dev_type_index;

    ezpi_form_configdev_digitalio = new Dialog_configdev_digitalio(this, EzloPi);
    ezpi_form_config_digital_ip = new Dialog_config_input(this, EzloPi);
    ezpi_form_config_analog_ip = new Dialog_adc(this, EzloPi);
    ezpi_form_config_onewire = new Dialog_config_onewire(this, EzloPi);
    ezpi_form_config_i2c = new Dialog_config_i2c(this, EzloPi);
    ezpi_form_config_spi = new Dialog_config_spi(this, EzloPi);

    connect(ezpi_form_configdev_digitalio, SIGNAL(ezpi_signal_dev_op_added(ezpi_dev_type)), this, SLOT(ezpi_receive_added_dev(ezpi_dev_type)));
    connect(ezpi_form_config_digital_ip, SIGNAL(ezpi_signal_dev_ip_added(ezpi_dev_type)), this, SLOT(ezpi_receive_added_dev(ezpi_dev_type)));
    connect(ezpi_form_config_analog_ip, SIGNAL(ezpi_signal_dev_adc_added(ezpi_dev_type)), this, SLOT(ezpi_receive_added_dev(ezpi_dev_type)));
    connect(ezpi_form_config_onewire, SIGNAL(ezpi_signal_dev_onewire_added(ezpi_dev_type)), this, SLOT(ezpi_receive_added_dev(ezpi_dev_type)));
    connect(ezpi_form_config_i2c, SIGNAL(ezpi_signal_dev_i2c_added(ezpi_dev_type)), this, SLOT(ezpi_receive_added_dev(ezpi_dev_type)));
    connect(ezpi_form_config_spi, SIGNAL(ezpi_signal_dev_spi_added(ezpi_dev_type)), this, SLOT(ezpi_receive_added_dev(ezpi_dev_type)));

    switch(dev_type_index) {
        case EZPI_DEV_TYPE_DIGITAL_OP: {
            ezpi_form_configdev_digitalio->setFixedSize(275, 380);
            ezpi_form_configdev_digitalio->setModal(true);
            ezpi_form_configdev_digitalio->show();
            break;
        }
        case EZPI_DEV_TYPE_DIGITAL_IP: {
            ezpi_form_config_digital_ip->setFixedSize(180, 310);
            ezpi_form_config_digital_ip->setModal(true);
            ezpi_form_config_digital_ip->show();
            break;
        }
        case EZPI_DEV_TYPE_ANALOG_IP: {
            ezpi_form_config_analog_ip->setFixedSize(170, 265);
            ezpi_form_config_analog_ip->setModal(true);
            ezpi_form_config_analog_ip->show();
           break;
        }
        case EZPI_DEV_TYPE_ANALOG_OP: {
            QMessageBox::information(this, "Unsupported", "This feature is not supported yet!");
            break;
        }
        case EZPI_DEV_TYPE_PWM: {
            QMessageBox::information(this, "Unsupported", "This feature is not supported yet!");
            break;
        }
        case EZPI_DEV_TYPE_UART: {
            QMessageBox::information(this, "Unsupported", "This feature is not supported yet!");
            break;
        }
        case EZPI_DEV_TYPE_ONE_WIRE: {
            ezpi_form_config_onewire->setFixedSize(335, 230);
            ezpi_form_config_onewire->setModal(true);
            ezpi_form_config_onewire->show();
            break;
        }
        case EZPI_DEV_TYPE_I2C: {
            ezpi_form_config_i2c->setFixedSize(325, 240);
            ezpi_form_config_i2c->setModal(true);
            ezpi_form_config_i2c->show();
            break;
        }
        case EZPI_DEV_TYPE_SPI: {
            ezpi_form_config_spi->setFixedHeight(296);
            ezpi_form_config_spi->setFixedWidth(190);
            ezpi_form_config_spi->setModal(true);
            ezpi_form_config_spi->show();
            break;
        }

        default: {
            QMessageBox::information(this, "Unknown", "Unknown type.");
        }
    }
}

void MainWindow::ezpi_receive_added_dev(ezpi_dev_type ezpi_added_dev_type) {

    std::vector <ezlogic_device_digital_ip_t> input_devices;
    std::vector <ezlogic_device_one_wire_t> onewire_devices;
    std::vector <ezlogic_device_I2C_t> i2c_devices;
    std::vector <ezlogic_device_SPI_t> spi_devices;

    qDebug() << "Added device type: " << QString::number(ezpi_added_dev_type);


    switch(ezpi_added_dev_type) {
        case EZPI_DEV_TYPE_DIGITAL_OP: {

            std::vector <ezlogic_device_digital_op_t> output_devices = EzloPi->EZPI_GET_OUTPUT_DEVICES();
            EZPI_UINT8 output_devices_total = (EZPI_UINT8)output_devices.size();
            ezlogic_device_digital_op_t output_device = output_devices[output_devices_total - 1];
            ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_DIGITAL_OP);
            ezlogic_table_adddev_digital_op(output_device);
            break;
        }
        case EZPI_DEV_TYPE_DIGITAL_IP: {

            std::vector <ezlogic_device_digital_ip_t> input_devices = EzloPi->EZPI_GET_INPUT_DEVICES();
            EZPI_UINT8 input_devices_total = (EZPI_UINT8)input_devices.size();
            ezlogic_device_digital_ip_t input_device = input_devices[input_devices_total - 1];
            ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_DIGITAL_IP);
            ezlogic_table_adddev_digital_ip(input_device);
            break;
        }

        case EZPI_DEV_TYPE_ANALOG_IP: {

            std::vector <ezlogic_device_analog_ip_t> adc_devices = EzloPi->EZPI_GET_AINPUT_DEVICES();
            EZPI_UINT8 adc_devices_total = (EZPI_UINT8)adc_devices.size();
            ezlogic_device_analog_ip_t adc_device = adc_devices[adc_devices_total - 1];
            ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_ANALOG_IP);
            ezlogic_table_adddev_analog_ip(adc_device);
            break;
        }

        case EZPI_DEV_TYPE_ONE_WIRE: {
            std::vector <ezlogic_device_one_wire_t> onewire_devices = EzloPi->EZPI_GET_ONEWIRE_DEVICES();
            EZPI_UINT8 onewire_devices_total = (EZPI_UINT8)onewire_devices.size();
            ezlogic_device_one_wire_t onewire_device = onewire_devices[onewire_devices_total - 1];
            ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_ONE_WIRE);
            ezlogic_table_adddev_onewire(onewire_device);
            break;
        }
        case EZPI_DEV_TYPE_I2C: {
            std::vector <ezlogic_device_I2C_t> i2c_devices = EzloPi->EZPI_GET_I2C_DEVICES();
            EZPI_UINT8 i2c_devices_total = (EZPI_UINT8)i2c_devices.size();
            ezlogic_device_I2C_t i2c_device = i2c_devices[i2c_devices_total - 1];
            ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_I2C);
            ezlogic_table_adddev_i2c(i2c_device);
            break;
        }
        case EZPI_DEV_TYPE_SPI: {
            std::vector <ezlogic_device_SPI_t> spi_devices = EzloPi->EZPI_GET_SPI_DEVICES();
            EZPI_UINT8 spi_devices_total = (EZPI_UINT8)spi_devices.size();
            ezlogic_device_SPI_t spi_device = spi_devices[spi_devices_total - 1];
            ezlogic_table_row_device_map.push_back(EZPI_DEV_TYPE_I2C);
            ezlogic_table_adddev_spi(spi_device);
            break;
        }
        default:
            break;
    }
}

void MainWindow::ezpi_serial_receive_wif(ezpi_cmd cmd) {
    ezpi_cmd_state = cmd;
    ezpi_timer_serial_complete.start(EZPI_SERIAL_READ_TIMEOUT);
}

// UI generated SLOTS

void MainWindow::on_pushButton_scan_uart_ports_clicked() {

    QMessageBox messageBoxNoUart;
    QSerialPortInfo ports;

    ui->comboBox_uart_list->clear(); // Clear the existing list

    ezpi_serial_ports_info_list.clear(); // Clear the existing list of serial port info

    if(ports.availablePorts().size() <= 0) {
        messageBoxNoUart.information(this, "No device found!", "We did not find any device connected, please check your connection and try again.");
//         qDebug() << "No device found";
    } else {
        qDebug() << "Available UART Ports";
        if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("Available UART Ports:\n");

        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            qDebug().noquote() << info.portName() << info.description() << info.manufacturer();
            if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(info.portName() + " " + info.description() + " " + info.manufacturer() + "\n");

            if(info.description() != "") {
                ui->comboBox_uart_list->addItem(info.portName());
                ezpi_serial_ports_info_list.push_back(info);
            }
        }

        for(QSerialPortInfo info : ezpi_serial_ports_info_list) {
            if(ui->comboBox_uart_list->currentText() == info.portName()) {
                ezpi_serial_port_info = info;
            }
        }
        ui->pushButton_connect_uart-> setEnabled(true);
    }
}
void MainWindow::ezpi_success_prov_dat(QNetworkReply *d) {

    qDebug() << "Added new device";
    ui->textBrowser_console_log->append("Added new device!");
    QByteArray response_bytes = d->readAll();

    QJsonParseError jerror;
    QJsonDocument jdoc= QJsonDocument::fromJson(response_bytes, &jerror);

    if(jerror.error != QJsonParseError::NoError) {

        qDebug() << "Message failed parcing json";
        ui->textBrowser_console_log->append("Error: Failed parcing json");
        return;
    }

    QJsonObject jobj_main = jdoc.object();

     uint8_t status = jobj_main["status"].toInt();
     uint8_t complete = jobj_main["complete"].toInt();

    if( (status == 1) && (complete == 1) ) {

        ui->textBrowser_console_log->append("Success: New device has been registered, you can login and find the device added.");

        QJsonObject jobj_data = jobj_main["data"].toObject();
        QString uuid = jobj_data["uuid"].toString();

        QJsonObject jobj_get_prov_json;
        QJsonObject jobj_param;

        jobj_param["uuid"] = uuid;
        jobj_get_prov_json["params"] = jobj_param;
        jobj_get_prov_json["call"] = QString("self_provision_get");

        QJsonDocument jdoc_get_prov_json(jobj_get_prov_json);

        QByteArray get_prov_json = jdoc_get_prov_json.toJson(QJsonDocument::Compact);


        // Make POST request
        QUrl url("https://api-cloud.ezlo.com/v1/request");
        QNetworkRequest request(url);
        request.setRawHeader("Authorization", "Bearer " + user_token.toLocal8Bit());

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);

        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(success_get_prov_jsons(QNetworkReply*)));

        // FIXME for debug
        qDebug() << "Sync" << QString::fromUtf8(get_prov_json.data(), get_prov_json.size());

        manager->post(request, get_prov_json);
    } else {
        QMessageBox::warning(this, "Communication failed", "Registration process aborted, try again closing and reopeaning the app!");
        return;
    }
}

void MainWindow::ezpi_success_get_prov_jsons(QNetworkReply *d) {

#if 1
     conv_u16_array_t ser_ver;
     conv_64_array_t id;

    struct uuid _uuid;

    QByteArray response_bytes = d->readAll();
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
        qDebug() << "controller_uuid: " << jobj_prov_data_data["controller_uuid"].toString();

        ui->textBrowser_console_log->append( QString::fromStdString("provision_server: ") + jobj_prov_data_prov_data["provision_server"].toString());
        ui->textBrowser_console_log->append( QString::fromStdString("cloud_server: ") + jobj_prov_data_prov_data["cloud_server"].toString());
        ui->textBrowser_console_log->append( QString::fromStdString("default_wifi_ssid: ") + jobj_prov_data_prov_data["default_wifi_ssid"].toString());
        ui->textBrowser_console_log->append( QString::fromStdString("default_wifi_password: ") + jobj_prov_data_prov_data["default_wifi_password"].toString());
        ui->textBrowser_console_log->append( QString::fromStdString("controller_uuid: ") + jobj_prov_data_data["controller_uuid"].toString());
        ui->textBrowser_console_log->append( QString::fromStdString("id or Serial: ") + QString::number(jobj_prov_data_prov_data["id"].toInt()));

        ser_ver.data = 0;
        // For Connection-ID-0
        ld_binary_array.insert(0, QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Serial
        ser_ver.data = 100;
        ld_binary_array.insert(2, QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Version
        ld_binary_array.insert(4, jobj_prov_data_prov_data["provision_server"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(0x104, jobj_prov_data_prov_data["provision_token"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(0x204, jobj_prov_data_prov_data["cloud_server"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(0x1000, jobj_prov_data_prov_data["signing_ca_certificate"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(0x3000, jobj_prov_data_prov_data["ssl_private_key"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(0x4000, jobj_prov_data_prov_data["ssl_shared_key"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

        // For Connection ID 1
        ser_ver.data = 1;
        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1, QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));          // Serial
        ser_ver.data = 100;
        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + 2, QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));      // Version
        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + 4, jobj_prov_data_prov_data["provision_server"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + 260, jobj_prov_data_prov_data["provision_token"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + 516, jobj_prov_data_prov_data["cloud_server"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + 0x1000, jobj_prov_data_prov_data["signing_ca_certificate"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + 0x3000, jobj_prov_data_prov_data["ssl_private_key"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_CONN_ID_1 + 0x4000, jobj_prov_data_prov_data["ssl_shared_key"].toString().toLocal8Bit());
        ld_binary_array.append('\0');

//        qDebug() << "Device ID : " << jobj_prov_data_prov_data["id"].toInt();

        ser_ver.data = 0;
        // For Connection-ID-0
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0, QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Serial
        ser_ver.data = 100;
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + 2, QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Version
        id.data = jobj_prov_data_prov_data["id"].toInt();
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + 4, QByteArray::fromRawData((const char *)id.data_bytes, 8));
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + 0x0C, dev_uuid.toLocal8Bit());
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + 0x1C, QString::fromStdString("US").toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + 0x24, jobj_prov_data_prov_data["default_wifi_ssid"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_0 + 0x44, jobj_prov_data_prov_data["default_wifi_password"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 0x84, QString::fromStdString("EzloPi"));
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 0x104, jobj_prov_data_prov_data["id"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 0x124, QString::fromStdString("unknown"));

        ser_ver.data = 0;
        // For Connection-ID-0
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1, QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Serial
        ser_ver.data = 100;
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 2, QByteArray::fromRawData((const char *)ser_ver.data_bytes, 2));           // Version
        id.data = jobj_prov_data_prov_data["id"].toInt();
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 4, QByteArray::fromRawData((const char *)id.data_bytes, 8));
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 0x0C, dev_uuid.toLocal8Bit());
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 0x1C, QString::fromStdString("US").toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 0x24, jobj_prov_data_prov_data["default_wifi_ssid"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 0x44, jobj_prov_data_prov_data["default_wifi_password"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 0x84, QString::fromStdString("EzloPi").toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 0x104, jobj_prov_data_prov_data["id"].toString().toLocal8Bit());
        ld_binary_array.append('\0');
        ld_binary_array.insert(SIZE_EZPI_OFFSET_HUB_ID_1 + 0x124, QString::fromStdString("unknown").toLocal8Bit());
        ld_binary_array.append('\0');


        if(!QFile::remove("ezpibins/ld.bin")) {
            qDebug() << "Failed deleting old file and create new ld.bin file.";
            ui->textBrowser_console_log->append("Failed deleting old file and create new ld.bin file.");
            return;
        }
        qDebug() << "Current dir: " << QDir::currentPath();
        QFile out("ezpibins/ld.bin");
        out.open(QIODevice::WriteOnly);
        out.write(ld_binary_array);
        out.close();

        QMessageBox::information(this, "Registration complete!", "Adding new device has been successful.\n Please note the ID from console below.\n" \
                                                                 "now flash the firmware!");
        ezpi_flag_registered = true;
    } else {
        QMessageBox::information(this, "Registration failed!", "Failed adding new device into the cloud platform, try again closing and reopeaning the app!");
    }

#else

    QFile ezpi_prov_file;
    ezpi_prov_file.setFileName("ld.bin");
    bool file_exists = ezpi_prov_file.exists();

    if(!QFile::remove("ld.bin")) {
        qDebug() << "Failed deleting old file and create new ld.bin file.";
        ui->textBrowser_console_log->append("Failed deleting old file and create new ld.bin file.");
        return;
    }
    qDebug() << "Current dir: " << QDir::currentPath();
    QFile out("ezpibins/ld.bin");
    out.open(QIODevice::WriteOnly);
    out.close();
#endif
}

// ACTION list 
void MainWindow::on_actionLogin_triggered() {
    ezpi_form_login->setFixedSize(350, 225);
    ezpi_form_login->setModal(true);
    ezpi_form_login->show();
}

void MainWindow::on_actionEnable_Log_triggered()
{
    if(ezpi_flag_enable_log == true) {
        qDebug() << "Disabled log \r\n";
        ui->textBrowser_console_log->append("Disabled log \n");
        ui->actionEnable_Log->setText("Enable Log");

        ezpi_flag_enable_log = false;
    } else {
        qDebug() << "Enabled log \r\n";
        ui->textBrowser_console_log->append("Enabled log \n");
        ui->actionEnable_Log->setText("Disable Log");
        ezpi_flag_enable_log = true;
    }
}

void MainWindow::on_actionRegister_triggered() {

    bool flag_login = ezpi_form_login->ezpi_get_flag_user_login();
    uint64_t login_expires = ezpi_form_login->ezpi_get_token_expiry_time();
    user_token = ezpi_form_login->ezpi_get_user_access_token();

    if(ezpi_flag_registered) {
        QMessageBox::information(this, "Device has been registered.", "A device has already been registered in your account.");
        return;
    }

    if(flag_login == true) {

        if((uint64_t)QDateTime::currentSecsSinceEpoch() < login_expires) {

//            qDebug() << "Token: " << user_token;

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
            request.setRawHeader("Authorization", "Bearer " + user_token.toLocal8Bit());

            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

            QNetworkAccessManager *manager = new QNetworkAccessManager(this);

            connect(manager, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(success_prov_dat(QNetworkReply*)));

            // FIXME for debug
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

void MainWindow::on_actionAbout_EzloPi_triggered() {

    QMessageBox::about(this, "EzloPi", "EzloPi V0.0.1");

}

void MainWindow::on_actionExit_triggered() {
    QApplication::quit();
}

void MainWindow::ezlogic_table_adddev_digital_op(ezlogic_device_digital_op_t output_device) {

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
}
void MainWindow::ezlogic_table_adddev_digital_ip(ezlogic_device_digital_ip_t input_device) {

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

}

void MainWindow::ezlogic_table_adddev_analog_ip(ezlogic_device_analog_ip_t adc_device) {

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

}

void MainWindow::ezlogic_table_adddev_onewire(ezlogic_device_one_wire_t onewire_device) {

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

}
void MainWindow::ezlogic_table_adddev_i2c(ezlogic_device_I2C_t i2c_device) {
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
}
void MainWindow::ezlogic_table_adddev_spi(ezlogic_device_SPI_t spi_device) {

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
}

void MainWindow::ezpi_serial_receive(void) {
    *ezpi_read_data_serial += ezpi_serial_port->readAll();
//    qDebug() << "Serial read on ready : " << QString::fromLocal8Bit(*ezpi_read_data_serial);
}

void MainWindow::ezpi_serial_process(void) {

    ezpi_timer_serial_complete.stop();

    qDebug() << "Data RX: " << QString::fromLocal8Bit(*ezpi_read_data_serial);

    ui->textBrowser_console_log->append("Data RX: " + QString::fromLocal8Bit(*ezpi_read_data_serial));
    switch (ezpi_cmd_state) {
        case CMD_ACTION_SET_WIFI:
            ezpi_action_set_wifi(*ezpi_read_data_serial);
            qDebug() << "Responding WiFi connection request!";
            break;
        case CMD_ACTION_GET_INFO:
            ezpi_action_check_info(*ezpi_read_data_serial);
            break;
        case CMD_ACTION_SET_CONFIG:
            ezpi_action_set_config_process(*ezpi_read_data_serial);
            break;
        case CMD_ACTION_GET_CONFIG:
            ezpi_action_get_config_process(*ezpi_read_data_serial);
            break;
        default:
            qDebug() << "Unknown CMD from UI !";
            ui->textBrowser_console_log->append("Unknown CMD from UI !");
            break;
    }
}

void MainWindow::ezpi_serial_transfer(QByteArray d) {
    ezpi_serial_port->flush();
    ezpi_serial_port->write(d.constData());
    ezpi_read_data_serial->clear();
    ezpi_timer_serial_complete.start(EZPI_SERIAL_READ_TIMEOUT);
}

void MainWindow::ezpi_action_check_info(QByteArray serial_read) {

    ezlogic_info_t get_info_fmw_info;
    ezpi_fimware_present = false;
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

#if 0
    if(json_map_root_get_info["cmd"].toUInt() != CMD_ACTION_GET_INFO) {
        return;
    }
#endif
    if(json_map_root_get_info["status"].toUInt() == 1) {

        get_info_fmw_info.v_sw = json_map_root_get_info["v_sw"].toUInt();
        get_info_fmw_info.v_type = json_map_root_get_info["v_type"].toUInt();
        get_info_fmw_info.build = json_map_root_get_info["build"].toUInt();
        get_info_fmw_info.v_idf = json_map_root_get_info["v_idf"].toUInt();
        get_info_fmw_info.uptime = json_map_root_get_info["uptime"].toUInt();
        get_info_fmw_info.build_date = json_map_root_get_info["build_date"].toUInt();

        EzloPi->EZPI_SET_FMW_INFO(get_info_fmw_info);

        if(get_info_fmw_info.v_sw > 1)    ezpi_fimware_present = true;
        else ezpi_fimware_present = false;
    }
}

void MainWindow::ezpi_action_get_config_process(QByteArray serial_read) {

    QJsonParseError jsonError;
    QJsonDocument doc_get_config = QJsonDocument::fromJson(serial_read, &jsonError);

    if (jsonError.error != QJsonParseError::NoError){
        qDebug() << jsonError.errorString();
        QMessageBox::warning(this, "Error!", "Incorrect data format received!");
        return;
    }

    QJsonObject obj_data_root_get_config = doc_get_config.object();
    QVariantMap json_map_root_get_config = obj_data_root_get_config.toVariantMap();

    if(json_map_root_get_config["cmd"].toUInt() != CMD_ACTION_GET_CONFIG) {
        return;
    }

    QVariantList list_get_config_device_detail = json_map_root_get_config["dev_detail"].toList();

    ezlogic_device_digital_op_t device_digital_op;
    ezlogic_device_digital_ip_t device_digital_ip;
    ezlogic_device_analog_ip_t device_analog_ip;
    ezlogic_device_one_wire_t device_onewire;
    ezlogic_device_I2C_t device_i2c;
    ezlogic_device_SPI_t device_spi;

    EZPI_UINT8 dev_count_get_config = 0;

    // Clear table contents
    ui->tableWidget_device_table->clearContents();

    // Clear internal device storage
    EzloPi->EZPI_CLEAR_OUTPUT_DEVICES();
    EzloPi->EZPI_CLEAR_INPUT_DEVICES();
    EzloPi->EZPI_CLEAR_AINPUT_DEVICES();
    EzloPi->EZPI_CLEAR_ONEWIRE_DEVICES();
    EzloPi->EZPI_CLEAR_I2C_DEVICES();
    EzloPi->EZPI_CLEAR_SPI_DEVICES();

    for(EZPI_UINT8 i = 0; i < list_get_config_device_detail.size(); i++) {

        dev_count_get_config++;

        QVariantMap get_config_device = list_get_config_device_detail[i].toMap();

        switch(get_config_device["dev_type"].toUInt()) {
            case EZPI_DEV_TYPE_DIGITAL_OP:
                device_digital_op.dev_name = get_config_device["dev_name"].toString();
                device_digital_op.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
                device_digital_op.id_room = get_config_device["id_room"].toUInt();
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
                device_digital_ip.id_room = get_config_device["id_room"].toUInt();
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
            device_analog_ip.id_room = get_config_device["id_room"].toUInt();
            device_analog_ip.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
            device_analog_ip.gpio = get_config_device["gpio"].toUInt();
            device_analog_ip.resln_bit = (ezpi_adc_resln)get_config_device["resln_bit"].toUInt();

            EzloPi->EZPI_ADD_AINPUT_DEVICE(device_analog_ip);

            ezlogic_table_adddev_analog_ip(device_analog_ip);
            break;

            case EZPI_DEV_TYPE_ONE_WIRE:
                device_onewire.dev_name = get_config_device["dev_name"].toString();
                device_onewire.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
                device_onewire.id_room = get_config_device["id_room"].toUInt();
                device_onewire.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
                device_onewire.val_ip = get_config_device["val_ip"].toBool();
                device_onewire.pull_up = get_config_device["pull_up"].toBool();
                device_onewire.gpio = get_config_device["gpio"].toUInt();

                EzloPi->EZPI_ADD_ONEWIRE_DEVICE(device_onewire);

                ezlogic_table_adddev_onewire(device_onewire);
                break;

            case EZPI_DEV_TYPE_I2C:
                device_i2c.dev_name = get_config_device["dev_name"].toString();
                device_i2c.dev_type = (ezpi_dev_type)get_config_device["dev_type"].toUInt();
                device_i2c.id_room = get_config_device["id_room"].toUInt();
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
                device_spi.id_room = get_config_device["id_room"].toUInt();
                device_spi.id_item = (ezpi_item_type)get_config_device["id_item"].toUInt();
                device_spi.gpio_miso = get_config_device["gpio_miso"].toUInt();
                device_spi.gpio_mosi = (ezpi_dev_type)get_config_device["gpio_mosi"].toUInt();
                device_spi.gpio_sck = get_config_device["gpio_sck"].toUInt();
                device_spi.gpio_cs = (ezpi_item_type)get_config_device["gpio_cs"].toUInt();

                EzloPi->EZPI_ADD_SPI_DEVICE(device_spi);

                ezlogic_table_adddev_spi(device_spi);
                break;
            default:
                break;
        }
    }

    if(dev_count_get_config < 1) QMessageBox::information(this, "No device!", "Device configurations not found1");
}

void MainWindow::ezpi_action_set_wifi(QByteArray wifi_response) {

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

void MainWindow::ezpi_action_set_config_process(QByteArray serial_read) {
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

//    qDebug() << "cmd: " << QString::number(json_map_root_set_config_response["cmd"].toUInt());
//    qDebug() << "status_write: " << QString::number(json_map_root_set_config_response["status_write"].toUInt());

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
