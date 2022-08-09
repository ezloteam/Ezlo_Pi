#ifndef DIALOG_CONFIG_ONEWIRE_H
#define DIALOG_CONFIG_ONEWIRE_H

#include <QDialog>
#include <QMessageBox>
#include "ezpi_data_types.h"

namespace Ui {
class Dialog_config_onewire;
}

class Dialog_config_onewire : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_onewire(QWidget *parent = nullptr, EzPi * EzloPi = nullptr);
    ~Dialog_config_onewire();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog_config_onewire *ui;
    EzPi * ezloPi_one_wire;
};

#endif // DIALOG_CONFIG_ONEWIRE_H
