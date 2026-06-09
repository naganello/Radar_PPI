#pragma once

#include "AsterixQtGlobal.h"

#include <QByteArray>
#include <QDateTime>
#include <QHostAddress>
#include <QMetaType>
#include <QString>
#include <QVector>

namespace asterixqt {

enum class Cat048ReportKind
{
    Unknown,
    Plot,
    Track,
    TrackEnd,
    NoDetection
};

enum class Cat240MessageType
{
    Unknown = 0,
    VideoSummary = 1,
    Video = 2
};

struct UdpPacket
{
    QByteArray payload;
    QHostAddress sender;
    quint16 senderPort = 0;
    QDateTime receiveTimeUtc;
};

struct Cat048Target
{
    quint8 sac = 0;
    quint8 sic = 0;
    Cat048ReportKind kind = Cat048ReportKind::Unknown;

    bool hasPolar = false;
    double rangeNm = 0.0;
    double azimuthDeg = 0.0;

    bool hasCartesian = false;
    double xNm = 0.0;
    double yNm = 0.0;

    bool hasFlightLevel = false;
    double flightLevel = 0.0;

    bool hasTrackNumber = false;
    quint16 trackNumber = 0;

    bool hasVelocity = false;
    double groundSpeedKt = 0.0;
    double headingDeg = 0.0;

    bool hasMode3A = false;
    QString mode3A;

    bool hasAircraftAddress = false;
    quint32 aircraftAddress = 0;

    bool hasAircraftId = false;
    QString aircraftId;

    bool hasTimeOfDay = false;
    double timeOfDaySeconds = 0.0;

    QDateTime receiveTimeUtc;
    QByteArray rawRecord;
};

struct Cat240VideoRadial
{
    quint8 sac = 0;
    quint8 sic = 0;
    Cat240MessageType messageType = Cat240MessageType::Unknown;

    bool hasMessageIndex = false;
    quint32 messageIndex = 0;

    bool hasTimeOfDay = false;
    double timeOfDaySeconds = 0.0;

    double startAzimuthDeg = 0.0;
    double endAzimuthDeg = 0.0;
    quint32 startRangeCell = 0;
    double cellDurationSeconds = 0.0;

    quint8 resolutionCode = 0;
    quint8 bitsPerCell = 0;
    bool compressed = false;

    quint16 validVideoOctets = 0;
    quint32 validCells = 0;
    QVector<quint32> amplitudes;

    QDateTime receiveTimeUtc;
    QByteArray rawRecord;
};

struct DecoderStats
{
    quint64 udpPacketsReceived = 0;
    quint64 astxBlocksDecoded = 0;
    quint64 cat048TargetsDecoded = 0;
    quint64 cat240RadialsDecoded = 0;
    quint64 decodeErrors = 0;
};

} // namespace asterixqt

Q_DECLARE_METATYPE(asterixqt::UdpPacket)
Q_DECLARE_METATYPE(asterixqt::Cat048Target)
Q_DECLARE_METATYPE(QVector<asterixqt::Cat048Target>)
Q_DECLARE_METATYPE(asterixqt::Cat240VideoRadial)
Q_DECLARE_METATYPE(QVector<asterixqt::Cat240VideoRadial>)
Q_DECLARE_METATYPE(asterixqt::DecoderStats)
