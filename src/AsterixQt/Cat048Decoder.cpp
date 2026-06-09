#include "AsterixQt/Cat048Decoder.h"

#include <cmath>

namespace asterixqt {

quint16 Cat048Decoder::readU16(const quint8* p)
{
    return (quint16(p[0]) << 8) | quint16(p[1]);
}

qint16 Cat048Decoder::readS16(const quint8* p)
{
    return qint16(readU16(p));
}

quint32 Cat048Decoder::readU24(const quint8* p)
{
    return (quint32(p[0]) << 16) | (quint32(p[1]) << 8) | quint32(p[2]);
}

QString Cat048Decoder::decodeAircraftId(const quint8* p)
{
    static const char table[64] = {
        ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
        'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
        'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
        'X', 'Y', 'Z', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', ' ', ' ', ' ', ' ', ' ', ' '
    };

    quint64 value = 0;
    for (int i = 0; i < 6; ++i) {
        value = (value << 8) | p[i];
    }

    QString text;
    for (int i = 7; i >= 0; --i) {
        const int index = int((value >> (i * 6)) & 0x3F);
        text.append(QLatin1Char(table[index]));
    }

    return text.trimmed();
}

QVector<Cat048Target> Cat048Decoder::decodeDataBlock(const QByteArray& dataBlock,
                                                     const QDateTime& receiveTimeUtc)
{
    QVector<Cat048Target> targets;

    if (dataBlock.size() < 3) {
        return targets;
    }

    const auto* bytes = reinterpret_cast<const quint8*>(dataBlock.constData());
    if (bytes[0] != 48) {
        return targets;
    }

    const int blockLength = readU16(bytes + 1);
    if (blockLength > dataBlock.size()) {
        return targets;
    }

    int offset = 3;

    while (offset < blockLength) {
        const int recordStart = offset;
        QVector<bool> frn(40, false);
        int frnIndex = 1;
        bool hasExtension = true;

        while (hasExtension && offset < blockLength) {
            const quint8 fspec = bytes[offset++];

            for (int bit = 7; bit >= 1; --bit) {
                if (frnIndex < frn.size()) {
                    frn[frnIndex] = (fspec & (1u << bit)) != 0;
                }
                ++frnIndex;
            }

            hasExtension = (fspec & 0x01) != 0;
        }

        Cat048Target target;
        target.receiveTimeUtc = receiveTimeUtc;

        if (frn[1]) {
            if (offset + 2 > blockLength) {
                break;
            }
            target.sac = bytes[offset];
            target.sic = bytes[offset + 1];
            offset += 2;
        }

        if (frn[2]) {
            if (offset + 3 > blockLength) {
                break;
            }
            target.hasTimeOfDay = true;
            target.timeOfDaySeconds = readU24(bytes + offset) / 128.0;
            offset += 3;
        }

        if (frn[3]) {
            if (offset + 1 > blockLength) {
                break;
            }

            quint8 descriptor = bytes[offset++];
            target.kind = (((descriptor >> 5) & 0x07) == 0)
                ? Cat048ReportKind::NoDetection
                : Cat048ReportKind::Plot;

            while ((descriptor & 0x01) && offset < blockLength) {
                descriptor = bytes[offset++];
            }
        }

        if (frn[4]) {
            if (offset + 4 > blockLength) {
                break;
            }
            target.hasPolar = true;
            target.rangeNm = readU16(bytes + offset) / 256.0;
            target.azimuthDeg = readU16(bytes + offset + 2) * 360.0 / 65536.0;
            offset += 4;
        }

        if (frn[5]) {
            offset += 2;
        }

        if (frn[6]) {
            offset += 2;
        }

        if (frn[7]) {
            if (offset + 1 > blockLength) {
                break;
            }
            quint8 primary = bytes[offset++];
            while ((primary & 0x01) && offset < blockLength) {
                primary = bytes[offset++];
            }
        }

        if (frn[8]) {
            offset += 3;
        }

        if (frn[9]) {
            offset += 6;
        }

        if (frn[10]) {
            if (offset + 1 > blockLength) {
                break;
            }
            const int rep = bytes[offset++];
            offset += rep * 8;
        }

        if (frn[11]) {
            if (offset + 2 > blockLength) {
                break;
            }
            target.hasTrackNumber = true;
            target.trackNumber = readU16(bytes + offset) & 0x0FFF;
            target.kind = Cat048ReportKind::Track;
            offset += 2;
        }

        if (frn[12]) {
            if (offset + 4 > blockLength) {
                break;
            }
            target.hasCartesian = true;
            target.xNm = readS16(bytes + offset) / 128.0;
            target.yNm = readS16(bytes + offset + 2) / 128.0;
            offset += 4;
        }

        if (frn[13]) {
            if (offset + 4 > blockLength) {
                break;
            }
            target.hasVelocity = true;
            target.groundSpeedKt = readU16(bytes + offset) * std::pow(2.0, -14.0) * 3600.0;
            target.headingDeg = readU16(bytes + offset + 2) * 360.0 / 65536.0;
            offset += 4;
        }

        if (offset > recordStart) {
            target.rawRecord = dataBlock.mid(recordStart, offset - recordStart);
        }

        if (target.hasPolar || target.hasTrackNumber) {
            targets.push_back(target);
        } else {
            break;
        }
    }

    return targets;
}

} // namespace asterixqt
