#include "dialog_config_spi.h"
#include "ui_dialog_config_spi.h"


Dialog_config_spi::Dialog_config_spi(QWidget *parent, EzPi * EzloPi) :
    QDialog(parent),
    ui(new Ui::Dialog_config_spi)
{
    ui->setupUi(this);

    ezloPi_spi = EzloPi;

    std::vector<EZPI_UINT8> gpio_pool = ezloPi_spi->EZPI_GET_GPIO_POOL();
    EZPI_UINT8 gpio_pool_count = gpio_pool.size();

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if((gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED) || (gpio_pool[i] == EZPI_DEV_TYPE_SPI))
            ui->comboBox_mosi->addItem(QString::number(i));
    }

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if((gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED) || (gpio_pool[i] == EZPI_DEV_TYPE_SPI))
            if(ui->comboBox_mosi->currentText() != QString::number(i)) {
                ui->comboBox_miso->addItem(QString::number(i));
            }
    }

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if((gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED) || (gpio_pool[i] == EZPI_DEV_TYPE_SPI))
            if((ui->comboBox_mosi->currentText() != QString::number(i)) && \
                    (ui->comboBox_miso->currentText() != QString::number(i))) {
                ui->comboBox_sck->addItem(QString::number(i));
            }
    }

    for(EZPI_UINT8 i = 0; i < gpio_pool_count; i++) {
        if(gpio_pool[i] == EZPI_DEV_TYPE_UNCONFIGURED)
            if((ui->comboBox_mosi->currentText() != QString::number(i)) && \
                    (ui->comboBox_miso->currentText() != QString::number(i)) && \
                    (ui->comboBox_sck->currentText() != QString::number(i))) {
                ui->comboBox_cs->addItem(QString::number(i));
            }
    }

    ezpi_ensure_no_same_pins();

    ui->lineEdit_device_name->setText(ezloPi_spi->EZPI_GET_DEV_TYPE(EZPI_DEV_TYPE_SPI) + \
                                      " " + QString::number(ezloPi_spi->EZPI_GET_SPI_DEVICES().size() + 1));

}

Dialog_config_spi::~Dialog_config_spi()
{
    delete ui;
}

void Dialog_config_spi::ezpi_ensure_no_same_pins() {
#if 0
    QPushButton * button_ok =  ui->buttonBox->button(QDialogButtonBox::Ok);

    if(ui->comboBox_cs->currentText() == ui->comboBox_miso->currentText()) {
        QMessageBox::information(this, "Same GPIO selected !", "Input and Output can not be same GPIO, please change and continue!");
        button_ok->setEnabled(false);
    } else {
        button_ok->setEnabled(true);
    }
#endif
}

void Dialog_config_spi::on_buttonBox_config_spi_accepted() {
    ezlogic_device_SPI_t spi_user_data;

    spi_user_data.dev_type = EZPI_DEV_TYPE_SPI;
    spi_user_data.dev_name = ui->lineEdit_device_name->text();
    spi_user_data.id_room = 0; // TBD
    spi_user_data.id_item = (ezpi_item_type)(ui->comboBox_spi_device_subtype->currentIndex() + EZPI_ITEM_TYPE_BMP280_SPI);
    spi_user_data.gpio_miso = ui->comboBox_miso->currentText().toInt();
    spi_user_data.gpio_mosi = ui->comboBox_mosi->currentText().toInt();
    spi_user_data.gpio_sck = ui->comboBox_sck->currentText().toInt();
    spi_user_data.gpio_cs = ui->comboBox_cs->currentText().toInt();

    // Update GPIO assignments with selected GPIO used for SPI
    ezloPi_spi->EZPI_SET_GPIO_POOL(spi_user_data.gpio_miso, EZPI_DEV_TYPE_SPI);
    ezloPi_spi->EZPI_SET_GPIO_POOL(spi_user_data.gpio_mosi, EZPI_DEV_TYPE_SPI);
    ezloPi_spi->EZPI_SET_GPIO_POOL(spi_user_data.gpio_sck, EZPI_DEV_TYPE_SPI);
    ezloPi_spi->EZPI_SET_GPIO_POOL(spi_user_data.gpio_cs, EZPI_DEV_TYPE_SPI);

    // Adding device to the device vector
    if(ezloPi_spi->EZPI_ADD_SPI_DEVICE(spi_user_data) == EZPI_SUCCESS) {
       QMessageBox::information(this, "Success", "Successfully added an SPI device.");
       // Trigger signal to add device in the table
       emit ezpi_signal_dev_spi_added(EZPI_DEV_TYPE_SPI);
    } else if(ezloPi_spi->EZPI_ADD_SPI_DEVICE(spi_user_data) == EZPI_ERROR_REACHED_MAX_DEV) {
       QMessageBox::information(this, "Error", "Error : Reached maximum SPI device limit.");
    } else {
        // Do nothing
    }
}


void Dialog_config_spi::on_comboBox_mosi_currentIndexChanged(int index) {

}


void Dialog_config_spi::on_comboBox_miso_currentIndexChanged(int index) {

}


void Dialog_config_spi::on_comboBox_sck_currentIndexChanged(int index) {

}


void Dialog_config_spi::on_comboBox_cs_currentIndexChanged(int index) {

}

