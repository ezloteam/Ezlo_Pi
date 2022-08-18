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

    read_data_serial = new QByteArray;
    data_action_set_wifi_char_array = new QByteArray;

//    QString read_data_serial;
    ezpi_serial_wifi = ezpi_serial_port;
//    connect(ezpi_serial_wifi, &QSerialPort::readyRead, this, &Dialog_WiFi::serialConnector);

//    ezpi_wif_rx_timer.callOnTimeout(this, &MainWindow::ezpi_serial_receive);
}

Dialog_WiFi::~Dialog_WiFi()
{
    delete ui;
    delete read_data_serial;
    delete data_action_set_wifi_char_array;
//    delete ezpi_serial_wifi;
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

#if 0
        read_data_serial = new QByteArray;

        ezpi_serial_wifi->write(data_action_set_wifi_char_array);
        ezpi_serial_wifi->flush();
        if(ezpi_serial_wifi->waitForBytesWritten()) {
            ezpi_wif_rx_timer.start(EZPI_SERIAL_READ_TIMEOUT);
        }
#endif

#if 0
        if (ezpi_serial_wifi->waitForBytesWritten()) {
            // read response
            if (ezpi_serial_wifi->waitForReadyRead(5000)) {
                QByteArray responseData = ezpi_serial_wifi->readAll();
                while (ezpi_serial_wifi->waitForReadyRead(10))
                    responseData += ezpi_serial_wifi->readAll();

                const EZPI_STRING response = QString::fromUtf8(responseData);
                qDebug() << "Response : " << response;
                process_response(response);
            } else {
                QMessageBox::warning(this, "Request time out!", "No response from the device !\n Connection status unknown !");
            }
        } else {

        }
#endif
    }
}

void Dialog_WiFi::process_response(QString data_response_set_wifi) {
#if 0
    QString response_data = data_response_set_wifi;

//    qDebug() << response_data;

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
        if(json_map_root_set_wifi_response["status_write"].toUInt() == 1) {
            if(json_map_root_set_wifi_response["status_connect"].toUInt() >= 1) {
                QMessageBox::information(this, "Success!", "WiFi connection successful.");
            } else {
                QMessageBox::warning(this, "Connection failed!", "WiFi connection failed.");
            }
        } else {
            QMessageBox::warning(this, "Connection failed!", "WiFi connection failed.");
        }
    } else {
        QMessageBox::warning(this, "Error!", "Unknown command received, WiFi: unknown status!");
    }
#endif
}

void Dialog_WiFi::serialConnector(void) {
//    *read_data_serial += ezpi_serial_wifi->readAll();
//    MainWindow::ezpi_serial_receive();
//    emit ezpi_signal_serial_rx_wifi(CMD_ACTION_SET_WIFI);
}

void Dialog_WiFi::ezpi_wifi_accumulate_serial_msg(void) {
//    ezpi_wif_rx_timer.stop();
//    process_response(EZPI_STRING::fromLocal8Bit(*read_data_serial));
#if 0
    if(read_data_serial->length() > 0) {
        qDebug() << "Serial read WiFi : " << QString::fromLocal8Bit(*read_data_serial);
        process_response(EZPI_STRING::fromLocal8Bit(*read_data_serial));
    } else {
        ezpi_wifi_serial_transfer(*data_action_set_wifi_char_array);
    }
#endif
}

void Dialog_WiFi::ezpi_wifi_serial_transfer(QByteArray d) {
    ezpi_serial_wifi->write(d.constData());
    ezpi_serial_wifi->flush();
    emit ezpi_signal_serial_rx_wifi(CMD_ACTION_SET_WIFI);
//    if(ezpi_serial_wifi->waitForBytesWritten()) {
//        ezpi_wif_rx_timer.start(EZPI_SERIAL_READ_TIMEOUT);
//    }
}
