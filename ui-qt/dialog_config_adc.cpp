#include "dialog_config_adc.h"
#include "ui_dialog_config_adc.h"

Dialog_config_adc::Dialog_config_adc(QWidget *parent, EzPi * EzloPi) :
    QDialog(parent),
    ui(new Ui::Dialog_config_adc)
{
    ui->setupUi(this);
    ezloPi_adc = EzloPi;

    std::vector<EZPI_UINT8> gpio_pool = ezloPi_adc->EZPI_GET_GPIO_POOL();
    EZPI_UINT8 gpio_pool_count = (EZPI_UINT8)gpio_pool.size();

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if(gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED)
            ui->comboBox_adc_gpio->addItem(QString::number(i));
    }

    ui->lineEdit_device_name->setText(ezloPi_adc->EZPI_GET_DEV_TYPE(EZPI_DEV_TYPE_ANALOG_IP) + \
                                      " " + QString::number(ezloPi_adc->EZPI_GET_AINPUT_DEVICES().size() + 1));
}

Dialog_config_adc::~Dialog_config_adc()
{
    delete ui;
}

void Dialog_config_adc::on_buttonBox_accepted() {

    ezpi_device_analog_ip_t adc_user_data;

    adc_user_data.dev_type = EZPI_DEV_TYPE_ANALOG_IP;
    adc_user_data.dev_name = ui->lineEdit_device_name->text();
    adc_user_data.id_room = 0; //TBD

    adc_user_data.id_item = (ezpi_item_type)(ui->comboBox_ADC_subtype->currentIndex() + EZPI_ITEM_TYPE_POT_GENERIC);

    adc_user_data.gpio = ui->comboBox_adc_gpio->currentText().toInt();

    adc_user_data.resln_bit = (ezpi_adc_resln)(ui->comboBox_adc_resolution->currentIndex() + 1);

    // Adding device to the device vector
    if(ezloPi_adc->EZPI_ADD_AINPUT_DEVICE(adc_user_data) == EZPI_SUCCESS) {
//       QMessageBox::information(this, "Success", "Successfully added an ADC device.");
       // Trigger signal to add device in the table
       emit ezpi_signal_dev_adc_added(EZPI_DEV_TYPE_ANALOG_IP);
    } else if(ezloPi_adc->EZPI_ADD_AINPUT_DEVICE(adc_user_data) == EZPI_ERROR_REACHED_MAX_DEV) {
       QMessageBox::information(this, "Error", "Error : Reached maximum ADC device limit.");
    } else {
        // Do nothing
    }

}

