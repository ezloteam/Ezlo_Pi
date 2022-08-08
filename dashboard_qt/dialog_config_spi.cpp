#include "dialog_config_spi.h"
#include "ui_dialog_config_spi.h"

Dialog_config_spi::Dialog_config_spi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_config_spi)
{
    ui->setupUi(this);
}

Dialog_config_spi::~Dialog_config_spi()
{
    delete ui;
}
