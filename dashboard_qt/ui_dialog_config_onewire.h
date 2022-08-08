/********************************************************************************
** Form generated from reading UI file 'dialog_config_onewire.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_CONFIG_ONEWIRE_H
#define UI_DIALOG_CONFIG_ONEWIRE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog_config_onewire
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_device_name;
    QLineEdit *lineEdit_device_name;
    QLabel *label_default_value_output;
    QComboBox *comboBox_default_value;
    QLabel *label_GPIO;
    QGroupBox *groupBox_resistor;
    QRadioButton *radioButton_pullup;
    QRadioButton *radioButton_pulldown;
    QComboBox *comboBox_output_gpio;
    QCheckBox *checkBox_invert_output;
    QComboBox *comboBox_output_gpio_2;
    QLabel *label_GPIO_2;

    void setupUi(QDialog *Dialog_config_onewire)
    {
        if (Dialog_config_onewire->objectName().isEmpty())
            Dialog_config_onewire->setObjectName(QString::fromUtf8("Dialog_config_onewire"));
        Dialog_config_onewire->resize(360, 282);
        buttonBox = new QDialogButtonBox(Dialog_config_onewire);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 230, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_device_name = new QLabel(Dialog_config_onewire);
        label_device_name->setObjectName(QString::fromUtf8("label_device_name"));
        label_device_name->setGeometry(QRect(34, 33, 121, 20));
        lineEdit_device_name = new QLineEdit(Dialog_config_onewire);
        lineEdit_device_name->setObjectName(QString::fromUtf8("lineEdit_device_name"));
        lineEdit_device_name->setGeometry(QRect(30, 60, 151, 26));
        label_default_value_output = new QLabel(Dialog_config_onewire);
        label_default_value_output->setObjectName(QString::fromUtf8("label_default_value_output"));
        label_default_value_output->setGeometry(QRect(25, 152, 121, 20));
        comboBox_default_value = new QComboBox(Dialog_config_onewire);
        comboBox_default_value->addItem(QString());
        comboBox_default_value->addItem(QString());
        comboBox_default_value->setObjectName(QString::fromUtf8("comboBox_default_value"));
        comboBox_default_value->setGeometry(QRect(25, 180, 111, 26));
        label_GPIO = new QLabel(Dialog_config_onewire);
        label_GPIO->setObjectName(QString::fromUtf8("label_GPIO"));
        label_GPIO->setGeometry(QRect(210, 156, 121, 20));
        groupBox_resistor = new QGroupBox(Dialog_config_onewire);
        groupBox_resistor->setObjectName(QString::fromUtf8("groupBox_resistor"));
        groupBox_resistor->setGeometry(QRect(210, 30, 121, 91));
        radioButton_pullup = new QRadioButton(groupBox_resistor);
        radioButton_pullup->setObjectName(QString::fromUtf8("radioButton_pullup"));
        radioButton_pullup->setGeometry(QRect(20, 20, 110, 24));
        radioButton_pulldown = new QRadioButton(groupBox_resistor);
        radioButton_pulldown->setObjectName(QString::fromUtf8("radioButton_pulldown"));
        radioButton_pulldown->setGeometry(QRect(20, 51, 110, 24));
        comboBox_output_gpio = new QComboBox(Dialog_config_onewire);
        comboBox_output_gpio->setObjectName(QString::fromUtf8("comboBox_output_gpio"));
        comboBox_output_gpio->setGeometry(QRect(209, 177, 121, 26));
        checkBox_invert_output = new QCheckBox(Dialog_config_onewire);
        checkBox_invert_output->setObjectName(QString::fromUtf8("checkBox_invert_output"));
        checkBox_invert_output->setGeometry(QRect(210, 130, 141, 24));
        comboBox_output_gpio_2 = new QComboBox(Dialog_config_onewire);
        comboBox_output_gpio_2->addItem(QString());
        comboBox_output_gpio_2->setObjectName(QString::fromUtf8("comboBox_output_gpio_2"));
        comboBox_output_gpio_2->setGeometry(QRect(25, 118, 141, 26));
        label_GPIO_2 = new QLabel(Dialog_config_onewire);
        label_GPIO_2->setObjectName(QString::fromUtf8("label_GPIO_2"));
        label_GPIO_2->setGeometry(QRect(30, 90, 121, 20));

        retranslateUi(Dialog_config_onewire);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog_config_onewire, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog_config_onewire, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog_config_onewire);
    } // setupUi

    void retranslateUi(QDialog *Dialog_config_onewire)
    {
        Dialog_config_onewire->setWindowTitle(QCoreApplication::translate("Dialog_config_onewire", "Dialog", nullptr));
        label_device_name->setText(QCoreApplication::translate("Dialog_config_onewire", "Device Name", nullptr));
        label_default_value_output->setText(QCoreApplication::translate("Dialog_config_onewire", "Default Value", nullptr));
        comboBox_default_value->setItemText(0, QCoreApplication::translate("Dialog_config_onewire", "HIGH", nullptr));
        comboBox_default_value->setItemText(1, QCoreApplication::translate("Dialog_config_onewire", "LOW", nullptr));

        label_GPIO->setText(QCoreApplication::translate("Dialog_config_onewire", "GPIO", nullptr));
        groupBox_resistor->setTitle(QCoreApplication::translate("Dialog_config_onewire", "Resistor", nullptr));
        radioButton_pullup->setText(QCoreApplication::translate("Dialog_config_onewire", "PULL-UP", nullptr));
        radioButton_pulldown->setText(QCoreApplication::translate("Dialog_config_onewire", "PULL-DOWN", nullptr));
        checkBox_invert_output->setText(QCoreApplication::translate("Dialog_config_onewire", "INVERT OUTPUT", nullptr));
        comboBox_output_gpio_2->setItemText(0, QCoreApplication::translate("Dialog_config_onewire", "DHT11", nullptr));

        label_GPIO_2->setText(QCoreApplication::translate("Dialog_config_onewire", "Device Subtype", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_config_onewire: public Ui_Dialog_config_onewire {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_CONFIG_ONEWIRE_H
