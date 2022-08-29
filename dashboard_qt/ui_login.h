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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
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
    QLabel *label_need_help_login;
    QLineEdit *lineEdit_password;
    QPushButton *pushButton_cancle;
    QPushButton *pushButton_login;
    QCheckBox *checkBox_show_password;

    void setupUi(QDialog *login)
    {
        if (login->objectName().isEmpty())
            login->setObjectName(QString::fromUtf8("login"));
        login->resize(346, 225);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        login->setWindowIcon(icon);
        label_user_name = new QLabel(login);
        label_user_name->setObjectName(QString::fromUtf8("label_user_name"));
        label_user_name->setGeometry(QRect(32, 47, 71, 16));
        label_password = new QLabel(login);
        label_password->setObjectName(QString::fromUtf8("label_password"));
        label_password->setGeometry(QRect(32, 80, 71, 16));
        label_enter_ezlo_credentials = new QLabel(login);
        label_enter_ezlo_credentials->setObjectName(QString::fromUtf8("label_enter_ezlo_credentials"));
        label_enter_ezlo_credentials->setGeometry(QRect(10, 20, 321, 16));
        lineEdit_user_name = new QLineEdit(login);
        lineEdit_user_name->setObjectName(QString::fromUtf8("lineEdit_user_name"));
        lineEdit_user_name->setGeometry(QRect(110, 47, 201, 23));
        layoutWidget = new QWidget(login);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(80, 140, 232, 22));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_4 = new QSpacerItem(38, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);

        label_need_help_login = new QLabel(layoutWidget);
        label_need_help_login->setObjectName(QString::fromUtf8("label_need_help_login"));
        label_need_help_login->setOpenExternalLinks(true);

        horizontalLayout_3->addWidget(label_need_help_login);

        lineEdit_password = new QLineEdit(login);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setGeometry(QRect(110, 80, 201, 23));
        lineEdit_password->setEchoMode(QLineEdit::Password);
        pushButton_cancle = new QPushButton(login);
        pushButton_cancle->setObjectName(QString::fromUtf8("pushButton_cancle"));
        pushButton_cancle->setGeometry(QRect(150, 180, 80, 31));
        pushButton_login = new QPushButton(login);
        pushButton_login->setObjectName(QString::fromUtf8("pushButton_login"));
        pushButton_login->setGeometry(QRect(250, 180, 80, 31));
        checkBox_show_password = new QCheckBox(login);
        checkBox_show_password->setObjectName(QString::fromUtf8("checkBox_show_password"));
        checkBox_show_password->setGeometry(QRect(110, 110, 161, 24));
#if QT_CONFIG(shortcut)
#endif // QT_CONFIG(shortcut)

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
#if QT_CONFIG(tooltip)
        label_need_help_login->setToolTip(QCoreApplication::translate("login", "Click for help from getvera login page.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_need_help_login->setText(QCoreApplication::translate("login", "<a href=\"https://home.getvera.com/users/login/\">Need Help in login ?</a>", nullptr));
        lineEdit_password->setPlaceholderText(QCoreApplication::translate("login", "Enter password", nullptr));
        pushButton_cancle->setText(QCoreApplication::translate("login", "Cancle", nullptr));
        pushButton_login->setText(QCoreApplication::translate("login", "Login", nullptr));
        checkBox_show_password->setText(QCoreApplication::translate("login", "Show password", nullptr));
    } // retranslateUi

};

namespace Ui {
    class login: public Ui_login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
