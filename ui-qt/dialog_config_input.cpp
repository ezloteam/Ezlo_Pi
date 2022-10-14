#include "dialog_config_input.h"
#include "ui_dialog_config_input.h"

Dialog_config_input::Dialog_config_input(QWidget *parent, EzPi * EzloPi) :
    QDialog(parent),
    ui(new Ui::Dialog_config_input)
{
    ui->setupUi(this);

    ezloPi_digital_ip = EzloPi;

    std::vector<EZPI_UINT8> gpio_pool = ezloPi_digital_ip->EZPI_GET_GPIO_POOL();
    EZPI_UINT8 gpio_pool_count = (EZPI_UINT8)gpio_pool.size();

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if((gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED) || (gpio_pool[i] == EZPI_DEV_TYPE_INPUT_ONLY))
            ui->comboBox_input_gpio->addItem(QString::number(i));
    }

    ui->lineEdit_device_name->setText(ezloPi_digital_ip->EZPI_GET_DEV_TYPE(EZPI_DEV_TYPE_DIGITAL_IP) + \
                                      " " + QString::number(ezloPi_digital_ip->EZPI_GET_INPUT_DEVICES().size() + 1));

}

Dialog_config_input::~Dialog_config_input() {
    delete ui;
}


void Dialog_config_input::on_buttonBox_accepted() {

    ezpi_device_digital_ip_t digital_ip_user_data;

    digital_ip_user_data.dev_type = EZPI_DEV_TYPE_DIGITAL_IP;
    digital_ip_user_data.dev_name = ui->lineEdit_device_name->text();
    digital_ip_user_data.id_room = 0; // TBD
    digital_ip_user_data.id_item = EZPI_ITEM_TYPE_BUTTON;

    ezpi_high_low digital_ip_default_value = (ezpi_high_low)ui->comboBox_default_value_input->currentIndex();
    digital_ip_default_value ? digital_ip_user_data.val_ip = true : digital_ip_user_data.val_ip = false;

    digital_ip_user_data.gpio = ui->comboBox_input_gpio->currentText().toInt();

    if(ui->radioButton_input_pullup->isChecked()) digital_ip_user_data.pull_up = true;
    else                                           digital_ip_user_data.pull_up = false;

    if(ui->checkBox_invert_input->checkState() == Qt::Checked)    digital_ip_user_data.logic_inv = true;
    else                                                           digital_ip_user_data.logic_inv = false;

    // Adding device to the device vector
    if(ezloPi_digital_ip->EZPI_ADD_INPUT_DEVICE(digital_ip_user_data) == EZPI_SUCCESS) {
//       QMessageBox::information(this, "Success", "Successfully added a input device.");
       // Trigger signal to add device in the table
       emit ezpi_signal_dev_ip_added(EZPI_DEV_TYPE_DIGITAL_IP);
    } else if(ezloPi_digital_ip->EZPI_ADD_INPUT_DEVICE(digital_ip_user_data) == EZPI_ERROR_REACHED_MAX_DEV) {
       QMessageBox::information(this, "Error", "Error : Reached maximum input device limit.");
    } else {
        // Do nothing
    }
}

