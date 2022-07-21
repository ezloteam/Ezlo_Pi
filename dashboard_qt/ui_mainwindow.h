/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionExit;
    QAction *actionAbout_EzloPi;
    QAction *actionEnable_Log;
    QAction *actionRegister;
    QAction *actionLogin;
    QWidget *centralWidget;
    QScrollArea *scrollArea_device_type;
    QWidget *scrollAreaWidgetContents_add_device;
    QLabel *label_select_device_type;
    QPushButton *pushButton_select_device_type_apply;
    QPushButton *pushButton_select_device_type_cancel;
    QComboBox *comboBox_device_type;
    QScrollArea *scrollArea_set_wifi_cred;
    QWidget *scrollAreaWidgetContents_set_wifi;
    QLabel *label_set_wifi;
    QLabel *label_set_wifi_ssid;
    QLineEdit *lineEdit_set_wifi_ssid;
    QLabel *label_set_wifi_pass;
    QLineEdit *lineEdit_set_wifi_pass;
    QPushButton *pushButton_set_wifi_apply;
    QPushButton *pushButton_set_wifi_cancel;
    QScrollArea *scrollArea_gpio_config;
    QWidget *scrollAreaWidgetContents;
    QLabel *label_gpio_out_gpio;
    QComboBox *comboBox_gpio_out_select_pin;
    QGroupBox *groupBox_gpio_our_default_shunt_res;
    QRadioButton *radioButton_gpio_out_default_pull_up;
    QRadioButton *radioButton_gpio_out_default_pull_down;
    QGroupBox *groupBox_gpio_our_default_bool;
    QRadioButton *radioButton_gpio_out_default_low;
    QRadioButton *radioButton_gpio_out_default_high;
    QPushButton *pushButton_gpio_apply;
    QPushButton *pushButton_gpio_cancel;
    QLabel *label_gpio_device_name;
    QLineEdit *lineEdit_gpio_device_name;
    QCheckBox *checkBox_gpio_led_button_enable;
    QLabel *label_gpio_input_gpio;
    QComboBox *comboBox_gpio_in_select_pin;
    QGroupBox *groupBox_gpio_in_default_bool;
    QRadioButton *radioButton_gpio_in_default_low;
    QRadioButton *radioButton_gpio_in_default_high;
    QCheckBox *checkBox_gpio_in_logic_type;
    QCheckBox *checkBox_gpio_out_logic_type;
    QGroupBox *groupBox_gpio_in_default_shunt_res;
    QRadioButton *radioButton_gpio_in_default_pull_up;
    QRadioButton *radioButton_gpio_in_default_pull_down;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_scan_uart_ports;
    QComboBox *comboBox_uart_list;
    QPushButton *pushButton_connect_uart;
    QPushButton *pushButton_set_wifi;
    QPushButton *pushButton_erase_flash;
    QPushButton *pushButton_flash_ezpi_bins;
    QPushButton *pushButton_clear_uart_direct_log;
    QSplitter *splitter;
    QTableWidget *tableWidget_device_table;
    QTextBrowser *textBrowser_console_log;
    QGroupBox *groupBox_device_config;
    QPushButton *pushButton_get_ezpi_config;
    QPushButton *pushButton_set_ezpi_config;
    QGroupBox *groupBox;
    QPushButton *pushButton_add_device;
    QPushButton *pushButton_remove_device;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuView;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(810, 628);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionAbout_EzloPi = new QAction(MainWindow);
        actionAbout_EzloPi->setObjectName(QString::fromUtf8("actionAbout_EzloPi"));
        actionEnable_Log = new QAction(MainWindow);
        actionEnable_Log->setObjectName(QString::fromUtf8("actionEnable_Log"));
        actionRegister = new QAction(MainWindow);
        actionRegister->setObjectName(QString::fromUtf8("actionRegister"));
        actionLogin = new QAction(MainWindow);
        actionLogin->setObjectName(QString::fromUtf8("actionLogin"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        scrollArea_device_type = new QScrollArea(centralWidget);
        scrollArea_device_type->setObjectName(QString::fromUtf8("scrollArea_device_type"));
        scrollArea_device_type->setGeometry(QRect(70, 140, 241, 181));
        scrollArea_device_type->setWidgetResizable(true);
        scrollAreaWidgetContents_add_device = new QWidget();
        scrollAreaWidgetContents_add_device->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_add_device"));
        scrollAreaWidgetContents_add_device->setGeometry(QRect(0, 0, 239, 179));
        label_select_device_type = new QLabel(scrollAreaWidgetContents_add_device);
        label_select_device_type->setObjectName(QString::fromUtf8("label_select_device_type"));
        label_select_device_type->setGeometry(QRect(80, 20, 81, 16));
        pushButton_select_device_type_apply = new QPushButton(scrollAreaWidgetContents_add_device);
        pushButton_select_device_type_apply->setObjectName(QString::fromUtf8("pushButton_select_device_type_apply"));
        pushButton_select_device_type_apply->setGeometry(QRect(30, 130, 75, 23));
        pushButton_select_device_type_cancel = new QPushButton(scrollAreaWidgetContents_add_device);
        pushButton_select_device_type_cancel->setObjectName(QString::fromUtf8("pushButton_select_device_type_cancel"));
        pushButton_select_device_type_cancel->setGeometry(QRect(140, 130, 75, 23));
        comboBox_device_type = new QComboBox(scrollAreaWidgetContents_add_device);
        comboBox_device_type->addItem(QString());
        comboBox_device_type->addItem(QString());
        comboBox_device_type->addItem(QString());
        comboBox_device_type->addItem(QString());
        comboBox_device_type->addItem(QString());
        comboBox_device_type->addItem(QString());
        comboBox_device_type->addItem(QString());
        comboBox_device_type->addItem(QString());
        comboBox_device_type->addItem(QString());
        comboBox_device_type->setObjectName(QString::fromUtf8("comboBox_device_type"));
        comboBox_device_type->setGeometry(QRect(60, 70, 121, 22));
        scrollArea_device_type->setWidget(scrollAreaWidgetContents_add_device);
        scrollArea_set_wifi_cred = new QScrollArea(centralWidget);
        scrollArea_set_wifi_cred->setObjectName(QString::fromUtf8("scrollArea_set_wifi_cred"));
        scrollArea_set_wifi_cred->setGeometry(QRect(200, 150, 251, 211));
        scrollArea_set_wifi_cred->setWidgetResizable(true);
        scrollAreaWidgetContents_set_wifi = new QWidget();
        scrollAreaWidgetContents_set_wifi->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_set_wifi"));
        scrollAreaWidgetContents_set_wifi->setGeometry(QRect(0, 0, 249, 209));
        label_set_wifi = new QLabel(scrollAreaWidgetContents_set_wifi);
        label_set_wifi->setObjectName(QString::fromUtf8("label_set_wifi"));
        label_set_wifi->setGeometry(QRect(110, 20, 47, 13));
        label_set_wifi_ssid = new QLabel(scrollAreaWidgetContents_set_wifi);
        label_set_wifi_ssid->setObjectName(QString::fromUtf8("label_set_wifi_ssid"));
        label_set_wifi_ssid->setGeometry(QRect(20, 60, 47, 13));
        lineEdit_set_wifi_ssid = new QLineEdit(scrollAreaWidgetContents_set_wifi);
        lineEdit_set_wifi_ssid->setObjectName(QString::fromUtf8("lineEdit_set_wifi_ssid"));
        lineEdit_set_wifi_ssid->setGeometry(QRect(20, 80, 201, 20));
        label_set_wifi_pass = new QLabel(scrollAreaWidgetContents_set_wifi);
        label_set_wifi_pass->setObjectName(QString::fromUtf8("label_set_wifi_pass"));
        label_set_wifi_pass->setGeometry(QRect(20, 110, 47, 13));
        lineEdit_set_wifi_pass = new QLineEdit(scrollAreaWidgetContents_set_wifi);
        lineEdit_set_wifi_pass->setObjectName(QString::fromUtf8("lineEdit_set_wifi_pass"));
        lineEdit_set_wifi_pass->setGeometry(QRect(20, 140, 201, 20));
        pushButton_set_wifi_apply = new QPushButton(scrollAreaWidgetContents_set_wifi);
        pushButton_set_wifi_apply->setObjectName(QString::fromUtf8("pushButton_set_wifi_apply"));
        pushButton_set_wifi_apply->setGeometry(QRect(30, 170, 75, 23));
        pushButton_set_wifi_cancel = new QPushButton(scrollAreaWidgetContents_set_wifi);
        pushButton_set_wifi_cancel->setObjectName(QString::fromUtf8("pushButton_set_wifi_cancel"));
        pushButton_set_wifi_cancel->setGeometry(QRect(130, 170, 75, 23));
        scrollArea_set_wifi_cred->setWidget(scrollAreaWidgetContents_set_wifi);
        scrollArea_gpio_config = new QScrollArea(centralWidget);
        scrollArea_gpio_config->setObjectName(QString::fromUtf8("scrollArea_gpio_config"));
        scrollArea_gpio_config->setGeometry(QRect(170, 110, 281, 361));
        scrollArea_gpio_config->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 279, 359));
        label_gpio_out_gpio = new QLabel(scrollAreaWidgetContents);
        label_gpio_out_gpio->setObjectName(QString::fromUtf8("label_gpio_out_gpio"));
        label_gpio_out_gpio->setGeometry(QRect(10, 60, 71, 16));
        comboBox_gpio_out_select_pin = new QComboBox(scrollAreaWidgetContents);
        comboBox_gpio_out_select_pin->setObjectName(QString::fromUtf8("comboBox_gpio_out_select_pin"));
        comboBox_gpio_out_select_pin->setGeometry(QRect(10, 80, 69, 22));
        groupBox_gpio_our_default_shunt_res = new QGroupBox(scrollAreaWidgetContents);
        groupBox_gpio_our_default_shunt_res->setObjectName(QString::fromUtf8("groupBox_gpio_our_default_shunt_res"));
        groupBox_gpio_our_default_shunt_res->setGeometry(QRect(10, 110, 120, 80));
        radioButton_gpio_out_default_pull_up = new QRadioButton(groupBox_gpio_our_default_shunt_res);
        radioButton_gpio_out_default_pull_up->setObjectName(QString::fromUtf8("radioButton_gpio_out_default_pull_up"));
        radioButton_gpio_out_default_pull_up->setGeometry(QRect(10, 20, 82, 17));
        radioButton_gpio_out_default_pull_down = new QRadioButton(groupBox_gpio_our_default_shunt_res);
        radioButton_gpio_out_default_pull_down->setObjectName(QString::fromUtf8("radioButton_gpio_out_default_pull_down"));
        radioButton_gpio_out_default_pull_down->setGeometry(QRect(10, 50, 82, 17));
        groupBox_gpio_our_default_bool = new QGroupBox(scrollAreaWidgetContents);
        groupBox_gpio_our_default_bool->setObjectName(QString::fromUtf8("groupBox_gpio_our_default_bool"));
        groupBox_gpio_our_default_bool->setGeometry(QRect(10, 200, 120, 80));
        radioButton_gpio_out_default_low = new QRadioButton(groupBox_gpio_our_default_bool);
        radioButton_gpio_out_default_low->setObjectName(QString::fromUtf8("radioButton_gpio_out_default_low"));
        radioButton_gpio_out_default_low->setGeometry(QRect(10, 20, 82, 17));
        radioButton_gpio_out_default_high = new QRadioButton(groupBox_gpio_our_default_bool);
        radioButton_gpio_out_default_high->setObjectName(QString::fromUtf8("radioButton_gpio_out_default_high"));
        radioButton_gpio_out_default_high->setGeometry(QRect(10, 50, 82, 17));
        pushButton_gpio_apply = new QPushButton(scrollAreaWidgetContents);
        pushButton_gpio_apply->setObjectName(QString::fromUtf8("pushButton_gpio_apply"));
        pushButton_gpio_apply->setGeometry(QRect(30, 330, 75, 23));
        pushButton_gpio_cancel = new QPushButton(scrollAreaWidgetContents);
        pushButton_gpio_cancel->setObjectName(QString::fromUtf8("pushButton_gpio_cancel"));
        pushButton_gpio_cancel->setGeometry(QRect(140, 330, 75, 23));
        label_gpio_device_name = new QLabel(scrollAreaWidgetContents);
        label_gpio_device_name->setObjectName(QString::fromUtf8("label_gpio_device_name"));
        label_gpio_device_name->setGeometry(QRect(10, 10, 81, 16));
        lineEdit_gpio_device_name = new QLineEdit(scrollAreaWidgetContents);
        lineEdit_gpio_device_name->setObjectName(QString::fromUtf8("lineEdit_gpio_device_name"));
        lineEdit_gpio_device_name->setGeometry(QRect(10, 30, 113, 20));
        lineEdit_gpio_device_name->setMaxLength(16);
        checkBox_gpio_led_button_enable = new QCheckBox(scrollAreaWidgetContents);
        checkBox_gpio_led_button_enable->setObjectName(QString::fromUtf8("checkBox_gpio_led_button_enable"));
        checkBox_gpio_led_button_enable->setGeometry(QRect(150, 30, 121, 20));
        label_gpio_input_gpio = new QLabel(scrollAreaWidgetContents);
        label_gpio_input_gpio->setObjectName(QString::fromUtf8("label_gpio_input_gpio"));
        label_gpio_input_gpio->setGeometry(QRect(150, 60, 71, 16));
        comboBox_gpio_in_select_pin = new QComboBox(scrollAreaWidgetContents);
        comboBox_gpio_in_select_pin->setObjectName(QString::fromUtf8("comboBox_gpio_in_select_pin"));
        comboBox_gpio_in_select_pin->setGeometry(QRect(150, 80, 69, 22));
        groupBox_gpio_in_default_bool = new QGroupBox(scrollAreaWidgetContents);
        groupBox_gpio_in_default_bool->setObjectName(QString::fromUtf8("groupBox_gpio_in_default_bool"));
        groupBox_gpio_in_default_bool->setGeometry(QRect(150, 200, 120, 80));
        radioButton_gpio_in_default_low = new QRadioButton(groupBox_gpio_in_default_bool);
        radioButton_gpio_in_default_low->setObjectName(QString::fromUtf8("radioButton_gpio_in_default_low"));
        radioButton_gpio_in_default_low->setGeometry(QRect(10, 20, 82, 17));
        radioButton_gpio_in_default_high = new QRadioButton(groupBox_gpio_in_default_bool);
        radioButton_gpio_in_default_high->setObjectName(QString::fromUtf8("radioButton_gpio_in_default_high"));
        radioButton_gpio_in_default_high->setGeometry(QRect(10, 50, 82, 17));
        checkBox_gpio_in_logic_type = new QCheckBox(scrollAreaWidgetContents);
        checkBox_gpio_in_logic_type->setObjectName(QString::fromUtf8("checkBox_gpio_in_logic_type"));
        checkBox_gpio_in_logic_type->setGeometry(QRect(160, 290, 101, 17));
        checkBox_gpio_out_logic_type = new QCheckBox(scrollAreaWidgetContents);
        checkBox_gpio_out_logic_type->setObjectName(QString::fromUtf8("checkBox_gpio_out_logic_type"));
        checkBox_gpio_out_logic_type->setGeometry(QRect(10, 290, 101, 17));
        groupBox_gpio_in_default_shunt_res = new QGroupBox(scrollAreaWidgetContents);
        groupBox_gpio_in_default_shunt_res->setObjectName(QString::fromUtf8("groupBox_gpio_in_default_shunt_res"));
        groupBox_gpio_in_default_shunt_res->setGeometry(QRect(150, 110, 120, 80));
        radioButton_gpio_in_default_pull_up = new QRadioButton(groupBox_gpio_in_default_shunt_res);
        radioButton_gpio_in_default_pull_up->setObjectName(QString::fromUtf8("radioButton_gpio_in_default_pull_up"));
        radioButton_gpio_in_default_pull_up->setGeometry(QRect(10, 20, 82, 17));
        radioButton_gpio_in_default_pull_down = new QRadioButton(groupBox_gpio_in_default_shunt_res);
        radioButton_gpio_in_default_pull_down->setObjectName(QString::fromUtf8("radioButton_gpio_in_default_pull_down"));
        radioButton_gpio_in_default_pull_down->setGeometry(QRect(10, 50, 82, 17));
        scrollArea_gpio_config->setWidget(scrollAreaWidgetContents);
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 726, 31));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton_scan_uart_ports = new QPushButton(layoutWidget);
        pushButton_scan_uart_ports->setObjectName(QString::fromUtf8("pushButton_scan_uart_ports"));

        horizontalLayout->addWidget(pushButton_scan_uart_ports);

        comboBox_uart_list = new QComboBox(layoutWidget);
        comboBox_uart_list->setObjectName(QString::fromUtf8("comboBox_uart_list"));

        horizontalLayout->addWidget(comboBox_uart_list);

        pushButton_connect_uart = new QPushButton(layoutWidget);
        pushButton_connect_uart->setObjectName(QString::fromUtf8("pushButton_connect_uart"));

        horizontalLayout->addWidget(pushButton_connect_uart);

        pushButton_set_wifi = new QPushButton(layoutWidget);
        pushButton_set_wifi->setObjectName(QString::fromUtf8("pushButton_set_wifi"));

        horizontalLayout->addWidget(pushButton_set_wifi);

        pushButton_erase_flash = new QPushButton(layoutWidget);
        pushButton_erase_flash->setObjectName(QString::fromUtf8("pushButton_erase_flash"));

        horizontalLayout->addWidget(pushButton_erase_flash);

        pushButton_flash_ezpi_bins = new QPushButton(layoutWidget);
        pushButton_flash_ezpi_bins->setObjectName(QString::fromUtf8("pushButton_flash_ezpi_bins"));

        horizontalLayout->addWidget(pushButton_flash_ezpi_bins);

        pushButton_clear_uart_direct_log = new QPushButton(layoutWidget);
        pushButton_clear_uart_direct_log->setObjectName(QString::fromUtf8("pushButton_clear_uart_direct_log"));

        horizontalLayout->addWidget(pushButton_clear_uart_direct_log);

        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setGeometry(QRect(10, 60, 611, 471));
        splitter->setOrientation(Qt::Vertical);
        tableWidget_device_table = new QTableWidget(splitter);
        if (tableWidget_device_table->columnCount() < 5)
            tableWidget_device_table->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget_device_table->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget_device_table->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget_device_table->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget_device_table->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget_device_table->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        tableWidget_device_table->setObjectName(QString::fromUtf8("tableWidget_device_table"));
        tableWidget_device_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        splitter->addWidget(tableWidget_device_table);
        textBrowser_console_log = new QTextBrowser(splitter);
        textBrowser_console_log->setObjectName(QString::fromUtf8("textBrowser_console_log"));
        splitter->addWidget(textBrowser_console_log);
        groupBox_device_config = new QGroupBox(centralWidget);
        groupBox_device_config->setObjectName(QString::fromUtf8("groupBox_device_config"));
        groupBox_device_config->setGeometry(QRect(637, 200, 151, 121));
        pushButton_get_ezpi_config = new QPushButton(groupBox_device_config);
        pushButton_get_ezpi_config->setObjectName(QString::fromUtf8("pushButton_get_ezpi_config"));
        pushButton_get_ezpi_config->setGeometry(QRect(20, 30, 111, 31));
        pushButton_set_ezpi_config = new QPushButton(groupBox_device_config);
        pushButton_set_ezpi_config->setObjectName(QString::fromUtf8("pushButton_set_ezpi_config"));
        pushButton_set_ezpi_config->setGeometry(QRect(20, 70, 111, 31));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(639, 60, 149, 129));
        pushButton_add_device = new QPushButton(groupBox);
        pushButton_add_device->setObjectName(QString::fromUtf8("pushButton_add_device"));
        pushButton_add_device->setGeometry(QRect(20, 30, 111, 31));
        pushButton_remove_device = new QPushButton(groupBox);
        pushButton_remove_device->setObjectName(QString::fromUtf8("pushButton_remove_device"));
        pushButton_remove_device->setGeometry(QRect(20, 80, 111, 31));
        MainWindow->setCentralWidget(centralWidget);
        layoutWidget->raise();
        splitter->raise();
        scrollArea_device_type->raise();
        scrollArea_set_wifi_cred->raise();
        scrollArea_gpio_config->raise();
        groupBox_device_config->raise();
        groupBox->raise();
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 810, 26));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionLogin);
        menuFile->addAction(actionRegister);
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout_EzloPi);
        menuView->addAction(actionEnable_Log);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "EzLogic - EzloPi", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionAbout_EzloPi->setText(QCoreApplication::translate("MainWindow", "About EzloPi", nullptr));
        actionEnable_Log->setText(QCoreApplication::translate("MainWindow", "Disable Log", nullptr));
