#include "dialog_config_uart.h"
#include "qpushbutton.h"
#include "ui_dialog_config_uart.h"


void Dialog_config_uart::ezpi_ensure_no_same_pins() {

    QPushButton * button_ok =  ui->buttonBox->button(QDialogButtonBox::Ok);

    if(ui->comboBox_uart_gpio_rx->currentText() == ui->comboBox_uart_gpio_tx->currentText()) {
        QMessageBox::information(this, "Same GPIO selected !", "Input and Output can not be same GPIO, please change and continue!");
        button_ok->setEnabled(false);
    } else {
        button_ok->setEnabled(true);
    }
}

Dialog_config_uart::Dialog_config_uart(QWidget *parent, EzPi * EzloPi) :
    QDialog(parent),
    ui(new Ui::Dialog_config_uart)
{
    ui->setupUi(this);

    ezloPi_uart = EzloPi;

    std::vector<EZPI_UINT8> gpio_pool = ezloPi_uart->EZPI_GET_GPIO_POOL();
    EZPI_UINT8 gpio_pool_count = (EZPI_UINT8)gpio_pool.size();

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if(gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED)
            ui->comboBox_uart_gpio_rx->addItem(QString::number(i));
    }

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if(gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED)
            if(ui->comboBox_uart_gpio_rx->currentText() != QString::number(i)) {
                ui->comboBox_uart_gpio_tx->addItem(QString::number(i));
            }
    }

    ui->lineEdit_device_name->setText(ezloPi_uart->EZPI_GET_DEV_TYPE(EZPI_DEV_TYPE_UART) + \
                                      " " + QString::number(ezloPi_uart->EZPI_GET_AINPUT_DEVICES().size() + 1));

}

void Dialog_config_uart::on_comboBox_uart_gpio_rx_currentIndexChanged(int index)
{
    ezpi_ensure_no_same_pins();
}


void Dialog_config_uart::on_comboBox_uart_gpio_tx_currentIndexChanged(int index)
{
    ezpi_ensure_no_same_pins();
}


Dialog_config_uart::~Dialog_config_uart()
{
    delete ui;
}


void Dialog_config_uart::on_buttonBox_accepted()
{

    ezpi_device_uart_t uart_user_data;

    uart_user_data.dev_type = EZPI_DEV_TYPE_UART;
    uart_user_data.dev_name = ui->lineEdit_device_name->text();
    uart_user_data.id_room = ""; //TBD

    uart_user_data.id_item = (ezpi_item_type)(ui->comboBox_uart_subtype->currentIndex() + EZPI_ITEM_TYPE_MB1013_SONAR);

    uart_user_data.gpio_rx = ui->comboBox_uart_gpio_rx->currentText().toInt();
    uart_user_data.gpio_tx = ui->comboBox_uart_gpio_tx->currentText().toInt();
    uart_user_data.baud_rate = ui->comboBox_uart_baudrate->currentText().toInt();

    // Adding device to the device vector
    if(ezloPi_uart->EZPI_ADD_UART_DEVICE(uart_user_data) == EZPI_SUCCESS) {
       // Trigger signal to add device in the table
       emit ezpi_signal_dev_uart_added(EZPI_DEV_TYPE_UART);
    } else if(ezloPi_uart->EZPI_ADD_UART_DEVICE(uart_user_data) == EZPI_ERROR_REACHED_MAX_DEV) {
       QMessageBox::information(this, "Error", "Error : Reached maximum UART device limit.");
    } else {
        // Do nothing
    }

}

