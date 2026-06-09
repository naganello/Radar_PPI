#pragma once

#include "AsterixTypes.h"

namespace asterixqt {

class ASTERIXQT_EXPORT Cat048Decoder
{
public:
    QVector<Cat048Target> decodeDataBlock(const QByteArray& dataBlock,
                                          const QDateTime& receiveTimeUtc);

private:
    static quint16 readU16(const quint8* p);
    static qint16 readS16(const quint8* p);
    static quint32 readU24(const quint8* p);
    static QString decodeAircraftId(const quint8* p);
};

} // namespace asterixqt
