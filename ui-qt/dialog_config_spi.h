#ifndef DIALOG_CONFIG_SPI_H
#define DIALOG_CONFIG_SPI_H

#include <QDialog>
#include<QMessageBox>
#include <QDebug>
#include "ezlopi.h"

namespace Ui {
class Dialog_config_spi;
}

class Dialog_config_spi : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_spi(QWidget *parent = nullptr, EzPi * EzloPi = nullptr);
    ~Dialog_config_spi();

private slots:
    void on_buttonBox_config_spi_accepted();

    void on_comboBox_mosi_currentIndexChanged(int index);

    void on_comboBox_miso_currentIndexChanged(int index);

    void on_comboBox_sck_currentIndexChanged(int index);

    void on_comboBox_cs_currentIndexChanged(int index);

private:
    void ezpi_ensure_no_same_pins();

    Ui::Dialog_config_spi *ui;
    EzPi * ezloPi_spi;
signals:
    void ezpi_signal_dev_spi_added(ezpi_dev_type);
};

#endif // DIALOG_CONFIG_SPI_H
