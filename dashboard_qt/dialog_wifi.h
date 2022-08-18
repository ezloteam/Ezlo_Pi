#ifndef DIALOG_WIFI_H
#define DIALOG_WIFI_H

#include <QDialog>
#include <QSerialPort>
#include <QTimer>
#include "ezpi_data_types.h"

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
    void serialConnector(void);

private:
    Ui::Dialog_WiFi *ui;
    QSerialPort * ezpi_serial_wifi;
    QTimer ezpi_wif_rx_timer;
    QByteArray * read_data_serial;

    QByteArray * data_action_set_wifi_char_array;

    EzPi * ezpi_set_wif;

    void ezpi_wifi_accumulate_serial_msg();
    void ezpi_wifi_serial_transfer(QByteArray);

signals:
    void ezpi_signal_serial_rx_wifi(ezpi_cmd);

};

#endif // DIALOG_WIFI_H
