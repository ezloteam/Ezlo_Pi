#ifndef DIALOG_CONFIG_INPUT_H
#define DIALOG_CONFIG_INPUT_H

#include <QDialog>

#include<QMessageBox>
#include <QDebug>

#include "ezlopi.h"

namespace Ui {
class Dialog_config_input;
}

class Dialog_config_input : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_input(QWidget *parent = nullptr, EzPi * EzloPi = nullptr);
    ~Dialog_config_input();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog_config_input *ui;
    EzPi * ezloPi_digital_ip;

signals:
    void ezpi_signal_dev_ip_added(ezpi_dev_type);
};

#endif // DIALOG_CONFIG_INPUT_H
