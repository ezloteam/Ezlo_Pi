#include <QDebug>
#include <QMessageBox>
#include "dialog_wifi.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "ui_dialog_wifi.h"

Dialog_WiFi::Dialog_WiFi(QWidget *parent, QSerialPort * ezpi_serial_port) :
    QDialog(parent),
    ui(new Ui::Dialog_WiFi)  
{    
    ui->setupUi(this);
    data_action_set_wifi_char_array = new QByteArray;
    ezpi_serial_wifi = ezpi_serial_port;
}

Dialog_WiFi::~Dialog_WiFi()
{
    delete ui;
    delete data_action_set_wifi_char_array;
}

void Dialog_WiFi::on_checkBox_view_password_stateChanged(int arg1) {
    if(arg1 == Qt::Unchecked) {
        ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
    } else if(arg1 == Qt::Checked) {
        ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Normal);
    }
}


void Dialog_WiFi::on_buttonBox_accepted() {

    if((ui->lineEdit_SSID->text().length() < 1) || ui->lineEdit_password->text().length() < 1) {

        QMessageBox::warning(this, "Invalid WiFi credentials!", "The SSID or password entered is not valid!");

    } else {

        // Send SSID and password
        EZPI_STRING ezpi_wifi_ssid = ui->lineEdit_SSID->text();
        EZPI_STRING ezpi_wifi_pass = ui->lineEdit_password->text();

        QJsonObject obj_action_set_wifi;

        obj_action_set_wifi["cmd"] = CMD_ACTION_SET_WIFI;
        obj_action_set_wifi["ssid"] = ezpi_wifi_ssid;
        obj_action_set_wifi["pass"] = ezpi_wifi_pass;

        QJsonDocument doc_action_set_wifi(obj_action_set_wifi);
        *data_action_set_wifi_char_array = doc_action_set_wifi.toJson(QJsonDocument::Compact);

        ezpi_wifi_serial_transfer(*data_action_set_wifi_char_array);
    }
}


void Dialog_WiFi::ezpi_wifi_serial_transfer(QByteArray d) {
    ezpi_serial_wifi->write(d.constData());
    ezpi_serial_wifi->flush();
    emit ezpi_signal_serial_rx_wifi(CMD_ACTION_SET_WIFI);
}
