#include "serialconnect.h"
#include "ui_serialconnect.h"

serialconnect::serialconnect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::serialconnect)
{
    ui->setupUi(this);

    _crc_table = crctable_generate(quint8(0x07));

    _serialpotr = new QSerialPort();

    connect(_serialpotr,&QSerialPort::errorOccurred,this,[](QSerialPort::SerialPortError error){
        Q_UNUSED(error)
    });
    connect(_serialpotr,&QSerialPort::readyRead,this,&serialconnect::serial_ready_read);

    scanSerialName();

    QListWidget *listSetial = new QListWidget(this);
    listSetial->setItemDelegate(new NoFocusFrameDelegate(this));

    ui->cb_serial->setModel(listSetial->model());
    ui->cb_serial->setView(listSetial);
    connect(ui->cb_serial,&ClickedComboBox::clicked,this,&serialconnect::scanSerialName);

}

serialconnect::~serialconnect()
{
    delete ui;
}

quint8 serialconnect::reverse(quint8 value)
{
  value = quint8(((value & 0xAA) >> 1) | ((value & 0x55) << 1));
  value = quint8(((value & 0xCC) >> 2) | ((value & 0x33) << 2));
  value = quint8((value >> 4) | (value << 4));

  return value;
}

QVector<quint8> serialconnect::crctable_generate(quint8 poly, bool refin, bool refout)
{
    QVector<quint8> array(256);
    // X^8+X^5+X^4+1 0x31
    // 1 0011 0001
    // X^8+X^2+X^1+1 0x07
    // 1 0000 0111
    // X8+X4+X3+X2+1 0x1d
    // 1 0001 1101
    for( int i = 0; i < 256; i++ )
    {
        array[i] = ( refin ) ? reverse(quint8(i)) : quint8(i);

        for( int n = 7; n >= 0; n-- )
        {
            if(array[i] & 0x80){
                array[i] = quint8(( array.at(i) << 1 ) ^ poly);
            }
            else{
                array[i] = quint8(array.at(i) << 1 );
            }
        }
        array[i] = ( refout ) ? reverse(array.at(i)) : array.at(i);
    }
    return array;
}

void serialconnect::scanSerialName(void)
{
    ui->cb_serial->clear();
    QList<QSerialPortInfo>  infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty())
    {
        _serialpotr->clear();
        return;
    }
    foreach (QSerialPortInfo info, infos)
    {
        ui->cb_serial->addItem(info.portName());
        ui->cb_serial->setCurrentIndex(0);
        ui->cb_serial->itemData(0).toString();
    }
}

void serialconnect::serial_ready_read()
{
    QByteArray array = _serialpotr->readAll();
    if( array.isEmpty()) return ;
    for( int i = 0; i < array.length(); i++ )
    {
        quint8 data = quint8(array.at(i));
        switch (_reviceState)
        {
        case kSerial_IDEL:
            _reviceState = (data == 0xAA) ? kSerial_HA : kSerial_IDEL;
            break;
        case kSerial_HA:
            _reviceState = (data == 0x55) ? kSerial_DIR : kSerial_IDEL;
            break;
        case kSerial_DIR:
            _currentRevicePack.crc = 0;
            _currentRevicePack.dir = data;
            _reviceState = kSerial_CMD;
            break;
        case kSerial_CMD:
            _currentRevicePack.cmd = data;
            _reviceState = kSerial_Lengt;
            break;
        case kSerial_Lengt:
            _currentRevicePack.length = data;

            _reviceDataCnt = 0;
            _reviceState = (_currentRevicePack.length == 0) ? kSerial_CRC : kSerial_DATA;
            break;
        case kSerial_DATA:
            _currentRevicePack.data.append(char(data));
            _reviceDataCnt++;
            if (_reviceDataCnt >= _currentRevicePack.length)
            {
                _reviceDataCnt = 0;
                _reviceState = kSerial_CRC;
            }
            break;
        case kSerial_CRC:
            _currentRevicePack.crc = data;
            _reviceState = kSerial_End;
            break;
        case kSerial_End:
            if(( data == 0xee )&&(cala_crc(_currentRevicePack) == _currentRevicePack.crc)){
                emit packReady(_currentRevicePack);
                _currentRevicePack.data.clear();
                //_revicePackList.append(_currentRevicePack);
            }
            _reviceState = kSerial_IDEL;
            break;
        default:
            _reviceState = kSerial_IDEL;
            break;
        }
    }
}

void serialconnect::on_bn_connect_pressed()
{
    if( _serialpotr->isOpen())
    {
        _serialpotr->close();
        ui->bn_connect->setText("Connect");
        emit isConnected(false);
    }
    else
    {
        if( ui->cb_serial->count() == 0 )
        {
            QMessageBox::warning(this,"Serial warning","Serial port not found");
            return;
        }
        _serialpotr->setBaudRate(1500000,QSerialPort::AllDirections);
        _serialpotr->setDataBits(QSerialPort::Data8);
        _serialpotr->setFlowControl(QSerialPort::NoFlowControl);
        _serialpotr->setParity(QSerialPort::NoParity);
        _serialpotr->setStopBits(QSerialPort::OneStop);
        _serialpotr->setPortName(ui->cb_serial->currentText());
        if( !_serialpotr->open(QIODevice::ReadWrite))
        {
            return ;
        }
        ui->bn_connect->setText("Close");
        emit isConnected(true);
    }
}

quint8 serialconnect::cala_crc(dataPack::transmission_pack_t &pack)
{
    quint8 crc = 0;
    crc = _crc_table.at(crc ^ 0xaa);
    crc = _crc_table.at(crc ^ 0x55);
    crc = _crc_table.at(crc ^ pack.dir);
    crc = _crc_table.at(crc ^ pack.cmd);
    crc = _crc_table.at(crc ^ pack.length);

    foreach(char data, pack.data)
    {
        crc = _crc_table.at(crc ^ quint8(data));
    }
    if(pack.crc != crc ) qDebug()<<QString().sprintf("CRC shoud be: %02x ",crc)<<QString().sprintf("but it's %02x",pack.crc);
    //qDebug()<<QString().sprintf("CRC shoud be: %02x ",crc)<<QString().sprintf("is %02x",pack.crc);
    return crc;
}
