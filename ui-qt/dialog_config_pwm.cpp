#include "dialog_config_pwm.h"
#include "ui_dialog_config_pwm.h"

Dialog_config_pwm::Dialog_config_pwm(QWidget *parent, EzPi * EzloPi) :
    QDialog(parent),
    ui(new Ui::Dialog_config_pwm)
{
    ui->setupUi(this);

    ezloPi_pwm = EzloPi;

    std::vector<EZPI_UINT8> gpio_pool = ezloPi_pwm->EZPI_GET_GPIO_POOL();
    EZPI_UINT8 gpio_pool_count = (EZPI_UINT8)gpio_pool.size();

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if(gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED)
            ui->comboBox_pwm_gpio->addItem(QString::number(i));
    }

    ui->lineEdit_device_name->setText(ezloPi_pwm->EZPI_GET_DEV_TYPE(EZPI_DEV_TYPE_PWM) + \
                                      " " + QString::number(ezloPi_pwm->EZPI_GET_PWM_DEVICES().size() + 1));

}

Dialog_config_pwm::~Dialog_config_pwm()
{
    delete ui;
}

void Dialog_config_pwm::on_buttonBox_accepted()
{


    ezpi_device_pwm_t pwm_user_data;

    pwm_user_data.dev_type = EZPI_DEV_TYPE_PWM;
    pwm_user_data.dev_name = ui->lineEdit_device_name->text();
    pwm_user_data.id_room = ""; //TBD

    pwm_user_data.id_item = (ezpi_item_type)(ui->comboBox_pwm_device_subtype->currentIndex() + EZPI_ITEM_TYPE_PWM_GENERIC_LOAD);

    pwm_user_data.gpio = ui->comboBox_pwm_gpio->currentText().toInt();

    pwm_user_data.pwm_resln = ui->comboBox_pwm_resolution->currentText().toInt();

    pwm_user_data.duty_cycle = ui->horizontalSlider_duty_cycle->value(); //ui->lineEdit_pwm_duty_cycle->text().toInt();
    pwm_user_data.freq_hz = ui->horizontalSlider_pwm_freq->value();

    // Adding device to the device vector
    if(ezloPi_pwm->EZPI_ADD_PWM_DEVICE(pwm_user_data) == EZPI_SUCCESS) {
       // Trigger signal to add device in the table
       emit ezpi_signal_dev_pwm_added(EZPI_DEV_TYPE_PWM);
    } else if(ezloPi_pwm->EZPI_ADD_PWM_DEVICE(pwm_user_data) == EZPI_ERROR_REACHED_MAX_DEV) {
       QMessageBox::information(this, "Error", "Error : Reached maximum PWM device limit.");
    } else {
        // Do nothing
    }

}

