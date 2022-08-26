#ifndef DIALOG_CONFIGDEV_DIGITALIO_H
#define DIALOG_CONFIGDEV_DIGITALIO_H

#include <QDialog>
#include<QMessageBox>
#include <QDebug>
#include "ezpi_data_types.h"

namespace Ui {
class Dialog_configdev_digitalio;
}

class Dialog_configdev_digitalio : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_configdev_digitalio(QWidget *parent = nullptr, EzPi * EzloPi = nullptr);
    ~Dialog_configdev_digitalio();

private slots:
    void on_buttonBox_accepted();

    void on_checkBox_enable_pushbutton_stateChanged(int arg1);


    void on_comboBox_output_gpio_currentIndexChanged(int index);

    void on_comboBox_input_gpio_currentIndexChanged(int index);

private:
    Ui::Dialog_configdev_digitalio *ui;
    EzPi * ezloPi_digital_io;
    void ezpi_ensure_no_same_pins();

signals:
    void ezpi_signal_dev_op_added(ezpi_dev_type);
};

#endif // DIALOG_CONFIGDEV_DIGITALIO_H
