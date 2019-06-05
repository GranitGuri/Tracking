/****************************************************************************
** Meta object code from reading C++ file 'VMainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../VMainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VMainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VMainWindow_t {
    QByteArrayData data[18];
    char stringdata0[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VMainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VMainWindow_t qt_meta_stringdata_VMainWindow = {
    {
QT_MOC_LITERAL(0, 0, 11), // "VMainWindow"
QT_MOC_LITERAL(1, 12, 8), // "xChanged"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 1), // "x"
QT_MOC_LITERAL(4, 24, 8), // "yChanged"
QT_MOC_LITERAL(5, 33, 1), // "y"
QT_MOC_LITERAL(6, 35, 8), // "zChanged"
QT_MOC_LITERAL(7, 44, 1), // "z"
QT_MOC_LITERAL(8, 46, 9), // "fxChanged"
QT_MOC_LITERAL(9, 56, 9), // "fyChanged"
QT_MOC_LITERAL(10, 66, 9), // "fzChanged"
QT_MOC_LITERAL(11, 76, 8), // "tChanged"
QT_MOC_LITERAL(12, 85, 1), // "t"
QT_MOC_LITERAL(13, 87, 8), // "slotExit"
QT_MOC_LITERAL(14, 96, 7), // "changeX"
QT_MOC_LITERAL(15, 104, 7), // "changeY"
QT_MOC_LITERAL(16, 112, 7), // "changeZ"
QT_MOC_LITERAL(17, 120, 7) // "changeT"

    },
    "VMainWindow\0xChanged\0\0x\0yChanged\0y\0"
    "zChanged\0z\0fxChanged\0fyChanged\0fzChanged\0"
    "tChanged\0t\0slotExit\0changeX\0changeY\0"
    "changeZ\0changeT"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VMainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,
       4,    1,   77,    2, 0x06 /* Public */,
       6,    1,   80,    2, 0x06 /* Public */,
       8,    1,   83,    2, 0x06 /* Public */,
       9,    1,   86,    2, 0x06 /* Public */,
      10,    1,   89,    2, 0x06 /* Public */,
      11,    1,   92,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    0,   95,    2, 0x0a /* Public */,
      14,    1,   96,    2, 0x0a /* Public */,
      15,    1,   99,    2, 0x0a /* Public */,
      16,    1,  102,    2, 0x0a /* Public */,
      17,    1,  105,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,   12,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,   12,

       0        // eod
};

void VMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VMainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->xChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->yChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->zChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->fxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->fyChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->fzChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->tChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->slotExit(); break;
        case 8: _t->changeX((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->changeY((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->changeZ((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->changeT((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (VMainWindow::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VMainWindow::xChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (VMainWindow::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VMainWindow::yChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (VMainWindow::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VMainWindow::zChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (VMainWindow::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VMainWindow::fxChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (VMainWindow::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VMainWindow::fyChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (VMainWindow::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VMainWindow::fzChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (VMainWindow::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VMainWindow::tChanged)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject VMainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_VMainWindow.data,
    qt_meta_data_VMainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *VMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VMainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int VMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void VMainWindow::xChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VMainWindow::yChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VMainWindow::zChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void VMainWindow::fxChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void VMainWindow::fyChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void VMainWindow::fzChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void VMainWindow::tChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
