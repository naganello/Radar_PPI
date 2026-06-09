#pragma once

#include "AsterixTypes.h"

namespace asterixqt {

class ASTERIXQT_EXPORT Cat240Decoder
{
public:
    QVector<Cat240VideoRadial> decodeDataBlock(const QByteArray& dataBlock,
                                               const QDateTime& receiveTimeUtc);

private:
    static quint16 readU16(const quint8* p);
    static quint32 readU24(const quint8* p);
    static quint32 readU32(const quint8* p);
    static quint8 bitsPerCellFromResolution(quint8 resolutionCode);
    static QVector<quint32> unpackCells(const QByteArray& bytes,
                                        quint8 bitsPerCell,
                                        quint32 validCells);
};

} // namespace asterixqt
