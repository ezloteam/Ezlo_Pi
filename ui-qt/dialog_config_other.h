#ifndef DIALOG_CONFIG_OTHER_H
#define DIALOG_CONFIG_OTHER_H

#include <QDialog>
#include<QMessageBox>
#include <QDebug>
#include "ezlopi.h"

namespace Ui {
class Dialog_config_other;
}

class Dialog_config_other : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_other(QWidget *parent = nullptr, EzPi * EzloPi = nullptr);
    ~Dialog_config_other();

private:
    Ui::Dialog_config_other *ui;
    EzPi * ezloPi_other;
signals:
    void ezpi_signal_dev_other_added(ezpi_dev_type);

private slots:
    void on_buttonBox_accepted();
    void on_checkBox_gpio1_stateChanged(int arg1);
    void on_checkBox_gpio2_stateChanged(int arg1);
    void on_checkBox_gpio3_stateChanged(int arg1);
};

#endif // DIALOG_CONFIG_OTHER_H
