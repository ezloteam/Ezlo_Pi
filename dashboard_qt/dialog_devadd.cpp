#include "dialog_devadd.h"
#include "ui_dialog_devadd.h"
#include "ezpi_data_types.h"

#include <QDebug>

Dialog_devadd::Dialog_devadd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_devadd)
{
    ui->setupUi(this);
}

Dialog_devadd::~Dialog_devadd()
{
    delete ui;
}

void Dialog_devadd::on_buttonBox_accepted() {
    EZPI_UINT8 dev_type_index = 1 + ui->comboBox_list_dev_type->currentIndex();
    emit ezpi_send_dev_type_selected(dev_type_index);
}

