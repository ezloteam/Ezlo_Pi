#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include<QtNetwork>


#define         SIZE_LOGIN_USER_NAME    30
#define         SIZE_LOGIN_PASSWORD     20

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

    QString ezpi_get_user_access_token() {return ezpi_user_token; }
    bool ezpi_get_flag_user_login() { return ezpi_flag_user_login; }
    uint64_t ezpi_get_token_expiry_time() { return ezpi_token_expiry_time; }

private slots:
    void on_buttonBox_accepted();

    void on_pushButton_login_clicked();

    void on_pushButton_cancle_clicked();

    void replyFinished(QNetworkReply *);

private:
    Ui::login *ui;
    QString ezpi_user_name;
    QString ezpi_user_pass;

    QString ezpi_user_token;
    bool ezpi_flag_user_login;
    uint64_t ezpi_token_expiry_time;
};

#endif // LOGIN_H
