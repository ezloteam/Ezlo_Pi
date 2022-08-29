/********************************************************************************
** Form generated from reading UI file 'dialog_wifi.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_WIFI_H
#define UI_DIALOG_WIFI_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_Dialog_WiFi
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEdit_SSID;
    QLineEdit *lineEdit_password;
    QLabel *label_ssid;
    QLabel *label_password;
    QCheckBox *checkBox_view_password;

    void setupUi(QDialog *Dialog_WiFi)
    {
        if (Dialog_WiFi->objectName().isEmpty())
            Dialog_WiFi->setObjectName(QString::fromUtf8("Dialog_WiFi"));
        Dialog_WiFi->resize(314, 162);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        Dialog_WiFi->setWindowIcon(icon);
        buttonBox = new QDialogButtonBox(Dialog_WiFi);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(0, 120, 301, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit_SSID = new QLineEdit(Dialog_WiFi);
        lineEdit_SSID->setObjectName(QString::fromUtf8("lineEdit_SSID"));
        lineEdit_SSID->setGeometry(QRect(90, 20, 211, 21));
        lineEdit_password = new QLineEdit(Dialog_WiFi);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setGeometry(QRect(90, 50, 211, 21));
        lineEdit_password->setEchoMode(QLineEdit::Password);
        label_ssid = new QLabel(Dialog_WiFi);
        label_ssid->setObjectName(QString::fromUtf8("label_ssid"));
        label_ssid->setGeometry(QRect(20, 20, 63, 20));
        label_password = new QLabel(Dialog_WiFi);
        label_password->setObjectName(QString::fromUtf8("label_password"));
        label_password->setGeometry(QRect(20, 50, 63, 20));
        checkBox_view_password = new QCheckBox(Dialog_WiFi);
        checkBox_view_password->setObjectName(QString::fromUtf8("checkBox_view_password"));
        checkBox_view_password->setGeometry(QRect(90, 80, 141, 24));

        retranslateUi(Dialog_WiFi);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog_WiFi, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog_WiFi, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog_WiFi);
    } // setupUi

    void retranslateUi(QDialog *Dialog_WiFi)
    {
        Dialog_WiFi->setWindowTitle(QCoreApplication::translate("Dialog_WiFi", "Configure WiFi", nullptr));
        lineEdit_SSID->setPlaceholderText(QCoreApplication::translate("Dialog_WiFi", "Enter WiFi SSID (WiFi name)", nullptr));
        lineEdit_password->setPlaceholderText(QCoreApplication::translate("Dialog_WiFi", "Enter wifi password", nullptr));
        label_ssid->setText(QCoreApplication::translate("Dialog_WiFi", "SSID", nullptr));
        label_password->setText(QCoreApplication::translate("Dialog_WiFi", "Password", nullptr));
        checkBox_view_password->setText(QCoreApplication::translate("Dialog_WiFi", "View Password ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_WiFi: public Ui_Dialog_WiFi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_WIFI_H
