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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

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
    QLineEdit *lineEdit_device_name;
    QLabel *label_device_name;
    QComboBox *comboBox_spi_device_subtype;
    QLabel *label_device_spi_subtype;

    void setupUi(QDialog *Dialog_config_spi)
    {
        if (Dialog_config_spi->objectName().isEmpty())
            Dialog_config_spi->setObjectName(QString::fromUtf8("Dialog_config_spi"));
        Dialog_config_spi->resize(198, 296);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        Dialog_config_spi->setWindowIcon(icon);
        buttonBox_config_spi = new QDialogButtonBox(Dialog_config_spi);
        buttonBox_config_spi->setObjectName(QString::fromUtf8("buttonBox_config_spi"));
        buttonBox_config_spi->setGeometry(QRect(10, 249, 171, 32));
        buttonBox_config_spi->setOrientation(Qt::Horizontal);
        buttonBox_config_spi->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_mosi = new QLabel(Dialog_config_spi);
        label_mosi->setObjectName(QString::fromUtf8("label_mosi"));
        label_mosi->setGeometry(QRect(20, 120, 41, 20));
        label_miso = new QLabel(Dialog_config_spi);
        label_miso->setObjectName(QString::fromUtf8("label_miso"));
        label_miso->setGeometry(QRect(20, 150, 41, 20));
        label_sck = new QLabel(Dialog_config_spi);
        label_sck->setObjectName(QString::fromUtf8("label_sck"));
        label_sck->setGeometry(QRect(20, 179, 41, 20));
        label_cs = new QLabel(Dialog_config_spi);
        label_cs->setObjectName(QString::fromUtf8("label_cs"));
        label_cs->setGeometry(QRect(20, 210, 41, 20));
        comboBox_mosi = new QComboBox(Dialog_config_spi);
        comboBox_mosi->setObjectName(QString::fromUtf8("comboBox_mosi"));
        comboBox_mosi->setGeometry(QRect(70, 120, 111, 26));
        comboBox_miso = new QComboBox(Dialog_config_spi);
        comboBox_miso->setObjectName(QString::fromUtf8("comboBox_miso"));
        comboBox_miso->setGeometry(QRect(70, 150, 111, 26));
        comboBox_sck = new QComboBox(Dialog_config_spi);
        comboBox_sck->setObjectName(QString::fromUtf8("comboBox_sck"));
        comboBox_sck->setGeometry(QRect(70, 180, 111, 26));
        comboBox_cs = new QComboBox(Dialog_config_spi);
        comboBox_cs->setObjectName(QString::fromUtf8("comboBox_cs"));
        comboBox_cs->setGeometry(QRect(70, 210, 111, 26));
        lineEdit_device_name = new QLineEdit(Dialog_config_spi);
        lineEdit_device_name->setObjectName(QString::fromUtf8("lineEdit_device_name"));
        lineEdit_device_name->setGeometry(QRect(16, 26, 141, 26));
        label_device_name = new QLabel(Dialog_config_spi);
        label_device_name->setObjectName(QString::fromUtf8("label_device_name"));
        label_device_name->setGeometry(QRect(15, 4, 121, 20));
        comboBox_spi_device_subtype = new QComboBox(Dialog_config_spi);
        comboBox_spi_device_subtype->addItem(QString());
        comboBox_spi_device_subtype->setObjectName(QString::fromUtf8("comboBox_spi_device_subtype"));
        comboBox_spi_device_subtype->setGeometry(QRect(14, 81, 141, 26));
        label_device_spi_subtype = new QLabel(Dialog_config_spi);
        label_device_spi_subtype->setObjectName(QString::fromUtf8("label_device_spi_subtype"));
        label_device_spi_subtype->setGeometry(QRect(16, 58, 121, 20));

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
        lineEdit_device_name->setPlaceholderText(QCoreApplication::translate("Dialog_config_spi", "Friendly name", nullptr));
        label_device_name->setText(QCoreApplication::translate("Dialog_config_spi", "Device Name", nullptr));
        comboBox_spi_device_subtype->setItemText(0, QCoreApplication::translate("Dialog_config_spi", "BME280", nullptr));

        label_device_spi_subtype->setText(QCoreApplication::translate("Dialog_config_spi", "Device Subtype", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_config_spi: public Ui_Dialog_config_spi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_CONFIG_SPI_H
