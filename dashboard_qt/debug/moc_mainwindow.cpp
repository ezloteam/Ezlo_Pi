/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[43];
    char stringdata0[1251];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 20), // "ezpi_log_write_flash"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 20), // "ezpi_log_erase_flash"
QT_MOC_LITERAL(4, 54, 19), // "ezpi_check_firmware"
QT_MOC_LITERAL(5, 74, 38), // "ezpi_message_info_no_firmware..."
QT_MOC_LITERAL(6, 113, 30), // "ezpi_receive_dev_type_selected"
QT_MOC_LITERAL(7, 144, 10), // "EZPI_UINT8"
QT_MOC_LITERAL(8, 155, 22), // "ezpi_receive_added_dev"
QT_MOC_LITERAL(9, 178, 13), // "ezpi_dev_type"
QT_MOC_LITERAL(10, 192, 9), // "on_serRX1"
QT_MOC_LITERAL(11, 202, 34), // "on_pushButton_connect_uart_cl..."
QT_MOC_LITERAL(12, 237, 32), // "on_pushButton_add_device_clicked"
QT_MOC_LITERAL(13, 270, 33), // "on_pushButton_gpio_cancel_cli..."
QT_MOC_LITERAL(14, 304, 32), // "on_pushButton_gpio_apply_clicked"
QT_MOC_LITERAL(15, 337, 35), // "on_pushButton_remove_device_c..."
QT_MOC_LITERAL(16, 373, 37), // "on_pushButton_get_ezpi_config..."
QT_MOC_LITERAL(17, 411, 37), // "on_pushButton_set_ezpi_config..."
QT_MOC_LITERAL(18, 449, 37), // "on_pushButton_flash_ezpi_bins..."
QT_MOC_LITERAL(19, 487, 43), // "on_pushButton_clear_uart_dire..."
QT_MOC_LITERAL(20, 531, 30), // "on_pushButton_set_wifi_clicked"
QT_MOC_LITERAL(21, 562, 37), // "on_pushButton_set_wifi_cancel..."
QT_MOC_LITERAL(22, 600, 36), // "on_pushButton_set_wifi_apply_..."
QT_MOC_LITERAL(23, 637, 46), // "on_pushButton_select_device_t..."
QT_MOC_LITERAL(24, 684, 47), // "on_pushButton_select_device_t..."
QT_MOC_LITERAL(25, 732, 50), // "on_comboBox_gpio_out_select_p..."
QT_MOC_LITERAL(26, 783, 4), // "arg1"
QT_MOC_LITERAL(27, 788, 49), // "on_comboBox_gpio_in_select_pi..."
QT_MOC_LITERAL(28, 838, 47), // "on_checkBox_gpio_led_button_e..."
QT_MOC_LITERAL(29, 886, 33), // "on_pushButton_erase_flash_cli..."
QT_MOC_LITERAL(30, 920, 37), // "on_pushButton_scan_uart_ports..."
QT_MOC_LITERAL(31, 958, 41), // "on_comboBox_uart_list_current..."
QT_MOC_LITERAL(32, 1000, 23), // "on_actionExit_triggered"
QT_MOC_LITERAL(33, 1024, 29), // "on_actionEnable_Log_triggered"
QT_MOC_LITERAL(34, 1054, 31), // "on_actionAbout_EzloPi_triggered"
QT_MOC_LITERAL(35, 1086, 27), // "on_actionRegister_triggered"
QT_MOC_LITERAL(36, 1114, 16), // "success_prov_dat"
QT_MOC_LITERAL(37, 1131, 14), // "QNetworkReply*"
QT_MOC_LITERAL(38, 1146, 1), // "d"
QT_MOC_LITERAL(39, 1148, 22), // "success_get_prov_jsons"
QT_MOC_LITERAL(40, 1171, 24), // "on_actionLogin_triggered"
QT_MOC_LITERAL(41, 1196, 48), // "on_comboBox_esp32_board_type_..."
QT_MOC_LITERAL(42, 1245, 5) // "index"

    },
    "MainWindow\0ezpi_log_write_flash\0\0"
    "ezpi_log_erase_flash\0ezpi_check_firmware\0"
    "ezpi_message_info_no_firmware_detected\0"
    "ezpi_receive_dev_type_selected\0"
    "EZPI_UINT8\0ezpi_receive_added_dev\0"
    "ezpi_dev_type\0on_serRX1\0"
    "on_pushButton_connect_uart_clicked\0"
    "on_pushButton_add_device_clicked\0"
    "on_pushButton_gpio_cancel_clicked\0"
    "on_pushButton_gpio_apply_clicked\0"
    "on_pushButton_remove_device_clicked\0"
    "on_pushButton_get_ezpi_config_clicked\0"
    "on_pushButton_set_ezpi_config_clicked\0"
    "on_pushButton_flash_ezpi_bins_clicked\0"
    "on_pushButton_clear_uart_direct_log_clicked\0"
    "on_pushButton_set_wifi_clicked\0"
    "on_pushButton_set_wifi_cancel_clicked\0"
    "on_pushButton_set_wifi_apply_clicked\0"
    "on_pushButton_select_device_type_apply_clicked\0"
    "on_pushButton_select_device_type_cancel_clicked\0"
    "on_comboBox_gpio_out_select_pin_currentTextChanged\0"
    "arg1\0on_comboBox_gpio_in_select_pin_currentTextChanged\0"
    "on_checkBox_gpio_led_button_enable_stateChanged\0"
    "on_pushButton_erase_flash_clicked\0"
    "on_pushButton_scan_uart_ports_clicked\0"
    "on_comboBox_uart_list_currentIndexChanged\0"
    "on_actionExit_triggered\0"
    "on_actionEnable_Log_triggered\0"
    "on_actionAbout_EzloPi_triggered\0"
    "on_actionRegister_triggered\0"
    "success_prov_dat\0QNetworkReply*\0d\0"
    "success_get_prov_jsons\0on_actionLogin_triggered\0"
    "on_comboBox_esp32_board_type_currentIndexChanged\0"
    "index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      35,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  189,    2, 0x08 /* Private */,
       3,    0,  190,    2, 0x08 /* Private */,
       4,    0,  191,    2, 0x08 /* Private */,
       5,    0,  192,    2, 0x08 /* Private */,
       6,    1,  193,    2, 0x08 /* Private */,
       8,    1,  196,    2, 0x08 /* Private */,
      10,    0,  199,    2, 0x08 /* Private */,
      11,    0,  200,    2, 0x08 /* Private */,
      12,    0,  201,    2, 0x08 /* Private */,
      13,    0,  202,    2, 0x08 /* Private */,
      14,    0,  203,    2, 0x08 /* Private */,
      15,    0,  204,    2, 0x08 /* Private */,
      16,    0,  205,    2, 0x08 /* Private */,
      17,    0,  206,    2, 0x08 /* Private */,
      18,    0,  207,    2, 0x08 /* Private */,
      19,    0,  208,    2, 0x08 /* Private */,
      20,    0,  209,    2, 0x08 /* Private */,
      21,    0,  210,    2, 0x08 /* Private */,
      22,    0,  211,    2, 0x08 /* Private */,
      23,    0,  212,    2, 0x08 /* Private */,
      24,    0,  213,    2, 0x08 /* Private */,
      25,    1,  214,    2, 0x08 /* Private */,
      27,    1,  217,    2, 0x08 /* Private */,
      28,    0,  220,    2, 0x08 /* Private */,
      29,    0,  221,    2, 0x08 /* Private */,
      30,    0,  222,    2, 0x08 /* Private */,
      31,    0,  223,    2, 0x08 /* Private */,
      32,    0,  224,    2, 0x08 /* Private */,
      33,    0,  225,    2, 0x08 /* Private */,
      34,    0,  226,    2, 0x08 /* Private */,
      35,    0,  227,    2, 0x08 /* Private */,
      36,    1,  228,    2, 0x08 /* Private */,
      39,    1,  231,    2, 0x08 /* Private */,
      40,    0,  234,    2, 0x08 /* Private */,
      41,    1,  235,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   26,
    QMetaType::Void, QMetaType::QString,   26,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 37,   38,
    QMetaType::Void, 0x80000000 | 37,   38,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   42,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ezpi_log_write_flash(); break;
        case 1: _t->ezpi_log_erase_flash(); break;
        case 2: _t->ezpi_check_firmware(); break;
        case 3: _t->ezpi_message_info_no_firmware_detected(); break;
        case 4: _t->ezpi_receive_dev_type_selected((*reinterpret_cast< EZPI_UINT8(*)>(_a[1]))); break;
        case 5: _t->ezpi_receive_added_dev((*reinterpret_cast< ezpi_dev_type(*)>(_a[1]))); break;
        case 6: _t->on_serRX1(); break;
        case 7: _t->on_pushButton_connect_uart_clicked(); break;
        case 8: _t->on_pushButton_add_device_clicked(); break;
        case 9: _t->on_pushButton_gpio_cancel_clicked(); break;
        case 10: _t->on_pushButton_gpio_apply_clicked(); break;
        case 11: _t->on_pushButton_remove_device_clicked(); break;
        case 12: _t->on_pushButton_get_ezpi_config_clicked(); break;
        case 13: _t->on_pushButton_set_ezpi_config_clicked(); break;
        case 14: _t->on_pushButton_flash_ezpi_bins_clicked(); break;
        case 15: _t->on_pushButton_clear_uart_direct_log_clicked(); break;
        case 16: _t->on_pushButton_set_wifi_clicked(); break;
        case 17: _t->on_pushButton_set_wifi_cancel_clicked(); break;
        case 18: _t->on_pushButton_set_wifi_apply_clicked(); break;
        case 19: _t->on_pushButton_select_device_type_apply_clicked(); break;
        case 20: _t->on_pushButton_select_device_type_cancel_clicked(); break;
        case 21: _t->on_comboBox_gpio_out_select_pin_currentTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 22: _t->on_comboBox_gpio_in_select_pin_currentTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 23: _t->on_checkBox_gpio_led_button_enable_stateChanged(); break;
        case 24: _t->on_pushButton_erase_flash_clicked(); break;
        case 25: _t->on_pushButton_scan_uart_ports_clicked(); break;
        case 26: _t->on_comboBox_uart_list_currentIndexChanged(); break;
        case 27: _t->on_actionExit_triggered(); break;
        case 28: _t->on_actionEnable_Log_triggered(); break;
        case 29: _t->on_actionAbout_EzloPi_triggered(); break;
        case 30: _t->on_actionRegister_triggered(); break;
        case 31: _t->success_prov_dat((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 32: _t->success_get_prov_jsons((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 33: _t->on_actionLogin_triggered(); break;
        case 34: _t->on_comboBox_esp32_board_type_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 31:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 32:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 35)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 35;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 35)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 35;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
