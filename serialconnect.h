#ifndef SERIALCONNECT_H
#define SERIALCONNECT_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QListWidget>
#include "nofocusframedelegate.h"
#include "datapack.h"
#include <QList>
#include <QDebug>

namespace Ui {
class serialconnect;
}

class serialconnect : public QWidget
{
    Q_OBJECT

public:
    enum {
        kSerial_IDEL = 0,
        kSerial_HA,
        kSerial_DIR,
        kSerial_CMD,
        kSerial_Lengt,
        kSerial_DATA,
        kSerial_CRC,
        kSerial_End,
        kSerial_STATE_MAX
    };

public:
    explicit serialconnect(QWidget *parent = nullptr);
    ~serialconnect();

    void scanSerialName(void);
    QVector<quint8> crctable_generate(quint8 poly, bool refin = false, bool refout = false);
    quint8 reverse(quint8);
    quint8 cala_crc(dataPack::transmission_pack_t &pack);

private slots:
    void serial_ready_read();
signals:
    void packReady(dataPack::transmission_pack_t pack);
    void isConnected(bool);

private slots:
    void on_bn_connect_pressed();


private:
    Ui::serialconnect *ui;
    QSerialPort *_serialpotr;

    QList<dataPack::transmission_pack_t> _revicePackList;
    int _reviceState = kSerial_IDEL;
    int _reviceDataCnt = 0;

    QVector<quint8> _crc_table;
    dataPack::transmission_pack_t _currentRevicePack;
};



#endif // SERIALCONNECT_H
