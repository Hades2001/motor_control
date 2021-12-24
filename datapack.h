#ifndef DATAPACK_H
#define DATAPACK_H

#include <QObject>
#include <QVector>

#define PI 3.141592653
#define R2D(_R) (_R * 57.295779)
#define D2R(_D) (_D * PI / 180 )

class dataPack
{
public:
    typedef enum{
        CMD_READ_ALL            = 0x25,

        CMD_SET_IDLE            = 0x90,
        CMD_SET_VELOCITY        = 0x91,
        CMD_SET_ANGLE           = 0x92,

        CMD_SET_ALL             = 0xA0,
        CMD_SET_SENSOR_OFFSET   = 0xA1,
        CMD_SET_DC              = 0xA2,
        CMD_SET_VOUT_LIMIT      = 0xA3,

        CMD_SET_VELOCITY_PID    = 0xB1,
        CMD_SET_ANGLE_PID       = 0xB2,
    }pack_cmd_t;
public:
    explicit dataPack(pack_cmd_t _cmd, QByteArray _data = QByteArray(), int _retime = 100,int _recnt = 5,quint8 poly=0x07, bool refin=false, bool refout=false):cmd(_cmd),data(_data){
        crctable_generate(poly,refin,refout);
        this->length = quint8(_data.length());
        cala_crc();

        this->retryTimeMs = _retime;
        this->_retryTimeCnt = _retime;
        this->retry = _recnt;
        this->_retryCnt = 0;
    }
    explicit dataPack(quint8 poly=0x07, bool refin=false, bool refout=false){
        crctable_generate(poly,refin,refout);
    }

    void init(quint8 _cmd, QByteArray _data,int _retime = 100,int _recnt = 5)
    {
        this->cmd = _cmd;
        this->length = quint8(_data.length());
        this->data = _data;

        this->retryTimeMs = _retime;
        this->_retryTimeCnt = _retime;
        this->retry = _recnt;
        this->_retryCnt = 0;
        cala_crc();
    }

    void set_retry(int _retime,int _recnt)
    {
        this->retryTimeMs = _retime;
        this->_retryTimeCnt = _retime;
        this->retry = _recnt;
        this->_retryCnt = 0;
    }

    void append(float _data){
        quint32* data_quint32 = reinterpret_cast<quint32*>(&_data);
        this->append(*data_quint32);
        this->length = quint8(data.length());
        cala_crc();
    }

    void append(quint32 _data){
        char* databyte = reinterpret_cast<char*>(&_data);
        this->data.append(databyte[0]);
        this->data.append(databyte[1]);
        this->data.append(databyte[2]);
        this->data.append(databyte[3]);
        this->length = quint8(data.length());
        cala_crc();
    }

    void append(qint32 _data){
        char* databyte = reinterpret_cast<char*>(&_data);
        this->data.append(databyte[0]);
        this->data.append(databyte[1]);
        this->data.append(databyte[2]);
        this->data.append(databyte[3]);
        this->length = quint8(data.length());
        cala_crc();
    }

    void append(QByteArray _data)
    {
       this->data.append(_data);
       this->length = quint8(data.length());
       cala_crc();
    }

    quint8 cala_crc(){
        quint8 _crc = 0;
        _crc = crc_table.at(_crc ^ 0xaa);
        _crc = crc_table.at(_crc ^ 0x55);
        _crc = crc_table.at(_crc ^ this->id);
        _crc = crc_table.at(_crc ^ this->cmd);
        _crc = crc_table.at(_crc ^ this->length);
        foreach(char data, this->data){
            _crc = crc_table.at(_crc ^ quint8(data));
        }
        this->crc = _crc;
        return crc;
    }
    bool check_crc(){
        quint8 _crc = 0;
        _crc = crc_table.at(_crc ^ 0xaa);
        _crc = crc_table.at(_crc ^ 0x55);
        _crc = crc_table.at(_crc ^ this->id);
        _crc = crc_table.at(_crc ^ this->cmd);
        _crc = crc_table.at(_crc ^ this->length);
        foreach(char data, this->data){
            _crc = crc_table.at(_crc ^ quint8(data));
        }
        return ( _crc == this->crc ) ? true : false;
    }

    QByteArray toArray()
    {
        QByteArray array;

        this->length = ( this->data.length() != this->length ) ? quint8(this->data.length()) : this->length;
        this->crc = this->cala_crc();

        array.append(char(0xAA));
        array.append(char(0x55));
        array.append(char(this->id));
        array.append(char(this->cmd));
        array.append(char(this->length));
        array.append(this->data);
        array.append(char(this->crc));
        array.append(char(0xee));
        return array;
    }

    quint8 reverse(quint8 value)
    {
      value = quint8(((value & 0xAA) >> 1) | ((value & 0x55) << 1));
      value = quint8(((value & 0xCC) >> 2) | ((value & 0x33) << 2));
      value = quint8((value >> 4) | (value << 4));

      return value;
    }

    void crctable_generate(quint8 poly, bool refin, bool refout){
        crc_table.clear();
        crc_table.resize(256);
        for( int i = 0; i < 256; i++ ){
            crc_table[i] = ( refin ) ? reverse(quint8(i)) : quint8(i);
            for( int n = 7; n >= 0; n-- ){
                if(crc_table[i] & 0x80){
                    crc_table[i] = quint8(( crc_table.at(i) << 1 ) ^ poly);
                }
                else{
                    crc_table[i] = quint8(crc_table.at(i) << 1 );
                }
            }
            crc_table[i] = ( refout ) ? reverse(crc_table.at(i)) : crc_table.at(i);
        }
    }
    /*
    dataPack &operator=(dataPack &pack){
        this->id = pack.id;
        this->cmd = pack.cmd;
        this->length = pack.length;
        this->data.clear();
        this->data = pack.data;
        this->crc = pack.crc;
        return *this;
    }*/

public:
    quint8 id;
    quint8 cmd;
    quint8 length;
    QByteArray data;
    quint8 crc;

    int retry;
    int retryTimeMs;
    int _retryCnt;
    int _retryTimeCnt;
private:
    QVector<quint8> crc_table;

};

#endif // DATAPACK_H
