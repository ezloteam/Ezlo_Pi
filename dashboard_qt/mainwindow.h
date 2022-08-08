#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTimer>
#include <QProcess>

#include<login.h>
#include<dialog_wifi.h>
#include<dialog_devadd.h>
#include<dialog_configdev_digitalio.h>
#include<dialog_config_input.h>
#include<dialog_config_onewire.h>
#include<dialog_config_i2c.h>
#include<dialog_config_spi.h>


#include "ezpi_data_types.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    // Custom slots
    void ezpi_log_write_flash();
    void ezpi_log_erase_flash();
    void ezpi_check_firmware();
    void ezpi_message_info_no_firmware_detected();
    void ezpi_receive_dev_type_selected(EZPI_UINT8);

    void on_serRX1();

    void on_pushButton_connect_uart_clicked();

    void on_pushButton_add_device_clicked();

    void on_pushButton_gpio_cancel_clicked();

    void on_pushButton_gpio_apply_clicked();

    void on_pushButton_remove_device_clicked();

    void on_pushButton_get_ezpi_config_clicked();

    void on_pushButton_set_ezpi_config_clicked();

    void on_pushButton_flash_ezpi_bins_clicked();

    void on_pushButton_clear_uart_direct_log_clicked();

    void on_pushButton_set_wifi_clicked();

    void on_pushButton_set_wifi_cancel_clicked();

    void on_pushButton_set_wifi_apply_clicked();

    void on_pushButton_select_device_type_apply_clicked();

    void on_pushButton_select_device_type_cancel_clicked();

    void on_comboBox_gpio_out_select_pin_currentTextChanged(const QString &arg1);

    void on_comboBox_gpio_in_select_pin_currentTextChanged(const QString &arg1);

    void on_checkBox_gpio_led_button_enable_stateChanged();

    void on_pushButton_erase_flash_clicked();

    void on_pushButton_scan_uart_ports_clicked();

    void on_comboBox_uart_list_currentIndexChanged();

    void on_actionExit_triggered();

    void on_actionEnable_Log_triggered();

    void on_actionAbout_EzloPi_triggered();

    void on_actionRegister_triggered();

    void success_prov_dat(QNetworkReply *d);
    void success_get_prov_jsons(QNetworkReply *d);

    void on_actionLogin_triggered();

    void on_comboBox_esp32_board_type_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    // Variables for port management
    std::vector<QSerialPortInfo> ezpi_serial_ports_info_list;
    QSerialPortInfo ezpi_serial_port_info;
    QSerialPort ezpi_serial_port;
    bool ezpi_flag_serial_port_open;

    // Timers
    QTimer ezpi_timer_ask_info;

    // flags
    bool ezpi_flag_is_ezlopi;

    QProcess * ezpi_process_erase_flash;
    QProcess * ezpi_process_write_flash;

    uint8_t ezpi_device_counter;

    bool ezpi_flag_enable_log;

    void pars(uchar *buf, int len);

    QString user_token;

    bool ezpi_flag_registered;

    login * ezpi_form_login;
    Dialog_WiFi * ezpi_form_WiFi;
    Dialog_devadd * ezpi_form_devadd;
    Dialog_config_input * ezpi_form_config_digital_ip;
    Dialog_configdev_digitalio * ezpi_form_configdev_digitalio;
    Dialog_config_onewire * ezpi_form_config_onewire;
    Dialog_config_i2c * ezpi_form_config_i2c;
    Dialog_config_spi * ezpi_form_config_spi;

    EzPi * EzloPi;

};

#endif // MAINWINDOW_H
