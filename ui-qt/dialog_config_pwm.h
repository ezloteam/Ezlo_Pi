#ifndef DIALOG_CONFIG_PWM_H
#define DIALOG_CONFIG_PWM_H

#include <QDialog>
#include <QMessageBox>
#include "ezlopi.h"

namespace Ui {
class Dialog_config_pwm;
}

class Dialog_config_pwm : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_pwm(QWidget *parent = nullptr, EzPi * EzloPi = nullptr);
    ~Dialog_config_pwm();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog_config_pwm *ui;
    EzPi * ezloPi_pwm;

signals:
    void ezpi_signal_dev_pwm_added(ezpi_dev_type);
};

#endif // DIALOG_CONFIG_PWM_H
