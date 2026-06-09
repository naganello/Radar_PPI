#include "AsterixQt/UdpMulticastReceiver.h"

#include <QDateTime>
#include <QNetworkDatagram>

namespace asterixqt {

UdpMulticastReceiver::UdpMulticastReceiver(QObject* parent)
    : QObject(parent)
{
}

bool UdpMulticastReceiver::start(const QHostAddress& group,
                                 quint16 port,
                                 const QHostAddress& local)
{
    stop();

    group_ = group;
    socket_ = new QUdpSocket(this);

    const bool bound = socket_->bind(local,
                                     port,
                                     QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    if (!bound) {
        emit errorOccurred(socket_->errorString());
        stop();
        return false;
    }

    if (!socket_->joinMulticastGroup(group_)) {
        emit errorOccurred(socket_->errorString());
        stop();
        return false;
    }

    connect(socket_, &QUdpSocket::readyRead,
            this, &UdpMulticastReceiver::onReadyRead);

    return true;
}

void UdpMulticastReceiver::stop()
{
    if (!socket_) {
        return;
    }

    if (!group_.isNull()) {
        socket_->leaveMulticastGroup(group_);
    }

    socket_->close();
    socket_->deleteLater();
    socket_ = nullptr;
}

void UdpMulticastReceiver::onReadyRead()
{
    while (socket_ && socket_->hasPendingDatagrams()) {
        const QNetworkDatagram datagram = socket_->receiveDatagram();

        UdpPacket packet;
        packet.payload = datagram.data();
        packet.sender = datagram.senderAddress();
        packet.senderPort = datagram.senderPort();
        packet.receiveTimeUtc = QDateTime::currentDateTimeUtc();

        emit packetReceived(packet);
    }
}

} // namespace asterixqt
