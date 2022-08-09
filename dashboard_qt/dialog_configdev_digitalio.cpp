#include "dialog_configdev_digitalio.h"
#include "qpushbutton.h"
#include "ui_dialog_configdev_digitalio.h"


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
//        qDebug() << "Gpio Pin: " << QString::number(i) << "value: " << QString::number(gpio_pool[i]);
    }
}

Dialog_configdev_digitalio::~Dialog_configdev_digitalio() {
    delete ui;
}

void Dialog_configdev_digitalio::on_buttonBox_accepted() {

    ezlogic_device_digital_op_t digital_op_user_data;

     digital_op_user_data.dev_type = EZPI_DEV_TYPE_DIGITAL_OP;
     digital_op_user_data.dev_name = ui->lineEdit_device_name->text();
     digital_op_user_data.id_room = 0; // TBD
     digital_op_user_data.id_item = ui->comboBox_output_subtype->currentIndex() + 1;

     ezpi_high_low digital_op_default_value = (ezpi_high_low)ui->comboBox_default_value_output->currentIndex();
     digital_op_default_value ? digital_op_user_data.val_op = true : digital_op_user_data.val_op = false;

     digital_op_user_data.gpio_out = ui->comboBox_output_gpio->currentText().toInt();

     if(ui->radioButton_output_pullup->isChecked()) digital_op_user_data.pullup_op = true;
     else                                           digital_op_user_data.pullup_op = false;

     if(ui->checkBox_invert_output->checkState() == Qt::Checked)    digital_op_user_data.op_inv = true;
     else                                                           digital_op_user_data.op_inv = false;

     // Update GPIO assignments with selected GPIO Output
     ezloPi_digital_io->EZPI_SET_GPIO_POOL(digital_op_user_data.gpio_out, EZPI_DEV_TYPE_DIGITAL_OP);

     if(ui->checkBox_enable_pushbutton->checkState() == Qt::Checked) {

         digital_op_user_data.is_ip = true;

         ezpi_high_low digital_ip_default_value = (ezpi_high_low)ui->comboBox_default_value_input->currentIndex();
         digital_ip_default_value ? digital_op_user_data.val_ip = true : digital_op_user_data.val_ip = false;
         digital_op_user_data.gpio_in = ui->comboBox_input_gpio->currentText().toInt();

         if(ui->radioButton_input_pullup->isChecked()) digital_op_user_data.pullup_ip = true;
         else                                           digital_op_user_data.pullup_ip = false;

         if(ui->checkBox_invert_input->checkState() == Qt::Checked)    digital_op_user_data.ip_inv = true;
         else                                                           digital_op_user_data.ip_inv = false;

         // Update GPIO assignments with selected GPIO Input
         ezloPi_digital_io->EZPI_SET_GPIO_POOL(digital_op_user_data.gpio_in, EZPI_DEV_TYPE_DIGITAL_IP);

     } else {
          digital_op_user_data.is_ip = false;
     }

     // Adding device to the device vector
     if(ezloPi_digital_io->EZPI_ADD_OUTPUT_DEVICE(digital_op_user_data) == EZPI_SUCCESS) {
        QMessageBox::information(this, "Success", "Successfully added a output device.");
     } else if(ezloPi_digital_io->EZPI_ADD_OUTPUT_DEVICE(digital_op_user_data) == EZPI_ERROR_REACHED_MAX_DEV) {
        QMessageBox::information(this, "Error", "Error : Reached maximum output device limit.");
     } else {
         // Do nothing
     }

     // TODO : add device on the table on the UI
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

