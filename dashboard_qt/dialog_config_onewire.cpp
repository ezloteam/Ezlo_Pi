#include "dialog_config_onewire.h"
#include "ui_dialog_config_onewire.h"

Dialog_config_onewire::Dialog_config_onewire(QWidget *parent, EzPi * EzloPi) :
    QDialog(parent),
    ui(new Ui::Dialog_config_onewire)
{
    ui->setupUi(this);

    ezloPi_one_wire = EzloPi;

    std::vector<EZPI_UINT8> gpio_pool = ezloPi_one_wire->EZPI_GET_GPIO_POOL();
    EZPI_UINT8 gpio_pool_count = gpio_pool.size();

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if(gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED)
            ui->comboBox_onewire_gpio->addItem(QString::number(i));
//        qDebug() << "Gpio Pin: " << QString::number(i) << "value: " << QString::number(gpio_pool[i]);
    }
}

Dialog_config_onewire::~Dialog_config_onewire() {
    delete ui;
}

void Dialog_config_onewire::on_buttonBox_accepted() {
    ezlogic_device_one_wire_t onewire_user_data;
    onewire_user_data.dev_type = EZPI_DEV_TYPE_ONE_WIRE;
    onewire_user_data.dev_name = ui->lineEdit_device_name->text();
    onewire_user_data.id_room = 0; //TBD
    onewire_user_data.id_item = EZPI_ITEM_TYPE_DHT11;

    ezpi_high_low digital_io_default_value = (ezpi_high_low)ui->comboBox_default_value_gpio->currentIndex();
    digital_io_default_value ? onewire_user_data.val_ip = true : onewire_user_data.val_ip = false;

    if(ui->radioButton_pullup->isChecked()) onewire_user_data.pull_up = true;
    else                                    onewire_user_data.pull_up = false;

    onewire_user_data.gpio = ui->comboBox_onewire_gpio->currentText().toInt();

    // Update GPIO assignments with selected GPIO Output
    ezloPi_one_wire->EZPI_SET_GPIO_POOL(onewire_user_data.gpio, EZPI_DEV_TYPE_ONE_WIRE);

    // Adding device to the device vector
    if(ezloPi_one_wire->EZPI_ADD_ONEWIRE_DEVICE(onewire_user_data) == EZPI_SUCCESS) {
       QMessageBox::information(this, "Success", "Successfully added a one wire device.");
       // Trigger signal to add device in the table
       emit ezpi_signal_dev_onewire_added(EZPI_DEV_TYPE_ONE_WIRE);
    } else if(ezloPi_one_wire->EZPI_ADD_ONEWIRE_DEVICE(onewire_user_data) == EZPI_ERROR_REACHED_MAX_DEV) {
       QMessageBox::information(this, "Error", "Error : Reached maximum one wire device limit.");
    } else {
        // Do nothing
    }
}

