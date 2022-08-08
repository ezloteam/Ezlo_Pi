/********************************************************************************
** Form generated from reading UI file 'dialog_config_input.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_CONFIG_INPUT_H
#define UI_DIALOG_CONFIG_INPUT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog_config_input
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_INPUT_GPIO;
    QComboBox *comboBox_default_value_input;
    QComboBox *comboBox_input_gpio;
    QLabel *label_default_value_input;
    QGroupBox *groupBox_resistor_input;
    QRadioButton *radioButton_input_pullup;
    QRadioButton *radioButton_input_pulldown;
    QCheckBox *checkBox_invert_input;

    void setupUi(QDialog *Dialog_config_input)
    {
        if (Dialog_config_input->objectName().isEmpty())
            Dialog_config_input->setObjectName(QString::fromUtf8("Dialog_config_input"));
        Dialog_config_input->resize(213, 396);
        buttonBox = new QDialogButtonBox(Dialog_config_input);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 340, 191, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_INPUT_GPIO = new QLabel(Dialog_config_input);
        label_INPUT_GPIO->setObjectName(QString::fromUtf8("label_INPUT_GPIO"));
        label_INPUT_GPIO->setGeometry(QRect(21, 10, 121, 20));
        comboBox_default_value_input = new QComboBox(Dialog_config_input);
        comboBox_default_value_input->addItem(QString());
        comboBox_default_value_input->addItem(QString());
        comboBox_default_value_input->setObjectName(QString::fromUtf8("comboBox_default_value_input"));
        comboBox_default_value_input->setGeometry(QRect(20, 250, 111, 26));
        comboBox_input_gpio = new QComboBox(Dialog_config_input);
        comboBox_input_gpio->setObjectName(QString::fromUtf8("comboBox_input_gpio"));
        comboBox_input_gpio->setGeometry(QRect(22, 39, 151, 26));
        label_default_value_input = new QLabel(Dialog_config_input);
        label_default_value_input->setObjectName(QString::fromUtf8("label_default_value_input"));
        label_default_value_input->setGeometry(QRect(20, 220, 121, 20));
        groupBox_resistor_input = new QGroupBox(Dialog_config_input);
        groupBox_resistor_input->setObjectName(QString::fromUtf8("groupBox_resistor_input"));
        groupBox_resistor_input->setGeometry(QRect(20, 80, 171, 121));
        radioButton_input_pullup = new QRadioButton(groupBox_resistor_input);
        radioButton_input_pullup->setObjectName(QString::fromUtf8("radioButton_input_pullup"));
        radioButton_input_pullup->setGeometry(QRect(20, 29, 110, 24));
        radioButton_input_pulldown = new QRadioButton(groupBox_resistor_input);
        radioButton_input_pulldown->setObjectName(QString::fromUtf8("radioButton_input_pulldown"));
        radioButton_input_pulldown->setGeometry(QRect(20, 70, 110, 24));
        checkBox_invert_input = new QCheckBox(Dialog_config_input);
        checkBox_invert_input->setObjectName(QString::fromUtf8("checkBox_invert_input"));
        checkBox_invert_input->setGeometry(QRect(20, 300, 151, 24));

        retranslateUi(Dialog_config_input);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog_config_input, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog_config_input, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog_config_input);
    } // setupUi

    void retranslateUi(QDialog *Dialog_config_input)
    {
        Dialog_config_input->setWindowTitle(QCoreApplication::translate("Dialog_config_input", "Dialog", nullptr));
        label_INPUT_GPIO->setText(QCoreApplication::translate("Dialog_config_input", "INPUT GPIO", nullptr));
        comboBox_default_value_input->setItemText(0, QCoreApplication::translate("Dialog_config_input", "HIGH", nullptr));
        comboBox_default_value_input->setItemText(1, QCoreApplication::translate("Dialog_config_input", "LOW", nullptr));

        label_default_value_input->setText(QCoreApplication::translate("Dialog_config_input", "Default Value", nullptr));
        groupBox_resistor_input->setTitle(QCoreApplication::translate("Dialog_config_input", "Resistor", nullptr));
        radioButton_input_pullup->setText(QCoreApplication::translate("Dialog_config_input", "PULL-UP", nullptr));
        radioButton_input_pulldown->setText(QCoreApplication::translate("Dialog_config_input", "PULL-DOWN", nullptr));
        checkBox_invert_input->setText(QCoreApplication::translate("Dialog_config_input", "INVERT INPUT", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_config_input: public Ui_Dialog_config_input {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_CONFIG_INPUT_H
