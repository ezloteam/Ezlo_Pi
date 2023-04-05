#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTimer>
#include <QProcess>

#include<dialog_login.h>
#include<dialog_wifi.h>
#include<dialog_devadd.h>
#include<dialog_configdev_digitalio.h>
#include<dialog_config_input.h>
#include<dialog_config_adc.h>
#include<dialog_config_pwm.h>
#include<dialog_config_uart.h>
#include<dialog_config_onewire.h>
#include<dialog_config_i2c.h>
#include<dialog_config_spi.h>
#include<dialog_config_other.h>

// Ezlo Pi Specefic header
#include "ezlopi.h"

enum ezlogic_table_column {
    EZLOZIC_TABLE_COLUMN_DEV_NAME,
    EZLOZIC_TABLE_COLUMN_DEV_TYPE,
    EZLOZIC_TABLE_COLUMN_ITEM_TYPE,
    EZLOZIC_TABLE_COLUMN_GPIOS,
    EZLOZIC_TABLE_COLUMN_TOTAL
};

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

    // Autogenerated UI slots
    void on_pushButton_scan_uart_ports_clicked();
    void on_pushButton_connect_uart_clicked();
    void on_comboBox_uart_list_currentIndexChanged();
    void on_pushButton_set_wifi_clicked();
    void on_pushButton_erase_flash_clicked();
    void on_pushButton_flash_ezpi_bins_clicked();
    void on_pushButton_clear_uart_direct_log_clicked();    
    void on_comboBox_esp32_board_type_currentIndexChanged(int index);
    void on_pushButton_add_device_clicked();
    void on_pushButton_remove_device_clicked();
    void on_pushButton_get_ezpi_config_clicked();
    void on_pushButton_set_ezpi_config_clicked();
    void on_pushButton_device_restart_clicked();

    // Custom slots
    void ezlogic_log_write_flash();
    void ezlogic_log_erase_flash();
    EZPI_BOOL ezlogic_check_firmware();
    void ezlogic_message_info_no_firmware_detected();
    void ezlogic_receive_dev_type_selected(EZPI_UINT8);
    void ezlogic_receive_added_dev(ezpi_dev_type);
    void ezlogic_serial_receive_wif(ezpi_cmd);
    void ezlogic_success_prov_dat(QNetworkReply *d);
    void ezlogic_success_get_prov_jsons(QNetworkReply *d);

    // UI Actions
    void on_actionRegister_triggered();
    void on_actionLogin_triggered();
    void on_actionAbout_EzloPi_triggered();
    void on_actionExit_triggered();
    void on_actionClear_Table_triggered();
    void on_actionDisable_triggered();
    void on_actionInfo_triggered();
    void on_actionDebug_triggered();

private:

    Ui::MainWindow *ui;

    login * ezlogic_form_login;
    Dialog_WiFi * ezlogic_form_WiFi;
    Dialog_devadd * ezlogic_form_devadd;
    Dialog_configdev_digitalio * ezlogic_form_configdev_digitalio;
    Dialog_config_input * ezlogic_form_config_digital_ip;
    Dialog_config_adc * ezlogic_form_config_analog_ip;
    Dialog_config_pwm * ezlogic_form_config_pwm;
    Dialog_config_uart * ezlogic_form_config_uart;
    Dialog_config_onewire * ezlogic_form_config_onewire;
    Dialog_config_i2c * ezlogic_form_config_i2c;
    Dialog_config_spi * ezlogic_form_config_spi;
    Dialog_config_other * ezlogic_form_config_other;

    // EzloPi object
    EzPi * EzloPi;

    // Variables for port management
    std::vector<QSerialPortInfo> ezlogic_serial_ports_info_list;
    QSerialPortInfo ezlogic_serial_port_info;
    QSerialPort * ezlogic_serial_port;

    EZPI_BOOL ezlogic_flag_serial_port_open;

    // Timers
    QTimer ezlogic_timer_ask_info;
    QTimer ezlogic_timer_serial_complete;

    // flags
    EZPI_BOOL ezlogic_flag_is_ezlopi;

    // External flasher program process handlers
    QProcess * ezlogic_process_erase_flash;
    QProcess * ezlogic_process_write_flash;

    // Total number of device counter
    uint8_t ezlogic_device_counter;

    // Console log flag
//    EZPI_BOOL ezlogic_flag_enable_log;

    // Provisioning task global variables
    QString ezlogic_prov_data_user_token;
    bool ezlogic_flag_registered;

    // EzloPi interfacing device storage for UI
    std::vector<ezpi_dev_type> ezlogic_table_row_device_map;

    // Serial data storage
    QByteArray * ezlogic_read_data_serial;

    // Command type at the time of UI interactivity
    ezpi_cmd ezlogic_cmd_state;

    // Flag if the connected device is EzloPi or not
    EZPI_BOOL ezlogic_flag_fimware_present;

    QLabel *ezlogic_status = nullptr;

    QString  ezlogic_uuid_self_prov;

    ezpi_log_level ezlogic_log_level;

    // Private methods
    void ezlogic_table_adddev_digital_op(ezpi_device_digital_op_t);
    void ezlogic_table_adddev_digital_ip(ezpi_device_digital_ip_t);
    void ezlogic_table_adddev_analog_ip(ezpi_device_analog_ip_t);
    void ezlogic_table_adddev_pwm(ezpi_device_pwm_t);
    void ezlogic_table_adddev_uart(ezpi_device_uart_t);
    void ezlogic_table_adddev_onewire(ezpi_device_one_wire_t);
    void ezlogic_table_adddev_i2c(ezpi_device_I2C_t);
    void ezlogic_table_adddev_spi(ezpi_device_SPI_t);
    void ezlogic_table_adddev_other(ezpi_device_other_t);

    void ezlogic_action_restart(QByteArray);
    void ezlogic_action_set_wifi(QByteArray);
    void ezlogic_action_check_info(QByteArray);
    void ezlogic_action_set_config_process(QByteArray);
    void ezlogic_action_get_config_process(QByteArray);

    void ezlogic_serial_transfer(QByteArray d);
    void ezlogic_serial_process(void);
    void ezlogic_clear_table_data(void);
    void ezpi_show_status_message(const QString&);
    void ezpi_update_dev_list(void);

public slots:
    void ezlogic_serial_receive(void);

};

#endif // MAINWINDOW_H
