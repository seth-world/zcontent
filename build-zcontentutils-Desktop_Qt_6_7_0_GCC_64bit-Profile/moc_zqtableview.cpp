/****************************************************************************
** Meta object code from reading C++ file 'zqtableview.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../zqt/zqtwidget/zqtableview.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'zqtableview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSZQTableViewENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSZQTableViewENDCLASS = QtMocHelpers::stringData(
    "ZQTableView",
    "selectAll",
    "",
    "supportedDropActions",
    "Qt::DropActions",
    "setSupportedDropActions",
    "pActions",
    "dropEvent",
    "QDropEvent*",
    "pEvent",
    "dataChanged",
    "QModelIndex",
    "topLeft",
    "bottomRight",
    "QList<int>",
    "roles"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSZQTableViewENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x0a,    1 /* Public */,
       3,    0,   51,    2, 0x10a,    2 /* Public | MethodIsConst  */,
       5,    1,   52,    2, 0x0a,    3 /* Public */,
       7,    1,   55,    2, 0x0a,    5 /* Public */,
      10,    3,   58,    2, 0x0a,    7 /* Public */,
      10,    2,   65,    2, 0x2a,   11 /* Public | MethodCloned */,

 // slots: parameters
    QMetaType::Void,
    0x80000000 | 4,
    QMetaType::Void, 0x80000000 | 4,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 11, 0x80000000 | 11, 0x80000000 | 14,   12,   13,   15,
    QMetaType::Void, 0x80000000 | 11, 0x80000000 | 11,   12,   13,

       0        // eod
};

Q_CONSTINIT const QMetaObject ZQTableView::staticMetaObject = { {
    QMetaObject::SuperData::link<QTableView::staticMetaObject>(),
    qt_meta_stringdata_CLASSZQTableViewENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSZQTableViewENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSZQTableViewENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ZQTableView, std::true_type>,
        // method 'selectAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'supportedDropActions'
        QtPrivate::TypeAndForceComplete<Qt::DropActions, std::false_type>,
        // method 'setSupportedDropActions'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::DropActions, std::false_type>,
        // method 'dropEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QDropEvent *, std::false_type>,
        // method 'dataChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<int> &, std::false_type>,
        // method 'dataChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>
    >,
    nullptr
} };

void ZQTableView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ZQTableView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->selectAll(); break;
        case 1: { Qt::DropActions _r = _t->supportedDropActions();
            if (_a[0]) *reinterpret_cast< Qt::DropActions*>(_a[0]) = std::move(_r); }  break;
        case 2: _t->setSupportedDropActions((*reinterpret_cast< std::add_pointer_t<Qt::DropActions>>(_a[1]))); break;
        case 3: _t->dropEvent((*reinterpret_cast< std::add_pointer_t<QDropEvent*>>(_a[1]))); break;
        case 4: _t->dataChanged((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QList<int>>>(_a[3]))); break;
        case 5: _t->dataChanged((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 2:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        }
    }
}

const QMetaObject *ZQTableView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ZQTableView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSZQTableViewENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QTableView::qt_metacast(_clname);
}

int ZQTableView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
