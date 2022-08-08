#include <QDebug>
#include "dialog_wifi.h"
#include "ui_dialog_wifi.h"

Dialog_WiFi::Dialog_WiFi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_WiFi)
{
    ui->setupUi(this);
}

Dialog_WiFi::~Dialog_WiFi()
{
    delete ui;
}

void Dialog_WiFi::on_checkBox_view_password_stateChanged(int arg1) {
    if(arg1 == Qt::Unchecked) {
        ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
    } else if(arg1 == Qt::Checked) {
        ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Normal);
    }
}


//void Dialog_WiFi::on_checkBox_view_password_pressed() {
//    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Normal);
//}


//void Dialog_WiFi::on_checkBox_view_password_released() {
//    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::PasswordEchoOnEdit);
//}

