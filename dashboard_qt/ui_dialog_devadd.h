/********************************************************************************
** Form generated from reading UI file 'dialog_devadd.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_DEVADD_H
#define UI_DIALOG_DEVADD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

QT_BEGIN_NAMESPACE

class Ui_Dialog_devadd
{
public:
    QDialogButtonBox *buttonBox;
    QComboBox *comboBox_list_dev_type;

    void setupUi(QDialog *Dialog_devadd)
    {
        if (Dialog_devadd->objectName().isEmpty())
            Dialog_devadd->setObjectName(QString::fromUtf8("Dialog_devadd"));
        Dialog_devadd->resize(247, 120);
        buttonBox = new QDialogButtonBox(Dialog_devadd);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 70, 191, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        comboBox_list_dev_type = new QComboBox(Dialog_devadd);
        comboBox_list_dev_type->addItem(QString());
        comboBox_list_dev_type->addItem(QString());
        comboBox_list_dev_type->addItem(QString());
        comboBox_list_dev_type->addItem(QString());
        comboBox_list_dev_type->addItem(QString());
        comboBox_list_dev_type->addItem(QString());
        comboBox_list_dev_type->addItem(QString());
        comboBox_list_dev_type->addItem(QString());
        comboBox_list_dev_type->addItem(QString());
        comboBox_list_dev_type->setObjectName(QString::fromUtf8("comboBox_list_dev_type"));
        comboBox_list_dev_type->setGeometry(QRect(20, 20, 211, 26));

        retranslateUi(Dialog_devadd);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog_devadd, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog_devadd, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog_devadd);
    } // setupUi

    void retranslateUi(QDialog *Dialog_devadd)
    {
        Dialog_devadd->setWindowTitle(QCoreApplication::translate("Dialog_devadd", "Choose Device", nullptr));
        comboBox_list_dev_type->setItemText(0, QCoreApplication::translate("Dialog_devadd", "Digital Output", nullptr));
        comboBox_list_dev_type->setItemText(1, QCoreApplication::translate("Dialog_devadd", "Digital Input", nullptr));
        comboBox_list_dev_type->setItemText(2, QCoreApplication::translate("Dialog_devadd", "Analog Input", nullptr));
        comboBox_list_dev_type->setItemText(3, QCoreApplication::translate("Dialog_devadd", "Analog Output", nullptr));
        comboBox_list_dev_type->setItemText(4, QCoreApplication::translate("Dialog_devadd", "PWM", nullptr));
        comboBox_list_dev_type->setItemText(5, QCoreApplication::translate("Dialog_devadd", "UART", nullptr));
        comboBox_list_dev_type->setItemText(6, QCoreApplication::translate("Dialog_devadd", "One Wire", nullptr));
        comboBox_list_dev_type->setItemText(7, QCoreApplication::translate("Dialog_devadd", "I2C", nullptr));
        comboBox_list_dev_type->setItemText(8, QCoreApplication::translate("Dialog_devadd", "SPI", nullptr));

    } // retranslateUi

};

namespace Ui {
    class Dialog_devadd: public Ui_Dialog_devadd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_DEVADD_H
