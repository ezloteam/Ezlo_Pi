/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_login
{
public:
    QLabel *label_user_name;
    QLabel *label_password;
    QLabel *label_enter_ezlo_credentials;
    QLineEdit *lineEdit_user_name;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_reset_password;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_sign_in;
    QLineEdit *lineEdit_password;
    QPushButton *pushButton_cancle;
    QPushButton *pushButton_login;

    void setupUi(QDialog *login)
    {
        if (login->objectName().isEmpty())
            login->setObjectName(QString::fromUtf8("login"));
        login->resize(346, 225);
        label_user_name = new QLabel(login);
        label_user_name->setObjectName(QString::fromUtf8("label_user_name"));
        label_user_name->setGeometry(QRect(32, 47, 66, 16));
        label_password = new QLabel(login);
        label_password->setObjectName(QString::fromUtf8("label_password"));
        label_password->setGeometry(QRect(32, 80, 57, 16));
        label_enter_ezlo_credentials = new QLabel(login);
        label_enter_ezlo_credentials->setObjectName(QString::fromUtf8("label_enter_ezlo_credentials"));
        label_enter_ezlo_credentials->setGeometry(QRect(31, 20, 291, 16));
        lineEdit_user_name = new QLineEdit(login);
        lineEdit_user_name->setObjectName(QString::fromUtf8("lineEdit_user_name"));
        lineEdit_user_name->setGeometry(QRect(110, 47, 201, 23));
        layoutWidget = new QWidget(login);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(80, 120, 232, 22));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_4 = new QSpacerItem(38, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);

        label_reset_password = new QLabel(layoutWidget);
        label_reset_password->setObjectName(QString::fromUtf8("label_reset_password"));

        horizontalLayout_3->addWidget(label_reset_password);

        horizontalSpacer_3 = new QSpacerItem(38, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        label_sign_in = new QLabel(layoutWidget);
        label_sign_in->setObjectName(QString::fromUtf8("label_sign_in"));

        horizontalLayout_3->addWidget(label_sign_in);

        lineEdit_password = new QLineEdit(login);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setGeometry(QRect(110, 80, 201, 23));
        lineEdit_password->setEchoMode(QLineEdit::Password);
        pushButton_cancle = new QPushButton(login);
        pushButton_cancle->setObjectName(QString::fromUtf8("pushButton_cancle"));
        pushButton_cancle->setGeometry(QRect(150, 180, 80, 23));
        pushButton_login = new QPushButton(login);
        pushButton_login->setObjectName(QString::fromUtf8("pushButton_login"));
        pushButton_login->setGeometry(QRect(250, 180, 80, 23));

        retranslateUi(login);

        QMetaObject::connectSlotsByName(login);
    } // setupUi

    void retranslateUi(QDialog *login)
    {
        login->setWindowTitle(QCoreApplication::translate("login", "Login to Ezlo Cloud", nullptr));
        label_user_name->setText(QCoreApplication::translate("login", "User name", nullptr));
        label_password->setText(QCoreApplication::translate("login", "Password", nullptr));
        label_enter_ezlo_credentials->setText(QCoreApplication::translate("login", "Enter ezlo user name and password to continue.", nullptr));
        lineEdit_user_name->setText(QString());
        lineEdit_user_name->setPlaceholderText(QCoreApplication::translate("login", "user name or email", nullptr));
        label_reset_password->setText(QCoreApplication::translate("login", "Reset Password", nullptr));
        label_sign_in->setText(QCoreApplication::translate("login", "Register", nullptr));
        lineEdit_password->setPlaceholderText(QCoreApplication::translate("login", "Enter password", nullptr));
        pushButton_cancle->setText(QCoreApplication::translate("login", "Cancle", nullptr));
        pushButton_login->setText(QCoreApplication::translate("login", "Login", nullptr));
    } // retranslateUi

};

namespace Ui {
    class login: public Ui_login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
