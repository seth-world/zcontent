/****************************************************************************
** Meta object code from reading C++ file 'zqtreewidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../zqt/zqtwidget/zqtreewidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'zqtreewidget.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSZQTreeWidgetENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSZQTreeWidgetENDCLASS = QtMocHelpers::stringData(
    "ZQTreeWidget",
    "SelectionModeChanged",
    "",
    "QAbstractItemView::SelectionMode",
    "keyPressEvent",
    "QKeyEvent*",
    "pEvent",
    "mouseDoubleClickEvent",
    "QMouseEvent*",
    "event",
    "mousePressEvent",
    "contextMenuEvent",
    "QContextMenuEvent*",
    "dragMoveEvent",
    "QDragMoveEvent*",
    "dragEnterEvent",
    "QDragEnterEvent*",
    "dragLeaveEvent",
    "QDragLeaveEvent*",
    "setSupportedDropActions",
    "Qt::DropActions",
    "pActions",
    "supportedDropActions",
    "dropEvent",
    "QDropEvent*",
    "expandedWidget",
    "QModelIndex",
    "index"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSZQTreeWidgetENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   86,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    1,   89,    2, 0x0a,    3 /* Public */,
       7,    1,   92,    2, 0x0a,    5 /* Public */,
      10,    1,   95,    2, 0x0a,    7 /* Public */,
      11,    1,   98,    2, 0x0a,    9 /* Public */,
      13,    1,  101,    2, 0x0a,   11 /* Public */,
      15,    1,  104,    2, 0x0a,   13 /* Public */,
      17,    1,  107,    2, 0x0a,   15 /* Public */,
      19,    1,  110,    2, 0x0a,   17 /* Public */,
      22,    0,  113,    2, 0x10a,   19 /* Public | MethodIsConst  */,
      23,    1,  114,    2, 0x0a,   20 /* Public */,
      25,    1,  117,    2, 0x0a,   22 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 12,    9,
    QMetaType::Void, 0x80000000 | 14,    6,
    QMetaType::Void, 0x80000000 | 16,    6,
    QMetaType::Void, 0x80000000 | 18,    6,
    QMetaType::Void, 0x80000000 | 20,   21,
    0x80000000 | 20,
    QMetaType::Void, 0x80000000 | 24,    6,
    QMetaType::Void, 0x80000000 | 26,   27,

       0        // eod
};

Q_CONSTINIT const QMetaObject ZQTreeWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QTreeWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSZQTreeWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSZQTreeWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSZQTreeWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ZQTreeWidget, std::true_type>,
        // method 'SelectionModeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractItemView::SelectionMode, std::false_type>,
        // method 'keyPressEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QKeyEvent *, std::false_type>,
        // method 'mouseDoubleClickEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QMouseEvent *, std::false_type>,
        // method 'mousePressEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QMouseEvent *, std::false_type>,
        // method 'contextMenuEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QContextMenuEvent *, std::false_type>,
        // method 'dragMoveEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QDragMoveEvent *, std::false_type>,
        // method 'dragEnterEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QDragEnterEvent *, std::false_type>,
        // method 'dragLeaveEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QDragLeaveEvent *, std::false_type>,
        // method 'setSupportedDropActions'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::DropActions, std::false_type>,
        // method 'supportedDropActions'
        QtPrivate::TypeAndForceComplete<Qt::DropActions, std::false_type>,
        // method 'dropEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QDropEvent *, std::false_type>,
        // method 'expandedWidget'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>
    >,
    nullptr
} };

void ZQTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ZQTreeWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->SelectionModeChanged((*reinterpret_cast< std::add_pointer_t<QAbstractItemView::SelectionMode>>(_a[1]))); break;
        case 1: _t->keyPressEvent((*reinterpret_cast< std::add_pointer_t<QKeyEvent*>>(_a[1]))); break;
        case 2: _t->mouseDoubleClickEvent((*reinterpret_cast< std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 3: _t->mousePressEvent((*reinterpret_cast< std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 4: _t->contextMenuEvent((*reinterpret_cast< std::add_pointer_t<QContextMenuEvent*>>(_a[1]))); break;
        case 5: _t->dragMoveEvent((*reinterpret_cast< std::add_pointer_t<QDragMoveEvent*>>(_a[1]))); break;
        case 6: _t->dragEnterEvent((*reinterpret_cast< std::add_pointer_t<QDragEnterEvent*>>(_a[1]))); break;
        case 7: _t->dragLeaveEvent((*reinterpret_cast< std::add_pointer_t<QDragLeaveEvent*>>(_a[1]))); break;
        case 8: _t->setSupportedDropActions((*reinterpret_cast< std::add_pointer_t<Qt::DropActions>>(_a[1]))); break;
        case 9: { Qt::DropActions _r = _t->supportedDropActions();
            if (_a[0]) *reinterpret_cast< Qt::DropActions*>(_a[0]) = std::move(_r); }  break;
        case 10: _t->dropEvent((*reinterpret_cast< std::add_pointer_t<QDropEvent*>>(_a[1]))); break;
        case 11: _t->expandedWidget((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ZQTreeWidget::*)(QAbstractItemView::SelectionMode );
            if (_t _q_method = &ZQTreeWidget::SelectionModeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *ZQTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ZQTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSZQTreeWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int ZQTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void ZQTreeWidget::SelectionModeChanged(QAbstractItemView::SelectionMode _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
