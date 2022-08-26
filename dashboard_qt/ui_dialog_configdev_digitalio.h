/********************************************************************************
** Form generated from reading UI file 'dialog_configdev_digitalio.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_CONFIGDEV_DIGITALIO_H
#define UI_DIALOG_CONFIGDEV_DIGITALIO_H

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

class Ui_Dialog_configdev_digitalio
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_device_name;
    QLineEdit *lineEdit_device_name;
    QCheckBox *checkBox_enable_pushbutton;
    QComboBox *comboBox_output_gpio;
    QComboBox *comboBox_input_gpio;
    QGroupBox *groupBox_resistor_output;
    QRadioButton *radioButton_output_pullup;
    QRadioButton *radioButton_output_pulldown;
    QGroupBox *groupBox_resistor_input;
    QRadioButton *radioButton_input_pullup;
    QRadioButton *radioButton_input_pulldown;
    QLabel *label_GPIO_OUTPUT;
    QLabel *label_INPUT_GPIO;
    QLabel *label_default_value_output;
    QLabel *label_default_value_input;
    QComboBox *comboBox_default_value_output;
    QComboBox *comboBox_default_value_input;
    QCheckBox *checkBox_invert_output;
    QCheckBox *checkBox_invert_input;
    QComboBox *comboBox_output_subtype;
    QLabel *label_device_name_2;

    void setupUi(QDialog *Dialog_configdev_digitalio)
    {
        if (Dialog_configdev_digitalio->objectName().isEmpty())
            Dialog_configdev_digitalio->setObjectName(QString::fromUtf8("Dialog_configdev_digitalio"));
        Dialog_configdev_digitalio->resize(274, 337);
        buttonBox = new QDialogButtonBox(Dialog_configdev_digitalio);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(72, 299, 191, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_device_name = new QLabel(Dialog_configdev_digitalio);
        label_device_name->setObjectName(QString::fromUtf8("label_device_name"));
        label_device_name->setGeometry(QRect(12, 2, 121, 20));
        lineEdit_device_name = new QLineEdit(Dialog_configdev_digitalio);
        lineEdit_device_name->setObjectName(QString::fromUtf8("lineEdit_device_name"));
        lineEdit_device_name->setGeometry(QRect(8, 23, 151, 26));
        lineEdit_device_name->setContextMenuPolicy(Qt::NoContextMenu);
        checkBox_enable_pushbutton = new QCheckBox(Dialog_configdev_digitalio);
        checkBox_enable_pushbutton->setObjectName(QString::fromUtf8("checkBox_enable_pushbutton"));
        checkBox_enable_pushbutton->setGeometry(QRect(144, 75, 101, 24));
        comboBox_output_gpio = new QComboBox(Dialog_configdev_digitalio);
        comboBox_output_gpio->setObjectName(QString::fromUtf8("comboBox_output_gpio"));
        comboBox_output_gpio->setGeometry(QRect(15, 122, 101, 20));
        comboBox_input_gpio = new QComboBox(Dialog_configdev_digitalio);
        comboBox_input_gpio->setObjectName(QString::fromUtf8("comboBox_input_gpio"));
        comboBox_input_gpio->setGeometry(QRect(148, 123, 101, 20));
        groupBox_resistor_output = new QGroupBox(Dialog_configdev_digitalio);
        groupBox_resistor_output->setObjectName(QString::fromUtf8("groupBox_resistor_output"));
        groupBox_resistor_output->setGeometry(QRect(12, 149, 111, 61));
        radioButton_output_pullup = new QRadioButton(groupBox_resistor_output);
        radioButton_output_pullup->setObjectName(QString::fromUtf8("radioButton_output_pullup"));
        radioButton_output_pullup->setGeometry(QRect(8, 11, 110, 24));
        radioButton_output_pullup->setChecked(true);
        radioButton_output_pulldown = new QRadioButton(groupBox_resistor_output);
        radioButton_output_pulldown->setObjectName(QString::fromUtf8("radioButton_output_pulldown"));
        radioButton_output_pulldown->setGeometry(QRect(8, 34, 110, 24));
        groupBox_resistor_input = new QGroupBox(Dialog_configdev_digitalio);
        groupBox_resistor_input->setObjectName(QString::fromUtf8("groupBox_resistor_input"));
        groupBox_resistor_input->setGeometry(QRect(146, 150, 111, 61));
        radioButton_input_pullup = new QRadioButton(groupBox_resistor_input);
        radioButton_input_pullup->setObjectName(QString::fromUtf8("radioButton_input_pullup"));
        radioButton_input_pullup->setGeometry(QRect(10, 14, 110, 24));
        radioButton_input_pullup->setChecked(true);
        radioButton_input_pulldown = new QRadioButton(groupBox_resistor_input);
        radioButton_input_pulldown->setObjectName(QString::fromUtf8("radioButton_input_pulldown"));
        radioButton_input_pulldown->setGeometry(QRect(10, 35, 110, 24));
        label_GPIO_OUTPUT = new QLabel(Dialog_configdev_digitalio);
        label_GPIO_OUTPUT->setObjectName(QString::fromUtf8("label_GPIO_OUTPUT"));
        label_GPIO_OUTPUT->setGeometry(QRect(14, 99, 121, 20));
        label_INPUT_GPIO = new QLabel(Dialog_configdev_digitalio);
        label_INPUT_GPIO->setObjectName(QString::fromUtf8("label_INPUT_GPIO"));
        label_INPUT_GPIO->setGeometry(QRect(149, 100, 121, 20));
        label_default_value_output = new QLabel(Dialog_configdev_digitalio);
        label_default_value_output->setObjectName(QString::fromUtf8("label_default_value_output"));
        label_default_value_output->setGeometry(QRect(12, 210, 111, 20));
        label_default_value_input = new QLabel(Dialog_configdev_digitalio);
        label_default_value_input->setObjectName(QString::fromUtf8("label_default_value_input"));
        label_default_value_input->setGeometry(QRect(146, 212, 121, 20));
        comboBox_default_value_output = new QComboBox(Dialog_configdev_digitalio);
        comboBox_default_value_output->addItem(QString());
        comboBox_default_value_output->addItem(QString());
        comboBox_default_value_output->setObjectName(QString::fromUtf8("comboBox_default_value_output"));
        comboBox_default_value_output->setGeometry(QRect(12, 230, 111, 26));
        comboBox_default_value_input = new QComboBox(Dialog_configdev_digitalio);
        comboBox_default_value_input->addItem(QString());
        comboBox_default_value_input->addItem(QString());
        comboBox_default_value_input->setObjectName(QString::fromUtf8("comboBox_default_value_input"));
        comboBox_default_value_input->setGeometry(QRect(146, 233, 111, 26));
        checkBox_invert_output = new QCheckBox(Dialog_configdev_digitalio);
        checkBox_invert_output->setObjectName(QString::fromUtf8("checkBox_invert_output"));
        checkBox_invert_output->setGeometry(QRect(12, 261, 111, 24));
        checkBox_invert_input = new QCheckBox(Dialog_configdev_digitalio);
        checkBox_invert_input->setObjectName(QString::fromUtf8("checkBox_invert_input"));
        checkBox_invert_input->setGeometry(QRect(146, 263, 111, 24));
        comboBox_output_subtype = new QComboBox(Dialog_configdev_digitalio);
        comboBox_output_subtype->addItem(QString());
        comboBox_output_subtype->addItem(QString());
        comboBox_output_subtype->addItem(QString());
        comboBox_output_subtype->setObjectName(QString::fromUtf8("comboBox_output_subtype"));
        comboBox_output_subtype->setGeometry(QRect(12, 74, 101, 20));
        label_device_name_2 = new QLabel(Dialog_configdev_digitalio);
        label_device_name_2->setObjectName(QString::fromUtf8("label_device_name_2"));
        label_device_name_2->setGeometry(QRect(14, 52, 81, 20));

        retranslateUi(Dialog_configdev_digitalio);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog_configdev_digitalio, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog_configdev_digitalio, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog_configdev_digitalio);
    } // setupUi

    void retranslateUi(QDialog *Dialog_configdev_digitalio)
    {
        Dialog_configdev_digitalio->setWindowTitle(QCoreApplication::translate("Dialog_configdev_digitalio", "Configure Digital Output", nullptr));
        label_device_name->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "Device Name", nullptr));
        lineEdit_device_name->setPlaceholderText(QCoreApplication::translate("Dialog_configdev_digitalio", "Device friendly name", nullptr));
        checkBox_enable_pushbutton->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "Enable Button", nullptr));
        groupBox_resistor_output->setTitle(QCoreApplication::translate("Dialog_configdev_digitalio", "Resistor", nullptr));
        radioButton_output_pullup->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "PULL-UP", nullptr));
        radioButton_output_pulldown->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "PULL-DOWN", nullptr));
        groupBox_resistor_input->setTitle(QCoreApplication::translate("Dialog_configdev_digitalio", "Resistor", nullptr));
        radioButton_input_pullup->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "PULL-UP", nullptr));
        radioButton_input_pulldown->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "PULL-DOWN", nullptr));
        label_GPIO_OUTPUT->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "OUTPUT GPIO", nullptr));
        label_INPUT_GPIO->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "INPUT GPIO", nullptr));
        label_default_value_output->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "Default Value", nullptr));
        label_default_value_input->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "Default Value", nullptr));
        comboBox_default_value_output->setItemText(0, QCoreApplication::translate("Dialog_configdev_digitalio", "LOW", nullptr));
        comboBox_default_value_output->setItemText(1, QCoreApplication::translate("Dialog_configdev_digitalio", "HIGH", nullptr));

        comboBox_default_value_input->setItemText(0, QCoreApplication::translate("Dialog_configdev_digitalio", "LOW", nullptr));
        comboBox_default_value_input->setItemText(1, QCoreApplication::translate("Dialog_configdev_digitalio", "HIGH", nullptr));

        checkBox_invert_output->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "INVERT OUTPUT", nullptr));
        checkBox_invert_input->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "INVERT INPUT", nullptr));
        comboBox_output_subtype->setItemText(0, QCoreApplication::translate("Dialog_configdev_digitalio", "LED", nullptr));
        comboBox_output_subtype->setItemText(1, QCoreApplication::translate("Dialog_configdev_digitalio", "Relay", nullptr));
        comboBox_output_subtype->setItemText(2, QCoreApplication::translate("Dialog_configdev_digitalio", "Plug", nullptr));

        label_device_name_2->setText(QCoreApplication::translate("Dialog_configdev_digitalio", "Device subtype", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_configdev_digitalio: public Ui_Dialog_configdev_digitalio {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_CONFIGDEV_DIGITALIO_H