#if QT_CONFIG(statustip)
        actionEnable_Log->setStatusTip(QCoreApplication::translate("MainWindow", "Check to enable log to the log console.", nullptr));
#endif // QT_CONFIG(statustip)
#if QT_CONFIG(shortcut)
        actionEnable_Log->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+L", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRegister->setText(QCoreApplication::translate("MainWindow", "Register", nullptr));
        actionLogin->setText(QCoreApplication::translate("MainWindow", "Login", nullptr));
        label_select_device_type->setText(QCoreApplication::translate("MainWindow", "Select TYPE", nullptr));
        pushButton_select_device_type_apply->setText(QCoreApplication::translate("MainWindow", "Apply", nullptr));
        pushButton_select_device_type_cancel->setText(QCoreApplication::translate("MainWindow", "Cancel ", nullptr));
        comboBox_device_type->setItemText(0, QCoreApplication::translate("MainWindow", "LED", nullptr));
        comboBox_device_type->setItemText(1, QCoreApplication::translate("MainWindow", "SWITCH", nullptr));
        comboBox_device_type->setItemText(2, QCoreApplication::translate("MainWindow", "PLUG", nullptr));
        comboBox_device_type->setItemText(3, QCoreApplication::translate("MainWindow", "TAMPER", nullptr));
        comboBox_device_type->setItemText(4, QCoreApplication::translate("MainWindow", "1WIRE", nullptr));
        comboBox_device_type->setItemText(5, QCoreApplication::translate("MainWindow", "I2C", nullptr));
        comboBox_device_type->setItemText(6, QCoreApplication::translate("MainWindow", "PWM", nullptr));
        comboBox_device_type->setItemText(7, QCoreApplication::translate("MainWindow", "ADC", nullptr));
        comboBox_device_type->setItemText(8, QCoreApplication::translate("MainWindow", "Frequency ", nullptr));

        label_set_wifi->setText(QCoreApplication::translate("MainWindow", "SET WiFi", nullptr));
        label_set_wifi_ssid->setText(QCoreApplication::translate("MainWindow", "SSID", nullptr));
        label_set_wifi_pass->setText(QCoreApplication::translate("MainWindow", "Password", nullptr));
        pushButton_set_wifi_apply->setText(QCoreApplication::translate("MainWindow", "Apply", nullptr));
        pushButton_set_wifi_cancel->setText(QCoreApplication::translate("MainWindow", "Cancel", nullptr));
        label_gpio_out_gpio->setText(QCoreApplication::translate("MainWindow", "OUT GPIO", nullptr));
        groupBox_gpio_our_default_shunt_res->setTitle(QCoreApplication::translate("MainWindow", "Resistor", nullptr));
        radioButton_gpio_out_default_pull_up->setText(QCoreApplication::translate("MainWindow", "PULL-UP", nullptr));
        radioButton_gpio_out_default_pull_down->setText(QCoreApplication::translate("MainWindow", "PULL-DOWN", nullptr));
        groupBox_gpio_our_default_bool->setTitle(QCoreApplication::translate("MainWindow", "Default value ", nullptr));
        radioButton_gpio_out_default_low->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        radioButton_gpio_out_default_high->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        pushButton_gpio_apply->setText(QCoreApplication::translate("MainWindow", "Apply", nullptr));
        pushButton_gpio_cancel->setText(QCoreApplication::translate("MainWindow", "Cancel ", nullptr));
        label_gpio_device_name->setText(QCoreApplication::translate("MainWindow", "DEVICE NAME", nullptr));
        checkBox_gpio_led_button_enable->setText(QCoreApplication::translate("MainWindow", "BUTTON ENABLE", nullptr));
        label_gpio_input_gpio->setText(QCoreApplication::translate("MainWindow", "INPUT  GPIO", nullptr));
        groupBox_gpio_in_default_bool->setTitle(QCoreApplication::translate("MainWindow", "Default value ", nullptr));
        radioButton_gpio_in_default_low->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        radioButton_gpio_in_default_high->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        checkBox_gpio_in_logic_type->setText(QCoreApplication::translate("MainWindow", "INPUT INVERT", nullptr));
        checkBox_gpio_out_logic_type->setText(QCoreApplication::translate("MainWindow", "OUT INVERT", nullptr));
        groupBox_gpio_in_default_shunt_res->setTitle(QCoreApplication::translate("MainWindow", "Resistor", nullptr));
        radioButton_gpio_in_default_pull_up->setText(QCoreApplication::translate("MainWindow", "PULL-UP", nullptr));
        radioButton_gpio_in_default_pull_down->setText(QCoreApplication::translate("MainWindow", "PULL-DOWN", nullptr));
        pushButton_scan_uart_ports->setText(QCoreApplication::translate("MainWindow", "Scan", nullptr));
        pushButton_connect_uart->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
        pushButton_set_wifi->setText(QCoreApplication::translate("MainWindow", "Configure WiFi", nullptr));
        pushButton_erase_flash->setText(QCoreApplication::translate("MainWindow", "Erase Device", nullptr));
        pushButton_flash_ezpi_bins->setText(QCoreApplication::translate("MainWindow", "Flash Firmware", nullptr));
        pushButton_clear_uart_direct_log->setText(QCoreApplication::translate("MainWindow", "Clear Logs", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidget_device_table->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "GPIO", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget_device_table->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget_device_table->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Resistor", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget_device_table->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Def Vol", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget_device_table->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "Type", nullptr));
        textBrowser_console_log->setPlaceholderText(QCoreApplication::translate("MainWindow", "log console", nullptr));
        groupBox_device_config->setTitle(QCoreApplication::translate("MainWindow", "Configure", nullptr));
        pushButton_get_ezpi_config->setText(QCoreApplication::translate("MainWindow", "Get config", nullptr));
        pushButton_set_ezpi_config->setText(QCoreApplication::translate("MainWindow", "Set Config", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Device", nullptr));
        pushButton_add_device->setText(QCoreApplication::translate("MainWindow", "Add Device", nullptr));
        pushButton_remove_device->setText(QCoreApplication::translate("MainWindow", "Remove Device", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
