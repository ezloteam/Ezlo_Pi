#include "dialog_config_i2c.h"
#include "ui_dialog_config_i2c.h"

Dialog_config_i2c::Dialog_config_i2c(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_config_i2c)
{
    ui->setupUi(this);
}

Dialog_config_i2c::~Dialog_config_i2c()
{
    delete ui;
}
