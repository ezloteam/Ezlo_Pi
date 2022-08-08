#ifndef DIALOG_CONFIG_SPI_H
#define DIALOG_CONFIG_SPI_H

#include <QDialog>

namespace Ui {
class Dialog_config_spi;
}

class Dialog_config_spi : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_spi(QWidget *parent = nullptr);
    ~Dialog_config_spi();

private:
    Ui::Dialog_config_spi *ui;
};

#endif // DIALOG_CONFIG_SPI_H
