#ifndef DATAPACK_H
#define DATAPACK_H

#include <QObject>

class dataPack
{

public:
    typedef struct transmission_pack
    {
        quint8 dir;
        quint8 cmd;
        quint8 length;
        QByteArray data;
        quint8 crc;
    }transmission_pack_t;

};

#endif // DATAPACK_H
