/****************************************************************************
** Meta object code from reading C++ file 'texteditmwn.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../zcontentutils/texteditmwn.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'texteditmwn.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStextEditMWnENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASStextEditMWnENDCLASS = QtMocHelpers::stringData(
    "textEditMWn",
    "morePressed",
    "",
    "closePressed",
    "wrapPressed",
    "filterPressed",
    "lineNumbersBTnClicked",
    "searchMainPressed",
    "searchPressed",
    "searchReturnPressed",
    "MenuAction",
    "QAction*",
    "pAction",
    "hasErrorLog"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStextEditMWnENDCLASS[] = {

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
       3,    0,   75,    2, 0x08,    2 /* Private */,
       4,    0,   76,    2, 0x08,    3 /* Private */,
       5,    0,   77,    2, 0x08,    4 /* Private */,
       6,    0,   78,    2, 0x08,    5 /* Private */,
       7,    0,   79,    2, 0x08,    6 /* Private */,
       8,    0,   80,    2, 0x08,    7 /* Private */,
       9,    0,   81,    2, 0x08,    8 /* Private */,
      10,    1,   82,    2, 0x08,    9 /* Private */,
      13,    0,   85,    2, 0x08,   11 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Bool,

       0        // eod
};

Q_CONSTINIT const QMetaObject textEditMWn::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASStextEditMWnENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStextEditMWnENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStextEditMWnENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<textEditMWn, std::true_type>,
        // method 'morePressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'closePressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'wrapPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'filterPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'lineNumbersBTnClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'searchMainPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'searchPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'searchReturnPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'MenuAction'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'hasErrorLog'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void textEditMWn::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<textEditMWn *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->morePressed(); break;
        case 1: _t->closePressed(); break;
        case 2: _t->wrapPressed(); break;
        case 3: _t->filterPressed(); break;
        case 4: _t->lineNumbersBTnClicked(); break;
        case 5: _t->searchMainPressed(); break;
        case 6: _t->searchPressed(); break;
        case 7: _t->searchReturnPressed(); break;
        case 8: _t->MenuAction((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 9: { bool _r = _t->hasErrorLog();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *textEditMWn::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *textEditMWn::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStextEditMWnENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int textEditMWn::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
