/****************************************************************************
** Meta object code from reading C++ file 'dialog_wifi.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../dialog_wifi.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialog_wifi.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Dialog_WiFi_t {
    QByteArrayData data[10];
    char stringdata0[171];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Dialog_WiFi_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Dialog_WiFi_t qt_meta_stringdata_Dialog_WiFi = {
    {
QT_MOC_LITERAL(0, 0, 11), // "Dialog_WiFi"
QT_MOC_LITERAL(1, 12, 26), // "ezpi_signal_serial_rx_wifi"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 8), // "ezpi_cmd"
QT_MOC_LITERAL(4, 49, 38), // "on_checkBox_view_password_sta..."
QT_MOC_LITERAL(5, 88, 4), // "arg1"
QT_MOC_LITERAL(6, 93, 21), // "on_buttonBox_accepted"
QT_MOC_LITERAL(7, 115, 16), // "process_response"
QT_MOC_LITERAL(8, 132, 22), // "data_response_set_wifi"
QT_MOC_LITERAL(9, 155, 15) // "serialConnector"

    },
    "Dialog_WiFi\0ezpi_signal_serial_rx_wifi\0"
    "\0ezpi_cmd\0on_checkBox_view_password_stateChanged\0"
    "arg1\0on_buttonBox_accepted\0process_response\0"
    "data_response_set_wifi\0serialConnector"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Dialog_WiFi[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   42,    2, 0x08 /* Private */,
       6,    0,   45,    2, 0x08 /* Private */,
       7,    1,   46,    2, 0x08 /* Private */,
       9,    0,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,

       0        // eod
};

void Dialog_WiFi::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Dialog_WiFi *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ezpi_signal_serial_rx_wifi((*reinterpret_cast< ezpi_cmd(*)>(_a[1]))); break;
        case 1: _t->on_checkBox_view_password_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_buttonBox_accepted(); break;
        case 3: _t->process_response((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->serialConnector(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Dialog_WiFi::*)(ezpi_cmd );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Dialog_WiFi::ezpi_signal_serial_rx_wifi)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Dialog_WiFi::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_Dialog_WiFi.data,
    qt_meta_data_Dialog_WiFi,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Dialog_WiFi::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Dialog_WiFi::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Dialog_WiFi.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Dialog_WiFi::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Dialog_WiFi::ezpi_signal_serial_rx_wifi(ezpi_cmd _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
