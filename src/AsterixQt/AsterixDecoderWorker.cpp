#include "AsterixQt/AsterixDecoderWorker.h"

namespace asterixqt {

AsterixDecoderWorker::AsterixDecoderWorker(ThreadSafeQueue<UdpPacket>* queue,
                                           QObject* parent)
    : QObject(parent)
    , queue_(queue)
{
}

void AsterixDecoderWorker::start()
{
    running_ = true;

    timer_ = new QTimer(this);
    timer_->setInterval(1);

    connect(timer_, &QTimer::timeout,
            this, &AsterixDecoderWorker::processLoop);

    timer_->start();
}

void AsterixDecoderWorker::stop()
{
    running_ = false;

    if (timer_) {
        timer_->stop();
        timer_->deleteLater();
        timer_ = nullptr;
    }
}

void AsterixDecoderWorker::processLoop()
{
    if (!running_ || !queue_) {
        return;
    }

    constexpr int maxPacketsPerTick = 512;
    for (int i = 0; i < maxPacketsPerTick; ++i) {
        UdpPacket packet;
        if (!queue_->tryPop(packet)) {
            break;
        }
        decodePacket(packet);
    }

    static int counter = 0;
    if (++counter >= 25) {
        counter = 0;
        emit statsUpdated(stats_);
    }
}

void AsterixDecoderWorker::decodePacket(const UdpPacket& packet)
{
    if (packet.payload.size() < 3) {
        return;
    }

    stats_.udpPacketsReceived++;
    const quint8 category = packet.payload[0];

    try {
        if (category == 48) {
            auto targets = cat048Decoder_.decodeDataBlock(packet.payload,
                                                          packet.receiveTimeUtc);
            if (!targets.isEmpty()) {
                stats_.astxBlocksDecoded++;
                stats_.cat048TargetsDecoded += targets.size();
                emit cat048Decoded(targets);
            }
        } else if (category == 240) {
            auto radials = cat240Decoder_.decodeDataBlock(packet.payload,
                                                          packet.receiveTimeUtc);
            if (!radials.isEmpty()) {
                stats_.astxBlocksDecoded++;
                stats_.cat240RadialsDecoded += radials.size();
                emit cat240Decoded(radials);
            }
        }
    } catch (...) {
        stats_.decodeErrors++;
        emit decodeError(QStringLiteral("ASTERIX decode error"));
    }
}

} // namespace asterixqt
