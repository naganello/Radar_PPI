#include "AsterixQt/AsterixService.h"

#include <QMetaObject>

namespace asterixqt {

AsterixService::AsterixService(QObject* parent)
    : QObject(parent)
{
    qRegisterMetaType<asterixqt::UdpPacket>();
    qRegisterMetaType<QVector<asterixqt::Cat048Target>>();
    qRegisterMetaType<QVector<asterixqt::Cat240VideoRadial>>();
    qRegisterMetaType<asterixqt::DecoderStats>();
}

AsterixService::~AsterixService()
{
    stop();
}

bool AsterixService::start(const QHostAddress& group,
                           quint16 port,
                           const QHostAddress& local)
{
    stop();

    receiver_ = new UdpMulticastReceiver;
    decoder_ = new AsterixDecoderWorker(&queue_);

    receiver_->moveToThread(&receiverThread_);
    decoder_->moveToThread(&decoderThread_);

    connect(&receiverThread_, &QThread::finished,
            receiver_, &QObject::deleteLater);
    connect(&decoderThread_, &QThread::finished,
            decoder_, &QObject::deleteLater);

    connect(receiver_, &UdpMulticastReceiver::packetReceived,
            this,
            [this](const UdpPacket& packet) {
                queue_.push(packet);
                emit packetReceived();
            },
            Qt::QueuedConnection);

    connect(receiver_, &UdpMulticastReceiver::errorOccurred,
            this, &AsterixService::errorOccurred,
            Qt::QueuedConnection);
    connect(decoder_, &AsterixDecoderWorker::cat048Decoded,
            this, &AsterixService::cat048Decoded,
            Qt::QueuedConnection);
    connect(decoder_, &AsterixDecoderWorker::cat240Decoded,
            this, &AsterixService::cat240Decoded,
            Qt::QueuedConnection);
    connect(decoder_, &AsterixDecoderWorker::statsUpdated,
            this, &AsterixService::statsUpdated,
            Qt::QueuedConnection);
    connect(decoder_, &AsterixDecoderWorker::decodeError,
            this, &AsterixService::errorOccurred,
            Qt::QueuedConnection);

    receiverThread_.start();
    decoderThread_.start();

    QMetaObject::invokeMethod(decoder_, "start", Qt::QueuedConnection);

    bool ok = false;
    QMetaObject::invokeMethod(receiver_,
                              [&]() {
                                  ok = receiver_->start(group, port, local);
                              },
                              Qt::BlockingQueuedConnection);

    return ok;
}

void AsterixService::stop()
{
    if (receiver_) {
        QMetaObject::invokeMethod(receiver_, "stop", Qt::BlockingQueuedConnection);
    }

    if (decoder_) {
        QMetaObject::invokeMethod(decoder_, "stop", Qt::BlockingQueuedConnection);
    }

    receiverThread_.quit();
    decoderThread_.quit();
    receiverThread_.wait();
    decoderThread_.wait();

    receiver_ = nullptr;
    decoder_ = nullptr;
    queue_.clear();
}

} // namespace asterixqt
