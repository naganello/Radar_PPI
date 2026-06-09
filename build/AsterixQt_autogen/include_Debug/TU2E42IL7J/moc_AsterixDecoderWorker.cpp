/****************************************************************************
** Meta object code from reading C++ file 'AsterixDecoderWorker.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../include/AsterixQt/AsterixDecoderWorker.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AsterixDecoderWorker.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9asterixqt20AsterixDecoderWorkerE_t {};
} // unnamed namespace

template <> constexpr inline auto asterixqt::AsterixDecoderWorker::qt_create_metaobjectdata<qt_meta_tag_ZN9asterixqt20AsterixDecoderWorkerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "asterixqt::AsterixDecoderWorker",
        "cat048Decoded",
        "",
        "QList<asterixqt::Cat048Target>",
        "targets",
        "cat240Decoded",
        "QList<asterixqt::Cat240VideoRadial>",
        "radials",
        "statsUpdated",
        "asterixqt::DecoderStats",
        "stats",
        "decodeError",
        "errorText",
        "start",
        "stop",
        "processLoop"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'cat048Decoded'
        QtMocHelpers::SignalData<void(const QVector<asterixqt::Cat048Target> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'cat240Decoded'
        QtMocHelpers::SignalData<void(const QVector<asterixqt::Cat240VideoRadial> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'statsUpdated'
        QtMocHelpers::SignalData<void(const asterixqt::DecoderStats &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'decodeError'
        QtMocHelpers::SignalData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
        // Slot 'start'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'stop'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'processLoop'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AsterixDecoderWorker, qt_meta_tag_ZN9asterixqt20AsterixDecoderWorkerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject asterixqt::AsterixDecoderWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9asterixqt20AsterixDecoderWorkerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9asterixqt20AsterixDecoderWorkerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9asterixqt20AsterixDecoderWorkerE_t>.metaTypes,
    nullptr
} };

void asterixqt::AsterixDecoderWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AsterixDecoderWorker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->cat048Decoded((*reinterpret_cast<std::add_pointer_t<QList<asterixqt::Cat048Target>>>(_a[1]))); break;
        case 1: _t->cat240Decoded((*reinterpret_cast<std::add_pointer_t<QList<asterixqt::Cat240VideoRadial>>>(_a[1]))); break;
        case 2: _t->statsUpdated((*reinterpret_cast<std::add_pointer_t<asterixqt::DecoderStats>>(_a[1]))); break;
        case 3: _t->decodeError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->start(); break;
        case 5: _t->stop(); break;
        case 6: _t->processLoop(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<asterixqt::Cat048Target> >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<asterixqt::Cat240VideoRadial> >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< asterixqt::DecoderStats >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AsterixDecoderWorker::*)(const QVector<asterixqt::Cat048Target> & )>(_a, &AsterixDecoderWorker::cat048Decoded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AsterixDecoderWorker::*)(const QVector<asterixqt::Cat240VideoRadial> & )>(_a, &AsterixDecoderWorker::cat240Decoded, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AsterixDecoderWorker::*)(const asterixqt::DecoderStats & )>(_a, &AsterixDecoderWorker::statsUpdated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AsterixDecoderWorker::*)(const QString & )>(_a, &AsterixDecoderWorker::decodeError, 3))
            return;
    }
}

const QMetaObject *asterixqt::AsterixDecoderWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *asterixqt::AsterixDecoderWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9asterixqt20AsterixDecoderWorkerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int asterixqt::AsterixDecoderWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void asterixqt::AsterixDecoderWorker::cat048Decoded(const QVector<asterixqt::Cat048Target> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void asterixqt::AsterixDecoderWorker::cat240Decoded(const QVector<asterixqt::Cat240VideoRadial> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void asterixqt::AsterixDecoderWorker::statsUpdated(const asterixqt::DecoderStats & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void asterixqt::AsterixDecoderWorker::decodeError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
