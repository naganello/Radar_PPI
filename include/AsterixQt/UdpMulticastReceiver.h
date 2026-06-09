#pragma once

#include "AsterixTypes.h"

#include <QObject>
#include <QUdpSocket>

namespace asterixqt {

class ASTERIXQT_EXPORT UdpMulticastReceiver : public QObject
{
    Q_OBJECT

public:
    explicit UdpMulticastReceiver(QObject* parent = nullptr);

public slots:
    bool start(const QHostAddress& group,
               quint16 port,
               const QHostAddress& local = QHostAddress::AnyIPv4);
    void stop();

signals:
    void packetReceived(const asterixqt::UdpPacket& packet);
    void errorOccurred(const QString& errorText);

private slots:
    void onReadyRead();

private:
    QUdpSocket* socket_ = nullptr;
    QHostAddress group_;
};

} // namespace asterixqt
