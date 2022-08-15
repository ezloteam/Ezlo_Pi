#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QDebug>
#include "string.h"

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

#include "ezuuid.h"
#include<iostream>

device_t device[EZPI_MAX_DEVICES];
int cou_dev = 0;

uchar gpio_m[EZPI_MAX_GPIOS] = {0xff,0xff,0xff,0xff,0xff,0,0xff,0xff,0xff,0xff,0xff,0xff,0,0,0,0,0xff,0,0,0,0,0,0,0,0xff,0,0,0};

bool is_start = false;
bool is_pars = false;
bool is_work = false;
bool is_func = false;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),    
    ezpi_flag_serial_port_open(false),
    ezpi_flag_is_ezlopi(false),
    ezpi_device_counter(0),
    ezpi_flag_enable_log(true),
    ezpi_flag_registered(false)
{
    // Create and Init UI
    ui = new Ui::MainWindow;
    // Set goemetry of the UI
    setFixedSize(810, 628);
    ui->setupUi(this);

    EzloPi = new EzPi();
    EzloPi->EZPI_SET_BOARD_TYPE(EZPI_BOARD_TYPE_ESP32_GENERIC);
    EzloPi->EZPI_INIT_BOARD();

    ezpi_form_login = new login(this);
    ezpi_form_WiFi = new Dialog_WiFi(this);
    ezpi_form_devadd = new Dialog_devadd(this);
    ezpi_form_configdev_digitalio = new Dialog_configdev_digitalio(this, EzloPi);
    ezpi_form_config_digital_ip = new Dialog_config_input(this);
    ezpi_form_config_onewire = new Dialog_config_onewire(this);
    ezpi_form_config_i2c = new Dialog_config_i2c(this);
    ezpi_form_config_spi = new Dialog_config_spi(this);

    connect(ezpi_form_devadd, SIGNAL(ezpi_send_dev_type_selected(EZPI_UINT8)), this, SLOT(ezpi_receive_dev_type_selected(EZPI_UINT8)));



    user_token.clear();

    // Hide pop-up widgets
    ui->scrollArea_gpio_config->setVisible(false);
    ui->scrollArea_set_wifi_cred->setVisible(false);
    ui->scrollArea_device_type->setVisible(false);

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

    connect(ui->comboBox_uart_list,SIGNAL(currentIndexChanged(const QString&)),
            this,SLOT(on_comboBox_uart_list_currentIndexChanged(const QString&)));

    // Timer initialization
    ezpi_timer_ask_info.callOnTimeout(this, &MainWindow::ezpi_message_info_no_firmware_detected);

}

MainWindow::~MainWindow() {
    ezpi_serial_port.close();
    delete ui;    
}

