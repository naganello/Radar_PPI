#pragma once

#include "AsterixTypes.h"
#include "Cat048Decoder.h"
#include "Cat240Decoder.h"
#include "ThreadSafeQueue.h"

#include <QObject>
#include <QTimer>

namespace asterixqt {

class ASTERIXQT_EXPORT AsterixDecoderWorker : public QObject
{
    Q_OBJECT

public:
    explicit AsterixDecoderWorker(ThreadSafeQueue<UdpPacket>* queue,
                                  QObject* parent = nullptr);

public slots:
    void start();
    void stop();

signals:
    void cat048Decoded(const QVector<asterixqt::Cat048Target>& targets);
    void cat240Decoded(const QVector<asterixqt::Cat240VideoRadial>& radials);
    void statsUpdated(const asterixqt::DecoderStats& stats);
    void decodeError(const QString& errorText);

private slots:
    void processLoop();

private:
    void decodePacket(const UdpPacket& packet);

    ThreadSafeQueue<UdpPacket>* queue_ = nullptr;
    QTimer* timer_ = nullptr;
    bool running_ = false;

    Cat048Decoder cat048Decoder_;
    Cat240Decoder cat240Decoder_;
    DecoderStats stats_;
};

} // namespace asterixqt
