/********************************************************************************
** Form generated from reading UI file 'dialog_config_i2c.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_CONFIG_I2C_H
#define UI_DIALOG_CONFIG_I2C_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_Dialog_config_i2c
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEdit_device_name;
    QLabel *label_GPIO_SDA;
    QCheckBox *checkBox_enable_pullup_sda;
    QComboBox *comboBox_i2c_gpio_sda;
    QLabel *label_device_name;
    QLabel *label_GPIO_SCL;
    QComboBox *comboBox_i2c_gpio_scl;
    QCheckBox *checkBox_enable_pullup_scl;
    QSpinBox *spinBox_i2c_slave_address;
    QLabel *label_i2c_slave_address;
    QComboBox *comboBox_i2c_subtype;
    QLabel *label_device_sub_type;

    void setupUi(QDialog *Dialog_config_i2c)
    {
        if (Dialog_config_i2c->objectName().isEmpty())
            Dialog_config_i2c->setObjectName(QString::fromUtf8("Dialog_config_i2c"));
        Dialog_config_i2c->resize(322, 240);
        buttonBox = new QDialogButtonBox(Dialog_config_i2c);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(110, 200, 201, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit_device_name = new QLineEdit(Dialog_config_i2c);
        lineEdit_device_name->setObjectName(QString::fromUtf8("lineEdit_device_name"));
        lineEdit_device_name->setGeometry(QRect(16, 29, 141, 26));
        label_GPIO_SDA = new QLabel(Dialog_config_i2c);
        label_GPIO_SDA->setObjectName(QString::fromUtf8("label_GPIO_SDA"));
        label_GPIO_SDA->setGeometry(QRect(19, 106, 121, 20));
        checkBox_enable_pullup_sda = new QCheckBox(Dialog_config_i2c);
        checkBox_enable_pullup_sda->setObjectName(QString::fromUtf8("checkBox_enable_pullup_sda"));
        checkBox_enable_pullup_sda->setGeometry(QRect(20, 156, 141, 24));
        comboBox_i2c_gpio_sda = new QComboBox(Dialog_config_i2c);
        comboBox_i2c_gpio_sda->setObjectName(QString::fromUtf8("comboBox_i2c_gpio_sda"));
        comboBox_i2c_gpio_sda->setGeometry(QRect(16, 129, 141, 26));
        label_device_name = new QLabel(Dialog_config_i2c);
        label_device_name->setObjectName(QString::fromUtf8("label_device_name"));
        label_device_name->setGeometry(QRect(20, 7, 121, 20));
        label_GPIO_SCL = new QLabel(Dialog_config_i2c);
        label_GPIO_SCL->setObjectName(QString::fromUtf8("label_GPIO_SCL"));
        label_GPIO_SCL->setGeometry(QRect(194, 105, 121, 20));
        comboBox_i2c_gpio_scl = new QComboBox(Dialog_config_i2c);
        comboBox_i2c_gpio_scl->setObjectName(QString::fromUtf8("comboBox_i2c_gpio_scl"));
        comboBox_i2c_gpio_scl->setGeometry(QRect(193, 129, 111, 26));
        checkBox_enable_pullup_scl = new QCheckBox(Dialog_config_i2c);
        checkBox_enable_pullup_scl->setObjectName(QString::fromUtf8("checkBox_enable_pullup_scl"));
        checkBox_enable_pullup_scl->setGeometry(QRect(190, 160, 141, 24));
        spinBox_i2c_slave_address = new QSpinBox(Dialog_config_i2c);
        spinBox_i2c_slave_address->setObjectName(QString::fromUtf8("spinBox_i2c_slave_address"));
        spinBox_i2c_slave_address->setGeometry(QRect(197, 69, 61, 26));
        spinBox_i2c_slave_address->setMaximum(127);
        label_i2c_slave_address = new QLabel(Dialog_config_i2c);
        label_i2c_slave_address->setObjectName(QString::fromUtf8("label_i2c_slave_address"));
        label_i2c_slave_address->setGeometry(QRect(198, 42, 101, 20));
        comboBox_i2c_subtype = new QComboBox(Dialog_config_i2c);
        comboBox_i2c_subtype->addItem(QString());
        comboBox_i2c_subtype->addItem(QString());
        comboBox_i2c_subtype->addItem(QString());
        comboBox_i2c_subtype->addItem(QString());
        comboBox_i2c_subtype->addItem(QString());
        comboBox_i2c_subtype->addItem(QString());
        comboBox_i2c_subtype->addItem(QString());
        comboBox_i2c_subtype->addItem(QString());
        comboBox_i2c_subtype->addItem(QString());
        comboBox_i2c_subtype->setObjectName(QString::fromUtf8("comboBox_i2c_subtype"));
        comboBox_i2c_subtype->setGeometry(QRect(17, 79, 141, 22));
        label_device_sub_type = new QLabel(Dialog_config_i2c);
        label_device_sub_type->setObjectName(QString::fromUtf8("label_device_sub_type"));
        label_device_sub_type->setGeometry(QRect(17, 59, 121, 16));

        retranslateUi(Dialog_config_i2c);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog_config_i2c, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog_config_i2c, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog_config_i2c);
    } // setupUi

    void retranslateUi(QDialog *Dialog_config_i2c)
    {
        Dialog_config_i2c->setWindowTitle(QCoreApplication::translate("Dialog_config_i2c", "Dialog", nullptr));
        lineEdit_device_name->setPlaceholderText(QCoreApplication::translate("Dialog_config_i2c", "Friendly name", nullptr));
        label_GPIO_SDA->setText(QCoreApplication::translate("Dialog_config_i2c", "SDA GPIO", nullptr));
        checkBox_enable_pullup_sda->setText(QCoreApplication::translate("Dialog_config_i2c", "Enable pullup", nullptr));
        label_device_name->setText(QCoreApplication::translate("Dialog_config_i2c", "Device Name", nullptr));
        label_GPIO_SCL->setText(QCoreApplication::translate("Dialog_config_i2c", "SCL GPIO", nullptr));
        checkBox_enable_pullup_scl->setText(QCoreApplication::translate("Dialog_config_i2c", "Enable pullup", nullptr));
        label_i2c_slave_address->setText(QCoreApplication::translate("Dialog_config_i2c", "Slave Address", nullptr));
        comboBox_i2c_subtype->setItemText(0, QCoreApplication::translate("Dialog_config_i2c", "MPU6050", nullptr));
        comboBox_i2c_subtype->setItemText(1, QCoreApplication::translate("Dialog_config_i2c", "ADXL345", nullptr));
        comboBox_i2c_subtype->setItemText(2, QCoreApplication::translate("Dialog_config_i2c", "GY271", nullptr));
        comboBox_i2c_subtype->setItemText(3, QCoreApplication::translate("Dialog_config_i2c", "MCP4725", nullptr));
        comboBox_i2c_subtype->setItemText(4, QCoreApplication::translate("Dialog_config_i2c", "GY530", nullptr));
        comboBox_i2c_subtype->setItemText(5, QCoreApplication::translate("Dialog_config_i2c", "DS1307", nullptr));
        comboBox_i2c_subtype->setItemText(6, QCoreApplication::translate("Dialog_config_i2c", "MAX30100", nullptr));
        comboBox_i2c_subtype->setItemText(7, QCoreApplication::translate("Dialog_config_i2c", "BMP280", nullptr));
        comboBox_i2c_subtype->setItemText(8, QCoreApplication::translate("Dialog_config_i2c", "LNA219", nullptr));

        label_device_sub_type->setText(QCoreApplication::translate("Dialog_config_i2c", "Device sub type", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_config_i2c: public Ui_Dialog_config_i2c {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_CONFIG_I2C_H
