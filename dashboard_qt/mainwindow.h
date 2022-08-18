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

    // Custom slots
    void ezpi_log_write_flash();
    void ezpi_log_erase_flash();
    EZPI_BOOL ezpi_check_firmware();
    void ezpi_message_info_no_firmware_detected();
    void ezpi_receive_dev_type_selected(EZPI_UINT8);
    void ezpi_receive_added_dev(ezpi_dev_type);

//    void ezpi_serial_receive(void);

    void on_pushButton_connect_uart_clicked();

    void on_pushButton_add_device_clicked();

    void on_pushButton_remove_device_clicked();

    void on_pushButton_get_ezpi_config_clicked();

    void on_pushButton_set_ezpi_config_clicked();

    void on_pushButton_flash_ezpi_bins_clicked();

    void on_pushButton_clear_uart_direct_log_clicked();

    void on_pushButton_set_wifi_clicked();

    void on_pushButton_select_device_type_apply_clicked();

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

    void ezpi_serial_receive_wif(ezpi_cmd);

private:
    Ui::MainWindow *ui;

    // Variables for port management
    std::vector<QSerialPortInfo> ezpi_serial_ports_info_list;

    QSerialPortInfo ezpi_serial_port_info;

    QSerialPort * ezpi_serial_port;

    bool ezpi_flag_serial_port_open;

    // Timers
    QTimer ezpi_timer_ask_info;
    QTimer ezpi_timer_serial_complete;

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

    void ezlogic_table_adddev_digital_op(ezlogic_device_digital_op_t);
    void ezlogic_table_adddev_digital_ip(ezlogic_device_digital_ip_t);
    void ezlogic_table_adddev_onewire(ezlogic_device_one_wire_t);
    void ezlogic_table_adddev_i2c(ezlogic_device_I2C_t);
    void ezlogic_table_adddev_spi(ezlogic_device_SPI_t);

    void ezpi_serial_transfer(QByteArray d);
    void ezpi_serial_process(void);

    void ezpi_action_check_info(QByteArray);
    void ezpi_action_set_config_process(QByteArray);
    void ezpi_action_get_config_process(QByteArray);
    void ezpi_action_set_wifi(QByteArray);

    std::vector<ezpi_dev_type> ezlogic_table_row_device_map;

    QByteArray * ezpi_read_data_serial;
//    QByteArray * read_data_serial;

    ezpi_cmd ezpi_cmd_state;

    EZPI_BOOL ezpi_fimware_present;
public slots:
    void ezpi_serial_receive(void);

};

#endif // MAINWINDOW_H
