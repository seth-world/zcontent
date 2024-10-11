/****************************************************************************
** Meta object code from reading C++ file 'zexceptiondlg.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../zcontentutils/zexceptiondlg.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'zexceptiondlg.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSZExceptionDLgENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSZExceptionDLgENDCLASS = QtMocHelpers::stringData(
    "ZExceptionDLg",
    "buttonPressed",
    "",
    "pValue",
    "DontShowClicked",
    "MoreClicked",
    "ErrlogClicked",
    "ZExceptionClicked",
    "ZExceptionPurgeClicked",
    "layoutSetup",
    "utf8VaryingString",
    "pTitle",
    "pDontShow",
    "resizeEvent",
    "QResizeEvent*",
    "pEvent"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSZExceptionDLgENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   62,    2, 0x0a,    1 /* Public */,
       4,    0,   65,    2, 0x0a,    3 /* Public */,
       5,    0,   66,    2, 0x0a,    4 /* Public */,
       6,    0,   67,    2, 0x0a,    5 /* Public */,
       7,    0,   68,    2, 0x0a,    6 /* Public */,
       8,    0,   69,    2, 0x0a,    7 /* Public */,
       9,    2,   70,    2, 0x0a,    8 /* Public */,
      13,    1,   75,    2, 0x08,   11 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Bool,   11,   12,
    QMetaType::Void, 0x80000000 | 14,   15,

       0        // eod
};

Q_CONSTINIT const QMetaObject ZExceptionDLg::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSZExceptionDLgENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSZExceptionDLgENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSZExceptionDLgENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ZExceptionDLg, std::true_type>,
        // method 'buttonPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'DontShowClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'MoreClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ErrlogClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ZExceptionClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ZExceptionPurgeClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'layoutSetup'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const utf8VaryingString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'resizeEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QResizeEvent *, std::false_type>
    >,
    nullptr
} };

void ZExceptionDLg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ZExceptionDLg *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->buttonPressed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->DontShowClicked(); break;
        case 2: _t->MoreClicked(); break;
        case 3: _t->ErrlogClicked(); break;
        case 4: _t->ZExceptionClicked(); break;
        case 5: _t->ZExceptionPurgeClicked(); break;
        case 6: _t->layoutSetup((*reinterpret_cast< std::add_pointer_t<utf8VaryingString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 7: _t->resizeEvent((*reinterpret_cast< std::add_pointer_t<QResizeEvent*>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *ZExceptionDLg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ZExceptionDLg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSZExceptionDLgENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ZExceptionDLg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
