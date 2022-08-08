#ifndef DIALOG_WIFI_H
#define DIALOG_WIFI_H

#include <QDialog>

namespace Ui {
class Dialog_WiFi;
}

class Dialog_WiFi : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_WiFi(QWidget *parent = nullptr);
    ~Dialog_WiFi();

private slots:
    void on_checkBox_view_password_stateChanged(int arg1);
//    void on_checkBox_view_password_pressed();

//    void on_checkBox_view_password_released();

private:
    Ui::Dialog_WiFi *ui;
};

#endif // DIALOG_WIFI_H
