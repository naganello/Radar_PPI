#pragma once

#include "AsterixDecoderWorker.h"
#include "ThreadSafeQueue.h"
#include "UdpMulticastReceiver.h"

#include <QObject>
#include <QThread>

namespace asterixqt {

class ASTERIXQT_EXPORT AsterixService : public QObject
{
    Q_OBJECT

public:
    explicit AsterixService(QObject* parent = nullptr);
    ~AsterixService() override;

    bool start(const QHostAddress& group,
               quint16 port,
               const QHostAddress& local = QHostAddress::AnyIPv4);
    void stop();

signals:
    void packetReceived();
    void cat048Decoded(const QVector<asterixqt::Cat048Target>& targets);
    void cat240Decoded(const QVector<asterixqt::Cat240VideoRadial>& radials);
    void statsUpdated(const asterixqt::DecoderStats& stats);
    void errorOccurred(const QString& errorText);

private:
    ThreadSafeQueue<UdpPacket> queue_;
    QThread receiverThread_;
    QThread decoderThread_;

    UdpMulticastReceiver* receiver_ = nullptr;
    AsterixDecoderWorker* decoder_ = nullptr;
};

} // namespace asterixqt
