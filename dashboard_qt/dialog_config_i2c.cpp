#include "dialog_config_i2c.h"
#include "ui_dialog_config_i2c.h"
#include "qpushbutton.h"

void Dialog_config_i2c::ezpi_ensure_no_same_pins() {

    QPushButton * button_ok =  ui->buttonBox->button(QDialogButtonBox::Ok);

    if(ui->comboBox_i2c_gpio_sda->currentText() == ui->comboBox_i2c_gpio_scl->currentText()) {
        QMessageBox::information(this, "Same GPIO selected !", "Input and Output can not be same GPIO, please change and continue!");
        button_ok->setEnabled(false);
    } else {
        button_ok->setEnabled(true);
    }
}


Dialog_config_i2c::Dialog_config_i2c(QWidget *parent, EzPi * EzloPi) :
    QDialog(parent),
    ui(new Ui::Dialog_config_i2c)
{
    ui->setupUi(this);

    ezloPi_i2c = EzloPi;

    std::vector<EZPI_UINT8> gpio_pool = ezloPi_i2c->EZPI_GET_GPIO_POOL();
    EZPI_UINT8 gpio_pool_count = gpio_pool.size();

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if((gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED) || (gpio_pool[i] == EZPI_DEV_TYPE_I2C))
            ui->comboBox_i2c_gpio_sda->addItem(QString::number(i));
    }

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if((gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED) || (gpio_pool[i] == EZPI_DEV_TYPE_I2C))
            if(ui->comboBox_i2c_gpio_sda->currentText() != QString::number(i)) {
                ui->comboBox_i2c_gpio_scl->addItem(QString::number(i));
            }
    }

    ezpi_ensure_no_same_pins();


}

Dialog_config_i2c::~Dialog_config_i2c()
{
    delete ui;
}

void Dialog_config_i2c::on_comboBox_i2c_gpio_sda_currentIndexChanged(int index) {
    ezpi_ensure_no_same_pins();
}


void Dialog_config_i2c::on_comboBox_i2c_gpio_scl_currentIndexChanged(int index) {
    ezpi_ensure_no_same_pins();
}


void Dialog_config_i2c::on_buttonBox_accepted() {
    ezlogic_device_I2C_t i2c_user_data;

    i2c_user_data.dev_type = EZPI_DEV_TYPE_I2C;
    i2c_user_data.dev_name = ui->lineEdit_device_name->text();
    i2c_user_data.id_room = 0; //TBD

    i2c_user_data.id_item = ui->comboBox_i2c_subtype->currentIndex() + EZPI_ITEM_TYPE_MPU6050;

    i2c_user_data.gpio_sda = ui->comboBox_i2c_gpio_sda->currentText().toInt();
    i2c_user_data.gpio_scl = ui->comboBox_i2c_gpio_scl->currentText().toInt();

    if(ui->checkBox_enable_pullup_scl->isChecked())  i2c_user_data.pullup_scl = true;
    else                                             i2c_user_data.pullup_scl = false;

    if(ui->checkBox_enable_pullup_sda->isChecked())  i2c_user_data.pullup_sda = true;
    else                                             i2c_user_data.pullup_sda = false;

    i2c_user_data.slave_addr = ui->spinBox_i2c_slave_address->value();

    // Update GPIO assignments with selected GPIO used for I2C
    ezloPi_i2c->EZPI_SET_GPIO_POOL(i2c_user_data.gpio_sda, EZPI_DEV_TYPE_I2C);
    ezloPi_i2c->EZPI_SET_GPIO_POOL(i2c_user_data.gpio_scl, EZPI_DEV_TYPE_I2C);

    // Adding device to the device vector
    if(ezloPi_i2c->EZPI_ADD_I2C_DEVICE(i2c_user_data) == EZPI_SUCCESS) {
       QMessageBox::information(this, "Success", "Successfully added an I2C device.");
       // Trigger signal to add device in the table
       emit ezpi_signal_dev_i2c_added(EZPI_DEV_TYPE_I2C);
    } else if(ezloPi_i2c->EZPI_ADD_I2C_DEVICE(i2c_user_data) == EZPI_ERROR_REACHED_MAX_DEV) {
       QMessageBox::information(this, "Error", "Error : Reached maximum I2C device limit.");
    } else {
        // Do nothing
    }
}

