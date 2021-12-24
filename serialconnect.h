#ifndef SERIALCONNECT_H
#define SERIALCONNECT_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QListWidget>
#include <QTimer>
#include <QList>
#include <QDebug>

#include "nofocusframedelegate.h"
#include "datapack.h"
#include "transmission.h"

namespace Ui {
class serialconnect;
}

class serialconnect : public transmission
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

    void deviceSendpack(dataPack pack);
    bool isOpen();
    int  close();

private slots:
    void serial_ready_read();
    void slot_send_task();

private slots:
    void on_bn_connect_pressed();

private:
    Ui::serialconnect *ui;
    QSerialPort *_serialpotr;
    QTimer *_tim_sendtask;

    QList<dataPack> _revicePackList;
    QList<dataPack> _sendPackList;
    int _reviceState = kSerial_IDEL;
    int _reviceDataCnt = 0;

    dataPack _currentRevicePack;
};



#endif // SERIALCONNECT_H
