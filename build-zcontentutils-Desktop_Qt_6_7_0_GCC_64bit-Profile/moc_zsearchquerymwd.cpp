/****************************************************************************
** Meta object code from reading C++ file 'zsearchquerymwd.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../zcontentutils/zsearchquerymwd.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'zsearchquerymwd.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSZSearchQueryMWdENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSZSearchQueryMWdENDCLASS = QtMocHelpers::stringData(
    "ZSearchQueryMWd",
    "ParseAndStoreQuery",
    "",
    "MenuTriggered",
    "QAction*",
    "pAction",
    "TableHeaderClicked",
    "pLogicalIndex",
    "TableHeaderCornerClicked",
    "BackwardClicked",
    "ForwardClicked",
    "ExecuteClicked",
    "QuitLBlClicked",
    "TextClearLBlClicked",
    "EntityListLBlClicked"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSZSearchQueryMWdENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x08,    1 /* Private */,
       3,    1,   75,    2, 0x08,    2 /* Private */,
       6,    1,   78,    2, 0x08,    4 /* Private */,
       8,    0,   81,    2, 0x08,    6 /* Private */,
       9,    0,   82,    2, 0x08,    7 /* Private */,
      10,    0,   83,    2, 0x08,    8 /* Private */,
      11,    0,   84,    2, 0x08,    9 /* Private */,
      12,    0,   85,    2, 0x08,   10 /* Private */,
      13,    0,   86,    2, 0x08,   11 /* Private */,
      14,    0,   87,    2, 0x08,   12 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ZSearchQueryMWd::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSZSearchQueryMWdENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSZSearchQueryMWdENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSZSearchQueryMWdENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ZSearchQueryMWd, std::true_type>,
        // method 'ParseAndStoreQuery'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'MenuTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'TableHeaderClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'TableHeaderCornerClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'BackwardClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ForwardClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ExecuteClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'QuitLBlClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'TextClearLBlClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'EntityListLBlClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ZSearchQueryMWd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ZSearchQueryMWd *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->ParseAndStoreQuery(); break;
        case 1: _t->MenuTriggered((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 2: _t->TableHeaderClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->TableHeaderCornerClicked(); break;
        case 4: _t->BackwardClicked(); break;
        case 5: _t->ForwardClicked(); break;
        case 6: _t->ExecuteClicked(); break;
        case 7: _t->QuitLBlClicked(); break;
        case 8: _t->TextClearLBlClicked(); break;
        case 9: _t->EntityListLBlClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *ZSearchQueryMWd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ZSearchQueryMWd::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSZSearchQueryMWdENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int ZSearchQueryMWd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
