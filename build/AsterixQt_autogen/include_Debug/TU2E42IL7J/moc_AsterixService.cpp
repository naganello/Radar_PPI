/****************************************************************************
** Meta object code from reading C++ file 'AsterixService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../include/AsterixQt/AsterixService.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AsterixService.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.12.0. It"
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
struct qt_meta_tag_ZN9asterixqt14AsterixServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto asterixqt::AsterixService::qt_create_metaobjectdata<qt_meta_tag_ZN9asterixqt14AsterixServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "asterixqt::AsterixService",
        "packetReceived",
        "",
        "cat048Decoded",
        "QList<asterixqt::Cat048Target>",
        "targets",
        "cat240Decoded",
        "QList<asterixqt::Cat240VideoRadial>",
        "radials",
        "statsUpdated",
        "asterixqt::DecoderStats",
        "stats",
        "errorOccurred",
        "errorText"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'packetReceived'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'cat048Decoded'
        QtMocHelpers::SignalData<void(const QVector<asterixqt::Cat048Target> &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Signal 'cat240Decoded'
        QtMocHelpers::SignalData<void(const QVector<asterixqt::Cat240VideoRadial> &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Signal 'statsUpdated'
        QtMocHelpers::SignalData<void(const asterixqt::DecoderStats &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AsterixService, qt_meta_tag_ZN9asterixqt14AsterixServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject asterixqt::AsterixService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9asterixqt14AsterixServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9asterixqt14AsterixServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9asterixqt14AsterixServiceE_t>.metaTypes,
    nullptr
} };

void asterixqt::AsterixService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AsterixService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->packetReceived(); break;
        case 1: _t->cat048Decoded((*reinterpret_cast<std::add_pointer_t<QList<asterixqt::Cat048Target>>>(_a[1]))); break;
        case 2: _t->cat240Decoded((*reinterpret_cast<std::add_pointer_t<QList<asterixqt::Cat240VideoRadial>>>(_a[1]))); break;
        case 3: _t->statsUpdated((*reinterpret_cast<std::add_pointer_t<asterixqt::DecoderStats>>(_a[1]))); break;
        case 4: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<asterixqt::Cat048Target> >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<asterixqt::Cat240VideoRadial> >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< asterixqt::DecoderStats >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AsterixService::*)()>(_a, &AsterixService::packetReceived, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AsterixService::*)(const QVector<asterixqt::Cat048Target> & )>(_a, &AsterixService::cat048Decoded, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AsterixService::*)(const QVector<asterixqt::Cat240VideoRadial> & )>(_a, &AsterixService::cat240Decoded, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AsterixService::*)(const asterixqt::DecoderStats & )>(_a, &AsterixService::statsUpdated, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AsterixService::*)(const QString & )>(_a, &AsterixService::errorOccurred, 4))
            return;
    }
}

const QMetaObject *asterixqt::AsterixService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *asterixqt::AsterixService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9asterixqt14AsterixServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int asterixqt::AsterixService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void asterixqt::AsterixService::packetReceived()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void asterixqt::AsterixService::cat048Decoded(const QVector<asterixqt::Cat048Target> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void asterixqt::AsterixService::cat240Decoded(const QVector<asterixqt::Cat240VideoRadial> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void asterixqt::AsterixService::statsUpdated(const asterixqt::DecoderStats & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void asterixqt::AsterixService::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
