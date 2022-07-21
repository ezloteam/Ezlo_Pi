#include "login.h"
#include "ui_login.h"

#include<QDebug>
#include<QMessageBox>
#include<QtNetwork>

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login),
    ezpi_flag_user_login(false),
    ezpi_token_expiry_time(0)
{
    ui->setupUi(this);
    // Initialize user token
    ezpi_user_token.clear();
}

login::~login() {
    delete ui;
}

void login::on_buttonBox_accepted() {
    qDebug() << "user name : " << ui->lineEdit_user_name->text();
    qDebug() << "Password : " << ui->lineEdit_password->text();
}

void login::on_pushButton_login_clicked() {

#if 1

    if((ui->lineEdit_user_name->text().length() < 1) || ui->lineEdit_password->text().length() < 1) {

        QMessageBox::warning(this, "Invalid credentials!", "The credentials entered is invalid!");

    } else {

//        memcpy(ezpi_user_name, ui->lineEdit_user_name->text().toLatin1(), ui->lineEdit_user_name->text().length());
//        ezpi_user_name[ui->lineEdit_user_name->text().length()] = 0;

//        memcpy(ezpi_user_pass, ui->lineEdit_password->text().toLatin1(), ui->lineEdit_password->text().length());
//        ezpi_user_name[ui->lineEdit_password->text().length()] = 0;

        ezpi_user_name = ui->lineEdit_user_name->text();
        ezpi_user_pass = ui->lineEdit_password->text();


        // Do the network tasks in here
        QUrl url("https://api-cloud.ezlo.com/v1/request");
        QNetworkRequest request(url);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);

        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(replyFinished(QNetworkReply*)));

        QJsonObject obj_param;
        obj_param["user_id"] = ezpi_user_name;
        obj_param["user_password"] = ezpi_user_pass;

        QJsonObject obj_main;
        obj_main["call"] = "login_with_id_and_password";
        obj_main["params"] = obj_param;

        QJsonDocument doc(obj_main);

        QByteArray data = doc.toJson(QJsonDocument::Compact);

        // FIXME for debug
        qDebug() << "Sync" << QString::fromUtf8(data.data(), data.size());

        manager->post(request, data);
    }

#else
        QUrl url("https://api-cloud.ezlo.com/v1/request");
        QNetworkRequest request(url);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);

        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(replyFinished(QNetworkReply*)));

        QJsonObject obj_param;
        obj_param["user_id"] = "ezlopitest001";
        obj_param["user_password"] = "ezlopitest001!2";

        QJsonObject obj_main;
        obj_main["call"] = "login_with_id_and_password";
        obj_main["params"] = obj_param;

        QJsonDocument doc(obj_main);

        QByteArray data = doc.toJson(QJsonDocument::Compact);

        // FIXME for debug
        qDebug() << "Sync" << QString::fromUtf8(data.data(), data.size());

        manager->post(request, data);
#endif
}

void login::on_pushButton_cancle_clicked() {
    QDialog::reject();
}

void login::replyFinished(QNetworkReply *rep) {

    QByteArray response_bytes = rep->readAll();

//    qDebug() << response_bytes;

#if 1

        QJsonParseError jerror;

        QJsonDocument jdoc= QJsonDocument::fromJson(response_bytes, &jerror);

        if(jerror.error != QJsonParseError::NoError) {
            // Message failed parcing json
            qDebug() << "Message failed parcing json";
            return;
        }

         QJsonObject jobj_main = jdoc.object();

          uint8_t status = jobj_main["status"].toInt();
          uint8_t complete = jobj_main["complete"].toInt();
          QJsonObject jobj_data = jobj_main["data"].toObject();
          QString token = jobj_data["token"].toString();
          uint64_t expiary = jobj_data["expires"].toInt();
          QJsonObject legacy_token = jobj_data["legacy_token"].toObject();
          QString auth = legacy_token["auth"].toString();
          QString sig = legacy_token["sig"].toString();


          if( (status == 1) && (complete == 1) ) {

              ezpi_flag_user_login  = true;
              ezpi_token_expiry_time = expiary;
              ezpi_user_token = token;

              QDialog::accept();

              QMessageBox::information(this, "Login Success !", "Login to ezlo cloud has been successful!");

          } else {
              // Incomplete
              qDebug() << "Incomplete and unknown status !";
              QMessageBox::information(this, "Login Failed !", "Failed login to ezlo cloud!");
          }
#endif
}
