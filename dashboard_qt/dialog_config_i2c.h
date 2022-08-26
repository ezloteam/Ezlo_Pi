#ifndef DIALOG_CONFIG_I2C_H
#define DIALOG_CONFIG_I2C_H

#include <QDialog>
#include<QMessageBox>
#include <QDebug>
#include "ezpi_data_types.h"

namespace Ui {
class Dialog_config_i2c;
}

class Dialog_config_i2c : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_i2c(QWidget *parent = nullptr, EzPi * EzloPi = nullptr);
    ~Dialog_config_i2c();

private slots:
    void on_comboBox_i2c_gpio_sda_currentIndexChanged(int index);

    void on_comboBox_i2c_gpio_scl_currentIndexChanged(int index);

    void on_buttonBox_accepted();

private:
    void ezpi_ensure_no_same_pins();

    Ui::Dialog_config_i2c *ui;
    EzPi * ezloPi_i2c;
signals:
    void ezpi_signal_dev_i2c_added(ezpi_dev_type);
};

#endif // DIALOG_CONFIG_I2C_H
