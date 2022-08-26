#ifndef DIALOG_WIFI_H
#define DIALOG_WIFI_H

#include <QDialog>
#include <QSerialPort>

namespace Ui {
class Dialog_WiFi;
}

class Dialog_WiFi : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_WiFi(QWidget *parent = nullptr, QSerialPort * ezpi_serial_port = nullptr);
    ~Dialog_WiFi();

private slots:
    void on_checkBox_view_password_stateChanged(int arg1);
    void on_buttonBox_accepted();
    void process_response(QString data_response_set_wifi);

private:
    Ui::Dialog_WiFi *ui;
    QSerialPort * ezpi_serial_wifi;

};

#endif // DIALOG_WIFI_H
