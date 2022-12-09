#ifndef DIALOG_CONFIG_UART_H
#define DIALOG_CONFIG_UART_H

#include <QDialog>
#include <QMessageBox>

#include "ezlopi.h"

namespace Ui {
class Dialog_config_uart;
}

class Dialog_config_uart : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_uart(QWidget *parent = nullptr, EzPi * EzloPi = nullptr);
    ~Dialog_config_uart();

private:
    Ui::Dialog_config_uart *ui;
    void ezpi_ensure_no_same_pins();
        EzPi * ezloPi_uart;

signals:
    void ezpi_signal_dev_uart_added(ezpi_dev_type);

private slots:
    void on_comboBox_uart_gpio_rx_currentIndexChanged(int index);
    void on_comboBox_uart_gpio_tx_currentIndexChanged(int index);
    void on_buttonBox_accepted();
};

#endif // DIALOG_CONFIG_UART_H
