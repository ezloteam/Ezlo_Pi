#include "dialog_config_other.h"
#include "ui_dialog_config_other.h"

Dialog_config_other::Dialog_config_other(QWidget *parent, EzPi * EzloPi) :
    QDialog(parent),
    ui(new Ui::Dialog_config_other)
{
    ui->setupUi(this);
    ezloPi_other = EzloPi;

    std::vector<EZPI_UINT8> gpio_pool = ezloPi_other->EZPI_GET_GPIO_POOL();
    EZPI_UINT8 gpio_pool_count = (EZPI_UINT8)gpio_pool.size();

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if(gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED)
            ui->comboBox_gpio1->addItem(QString::number(i));
    }

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if(gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED) {
            if(ui->comboBox_gpio1->currentText() != QString::number(i)) {
                ui->comboBox_gpio2->addItem(QString::number(i));
            }
        }
    }

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if(gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED) {
            if((ui->comboBox_gpio1->currentText() != QString::number(i)) && \
                    (ui->comboBox_gpio2->currentText() != QString::number(i))) {
                ui->comboBox_gpio3->addItem(QString::number(i));
            }
        }
    }

    ui->lineEdit_device_name->setText(ezloPi_other->EZPI_GET_DEV_TYPE(EZPI_DEV_TYPE_OTHER) + \
                                      " " + QString::number(ezloPi_other->EZPI_GET_OTHER_DEVICES().size() + 1));

    ui->comboBox_gpio1->setEnabled(false);
    ui->comboBox_gpio2->setEnabled(false);
    ui->comboBox_gpio3->setEnabled(false);
}

Dialog_config_other::~Dialog_config_other()
{
    delete ui;
}

void Dialog_config_other::on_buttonBox_accepted() {
    ezpi_device_other_t other_user_data;

    other_user_data.dev_type = EZPI_DEV_TYPE_OTHER;
    other_user_data.dev_name = ui->lineEdit_device_name->text();
    other_user_data.id_room = 0; // TBD
    other_user_data.id_item = (ezpi_item_type)(ui->comboBox_other_device_subtype->currentIndex() + EZPI_ITEM_TYPE_DOOR_SENSOR);

    other_user_data.en_gpio1 = ui->checkBox_gpio1->isChecked();
    other_user_data.gpio1 = ui->comboBox_gpio1->currentText().toInt();
    if(other_user_data.en_gpio1) {
        ezloPi_other->EZPI_SET_GPIO_POOL(other_user_data.gpio1, EZPI_DEV_TYPE_OTHER);
    }

    other_user_data.en_gpio2 = ui->checkBox_gpio2->isChecked();
    other_user_data.gpio2 = ui->comboBox_gpio2->currentText().toInt();
    if(other_user_data.en_gpio2) {
        ezloPi_other->EZPI_SET_GPIO_POOL(other_user_data.gpio2, EZPI_DEV_TYPE_OTHER);
    }

    other_user_data.en_gpio3 = ui->checkBox_gpio3->isChecked();
    other_user_data.gpio3 = ui->comboBox_gpio3->currentText().toInt();
    if(other_user_data.en_gpio3) {
        ezloPi_other->EZPI_SET_GPIO_POOL(other_user_data.gpio3, EZPI_DEV_TYPE_OTHER);
    }

    // Adding device to the device vector
    if(ezloPi_other->EZPI_ADD_OTHER_DEVICE(other_user_data) == EZPI_SUCCESS) {
       // Trigger signal to add device in the table
       emit ezpi_signal_dev_other_added(EZPI_DEV_TYPE_OTHER);
    } else if(ezloPi_other->EZPI_ADD_OTHER_DEVICE(other_user_data) == EZPI_ERROR_REACHED_MAX_DEV) {
       QMessageBox::information(this, "Error", "Error : Reached maximum Other device limit.");
    } else {
        // Do nothing
    }

}


void Dialog_config_other::on_checkBox_gpio1_stateChanged(int arg1) {
    if(arg1 == Qt::Unchecked) {
        ui->comboBox_gpio1->setEnabled(false);
    } else if(arg1 == Qt::Checked) {
        ui->comboBox_gpio1->setEnabled(true);
    } else {
        // Do nothing
    }
}


void Dialog_config_other::on_checkBox_gpio2_stateChanged(int arg1) {
    if(arg1 == Qt::Unchecked) {
        ui->comboBox_gpio2->setEnabled(false);
    } else if(arg1 == Qt::Checked) {
        ui->comboBox_gpio2->setEnabled(true);
    } else {
        // Do nothing
    }

}


void Dialog_config_other::on_checkBox_gpio3_stateChanged(int arg1) {
    if(arg1 == Qt::Unchecked) {
        ui->comboBox_gpio3->setEnabled(false);
    } else if(arg1 == Qt::Checked) {
        ui->comboBox_gpio3->setEnabled(true);
    } else {
        // Do nothing
    }
}

