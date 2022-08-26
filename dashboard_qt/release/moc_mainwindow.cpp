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
    QByteArrayData data[40];
    char stringdata0[1067];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 37), // "on_pushButton_scan_uart_ports..."
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 34), // "on_pushButton_connect_uart_cl..."
QT_MOC_LITERAL(4, 85, 41), // "on_comboBox_uart_list_current..."
QT_MOC_LITERAL(5, 127, 30), // "on_pushButton_set_wifi_clicked"
QT_MOC_LITERAL(6, 158, 33), // "on_pushButton_erase_flash_cli..."
QT_MOC_LITERAL(7, 192, 37), // "on_pushButton_flash_ezpi_bins..."
QT_MOC_LITERAL(8, 230, 43), // "on_pushButton_clear_uart_dire..."
QT_MOC_LITERAL(9, 274, 48), // "on_comboBox_esp32_board_type_..."
QT_MOC_LITERAL(10, 323, 5), // "index"
QT_MOC_LITERAL(11, 329, 32), // "on_pushButton_add_device_clicked"
QT_MOC_LITERAL(12, 362, 35), // "on_pushButton_remove_device_c..."
QT_MOC_LITERAL(13, 398, 37), // "on_pushButton_get_ezpi_config..."
QT_MOC_LITERAL(14, 436, 37), // "on_pushButton_set_ezpi_config..."
QT_MOC_LITERAL(15, 474, 36), // "on_pushButton_device_restart_..."
QT_MOC_LITERAL(16, 511, 23), // "ezlogic_log_write_flash"
QT_MOC_LITERAL(17, 535, 23), // "ezlogic_log_erase_flash"
QT_MOC_LITERAL(18, 559, 22), // "ezlogic_check_firmware"
QT_MOC_LITERAL(19, 582, 9), // "EZPI_BOOL"
QT_MOC_LITERAL(20, 592, 41), // "ezlogic_message_info_no_firmw..."
QT_MOC_LITERAL(21, 634, 33), // "ezlogic_receive_dev_type_sele..."
QT_MOC_LITERAL(22, 668, 10), // "EZPI_UINT8"
QT_MOC_LITERAL(23, 679, 25), // "ezlogic_receive_added_dev"
QT_MOC_LITERAL(24, 705, 13), // "ezpi_dev_type"
QT_MOC_LITERAL(25, 719, 26), // "ezlogic_serial_receive_wif"
QT_MOC_LITERAL(26, 746, 8), // "ezpi_cmd"
QT_MOC_LITERAL(27, 755, 24), // "ezlogic_success_prov_dat"
QT_MOC_LITERAL(28, 780, 14), // "QNetworkReply*"
QT_MOC_LITERAL(29, 795, 1), // "d"
QT_MOC_LITERAL(30, 797, 30), // "ezlogic_success_get_prov_jsons"
QT_MOC_LITERAL(31, 828, 27), // "on_actionRegister_triggered"
QT_MOC_LITERAL(32, 856, 24), // "on_actionLogin_triggered"
QT_MOC_LITERAL(33, 881, 31), // "on_actionAbout_EzloPi_triggered"
QT_MOC_LITERAL(34, 913, 23), // "on_actionExit_triggered"
QT_MOC_LITERAL(35, 937, 30), // "on_actionClear_Table_triggered"
QT_MOC_LITERAL(36, 968, 26), // "on_actionDisable_triggered"
QT_MOC_LITERAL(37, 995, 23), // "on_actionInfo_triggered"
QT_MOC_LITERAL(38, 1019, 24), // "on_actionDebug_triggered"
QT_MOC_LITERAL(39, 1044, 22) // "ezlogic_serial_receive"

    },
    "MainWindow\0on_pushButton_scan_uart_ports_clicked\0"
    "\0on_pushButton_connect_uart_clicked\0"
    "on_comboBox_uart_list_currentIndexChanged\0"
    "on_pushButton_set_wifi_clicked\0"
    "on_pushButton_erase_flash_clicked\0"
    "on_pushButton_flash_ezpi_bins_clicked\0"
    "on_pushButton_clear_uart_direct_log_clicked\0"
    "on_comboBox_esp32_board_type_currentIndexChanged\0"
    "index\0on_pushButton_add_device_clicked\0"
    "on_pushButton_remove_device_clicked\0"
    "on_pushButton_get_ezpi_config_clicked\0"
    "on_pushButton_set_ezpi_config_clicked\0"
    "on_pushButton_device_restart_clicked\0"
    "ezlogic_log_write_flash\0ezlogic_log_erase_flash\0"
    "ezlogic_check_firmware\0EZPI_BOOL\0"
    "ezlogic_message_info_no_firmware_detected\0"
    "ezlogic_receive_dev_type_selected\0"
    "EZPI_UINT8\0ezlogic_receive_added_dev\0"
    "ezpi_dev_type\0ezlogic_serial_receive_wif\0"
    "ezpi_cmd\0ezlogic_success_prov_dat\0"
    "QNetworkReply*\0d\0ezlogic_success_get_prov_jsons\0"
    "on_actionRegister_triggered\0"
    "on_actionLogin_triggered\0"
    "on_actionAbout_EzloPi_triggered\0"
    "on_actionExit_triggered\0"
    "on_actionClear_Table_triggered\0"
    "on_actionDisable_triggered\0"
    "on_actionInfo_triggered\0"
    "on_actionDebug_triggered\0"
    "ezlogic_serial_receive"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      31,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  169,    2, 0x08 /* Private */,
       3,    0,  170,    2, 0x08 /* Private */,
       4,    0,  171,    2, 0x08 /* Private */,
       5,    0,  172,    2, 0x08 /* Private */,
       6,    0,  173,    2, 0x08 /* Private */,
       7,    0,  174,    2, 0x08 /* Private */,
       8,    0,  175,    2, 0x08 /* Private */,
       9,    1,  176,    2, 0x08 /* Private */,
      11,    0,  179,    2, 0x08 /* Private */,
      12,    0,  180,    2, 0x08 /* Private */,
      13,    0,  181,    2, 0x08 /* Private */,
      14,    0,  182,    2, 0x08 /* Private */,
      15,    0,  183,    2, 0x08 /* Private */,
      16,    0,  184,    2, 0x08 /* Private */,
      17,    0,  185,    2, 0x08 /* Private */,
      18,    0,  186,    2, 0x08 /* Private */,
      20,    0,  187,    2, 0x08 /* Private */,
      21,    1,  188,    2, 0x08 /* Private */,
      23,    1,  191,    2, 0x08 /* Private */,
      25,    1,  194,    2, 0x08 /* Private */,
      27,    1,  197,    2, 0x08 /* Private */,
      30,    1,  200,    2, 0x08 /* Private */,
      31,    0,  203,    2, 0x08 /* Private */,
      32,    0,  204,    2, 0x08 /* Private */,
      33,    0,  205,    2, 0x08 /* Private */,
      34,    0,  206,    2, 0x08 /* Private */,
      35,    0,  207,    2, 0x08 /* Private */,
      36,    0,  208,    2, 0x08 /* Private */,
      37,    0,  209,    2, 0x08 /* Private */,
      38,    0,  210,    2, 0x08 /* Private */,
      39,    0,  211,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    0x80000000 | 19,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 22,    2,
    QMetaType::Void, 0x80000000 | 24,    2,
    QMetaType::Void, 0x80000000 | 26,    2,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_pushButton_scan_uart_ports_clicked(); break;
        case 1: _t->on_pushButton_connect_uart_clicked(); break;
        case 2: _t->on_comboBox_uart_list_currentIndexChanged(); break;
        case 3: _t->on_pushButton_set_wifi_clicked(); break;
        case 4: _t->on_pushButton_erase_flash_clicked(); break;
        case 5: _t->on_pushButton_flash_ezpi_bins_clicked(); break;
        case 6: _t->on_pushButton_clear_uart_direct_log_clicked(); break;
        case 7: _t->on_comboBox_esp32_board_type_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->on_pushButton_add_device_clicked(); break;
        case 9: _t->on_pushButton_remove_device_clicked(); break;
        case 10: _t->on_pushButton_get_ezpi_config_clicked(); break;
        case 11: _t->on_pushButton_set_ezpi_config_clicked(); break;
        case 12: _t->on_pushButton_device_restart_clicked(); break;
        case 13: _t->ezlogic_log_write_flash(); break;
        case 14: _t->ezlogic_log_erase_flash(); break;
        case 15: { EZPI_BOOL _r = _t->ezlogic_check_firmware();
            if (_a[0]) *reinterpret_cast< EZPI_BOOL*>(_a[0]) = std::move(_r); }  break;
        case 16: _t->ezlogic_message_info_no_firmware_detected(); break;
        case 17: _t->ezlogic_receive_dev_type_selected((*reinterpret_cast< EZPI_UINT8(*)>(_a[1]))); break;
        case 18: _t->ezlogic_receive_added_dev((*reinterpret_cast< ezpi_dev_type(*)>(_a[1]))); break;
        case 19: _t->ezlogic_serial_receive_wif((*reinterpret_cast< ezpi_cmd(*)>(_a[1]))); break;
        case 20: _t->ezlogic_success_prov_dat((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 21: _t->ezlogic_success_get_prov_jsons((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 22: _t->on_actionRegister_triggered(); break;
        case 23: _t->on_actionLogin_triggered(); break;
        case 24: _t->on_actionAbout_EzloPi_triggered(); break;
        case 25: _t->on_actionExit_triggered(); break;
        case 26: _t->on_actionClear_Table_triggered(); break;
        case 27: _t->on_actionDisable_triggered(); break;
        case 28: _t->on_actionInfo_triggered(); break;
        case 29: _t->on_actionDebug_triggered(); break;
        case 30: _t->ezlogic_serial_receive(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 20:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 21:
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
        if (_id < 31)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 31)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
