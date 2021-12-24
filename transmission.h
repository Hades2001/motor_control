#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <QWidget>
#include "datapack.h"

class transmission : public QWidget
{
    Q_OBJECT
public:
    explicit transmission(QWidget *parent = nullptr):
        QWidget(parent)
    {
        _id = 0;
    }

    void sendpack(dataPack pack)
    {
        pack.id = this->getID();
        deviceSendpack(pack);
    }

    void sendpack(dataPack pack,int _retime,int _recnt)
    {
        pack.id = this->getID();
        pack.set_retry(_retime,_recnt);
        deviceSendpack(pack);
    }
    virtual bool isOpen() = 0;
    virtual int  close() = 0;

private:
    virtual void deviceSendpack(dataPack pack) = 0;
    quint8 getID()
    {
        _id++;
        _id = ( _id >= 0xff ) ? 0 : _id;
        return quint8(_id);
    }

signals:
    void sig_packready(dataPack pack);
    void sig_closed();
    void sig_open();
    void sig_error();
    void sig_send_error(int id);

public slots:

private:
    int _id = 0;
};

#endif // TRANSMISSION_H
