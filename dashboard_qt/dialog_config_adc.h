#ifndef DIALOG_CONFIG_ADC_H
#define DIALOG_CONFIG_ADC_H

#include <QDialog>
#include <QMessageBox>

#include "ezpi_data_types.h"

namespace Ui {
class Dialog_config_adc;
}

class Dialog_config_adc : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_adc(QWidget *parent = nullptr, EzPi * EzloPi = nullptr);
    ~Dialog_config_adc();

private:
    Ui::Dialog_config_adc *ui;
    EzPi * ezloPi_adc;

private slots:
    void on_buttonBox_accepted();

signals:
    void ezpi_signal_dev_adc_added(ezpi_dev_type);
};

#endif // DIALOG_CONFIG_ADC_H
