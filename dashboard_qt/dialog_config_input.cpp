#include "dialog_config_input.h"
#include "ui_dialog_config_input.h"

Dialog_config_input::Dialog_config_input(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_config_input)
{
    ui->setupUi(this);
}

Dialog_config_input::~Dialog_config_input()
{
    delete ui;
}
