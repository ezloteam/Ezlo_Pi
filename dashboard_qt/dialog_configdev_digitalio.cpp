#include "dialog_configdev_digitalio.h"
#include "qpushbutton.h"
#include "ui_dialog_configdev_digitalio.h"

#include<QMessageBox>


void Dialog_configdev_digitalio::ezpi_ensure_no_same_pins() {

    QPushButton * button_ok =  ui->buttonBox->button(QDialogButtonBox::Ok);

    if(ui->comboBox_output_gpio->currentText() == ui->comboBox_input_gpio->currentText()) {
        QMessageBox::information(this, "Same GPIO selected !", "Input and Output can not be same GPIO, please change and continue!");
        button_ok->setEnabled(false);
    } else {
        button_ok->setEnabled(true);
    }
}

Dialog_configdev_digitalio::Dialog_configdev_digitalio(QWidget *parent, EzPi * EzloPi) :
    QDialog(parent),
    ui(new Ui::Dialog_configdev_digitalio) {

    ui->setupUi(this);

    ezloPi_digital_io = EzloPi;

    ui->checkBox_enable_pushbutton->setCheckState(Qt::Unchecked);

    ui->comboBox_input_gpio->setEnabled(false);
    ui->groupBox_resistor_input->setEnabled(false);
    ui->comboBox_default_value_input->setEnabled(false);
    ui->checkBox_invert_input->setEnabled(false);

    std::vector<EZPI_UINT8> gpio_pool = ezloPi_digital_io->EZPI_GET_GPIO_POOL();
    EZPI_UINT8 gpio_pool_count = gpio_pool.size();

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if((gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED) || (gpio_pool[i] == EZPI_DEV_TYPE_OUTPUT_ONLY))
            ui->comboBox_output_gpio->addItem(QString::number(i));
    }
}

Dialog_configdev_digitalio::~Dialog_configdev_digitalio() {
    delete ui;
}

void Dialog_configdev_digitalio::on_buttonBox_accepted() {


}


void Dialog_configdev_digitalio::on_checkBox_enable_pushbutton_stateChanged(int arg1) {

    QPushButton * button_ok =  ui->buttonBox->button(QDialogButtonBox::Ok);
    if(arg1 == Qt::Checked) {
        ui->comboBox_input_gpio->setEnabled(true);
        ui->groupBox_resistor_input->setEnabled(true);
        ui->comboBox_default_value_input->setEnabled(true);
        ui->checkBox_invert_input->setEnabled(true);

        std::vector<EZPI_UINT8> gpio_pool = ezloPi_digital_io->EZPI_GET_GPIO_POOL();
        EZPI_UINT8 gpio_pool_count = gpio_pool.size();

        for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
            if((gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED) || (gpio_pool[i] == EZPI_DEV_TYPE_INPUT_ONLY)) {

                if(ui->comboBox_output_gpio->currentText() != QString::number(i)) {
                    ui->comboBox_input_gpio->addItem(QString::number(i));
                }
            }
        }

        ezpi_ensure_no_same_pins();

    } else if (arg1 == Qt::Unchecked) {
        ui->comboBox_input_gpio->setEnabled(false);
        ui->groupBox_resistor_input->setEnabled(false);
        ui->comboBox_default_value_input->setEnabled(false);
        ui->checkBox_invert_input->setEnabled(false);
        button_ok->setEnabled(true);
    } else {
        // Do nothing
    }
}


void Dialog_configdev_digitalio::on_comboBox_output_gpio_currentIndexChanged(int index) {
    ezpi_ensure_no_same_pins();
}


void Dialog_configdev_digitalio::on_comboBox_input_gpio_currentIndexChanged(int index) {
    ezpi_ensure_no_same_pins();
}

