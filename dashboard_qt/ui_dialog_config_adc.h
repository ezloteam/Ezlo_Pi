/********************************************************************************
** Form generated from reading UI file 'dialog_config_adc.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_CONFIG_ADC_H
#define UI_DIALOG_CONFIG_ADC_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_Dialog_config_adc
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEdit_device_name;
    QComboBox *comboBox_ADC_subtype;
    QLabel *label_device_name;
    QLabel *label_adc_gpio;
    QLabel *label_adc_resolution;
    QComboBox *comboBox_adc_gpio;
    QLabel *label_device_sub_type;
    QComboBox *comboBox;

    void setupUi(QDialog *Dialog_config_adc)
    {
        if (Dialog_config_adc->objectName().isEmpty())
            Dialog_config_adc->setObjectName(QString::fromUtf8("Dialog_config_adc"));
        Dialog_config_adc->resize(167, 262);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        Dialog_config_adc->setWindowIcon(icon);
        buttonBox = new QDialogButtonBox(Dialog_config_adc);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 210, 151, 41));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit_device_name = new QLineEdit(Dialog_config_adc);
        lineEdit_device_name->setObjectName(QString::fromUtf8("lineEdit_device_name"));
        lineEdit_device_name->setGeometry(QRect(10, 28, 141, 26));
        comboBox_ADC_subtype = new QComboBox(Dialog_config_adc);
        comboBox_ADC_subtype->addItem(QString());
        comboBox_ADC_subtype->setObjectName(QString::fromUtf8("comboBox_ADC_subtype"));
        comboBox_ADC_subtype->setGeometry(QRect(11, 78, 141, 22));
        label_device_name = new QLabel(Dialog_config_adc);
        label_device_name->setObjectName(QString::fromUtf8("label_device_name"));
        label_device_name->setGeometry(QRect(14, 6, 121, 20));
        label_adc_gpio = new QLabel(Dialog_config_adc);
        label_adc_gpio->setObjectName(QString::fromUtf8("label_adc_gpio"));
        label_adc_gpio->setGeometry(QRect(13, 105, 121, 20));
        label_adc_resolution = new QLabel(Dialog_config_adc);
        label_adc_resolution->setObjectName(QString::fromUtf8("label_adc_resolution"));
        label_adc_resolution->setGeometry(QRect(10, 155, 101, 20));
        comboBox_adc_gpio = new QComboBox(Dialog_config_adc);
        comboBox_adc_gpio->setObjectName(QString::fromUtf8("comboBox_adc_gpio"));
        comboBox_adc_gpio->setGeometry(QRect(10, 128, 141, 26));
        label_device_sub_type = new QLabel(Dialog_config_adc);
        label_device_sub_type->setObjectName(QString::fromUtf8("label_device_sub_type"));
        label_device_sub_type->setGeometry(QRect(11, 58, 121, 16));
        comboBox = new QComboBox(Dialog_config_adc);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(10, 176, 141, 26));

        retranslateUi(Dialog_config_adc);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog_config_adc, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog_config_adc, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog_config_adc);
    } // setupUi

    void retranslateUi(QDialog *Dialog_config_adc)
    {
        Dialog_config_adc->setWindowTitle(QCoreApplication::translate("Dialog_config_adc", "Dialog", nullptr));
        lineEdit_device_name->setPlaceholderText(QCoreApplication::translate("Dialog_config_adc", "Friendly name", nullptr));
        comboBox_ADC_subtype->setItemText(0, QCoreApplication::translate("Dialog_config_adc", "POT : Generic", nullptr));

        label_device_name->setText(QCoreApplication::translate("Dialog_config_adc", "Device Name", nullptr));
        label_adc_gpio->setText(QCoreApplication::translate("Dialog_config_adc", "ADC input pin", nullptr));
        label_adc_resolution->setText(QCoreApplication::translate("Dialog_config_adc", "Resolution", nullptr));
        label_device_sub_type->setText(QCoreApplication::translate("Dialog_config_adc", "Device sub type", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("Dialog_config_adc", "8-Bit", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("Dialog_config_adc", "10-Bit", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("Dialog_config_adc", "12-Bit", nullptr));

    } // retranslateUi

};

namespace Ui {
    class Dialog_config_adc: public Ui_Dialog_config_adc {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_CONFIG_ADC_H
