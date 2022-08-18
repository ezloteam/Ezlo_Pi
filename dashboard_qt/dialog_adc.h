#ifndef DIALOG_ADC_H
#define DIALOG_ADC_H

#include <QDialog>
#include <QMessageBox>

#include "ezpi_data_types.h"

namespace Ui {
class Dialog_adc;
}

class Dialog_adc : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_adc(QWidget *parent = nullptr, EzPi * EzloPi = nullptr);
    ~Dialog_adc();

private:
    Ui::Dialog_adc *ui;
    EzPi * ezloPi_adc;

private slots:
    void on_buttonBox_accepted();

signals:
    void ezpi_signal_dev_adc_added(ezpi_dev_type);
};

#endif // DIALOG_ADC_H
