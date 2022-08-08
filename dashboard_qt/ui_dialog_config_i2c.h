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
    QLabel *label_GPIO;
    QCheckBox *checkBox_invert_output;
    QComboBox *comboBox_output_gpio;
    QLabel *label_device_name;
    QLabel *label_GPIO_2;
    QComboBox *comboBox_output_gpio_2;
    QCheckBox *checkBox_invert_output_2;
    QSpinBox *spinBox;
    QLabel *label_GPIO_3;
    QComboBox *comboBox;
    QLabel *label;

    void setupUi(QDialog *Dialog_config_i2c)
    {
        if (Dialog_config_i2c->objectName().isEmpty())
            Dialog_config_i2c->setObjectName(QString::fromUtf8("Dialog_config_i2c"));
        Dialog_config_i2c->resize(378, 309);
        buttonBox = new QDialogButtonBox(Dialog_config_i2c);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 260, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit_device_name = new QLineEdit(Dialog_config_i2c);
        lineEdit_device_name->setObjectName(QString::fromUtf8("lineEdit_device_name"));
        lineEdit_device_name->setGeometry(QRect(16, 37, 151, 26));
        label_GPIO = new QLabel(Dialog_config_i2c);
        label_GPIO->setObjectName(QString::fromUtf8("label_GPIO"));
        label_GPIO->setGeometry(QRect(19, 137, 121, 20));
        checkBox_invert_output = new QCheckBox(Dialog_config_i2c);
        checkBox_invert_output->setObjectName(QString::fromUtf8("checkBox_invert_output"));
        checkBox_invert_output->setGeometry(QRect(20, 210, 141, 24));
        comboBox_output_gpio = new QComboBox(Dialog_config_i2c);
        comboBox_output_gpio->setObjectName(QString::fromUtf8("comboBox_output_gpio"));
        comboBox_output_gpio->setGeometry(QRect(16, 165, 141, 26));
        label_device_name = new QLabel(Dialog_config_i2c);
        label_device_name->setObjectName(QString::fromUtf8("label_device_name"));
        label_device_name->setGeometry(QRect(20, 10, 121, 20));
        label_GPIO_2 = new QLabel(Dialog_config_i2c);
        label_GPIO_2->setObjectName(QString::fromUtf8("label_GPIO_2"));
        label_GPIO_2->setGeometry(QRect(213, 138, 121, 20));
        comboBox_output_gpio_2 = new QComboBox(Dialog_config_i2c);
        comboBox_output_gpio_2->setObjectName(QString::fromUtf8("comboBox_output_gpio_2"));
        comboBox_output_gpio_2->setGeometry(QRect(210, 166, 141, 26));
        checkBox_invert_output_2 = new QCheckBox(Dialog_config_i2c);
        checkBox_invert_output_2->setObjectName(QString::fromUtf8("checkBox_invert_output_2"));
        checkBox_invert_output_2->setGeometry(QRect(210, 210, 141, 24));
        spinBox = new QSpinBox(Dialog_config_i2c);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setGeometry(QRect(290, 98, 61, 26));
        spinBox->setMaximum(127);
        label_GPIO_3 = new QLabel(Dialog_config_i2c);
        label_GPIO_3->setObjectName(QString::fromUtf8("label_GPIO_3"));
        label_GPIO_3->setGeometry(QRect(210, 100, 101, 20));
        comboBox = new QComboBox(Dialog_config_i2c);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(20, 100, 151, 22));
        label = new QLabel(Dialog_config_i2c);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 80, 101, 16));

        retranslateUi(Dialog_config_i2c);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog_config_i2c, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog_config_i2c, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog_config_i2c);
    } // setupUi

    void retranslateUi(QDialog *Dialog_config_i2c)
    {
        Dialog_config_i2c->setWindowTitle(QCoreApplication::translate("Dialog_config_i2c", "Dialog", nullptr));
        label_GPIO->setText(QCoreApplication::translate("Dialog_config_i2c", "SDA GPIO", nullptr));
        checkBox_invert_output->setText(QCoreApplication::translate("Dialog_config_i2c", "Enable pullup", nullptr));
        label_device_name->setText(QCoreApplication::translate("Dialog_config_i2c", "Device Name", nullptr));
        label_GPIO_2->setText(QCoreApplication::translate("Dialog_config_i2c", "SCL GPIO", nullptr));
        checkBox_invert_output_2->setText(QCoreApplication::translate("Dialog_config_i2c", "Enable pullup", nullptr));
        label_GPIO_3->setText(QCoreApplication::translate("Dialog_config_i2c", "Slave Address", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("Dialog_config_i2c", "MPU6050", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("Dialog_config_i2c", "ADXL345", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("Dialog_config_i2c", "GY271", nullptr));
        comboBox->setItemText(3, QCoreApplication::translate("Dialog_config_i2c", "MCP4725", nullptr));
        comboBox->setItemText(4, QCoreApplication::translate("Dialog_config_i2c", "GY530", nullptr));
        comboBox->setItemText(5, QCoreApplication::translate("Dialog_config_i2c", "DS1307", nullptr));
        comboBox->setItemText(6, QCoreApplication::translate("Dialog_config_i2c", "MAX30100", nullptr));
        comboBox->setItemText(7, QCoreApplication::translate("Dialog_config_i2c", "BMP280", nullptr));
        comboBox->setItemText(8, QCoreApplication::translate("Dialog_config_i2c", "LNA219", nullptr));

        label->setText(QCoreApplication::translate("Dialog_config_i2c", "Device sub type", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_config_i2c: public Ui_Dialog_config_i2c {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_CONFIG_I2C_H
