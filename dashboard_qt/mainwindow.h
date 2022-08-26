<<<<<<< HEAD
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

private:
    Ui::MainWindow *ui;

    // Variables for port management
    std::vector<QSerialPortInfo> ezpi_serial_ports_info_list;

    QSerialPortInfo ezpi_serial_port_info;

    QSerialPort * ezpi_serial_port;

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

    void ezlogic_table_adddev_digital_op(ezlogic_device_digital_op_t);
    void ezlogic_table_adddev_digital_ip(ezlogic_device_digital_ip_t);
    void ezlogic_table_adddev_onewire(ezlogic_device_one_wire_t);
    void ezlogic_table_adddev_i2c(ezlogic_device_I2C_t);
    void ezlogic_table_adddev_spi(ezlogic_device_SPI_t);

    std::vector<ezpi_dev_type> ezlogic_table_row_device_map;


};

#endif // MAINWINDOW_H
=======
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTimer>
#include <QProcess>

#include<login.h>

enum ezpi_devices {
    DEV_NONE = 0,
    DEV_USER
};

enum ezpi_dev_type {
    DEV_TYPE_LED,
    DEV_TYPE_SWITCH,
    DEV_TYPE_PLUG,
    DEV_TYPE_TAMPER,
    DEV_TYPE_ONE_WIRE,
    DEV_TYPE_I2C,
    DEV_TYPE_PWM,
    DEV_TYPE_ADC,
    DEV_TYPE_FREQ,
    DEV_TYPE_TOTAL
};

#define     SIZE_DEVICE_ID      8
#define     SIZE_DEVICE_FNAME   20
#define     SIZE_ROOM_ID        8
#define     SIZE_ID_I           8
#define     SIZE_UART_NAME      20


#define     MAX_DEVICES         10
#define     MAX_GPIOS           28

#define     EZPI_ID             1655702685UL        // Random Ezpi ID
#define     EZPI_DEFAULT_BAUD   115200UL
#define     EZPI_MAX_DEVICES    10
#define     EZPI_MAX_GPIOS      28


#define     MAX_DEV             EZPI_MAX_DEVICES

#define     SIZE_EZPI_OFFSET_CONN_ID_0                          0X0000
#define     SIZE_EZPI_OFFSET_CONN_ID_1                          0X7000
#define     SIZE_EZPI_OFFSET_HUB_ID_0                           0XE000
#define     SIZE_EZPI_OFFSET_HUB_ID_1                           0XF000

struct device_t {
    char dev_id[SIZE_DEVICE_ID];
    unsigned char dev_type;
    char Name[SIZE_DEVICE_FNAME];
    char roomId[SIZE_ROOM_ID];
    char id_i[SIZE_ID_I];
    bool input_vol;
    bool out_vol;
    unsigned char input_gpio;
    unsigned char out_gpio;
    bool is_input;
    bool checkBox_gpio_in_logic_type;
    bool checkBox_gpio_out_logic_type;
    bool is_meter;
};

typedef union conv_u16_array {
    uint8_t data_bytes[sizeof(uint16_t)];
    uint16_t data;
} conv_u16_array_t;

typedef union conv_u64_array {
    uint8_t data_bytes[sizeof(uint64_t)];
    uint64_t data;
} conv_64_array_t;

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
    void on_serRX1();

    void ezpi_log_write_flash();

    void ezpi_log_erase_flash();

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

    void ezpi_check_firmware();
    void ezpi_message_info_no_firmware_detected();

    void on_actionExit_triggered();

    void on_actionEnable_Log_triggered();

    void on_actionAbout_EzloPi_triggered();

    void on_actionRegister_triggered();

    void success_prov_dat(QNetworkReply *d);
    void success_get_prov_jsons(QNetworkReply *d);

    void on_actionLogin_triggered();

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

    login * ezpi_form_login;

    QString user_token;

    bool ezpi_flag_registered;

};

#endif // MAINWINDOW_H
>>>>>>> master