void MainWindow::ezpi_check_firmware() {

    QByteArray ezpi_byte_array_write_to_serial;

    quint64 bytes_wrote = ezpi_serial_port.write("$GET_CONFIG\n");
    if(bytes_wrote > 0) {
        qDebug() << bytes_wrote << " bytes written!";
//        if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(" bytes written!");
        ezpi_timer_ask_info.start(5000);
    }

    connect(&ezpi_serial_port, &QSerialPort::readyRead, this, &MainWindow::on_serRX1);

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


void MainWindow::pars(uchar *buf, int len) {

    switch (buf[1]){

    case GET_CONFIG:
    {
        int row = ui->tableWidget_device_table->rowCount();
        if(row>0){
            for (int n=0;n<row;n++){
                ui->tableWidget_device_table->removeRow(0);
            }
        }
        cou_dev = 0;
        for (int j=0;j<28;j++)
        {
            QString str;
            str = str.asprintf("%d",j);
            gpio_m[j] = buf[2+j];
            qDebug() << "gpio_m[" << j<< "] = " << gpio_m[j];
        }
        qDebug() << "copy buf ";
        break;
    }
    case SET_CONFIG:{

            QMessageBox::information(this,"Info","Send to ESP OK!");
            break;
    }
    case FIRST_DEV:
    case SET_DEV:
    {
        if(buf[1] == FIRST_DEV)
        {
            cou_dev = 0;
        }
        memcpy((void*)&device[cou_dev],&buf[2],len-2);

        int row = ui->tableWidget_device_table->rowCount();
        ui->tableWidget_device_table->setRowCount(row+1);
        QString str,str1,str2;
        int nom = device[cou_dev].out_gpio;
        int nom1 = device[cou_dev].input_gpio;
        if (nom)
        {
            str = str.asprintf("%d", nom);
            str1 = "OUT";
        }
        if(nom1)
        {
            if (nom){
                str = str + " / ";
                str1 = str1 + " / ";
            }

            str = str + str2.asprintf("%d",nom1);
            str1 = str1 + "IN";
        }
        QTableWidgetItem *gtwi = new QTableWidgetItem(str);
        ui->tableWidget_device_table->setItem(row,0,gtwi);
        gtwi = new QTableWidgetItem(str1);
        ui->tableWidget_device_table->setItem(row,1,gtwi);
        str1 = str1.asprintf("%d",device[cou_dev].out_vol);
        gtwi = new QTableWidgetItem(str1);
        ui->tableWidget_device_table->setItem(row,3,gtwi);
        ui->comboBox_device_type->setCurrentIndex(device[cou_dev].dev_type);
        gtwi = new QTableWidgetItem(ui->comboBox_device_type->currentText());
        ui->tableWidget_device_table->setItem(row,4,gtwi);
        cou_dev++;
        device[cou_dev].Name[0] = 0;
        device[cou_dev].is_meter = false;
        break;
    }
    case SET_VOL:{
        QString str;
        str = str.asprintf("Pin %d STATE - %d", buf[2],buf[3]);
        QMessageBox::information(this, "Info", str);
    }
    case SET_WiFi:{
        if (buf[2]){
            QMessageBox::information(this,"Info","WiFi set successfully !");
        }else{
            QMessageBox::information(this,"Info","WiFi set Error !");
        }
        break;
    }
    }
}

void MainWindow::on_serRX1() {
    static int cou = 0;
    static int len_d = 0;
    static uchar buf[256];
    QByteArray dat=ezpi_serial_port.readAll();
    if(!dat.length())
    {
        return;
    }
    qDebug() << "Read " << dat.length() << " bytes";

    QString str,str1;
    str1.asprintf("%s",(char*)dat.data());
    str.asprintf("%s",str1.toStdString().c_str());    
#if 1
    qDebug() << str1;
    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(str1);

    if(ezpi_flag_enable_log){
        ui->textBrowser_console_log->append(dat.data());
        qDebug() << dat;
    }
#endif

    if(!is_start) {
        qDebug() << dat;

        if(dat.at(0) == '$') {

            is_start = true;

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
        }
    } else {
        for(int i = 0; i<dat.length();i++){
            if(!is_pars){
                if((uchar)dat.at(i) == 0xc5){
                    is_pars = true;
                    qDebug() << "is_pars = true";
                    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("is_pars = true");
                    cou = 0;
                }
            }else{
                buf[cou] = dat.at(i);
                if(cou == 0)
                {
                    len_d = buf[cou];
                }
                cou++;
                if (cou == len_d)
                {
                    pars(buf,cou);
                    is_pars = false;
                    cou = 0;
                }
            }
        }
    }
}

void MainWindow::on_pushButton_connect_uart_clicked() {

    ezpi_serial_port.setPort(ezpi_serial_port_info);
    ezpi_serial_port.setBaudRate(460800);
    if(ezpi_flag_serial_port_open == false) {
        if(!ezpi_serial_port_info.isBusy()) {

            if(ezpi_serial_port.open(QIODevice::ReadWrite)) {
//                if(1) {
                ezpi_flag_serial_port_open = true;
                is_start = false;

                //Modify UI elements:
                ui->pushButton_connect_uart->setText("Close");
                ui->pushButton_scan_uart_ports->setDisabled(true);
                ui->comboBox_uart_list->setDisabled(true);

                ui->actionRegister->setEnabled(true);

                // Display message on console
                qDebug() << ezpi_serial_port_info.portName() << " serial port is open.";
                if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(ezpi_serial_port_info.portName() + " serial port is open.");

                // Check firmware
                ezpi_check_firmware();
            } else {
                qDebug() << "Failed opeaning serial port: " << ui->comboBox_uart_list->currentText();
                if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("Failed opeaning serial port: " + ui->comboBox_uart_list->currentText());
            }
        } else {
            qDebug() << ezpi_serial_port_info.portName() <<  ": port is busy !";
            if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(ezpi_serial_port_info.portName() + QString::fromLocal8Bit(": port is busy !"));
        }
    } else {
        ezpi_serial_port.close();
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

void MainWindow::on_pushButton_add_device_clicked() {

    if (ezpi_device_counter >= EZPI_MAX_DEVICES) {
        QMessageBox::information(this,"Device full","Devices reaches top limit, no more devices can be added");
        return;
    }

    ezpi_form_devadd->setModal(true);
    ezpi_form_devadd->show();
}

void MainWindow::on_pushButton_gpio_cancel_clicked() {
    ui->scrollArea_gpio_config->setVisible(false);
    is_func = false;
}

void MainWindow::on_pushButton_gpio_apply_clicked() {

    QString str, str1;

    if(!is_func) {
        memcpy(device[cou_dev].Name ,ui->lineEdit_gpio_device_name->text().toLatin1(),ui->lineEdit_gpio_device_name->text().length());
        device[cou_dev].Name[ui->lineEdit_gpio_device_name->text().length()] = 0;
        device[cou_dev].is_meter = false;

        if(ui->comboBox_gpio_out_select_pin->isEnabled()) {
            str = ui->comboBox_gpio_out_select_pin->currentText();
            str1 = "OUT";
        }

        if(ui->comboBox_gpio_in_select_pin->isEnabled()) {
            if (str1 != "") {
                str = str + " / ";
            }
            str = str + ui->comboBox_gpio_in_select_pin->currentText();
            if (str1 != "")
                str1 = str1 + " / ";
            str1 = str1 + "IN";
        }

        QTableWidgetItem *gtwi = new QTableWidgetItem(str);
        int row = ui->tableWidget_device_table->rowCount();
        ui->tableWidget_device_table->setRowCount(row+1);
        int nom = ui->comboBox_gpio_in_select_pin->currentText().toInt();

        ui->tableWidget_device_table->setItem(row,0,gtwi);
        ui->scrollArea_gpio_config->setVisible(false);

        QTableWidgetItem *gtwi1 = new QTableWidgetItem(str1);
        ui->tableWidget_device_table->setItem(row,1,gtwi1);


        if(ui->comboBox_gpio_in_select_pin->isEnabled()) {

            gpio_m[nom] = 1;
            device[cou_dev].is_input = true;
            device[cou_dev].input_gpio = nom;
            device[cou_dev].input_vol = (ui->radioButton_gpio_in_default_pull_up->isChecked() ? 0:1);
            device[cou_dev].checkBox_gpio_in_logic_type = ui->checkBox_gpio_in_logic_type->isChecked();

            if(ui->radioButton_gpio_in_default_pull_up->isChecked()) {
                gtwi = new QTableWidgetItem("PU ");
            } else {
                gtwi = new QTableWidgetItem("PD ");
                gpio_m[nom] += 4;
            }
            ui->tableWidget_device_table->setItem(row,2,gtwi);

            gpio_m[nom] += ui->comboBox_device_type->currentIndex()*16;

        } else {
            device[cou_dev].input_gpio = 0;
        }

        if(ui->comboBox_gpio_out_select_pin->isEnabled()) {
            str = ui->comboBox_gpio_out_select_pin->currentText();
            int nom = str.toInt();
            gpio_m[nom] = 2;

            device[cou_dev].out_gpio = nom;
            device[cou_dev].out_vol = (ui->radioButton_gpio_out_default_low->isChecked() ? 0:1);
            device[cou_dev].checkBox_gpio_out_logic_type = ui->checkBox_gpio_out_logic_type->isChecked();

            if(ui->radioButton_gpio_out_default_pull_up->isChecked()){
                // gtwi = new QTableWidgetItem("PULL-UP");
            } else {
               // gtwi = new QTableWidgetItem("PULL-DOWN");
                gpio_m[nom] += 4;
            }

            if(ui->radioButton_gpio_out_default_low->isChecked()) {
                gtwi = new QTableWidgetItem("   0  ");
            } else {
                gtwi = new QTableWidgetItem("   1  ");
                gpio_m[nom] += 8;
            }
            ui->tableWidget_device_table->setItem(row,3,gtwi);
            gpio_m[nom] += ui->comboBox_device_type->currentIndex()*16;

        } else {
            device[cou_dev].out_gpio = 0;
        }

        gtwi = new QTableWidgetItem(ui->comboBox_device_type->currentText());
        ui->tableWidget_device_table->setItem(row,4,gtwi);
        device[cou_dev].dev_type = ui->comboBox_device_type->currentIndex();
        QString str_u = QUuid::createUuid().toString();
        str_u.remove(QRegularExpression("{|}|-")); // if you want only hex numbers
        qDebug() << str_u;
        for(int u=0;u<8;u++)
            device[cou_dev].dev_id[u]=str_u.at(u).toLatin1();
        device[cou_dev].dev_id[7] = 0;
        for(int u=8;u<16;u++)
            device[cou_dev].id_i[u-8]=str_u.at(u).toLatin1();
        device[cou_dev].id_i[7] = 0;
        device[cou_dev].roomId[0] = 0;
        qDebug() << "dev_id" << device[cou_dev].dev_id;
        qDebug() << "id_i" << device[cou_dev].id_i;
        cou_dev ++;
        device[cou_dev].Name[0] = 0;
        qDebug() << "Devise struc size" << sizeof(device_t);
    } else {
        int nom = ui->comboBox_gpio_in_select_pin->currentText().toInt();
        gpio_m[nom] = 1;
        if(ui->radioButton_gpio_in_default_pull_up->isChecked()) {
        } else {
            gpio_m[nom] += 4;
        }
        gpio_m[nom] += 0xF0;
        nom = ui->comboBox_gpio_out_select_pin->currentText().toInt();
        gpio_m[nom] = 2;
        if(ui->radioButton_gpio_out_default_pull_up->isChecked()) {
        } else {
            gpio_m[nom] += 4;
        }

        if(ui->radioButton_gpio_out_default_low->isChecked()) {
        } else {
            gpio_m[nom] += 8;
        }
        gpio_m[nom] += 0xF0;
        ui->scrollArea_gpio_config->setVisible(false);
        is_func = false;
    }
}

void MainWindow::on_pushButton_remove_device_clicked() {

    int row = ui->tableWidget_device_table->currentRow();
    if(row<0)
        return;

    QTableWidgetItem *gtwi1 = ui->tableWidget_device_table->item(row,0);
    QString str = gtwi1->text();
    qDebug() << "DELETE " << str;
    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("DELETE " + str + "\n");
    QString str1,str2;
    int nom,nom1;
    for (int i=0;i<cou_dev;i++) {
        str1 = "";
        nom = 0;
        nom1 = 0;
        if(device[i].out_gpio != 0) {
            str1=str2.asprintf("%d",device[i].out_gpio);
            nom = device[i].out_gpio;
        }

        if (device[i].input_gpio != 0) {
            nom1 = device[i].input_gpio;
            if(device[i].out_gpio != 0) {
                str1=str1 + " / ";
            }
            str1=str1 + str2.asprintf("%d",device[i].input_gpio);
        }
        qDebug() << "str"  << str << " str1 " << str1;
        if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("str"  + str + " str1 " + str1 + "\n");
        if(str.toLatin1() == str1.toLatin1()) {
            if (nom)
                gpio_m[nom] = 0;
            if (nom1)
                gpio_m[nom1] = 0;
            ui->tableWidget_device_table->removeRow(row);
            nom = cou_dev - i - 1;
            cou_dev--;

            if (nom>0)  memcpy(&device[i],&device[i+1],nom*sizeof(device_t));

            device[cou_dev].Name[0] = 0;
            device[cou_dev].out_gpio = 0;
            device[cou_dev].input_gpio = 0;
            device[cou_dev].is_meter = false;
            for (int j=0;j<cou_dev;j++) {
                qDebug() << "DEV" << j << " INPUT " << device[j].input_gpio << " OUT " << device[j].out_gpio;
            }
            return;
        }
    }
//    int nom = str.toInt();
//    gpio_m[nom] = 0;
//    ui->tableWidget_device_table->removeRow(row);
}

void MainWindow::on_pushButton_get_ezpi_config_clicked() {
    QByteArray buf;

    buf.append(0x95);
    buf.append(2);
    buf.append(0xa0);
    ezpi_serial_port.setBaudRate(460800);
    ezpi_serial_port.write(buf, 3);
    qDebug() << "Send" << buf;

    ezpi_serial_port.flush();

    connect(&ezpi_serial_port, &QSerialPort::readyRead, this, &MainWindow::on_serRX1);

}

void MainWindow::on_pushButton_set_ezpi_config_clicked() {

    uchar buf[60];

    buf[0]=0x95;
    buf[1]=30;
    buf[2]=0xa1;
    memcpy(&buf[3],gpio_m,28);
    ezpi_serial_port.write((const char*)buf,31);

    qDebug() << "Sent from QT" << buf;
    for (int j=0;j<28;j++) {
        QString str;
        str = str.asprintf("%d",j);
        gpio_m[j] = buf[2+j];
        qDebug() << "gpio_m[" << j<< "] = " << gpio_m[j];
    }

    if(!device[0].Name[0])
        return;
    buf[1] = sizeof(device_t) + 2;

    connect(&ezpi_serial_port, &QSerialPort::readyRead, this, &MainWindow::on_serRX1);

//    memcpy(&buf[3],&device[0],sizeof(device_t));
//    ezpi_serial_port.write((const char*)buf,sizeof(device_t) + 3);
    for (int i=0;i<MAX_DEV;i++) {

        if(!device[i].Name[0]) {
            buf[1] = 2;
            buf[2] = END_DEV;
            ezpi_serial_port.write((const char*)buf,3);
            return;
        }

        if(i == 0)
            buf[2] = FIRST_DEV;
        else
            buf[2] = SET_DEV;

        memcpy(&buf[3],&device[i],sizeof(device_t));
        ezpi_serial_port.write((const char*)buf,sizeof(device_t) + 3);
        qDebug() << "Sending from QT: " << buf;
//        if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("Sending from QT: " + QString::fromWCharArray(buf) + "\n");
    }
}

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
        ezpi_serial_port.open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    } else if(str_StandardError.contains("Hard resetting") || str_StandardOutput.contains("Hard resetting")) {
        ezpi_message_box_failed_erase.information(this, "Success flashing the device!", "Flashing ezlopi firmware to the device was successful! You can now gracefully disconnect the device and move further!");
        ezpi_serial_port.open(QIODevice::ReadWrite);
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
        ezpi_serial_port.open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    } else if(str_StandardOutput.contains("Hard resetting") || str_StandardError.contains("Hard resetting")) {
        ezpi_message_box_failed_erase.information(this, "Success erasing the device!", "Erasing the device was successful! You can now proceed forward flashing the ezlopi firmware!");
        ezpi_serial_port.open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    }
}


void MainWindow::on_pushButton_flash_ezpi_bins_clicked() {

    ui->textBrowser_console_log->clear();

    ezpi_process_write_flash = new QProcess(this);

    connect(ezpi_process_write_flash, &QProcess::readyReadStandardOutput, this, &MainWindow::ezpi_log_write_flash);
    connect(ezpi_process_write_flash, &QProcess::readyReadStandardError, this, &MainWindow::ezpi_log_write_flash);

//    QString dir;
//    QString path_files = QDir::currentPath();

//    dir = path_files + "/esptool.exe";

//    path_files = QFileInfo(dir).absoluteDir().absolutePath();
//    qDebug() << path_files;
//    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(path_files + "\n");

//    QString fl = dir;

//    dir = path_files + "/bootloader.bin";

    #ifdef __linux__
            QString ser_port = "/dev/" + ezpi_serial_port_info.portName();
    #elif _WIN32
        QString ser_port = ezpi_serial_port_info.portName();
    #else

    #endif

    ezpi_serial_port.close();
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

void MainWindow::on_pushButton_set_wifi_clicked() {
    ezpi_form_WiFi->show();
}

void MainWindow::on_pushButton_set_wifi_cancel_clicked() {
    ui->scrollArea_set_wifi_cred->setVisible(false);
}

void MainWindow::on_pushButton_set_wifi_apply_clicked() {
    uchar buf[100] = {0};

    memcpy(buf+3,ui->lineEdit_set_wifi_ssid->text().toStdString().c_str(),ui->lineEdit_set_wifi_ssid->text().length());
    buf[3+ui->lineEdit_set_wifi_ssid->text().length()] = 0;
    memcpy(buf+35, ui->lineEdit_set_wifi_pass->text().toStdString().c_str(),ui->lineEdit_set_wifi_pass->text().length());
    buf[35+ui->lineEdit_set_wifi_pass->text().length()] = 0;
    buf[0]=0x95;
    buf[1]=98;
    buf[2]=SET_WiFi & 0xff;
    ezpi_serial_port.write((const char*)buf,99);

    ui->textBrowser_console_log->append(QString::fromLocal8Bit(QByteArray::fromRawData((const char*)buf, 99)));
    QString str1;
    str1 = str1.asprintf("%s",buf);
    qDebug() << "Send" << str1;
    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("Send" + str1 + "\n");
    ui->scrollArea_set_wifi_cred->setVisible(false);
}

void MainWindow::on_pushButton_select_device_type_apply_clicked() {

}

void MainWindow::on_pushButton_select_device_type_cancel_clicked() {
    ui->scrollArea_device_type->setVisible(false);
}

void MainWindow::on_comboBox_gpio_out_select_pin_currentTextChanged(const QString &arg1) {
    if (is_work)
        return;
    is_work = true;
    QString str,str2;
    str2 = ui->comboBox_gpio_in_select_pin->currentText();
    int gp_i = ui->comboBox_gpio_out_select_pin->currentText().toInt();
    ui->comboBox_gpio_in_select_pin->clear();
    for(int i=0;i<27;i++) {
        if(gpio_m[i] == 0) {
            if(i!=gp_i) {
                ui->comboBox_gpio_in_select_pin->addItem(str.asprintf("%d",i));
            }
        }
    }
    if(arg1!=str2) {
        ui->comboBox_gpio_in_select_pin->setCurrentText(str2);
    }
    is_work = false;
}

void MainWindow::on_comboBox_gpio_in_select_pin_currentTextChanged(const QString &arg1) {
    if (is_work)
        return;
    is_work = true;
    QString str,str2;
    str2 = ui->comboBox_gpio_out_select_pin->currentText();
    int gp_i = ui->comboBox_gpio_in_select_pin->currentText().toInt();
    ui->comboBox_gpio_out_select_pin->clear();
    for(int i=0;i<27;i++) {
        if(gpio_m[i] == 0) {
            if(i!=gp_i) {
                ui->comboBox_gpio_out_select_pin->addItem(str.asprintf("%d",i));
            }
        }
    }

    if(arg1!=str2) {
        ui->comboBox_gpio_out_select_pin->setCurrentText(str2);
    }
    is_work = false;
}

void MainWindow::on_checkBox_gpio_led_button_enable_stateChanged() {

    if (ui->checkBox_gpio_led_button_enable->isChecked()) {

        ui->comboBox_gpio_in_select_pin->setEnabled(true);
        ui->groupBox_gpio_in_default_shunt_res->setEnabled(true);
        ui->groupBox_gpio_in_default_bool->setEnabled(true);
        ui->radioButton_gpio_in_default_pull_up->setChecked(true);
        ui->radioButton_gpio_in_default_low->setChecked(true);
        ui->checkBox_gpio_in_logic_type->setEnabled(true);

    } else {

        ui->comboBox_gpio_in_select_pin->setEnabled(false);
        ui->groupBox_gpio_in_default_shunt_res->setEnabled(false);
        ui->groupBox_gpio_in_default_bool->setEnabled(false);
        ui->checkBox_gpio_in_logic_type->setEnabled(false);
    }
}

void MainWindow::on_pushButton_erase_flash_clicked() {

    ezpi_process_erase_flash = new QProcess(this);

    connect(ezpi_process_erase_flash, &QProcess::readyReadStandardOutput, this, &MainWindow::ezpi_log_erase_flash);
    connect(ezpi_process_erase_flash, &QProcess::readyReadStandardError, this, &MainWindow::ezpi_log_erase_flash);

    ezpi_serial_port.close();
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

void MainWindow::on_actionExit_triggered() {
    QApplication::quit();
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

void MainWindow::on_actionAbout_EzloPi_triggered() {

    QMessageBox::about(this, "EzloPi", "EzloPi V0.0.1");

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

        if(QDateTime::currentSecsSinceEpoch() < login_expires) {

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

void MainWindow::success_prov_dat(QNetworkReply *d) {

    qDebug() << "Added new device";
    ui->textBrowser_console_log->append("Added new device!");
    QByteArray response_bytes = d->readAll();

//    qDebug() << response_bytes;


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

void MainWindow::success_get_prov_jsons(QNetworkReply *d) {

#if 1
     conv_u16_array_t ser_ver;
     conv_64_array_t id;

    struct uuid _uuid;

    QByteArray response_bytes = d->readAll();

//    qDebug() << response_bytes;


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
//    out.write(ld_binary_array);
    out.close();
#endif
}

void MainWindow::on_actionLogin_triggered() {
    ezpi_form_login->setModal(true);
    ezpi_form_login->show();
}

void MainWindow::ezpi_receive_dev_type_selected(EZPI_UINT8 dev_type_index) {
    qDebug() << "Device type Index: " << dev_type_index;


    switch(dev_type_index) {
        case EZPI_DEV_TYPE_DIGITAL_OP: {
            ezpi_form_configdev_digitalio->show();
            break;
        }
        case EZPI_DEV_TYPE_DIGITAL_IP: {
            ezpi_form_config_digital_ip->show();
            break;
        }
        case EZPI_DEV_TYPE_ANALOG_IP: {
           QMessageBox::information(this, "Unsupported", "This feature is not supported yet!");
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
            ezpi_form_config_onewire->show();
            break;
        }
        case EZPI_DEV_TYPE_I2C: {
            ezpi_form_config_i2c->show();
            break;
        }
        case EZPI_DEV_TYPE_SPI: {
            ezpi_form_config_spi->show();
            break;
        }

        default: {
            QMessageBox::information(this, "Unknown", "Unknown type.");
        }
    }
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

=======
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QDebug>
#include "string.h"

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

#include "ezuuid.h"
#include<iostream>

enum commands {
    GET_CONFIG = 0xa0,
    SET_CONFIG,
    SET_VOL,
    SET_WiFi,
    FIRST_DEV,
    SET_DEV,
    GET_DEV,
    END_DEV
} ;

device_t device[EZPI_MAX_DEVICES];
int cou_dev = 0;

uchar gpio_m[EZPI_MAX_GPIOS] ={0xff,0xff,0x00,0xff,0xff,0,0xff,0xff,0xff,0xff,0xff,0xff,0,0,0,0,0xff,0,0,0,0,0,0,0,0xff,0,0,0};

bool is_start = false;
bool is_pars = false;
bool is_work = false;
bool is_func = false;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),    
    ezpi_flag_serial_port_open(false),
    ezpi_flag_is_ezlopi(false),
    ezpi_device_counter(0),
    ezpi_flag_enable_log(true),
    ezpi_flag_registered(false)
{
    // Create and Init UI
    ui = new Ui::MainWindow;
    // Set goemetry of the UI
    setFixedSize(810, 628);
    ui->setupUi(this);

    ezpi_form_login = new login(this);

//    QTimer::singleShot(1000, this, [=]() {
//        ezpi_form_login->setModal(true);
//        ezpi_form_login->show();
//    });

     user_token.clear();

    // Hide pop-up widgets
    ui->scrollArea_gpio_config->setVisible(false);
    ui->scrollArea_set_wifi_cred->setVisible(false);
    ui->scrollArea_device_type->setVisible(false);

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

    connect(ui->comboBox_uart_list,SIGNAL(currentIndexChanged(const QString&)),
            this,SLOT(on_comboBox_uart_list_currentIndexChanged(const QString&)));


    // Timer initialization
    ezpi_timer_ask_info.callOnTimeout(this, &MainWindow::ezpi_message_info_no_firmware_detected);

//    qDebug() << QUuid::createUuidV3(QUuid{}, uuid).toString().toLocal8Bit();
//    qDebug() << QString::fromStdString(QUuid(uuid).toRfc4122().toStdString());

//    QUuid::createUuidV3()
//    QUuid myQTUUID = QUuid::createUuid();

}

MainWindow::~MainWindow() {
    ezpi_serial_port.close();
    delete ui;    
}

void MainWindow::ezpi_check_firmware() {

    QByteArray ezpi_byte_array_write_to_serial;

    quint64 bytes_wrote = ezpi_serial_port.write("$GET_CONFIG\n");
    if(bytes_wrote > 0) {
        qDebug() << bytes_wrote << " bytes written!";
//        if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(" bytes written!");
        ezpi_timer_ask_info.start(5000);
    }

    connect(&ezpi_serial_port, &QSerialPort::readyRead, this, &MainWindow::on_serRX1);

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


void MainWindow::pars(uchar *buf, int len) {

    switch (buf[1]){

    case GET_CONFIG:
    {
        int row = ui->tableWidget_device_table->rowCount();
        if(row>0){
            for (int n=0;n<row;n++){
                ui->tableWidget_device_table->removeRow(0);
            }
        }
        cou_dev = 0;
        for (int j=0;j<28;j++)
        {
            QString str;
            str = str.asprintf("%d",j);
            gpio_m[j] = buf[2+j];
            qDebug() << "gpio_m[" << j<< "] = " << gpio_m[j];
        }
        qDebug() << "copy buf ";
        break;
    }
    case SET_CONFIG:{
        /*
        if(cou_dev){
            for(int i=0; cou_dev; i++){
                QByteArray buf1;
                buf1.append(0x95);
                buf1.append(sizeof(device_t) + 2);
                if(i == 0){
                    buf1.append(FIRST_DEV);
                }
                else{
                    buf1.append(SET_DEV);
                }
                unsigned char buf_d[sizeof(device_t)];
                memcpy(buf_d,(void*)&device[i],sizeof(device_t));
                for(unsigned int j = 0; j<sizeof(device_t); j++){
                    buf1.append(buf_d[j]);
                }
                ezpi_serial_port.write(buf1,3 + sizeof(device_t));
            }
            QByteArray buf1;
            buf1.append(0x95);
            buf1.append(2);
            buf1.append(END_DEV);
            ezpi_serial_port.write(buf1,3);
        }
        */

            QMessageBox::information(this,"Info","Send to ESP OK!");
            break;
    }
    case FIRST_DEV:
    case SET_DEV:
    {
        if(buf[1] == FIRST_DEV)
        {
            cou_dev = 0;
        }
        memcpy((void*)&device[cou_dev],&buf[2],len-2);

        int row = ui->tableWidget_device_table->rowCount();
        ui->tableWidget_device_table->setRowCount(row+1);
        QString str,str1,str2;
        int nom = device[cou_dev].out_gpio;
        int nom1 = device[cou_dev].input_gpio;
        if (nom)
        {
            str = str.asprintf("%d", nom);
            str1 = "OUT";
        }
        if(nom1)
        {
            if (nom){
                str = str + " / ";
                str1 = str1 + " / ";
            }

            str = str + str2.asprintf("%d",nom1);
            str1 = str1 + "IN";
        }
        QTableWidgetItem *gtwi = new QTableWidgetItem(str);
        ui->tableWidget_device_table->setItem(row,0,gtwi);
        gtwi = new QTableWidgetItem(str1);
        ui->tableWidget_device_table->setItem(row,1,gtwi);
        str1 = str1.asprintf("%d",device[cou_dev].out_vol);
        gtwi = new QTableWidgetItem(str1);
        ui->tableWidget_device_table->setItem(row,3,gtwi);
        ui->comboBox_device_type->setCurrentIndex(device[cou_dev].dev_type);
        gtwi = new QTableWidgetItem(ui->comboBox_device_type->currentText());
        ui->tableWidget_device_table->setItem(row,4,gtwi);
        cou_dev++;
        device[cou_dev].Name[0] = 0;
        device[cou_dev].is_meter = false;
        break;
    }
    case SET_VOL:{
        QString str;
        str = str.asprintf("Pin %d STATE - %d", buf[2],buf[3]);
        QMessageBox::information(this, "Info", str);
    }
    case SET_WiFi:{
        if (buf[2]){
            QMessageBox::information(this,"Info","WiFi set successfully !");
        }else{
            QMessageBox::information(this,"Info","WiFi set Error !");
        }
        break;
    }
    }
}

void MainWindow::on_serRX1() {
    static int cou = 0;
    static int len_d = 0;
    static uchar buf[256];
    QByteArray dat=ezpi_serial_port.readAll();
    if(!dat.length())
    {
        return;
    }
    qDebug() << "Read " << dat.length() << " bytes";

    QString str,str1;
    str1.asprintf("%s",(char*)dat.data());
    str.asprintf("%s",str1.toStdString().c_str());
    qDebug() << str1;
    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(str1);



    if(ezpi_flag_enable_log){
        ui->textBrowser_console_log->append(dat.data());
        qDebug() << dat;
    }

    if(!is_start){
        qDebug() << dat;

        if(dat.at(0) == '$') {

            is_start = true;

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
        }
    } else {
        for(int i = 0; i<dat.length();i++){
            if(!is_pars){
                if((uchar)dat.at(i) == 0xc5){
                    is_pars = true;
                    qDebug() << "is_pars = true";
                    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("is_pars = true");
                    cou = 0;
                }
            }else{
                buf[cou] = dat.at(i);
                if(cou == 0)
                {
                    len_d = buf[cou];
                }
                cou++;
                if (cou == len_d)
                {
                    pars(buf,cou);
                    is_pars = false;
                    cou = 0;
                }
            }
        }
    }
}

void MainWindow::on_pushButton_connect_uart_clicked() {

    ezpi_serial_port.setPort(ezpi_serial_port_info);
    ezpi_serial_port.setBaudRate(460800);
    if(ezpi_flag_serial_port_open == false) {
        if(!ezpi_serial_port_info.isBusy()) {

            if(ezpi_serial_port.open(QIODevice::ReadWrite)) {
//                if(1) {
                ezpi_flag_serial_port_open = true;
                is_start = false;

                //Modify UI elements:
                ui->pushButton_connect_uart->setText("Close");
                ui->pushButton_scan_uart_ports->setDisabled(true);
                ui->comboBox_uart_list->setDisabled(true);

                ui->actionRegister->setEnabled(true);

                // Display message on console
                qDebug() << ezpi_serial_port_info.portName() << " serial port is open.";
                if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(ezpi_serial_port_info.portName() + " serial port is open.");

                // Check firmware
                ezpi_check_firmware();
            } else {
                qDebug() << "Failed opeaning serial port: " << ui->comboBox_uart_list->currentText();
                if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("Failed opeaning serial port: " + ui->comboBox_uart_list->currentText());
            }
        } else {
            qDebug() << ezpi_serial_port_info.portName() <<  ": port is busy !";
            if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(ezpi_serial_port_info.portName() + QString::fromLocal8Bit(": port is busy !"));
        }
    } else {
        ezpi_serial_port.close();
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

void MainWindow::on_pushButton_add_device_clicked() {

    if (ezpi_device_counter >= EZPI_MAX_DEVICES) {
        QMessageBox::information(this,"Device full","Devices reaches top limit, no more devices can be added");
        return;
    }
    ui->comboBox_device_type->setCurrentIndex(0);
    ui->scrollArea_device_type->setVisible(true);
}

void MainWindow::on_pushButton_gpio_cancel_clicked() {
    ui->scrollArea_gpio_config->setVisible(false);
    is_func = false;
}

void MainWindow::on_pushButton_gpio_apply_clicked() {

    QString str, str1;

    if(!is_func) {
        memcpy(device[cou_dev].Name ,ui->lineEdit_gpio_device_name->text().toLatin1(),ui->lineEdit_gpio_device_name->text().length());
        device[cou_dev].Name[ui->lineEdit_gpio_device_name->text().length()] = 0;
        device[cou_dev].is_meter = false;

        if(ui->comboBox_gpio_out_select_pin->isEnabled()) {
            str = ui->comboBox_gpio_out_select_pin->currentText();
            str1 = "OUT";
        }

        if(ui->comboBox_gpio_in_select_pin->isEnabled()) {
            if (str1 != "") {
                str = str + " / ";
            }
            str = str + ui->comboBox_gpio_in_select_pin->currentText();
            if (str1 != "")
                str1 = str1 + " / ";
            str1 = str1 + "IN";
        }

        QTableWidgetItem *gtwi = new QTableWidgetItem(str);
        int row = ui->tableWidget_device_table->rowCount();
        ui->tableWidget_device_table->setRowCount(row+1);
        int nom = ui->comboBox_gpio_in_select_pin->currentText().toInt();

        ui->tableWidget_device_table->setItem(row,0,gtwi);
        ui->scrollArea_gpio_config->setVisible(false);

        QTableWidgetItem *gtwi1 = new QTableWidgetItem(str1);
        ui->tableWidget_device_table->setItem(row,1,gtwi1);


        if(ui->comboBox_gpio_in_select_pin->isEnabled()) {

            gpio_m[nom] = 1;
            device[cou_dev].is_input = true;
            device[cou_dev].input_gpio = nom;
            device[cou_dev].input_vol = (ui->radioButton_gpio_in_default_pull_up->isChecked() ? 0:1);
            device[cou_dev].checkBox_gpio_in_logic_type = ui->checkBox_gpio_in_logic_type->isChecked();

            if(ui->radioButton_gpio_in_default_pull_up->isChecked()) {
                gtwi = new QTableWidgetItem("PU ");
            } else {
                gtwi = new QTableWidgetItem("PD ");
                gpio_m[nom] += 4;
            }
            ui->tableWidget_device_table->setItem(row,2,gtwi);

            gpio_m[nom] += ui->comboBox_device_type->currentIndex()*16;

        } else {
            device[cou_dev].input_gpio = 0;
        }

        if(ui->comboBox_gpio_out_select_pin->isEnabled()) {
            str = ui->comboBox_gpio_out_select_pin->currentText();
            int nom = str.toInt();
            gpio_m[nom] = 2;

            device[cou_dev].out_gpio = nom;
            device[cou_dev].out_vol = (ui->radioButton_gpio_out_default_low->isChecked() ? 0:1);
            device[cou_dev].checkBox_gpio_out_logic_type = ui->checkBox_gpio_out_logic_type->isChecked();

            if(ui->radioButton_gpio_out_default_pull_up->isChecked()){
                // gtwi = new QTableWidgetItem("PULL-UP");
            } else {
               // gtwi = new QTableWidgetItem("PULL-DOWN");
                gpio_m[nom] += 4;
            }

            if(ui->radioButton_gpio_out_default_low->isChecked()) {
                gtwi = new QTableWidgetItem("   0  ");
            } else {
                gtwi = new QTableWidgetItem("   1  ");
                gpio_m[nom] += 8;
            }
            ui->tableWidget_device_table->setItem(row,3,gtwi);
            gpio_m[nom] += ui->comboBox_device_type->currentIndex()*16;

        } else {
            device[cou_dev].out_gpio = 0;
        }

        gtwi = new QTableWidgetItem(ui->comboBox_device_type->currentText());
        ui->tableWidget_device_table->setItem(row,4,gtwi);
        device[cou_dev].dev_type = ui->comboBox_device_type->currentIndex();
        QString str_u = QUuid::createUuid().toString();
        str_u.remove(QRegularExpression("{|}|-")); // if you want only hex numbers
        qDebug() << str_u;
        for(int u=0;u<8;u++)
            device[cou_dev].dev_id[u]=str_u.at(u).toLatin1();
        device[cou_dev].dev_id[7] = 0;
        for(int u=8;u<16;u++)
            device[cou_dev].id_i[u-8]=str_u.at(u).toLatin1();
        device[cou_dev].id_i[7] = 0;
        device[cou_dev].roomId[0] = 0;
        qDebug() << "dev_id" << device[cou_dev].dev_id;
        qDebug() << "id_i" << device[cou_dev].id_i;
        cou_dev ++;
        device[cou_dev].Name[0] = 0;
        qDebug() << "Devise struc size" << sizeof(device_t);
    } else {
        int nom = ui->comboBox_gpio_in_select_pin->currentText().toInt();
        gpio_m[nom] = 1;
        if(ui->radioButton_gpio_in_default_pull_up->isChecked()) {
        } else {
            gpio_m[nom] += 4;
        }
        gpio_m[nom] += 0xF0;
        nom = ui->comboBox_gpio_out_select_pin->currentText().toInt();
        gpio_m[nom] = 2;
        if(ui->radioButton_gpio_out_default_pull_up->isChecked()) {
        } else {
            gpio_m[nom] += 4;
        }

        if(ui->radioButton_gpio_out_default_low->isChecked()) {
        } else {
            gpio_m[nom] += 8;
        }
        gpio_m[nom] += 0xF0;
        ui->scrollArea_gpio_config->setVisible(false);
        is_func = false;
    }
}

void MainWindow::on_pushButton_remove_device_clicked() {

    int row = ui->tableWidget_device_table->currentRow();
    if(row<0)
        return;

    QTableWidgetItem *gtwi1 = ui->tableWidget_device_table->item(row,0);
    QString str = gtwi1->text();
    qDebug() << "DELETE " << str;
    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("DELETE " + str + "\n");
    QString str1,str2;
    int nom,nom1;
    for (int i=0;i<cou_dev;i++) {
        str1 = "";
        nom = 0;
        nom1 = 0;
        if(device[i].out_gpio != 0) {
            str1=str2.asprintf("%d",device[i].out_gpio);
            nom = device[i].out_gpio;
        }

        if (device[i].input_gpio != 0) {
            nom1 = device[i].input_gpio;
            if(device[i].out_gpio != 0) {
                str1=str1 + " / ";
            }
            str1=str1 + str2.asprintf("%d",device[i].input_gpio);
        }
        qDebug() << "str"  << str << " str1 " << str1;
        if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("str"  + str + " str1 " + str1 + "\n");
        if(str.toLatin1() == str1.toLatin1()) {
            if (nom)
                gpio_m[nom] = 0;
            if (nom1)
                gpio_m[nom1] = 0;
            ui->tableWidget_device_table->removeRow(row);
            nom = cou_dev - i - 1;
            cou_dev--;

            if (nom>0)  memcpy(&device[i],&device[i+1],nom*sizeof(device_t));

            device[cou_dev].Name[0] = 0;
            device[cou_dev].out_gpio = 0;
            device[cou_dev].input_gpio = 0;
            device[cou_dev].is_meter = false;
            for (int j=0;j<cou_dev;j++) {
                qDebug() << "DEV" << j << " INPUT " << device[j].input_gpio << " OUT " << device[j].out_gpio;
            }
            return;
        }
    }
//    int nom = str.toInt();
//    gpio_m[nom] = 0;
//    ui->tableWidget_device_table->removeRow(row);
}

void MainWindow::on_pushButton_get_ezpi_config_clicked() {
    QByteArray buf;

    buf.append(0x95);
    buf.append(2);
    buf.append(0xa0);
    ezpi_serial_port.setBaudRate(460800);
    ezpi_serial_port.write(buf, 3);
    qDebug() << "Send" << buf;

    ezpi_serial_port.flush();

    connect(&ezpi_serial_port, &QSerialPort::readyRead, this, &MainWindow::on_serRX1);

}

void MainWindow::on_pushButton_set_ezpi_config_clicked() {

    uchar buf[60];

    buf[0]=0x95;
    buf[1]=30;
    buf[2]=0xa1;
    memcpy(&buf[3],gpio_m,28);
    ezpi_serial_port.write((const char*)buf,31);
    qDebug() << "Send" << buf;

    if(!device[0].Name[0])
        return;
    buf[1] = sizeof(device_t) + 2;

    connect(&ezpi_serial_port, &QSerialPort::readyRead, this, &MainWindow::on_serRX1);

//    memcpy(&buf[3],&device[0],sizeof(device_t));
//    ezpi_serial_port.write((const char*)buf,sizeof(device_t) + 3);
    for (int i=0;i<MAX_DEV;i++) {

        if(!device[i].Name[0]) {
            buf[1] = 2;
            buf[2] = END_DEV;
            ezpi_serial_port.write((const char*)buf,3);
            return;
        }

        if(i == 0)
            buf[2] = FIRST_DEV;
        else
            buf[2] = SET_DEV;

        memcpy(&buf[3],&device[i],sizeof(device_t));
        ezpi_serial_port.write((const char*)buf,sizeof(device_t) + 3);
        qDebug() << "Sending from QT: " << buf;
//        if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("Sending from QT: " + QString::fromWCharArray(buf) + "\n");
    }
}

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
        ezpi_serial_port.open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    } else if(str_StandardError.contains("Hard resetting") || str_StandardOutput.contains("Hard resetting")) {
        ezpi_message_box_failed_erase.information(this, "Success flashing the device!", "Flashing ezlopi firmware to the device was successful! You can now gracefully disconnect the device and move further!");
        ezpi_serial_port.open(QIODevice::ReadWrite);
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
        ezpi_serial_port.open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    } else if(str_StandardOutput.contains("Hard resetting") || str_StandardError.contains("Hard resetting")) {
        ezpi_message_box_failed_erase.information(this, "Success erasing the device!", "Erasing the device was successful! You can now proceed forward flashing the ezlopi firmware!");
        ezpi_serial_port.open(QIODevice::ReadWrite);
        ui->pushButton_connect_uart->setEnabled(true);
    }
}


void MainWindow::on_pushButton_flash_ezpi_bins_clicked() {

    ui->textBrowser_console_log->clear();

    ezpi_process_write_flash = new QProcess(this);

    connect(ezpi_process_write_flash, &QProcess::readyReadStandardOutput, this, &MainWindow::ezpi_log_write_flash);
    connect(ezpi_process_write_flash, &QProcess::readyReadStandardError, this, &MainWindow::ezpi_log_write_flash);

//    QString dir;
//    QString path_files = QDir::currentPath();

//    dir = path_files + "/esptool.exe";

//    path_files = QFileInfo(dir).absoluteDir().absolutePath();
//    qDebug() << path_files;
//    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(path_files + "\n");

//    QString fl = dir;

//    dir = path_files + "/bootloader.bin";

    #ifdef __linux__
            QString ser_port = "/dev/" + ezpi_serial_port_info.portName();
    #elif _WIN32
        QString ser_port = ezpi_serial_port_info.portName();
    #else

    #endif

    ezpi_serial_port.close();
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
                                "0x10000 ezpibins/esp_configs.bin " + \
                                "0x2E0000 ezpibins/ld.bin";


    qDebug() << "Current dir : " << QDir::currentPath();
    ui->textBrowser_console_log->append("Current dir: " + QDir::currentPath());


//    QString command = "";
//    for(auto args : arguments) {
//        command += args + " ";
//    }
    qDebug() << "Arguments : " << argument_string;

//    ezpi_process_write_flash->setArguments(arguments);
//    ezpi_process_write_flash->arguments();
    ezpi_process_write_flash->setNativeArguments(argument_string);
    ezpi_process_write_flash->start();
#if 0
    qDebug() << command;

    QString fl1 = " -p " + ser_port + " -b 460800 --before default_reset --after hard_reset --chip esp32  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 " +
            path_files + "/bootloader.bin 0x8000 " + path_files + "/partition-table.bin 0x10000 " + path_files + "/esp_configs.bin 0x2E0000 " + path_files + "/ld.bin";
    QString comm = fl + /* ui->comboBox_uart_list->currentText() + */ fl1;
    qDebug() << comm;
    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(comm + "\n");

    ezpi_process_write_flash->start(comm);
#endif
}

void MainWindow::on_pushButton_clear_uart_direct_log_clicked() {
    ui->textBrowser_console_log->clear();
}

void MainWindow::on_pushButton_set_wifi_clicked() {
    ui->lineEdit_set_wifi_ssid->setText("");
    ui->lineEdit_set_wifi_pass->setText("");
    ui->scrollArea_set_wifi_cred->setVisible(true);
}

void MainWindow::on_pushButton_set_wifi_cancel_clicked() {
    ui->scrollArea_set_wifi_cred->setVisible(false);
}

void MainWindow::on_pushButton_set_wifi_apply_clicked() {
    uchar buf[100] = {0};

    memcpy(buf+3,ui->lineEdit_set_wifi_ssid->text().toStdString().c_str(),ui->lineEdit_set_wifi_ssid->text().length());
    buf[3+ui->lineEdit_set_wifi_ssid->text().length()] = 0;
    memcpy(buf+35, ui->lineEdit_set_wifi_pass->text().toStdString().c_str(),ui->lineEdit_set_wifi_pass->text().length());
    buf[35+ui->lineEdit_set_wifi_pass->text().length()] = 0;
    buf[0]=0x95;
    buf[1]=98;
    buf[2]=SET_WiFi & 0xff;
    ezpi_serial_port.write((const char*)buf,99);
    QString str1;
    str1 = str1.asprintf("%s",buf);
    qDebug() << "Send[SSID]: " << &str1[3];
    qDebug() << "Send[PASS]: " << &str1[35];
    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append("Send" + str1 + "\n");
    ui->scrollArea_set_wifi_cred->setVisible(false);
}

void MainWindow::on_pushButton_select_device_type_apply_clicked() {

    QString str;

    ui->scrollArea_device_type->setVisible(false);

    if(ui->comboBox_device_type->currentIndex() >= DEV_TYPE_TAMPER) {
        QMessageBox::information(this,"Unsupported","Not yet supported");
        return;
    }

    ui->comboBox_gpio_out_select_pin->clear();

    for(int i = 0; i < EZPI_MAX_GPIOS; i++) {
        if(gpio_m[i] == DEV_NONE) {
            ui->comboBox_gpio_out_select_pin->addItem(str.asprintf("%d",i));
        }
    }

    int gp_i = ui->comboBox_gpio_out_select_pin->currentText().toInt();

    ui->comboBox_gpio_in_select_pin->clear();

    for(int i = 0; i < EZPI_MAX_GPIOS; i++) {
        if(gpio_m[i] == DEV_NONE) {
            if(i!=gp_i) {
                ui->comboBox_gpio_in_select_pin->addItem(str.asprintf("%d",i));
            }
        }
    }

    ui->groupBox_gpio_our_default_bool->setEnabled(true);
    ui->lineEdit_gpio_device_name->setText(ui->comboBox_device_type->currentText());
    ui->comboBox_gpio_out_select_pin->setEnabled(true);
    ui->groupBox_gpio_our_default_shunt_res->setEnabled(true);
    ui->groupBox_gpio_our_default_bool->setEnabled(true);

    if(ui->comboBox_device_type->currentIndex() == DEV_TYPE_LED) {

        ui->checkBox_gpio_led_button_enable->setChecked(false);

    } else {

        ui->checkBox_gpio_led_button_enable->setChecked(true);

        if(ui->comboBox_device_type->currentIndex() == DEV_TYPE_TAMPER) {

            ui->comboBox_gpio_out_select_pin->clear();
            ui->comboBox_gpio_out_select_pin->setEnabled(false);
            ui->groupBox_gpio_our_default_shunt_res->setEnabled(false);
            ui->groupBox_gpio_our_default_bool->setEnabled(false);
        }
    }

    ui->radioButton_gpio_out_default_pull_up->setChecked(true);
    ui->radioButton_gpio_out_default_low->setChecked(true);


    ui->scrollArea_gpio_config->setVisible(true);
}

void MainWindow::on_pushButton_select_device_type_cancel_clicked() {
    ui->scrollArea_device_type->setVisible(false);
}

void MainWindow::on_comboBox_gpio_out_select_pin_currentTextChanged(const QString &arg1) {
    if (is_work)
        return;
    is_work = true;
    QString str,str2;
    str2 = ui->comboBox_gpio_in_select_pin->currentText();
    int gp_i = ui->comboBox_gpio_out_select_pin->currentText().toInt();
    ui->comboBox_gpio_in_select_pin->clear();
    for(int i=0;i<27;i++) {
        if(gpio_m[i] == 0) {
            if(i!=gp_i) {
                ui->comboBox_gpio_in_select_pin->addItem(str.asprintf("%d",i));
            }
        }
    }
    if(arg1!=str2) {
        ui->comboBox_gpio_in_select_pin->setCurrentText(str2);
    }
    is_work = false;
}

void MainWindow::on_comboBox_gpio_in_select_pin_currentTextChanged(const QString &arg1) {
    if (is_work)
        return;
    is_work = true;
    QString str,str2;
    str2 = ui->comboBox_gpio_out_select_pin->currentText();
    int gp_i = ui->comboBox_gpio_in_select_pin->currentText().toInt();
    ui->comboBox_gpio_out_select_pin->clear();
    for(int i=0;i<27;i++) {
        if(gpio_m[i] == 0) {
            if(i!=gp_i) {
                ui->comboBox_gpio_out_select_pin->addItem(str.asprintf("%d",i));
            }
        }
    }

    if(arg1!=str2) {
        ui->comboBox_gpio_out_select_pin->setCurrentText(str2);
    }
    is_work = false;
}

void MainWindow::on_checkBox_gpio_led_button_enable_stateChanged() {

    if (ui->checkBox_gpio_led_button_enable->isChecked()) {

        ui->comboBox_gpio_in_select_pin->setEnabled(true);
        ui->groupBox_gpio_in_default_shunt_res->setEnabled(true);
        ui->groupBox_gpio_in_default_bool->setEnabled(true);
        ui->radioButton_gpio_in_default_pull_up->setChecked(true);
        ui->radioButton_gpio_in_default_low->setChecked(true);
        ui->checkBox_gpio_in_logic_type->setEnabled(true);

    } else {

        ui->comboBox_gpio_in_select_pin->setEnabled(false);
        ui->groupBox_gpio_in_default_shunt_res->setEnabled(false);
        ui->groupBox_gpio_in_default_bool->setEnabled(false);
        ui->checkBox_gpio_in_logic_type->setEnabled(false);
    }
}

void MainWindow::on_pushButton_erase_flash_clicked() {

    ezpi_process_erase_flash = new QProcess(this);

    connect(ezpi_process_erase_flash, &QProcess::readyReadStandardOutput, this, &MainWindow::ezpi_log_erase_flash);
    connect(ezpi_process_erase_flash, &QProcess::readyReadStandardError, this, &MainWindow::ezpi_log_erase_flash);

//    QString dir;
//    QString path_files = QDir::currentPath();

//    dir = path_files + "/esptool.exe ";

//    qDebug() << dir;
//    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(dir + "\n");

//    path_files = QFileInfo(dir).absoluteDir().absolutePath();
//    qDebug() << path_files;
//    if(ezpi_flag_enable_log) ui->textBrowser_console_log->append(path_files + "\n");

//    QString fl = dir;

    ezpi_serial_port.close();
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

void MainWindow::on_actionExit_triggered() {
    QApplication::quit();
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

void MainWindow::on_actionAbout_EzloPi_triggered() {

    QMessageBox::about(this, "EzloPi", "EzloPi V0.0.1");

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

        if(QDateTime::currentSecsSinceEpoch() < login_expires) {
//            ui->textBrowser_console_log->append("Login Success !");
//            if(flag_login) qDebug() << "Login Success !";
            qDebug() << "Token: " << user_token;

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

void MainWindow::success_prov_dat(QNetworkReply *d) {
#if 1
    qDebug() << "Added new device";
    ui->textBrowser_console_log->append("Added new device!");
    QByteArray response_bytes = d->readAll();

//    qDebug() << response_bytes;


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

        // Get prov lists :
   //     {"call":"self_provision_get","params":{"uuid":"131d03bf-28ae-4e72-b63b-47a18e1af2f5"}}
       #endif

   //     user_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ0b2tlbiI6eyJ1dWlkIjoiOWZjMTg2ZTAtZjYxMS0xMWVjLWE5ZjctMDdhYjAxODFiNTA0IiwiZXhwaXJlc190cyI6MTY1NjQxODAwOSwiZ2VuZXJhdGVkX3RzIjoxNjU2MzMxNjA5LCJ0eXBlIjoidXNlciJ9LCJsb2NhdGlvbiI6eyJpZCI6NTAyMDU5MiwidXVpZCI6IjUzOWQzZmEwLWYyY2ItMTFlYy04ZjgwLTNmZDVhM2JhYjcxNyJ9LCJvZW0iOnsiaWQiOjF9LCJ1c2VyIjp7ImlkIjo1MDIwOTQyLCJ1dWlkIjoiNTM4ODdmMjAtZjJjYi0xMWVjLWFhMTktNDdlZjVlOWVhOTMxIn0sImxlZ2FjeSI6eyJhY2NvdW50Ijp7ImlkIjo1MDIwNTkyfX0sImlhdCI6MTY1NjMzMTYxMCwiZXhwIjoxNjU2NDE4MDA4fQ.GODCEgk2pr4ZxC-vtSiobCjqG3DvFY16ic1ityRzi9Y";
        QJsonObject jobj_get_prov_json;
        QJsonObject jobj_param;

        jobj_param["uuid"] = uuid;
   //     jobj_param["uuid"] = "7a3424be-d62c-4305-9184-72c95057e9f9";
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

void MainWindow::success_get_prov_jsons(QNetworkReply *d) {

#if 1
     conv_u16_array_t ser_ver;
     conv_64_array_t id;

    struct uuid _uuid;

    QByteArray response_bytes = d->readAll();

//    qDebug() << response_bytes;


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
//        ui->textBrowser_console_log->append( QString::fromStdString("provision_token: ") + jobj_prov_data_prov_data["provision_token"].toString());
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
//    out.write(ld_binary_array);
    out.close();
#endif
}

void MainWindow::on_actionLogin_triggered() {
    ezpi_form_login->setModal(true);
    ezpi_form_login->show();
}

>>>>>>> master
