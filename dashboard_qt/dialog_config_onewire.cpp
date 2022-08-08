#include "dialog_config_onewire.h"
#include "ui_dialog_config_onewire.h"

Dialog_config_onewire::Dialog_config_onewire(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_config_onewire)
{
    ui->setupUi(this);
}

Dialog_config_onewire::~Dialog_config_onewire()
{
    delete ui;
}
