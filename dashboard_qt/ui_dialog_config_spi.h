/********************************************************************************
** Form generated from reading UI file 'dialog_config_spi.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_CONFIG_SPI_H
#define UI_DIALOG_CONFIG_SPI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_Dialog_config_spi
{
public:
    QDialogButtonBox *buttonBox_config_spi;
    QLabel *label_mosi;
    QLabel *label_miso;
    QLabel *label_sck;
    QLabel *label_cs;
    QComboBox *comboBox_mosi;
    QComboBox *comboBox_miso;
    QComboBox *comboBox_sck;
    QComboBox *comboBox_cs;

    void setupUi(QDialog *Dialog_config_spi)
    {
        if (Dialog_config_spi->objectName().isEmpty())
            Dialog_config_spi->setObjectName(QString::fromUtf8("Dialog_config_spi"));
        Dialog_config_spi->resize(212, 230);
        buttonBox_config_spi = new QDialogButtonBox(Dialog_config_spi);
        buttonBox_config_spi->setObjectName(QString::fromUtf8("buttonBox_config_spi"));
        buttonBox_config_spi->setGeometry(QRect(10, 180, 191, 32));
        buttonBox_config_spi->setOrientation(Qt::Horizontal);
        buttonBox_config_spi->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_mosi = new QLabel(Dialog_config_spi);
        label_mosi->setObjectName(QString::fromUtf8("label_mosi"));
        label_mosi->setGeometry(QRect(20, 40, 41, 20));
        label_miso = new QLabel(Dialog_config_spi);
        label_miso->setObjectName(QString::fromUtf8("label_miso"));
        label_miso->setGeometry(QRect(20, 70, 41, 20));
        label_sck = new QLabel(Dialog_config_spi);
        label_sck->setObjectName(QString::fromUtf8("label_sck"));
        label_sck->setGeometry(QRect(20, 99, 41, 20));
        label_cs = new QLabel(Dialog_config_spi);
        label_cs->setObjectName(QString::fromUtf8("label_cs"));
        label_cs->setGeometry(QRect(20, 130, 41, 20));
        comboBox_mosi = new QComboBox(Dialog_config_spi);
        comboBox_mosi->setObjectName(QString::fromUtf8("comboBox_mosi"));
        comboBox_mosi->setGeometry(QRect(70, 40, 111, 26));
        comboBox_miso = new QComboBox(Dialog_config_spi);
        comboBox_miso->setObjectName(QString::fromUtf8("comboBox_miso"));
        comboBox_miso->setGeometry(QRect(70, 70, 111, 26));
        comboBox_sck = new QComboBox(Dialog_config_spi);
        comboBox_sck->setObjectName(QString::fromUtf8("comboBox_sck"));
        comboBox_sck->setGeometry(QRect(70, 100, 111, 26));
        comboBox_cs = new QComboBox(Dialog_config_spi);
        comboBox_cs->setObjectName(QString::fromUtf8("comboBox_cs"));
        comboBox_cs->setGeometry(QRect(70, 130, 111, 26));

        retranslateUi(Dialog_config_spi);
        QObject::connect(buttonBox_config_spi, SIGNAL(accepted()), Dialog_config_spi, SLOT(accept()));
        QObject::connect(buttonBox_config_spi, SIGNAL(rejected()), Dialog_config_spi, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog_config_spi);
    } // setupUi

    void retranslateUi(QDialog *Dialog_config_spi)
    {
        Dialog_config_spi->setWindowTitle(QCoreApplication::translate("Dialog_config_spi", "Dialog", nullptr));
        label_mosi->setText(QCoreApplication::translate("Dialog_config_spi", "MOSI", nullptr));
        label_miso->setText(QCoreApplication::translate("Dialog_config_spi", "MISO", nullptr));
        label_sck->setText(QCoreApplication::translate("Dialog_config_spi", "SCK", nullptr));
        label_cs->setText(QCoreApplication::translate("Dialog_config_spi", "CS", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_config_spi: public Ui_Dialog_config_spi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_CONFIG_SPI_H
