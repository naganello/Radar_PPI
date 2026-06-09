#include "AsterixQt/Cat240Decoder.h"

namespace asterixqt {

quint16 Cat240Decoder::readU16(const quint8* p)
{
    return (quint16(p[0]) << 8) | quint16(p[1]);
}

quint32 Cat240Decoder::readU24(const quint8* p)
{
    return (quint32(p[0]) << 16) | (quint32(p[1]) << 8) | quint32(p[2]);
}

quint32 Cat240Decoder::readU32(const quint8* p)
{
    return (quint32(p[0]) << 24)
        | (quint32(p[1]) << 16)
        | (quint32(p[2]) << 8)
        | quint32(p[3]);
}

quint8 Cat240Decoder::bitsPerCellFromResolution(quint8 resolutionCode)
{
    switch (resolutionCode) {
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 4;
    case 4:
        return 8;
    case 5:
        return 16;
    case 6:
        return 32;
    default:
        return 0;
    }
}

QVector<quint32> Cat240Decoder::unpackCells(const QByteArray& bytes,
                                            quint8 bitsPerCell,
                                            quint32 validCells)
{
    QVector<quint32> cells;

    if (bitsPerCell == 0) {
        return cells;
    }

    const auto* data = reinterpret_cast<const quint8*>(bytes.constData());
    const int totalBits = bytes.size() * 8;
    int bitPosition = 0;

    while (bitPosition + bitsPerCell <= totalBits && quint32(cells.size()) < validCells) {
        quint32 value = 0;

        for (int i = 0; i < bitsPerCell; ++i) {
            const int absoluteBit = bitPosition + i;
            value = (value << 1) | ((data[absoluteBit / 8] >> (7 - (absoluteBit % 8))) & 0x01);
        }

        cells.push_back(value);
        bitPosition += bitsPerCell;
    }

    return cells;
}

QVector<Cat240VideoRadial> Cat240Decoder::decodeDataBlock(const QByteArray& dataBlock,
                                                          const QDateTime& receiveTimeUtc)
{
    QVector<Cat240VideoRadial> radials;

    if (dataBlock.size() < 3) {
        return radials;
    }

    const auto* bytes = reinterpret_cast<const quint8*>(dataBlock.constData());
    if (bytes[0] != 240) {
        return radials;
    }

    const int blockLength = readU16(bytes + 1);
    if (blockLength > dataBlock.size()) {
        return radials;
    }

    int offset = 3;
    while (offset < blockLength) {
        const int recordStart = offset;
        QVector<bool> frn(16, false);
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

        Cat240VideoRadial radial;
        radial.receiveTimeUtc = receiveTimeUtc;

        if (frn[1]) {
            if (offset + 2 > blockLength) {
                break;
            }
            radial.sac = bytes[offset];
            radial.sic = bytes[offset + 1];
            offset += 2;
        }

        if (frn[2]) {
            if (offset + 1 > blockLength) {
                break;
            }
            const quint8 messageType = bytes[offset++];
            radial.messageType = messageType == 2
                ? Cat240MessageType::Video
                : (messageType == 1 ? Cat240MessageType::VideoSummary : Cat240MessageType::Unknown);
        }

        if (frn[3]) {
            if (offset + 4 > blockLength) {
                break;
            }
            radial.hasMessageIndex = true;
            radial.messageIndex = readU32(bytes + offset);
            offset += 4;
        }

        if (frn[4]) {
            if (offset + 1 > blockLength) {
                break;
            }
            const int rep = bytes[offset++];
            if (offset + rep > blockLength) {
                break;
            }
            offset += rep;
        }

        if (frn[5]) {
            if (offset + 12 > blockLength) {
                break;
            }
            radial.startAzimuthDeg = readU16(bytes + offset) * 360.0 / 65536.0;
            radial.endAzimuthDeg = readU16(bytes + offset + 2) * 360.0 / 65536.0;
            radial.startRangeCell = readU32(bytes + offset + 4);
            radial.cellDurationSeconds = readU32(bytes + offset + 8) * 1e-9;
            offset += 12;
        }

        if (frn[6]) {
            if (offset + 12 > blockLength) {
                break;
            }
            radial.startAzimuthDeg = readU16(bytes + offset) * 360.0 / 65536.0;
            radial.endAzimuthDeg = readU16(bytes + offset + 2) * 360.0 / 65536.0;
            radial.startRangeCell = readU32(bytes + offset + 4);
            radial.cellDurationSeconds = readU32(bytes + offset + 8) * 1e-15;
            offset += 12;
        }

        if (frn[7]) {
            if (offset + 2 > blockLength) {
                break;
            }
            const quint16 raw = readU16(bytes + offset);
            radial.compressed = (raw & 0x8000) != 0;
            radial.resolutionCode = raw & 0x00FF;
            radial.bitsPerCell = bitsPerCellFromResolution(radial.resolutionCode);
            offset += 2;
        }

        if (frn[8]) {
            if (offset + 5 > blockLength) {
                break;
            }
            radial.validVideoOctets = readU16(bytes + offset);
            radial.validCells = readU24(bytes + offset + 2);
            offset += 5;
        }

        QByteArray videoBytes;
        if (frn[9]) {
            if (offset + 1 > blockLength) {
                break;
            }
            const int rep = bytes[offset++];
            const int length = rep * 4;
            if (offset + length > blockLength) {
                break;
            }
            videoBytes = dataBlock.mid(offset, length);
            offset += length;
        }

        if (frn[10]) {
            if (offset + 1 > blockLength) {
                break;
            }
            const int rep = bytes[offset++];
            const int length = rep * 64;
            if (offset + length > blockLength) {
                break;
            }
            videoBytes = dataBlock.mid(offset, length);
            offset += length;
        }

        if (frn[11]) {
            if (offset + 1 > blockLength) {
                break;
            }
            const int rep = bytes[offset++];
            const int length = rep * 256;
            if (offset + length > blockLength) {
                break;
            }
            videoBytes = dataBlock.mid(offset, length);
            offset += length;
        }

        if (frn[12]) {
            if (offset + 3 > blockLength) {
                break;
            }
            radial.hasTimeOfDay = true;
            radial.timeOfDaySeconds = readU24(bytes + offset) / 128.0;
            offset += 3;
        }

        if (!radial.compressed && !videoBytes.isEmpty()) {
            if (radial.validVideoOctets > 0 && radial.validVideoOctets < videoBytes.size()) {
                videoBytes.truncate(radial.validVideoOctets);
            }
            radial.amplitudes = unpackCells(videoBytes,
                                            radial.bitsPerCell,
                                            radial.validCells);
        }

        if (offset > recordStart) {
            radial.rawRecord = dataBlock.mid(recordStart, offset - recordStart);
        }

        if (radial.messageType == Cat240MessageType::Video) {
            radials.push_back(radial);
        }

        if (offset == recordStart) {
            break;
        }
    }

    return radials;
}

} // namespace asterixqt
