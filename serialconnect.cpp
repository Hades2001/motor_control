#include "serialconnect.h"
#include "ui_serialconnect.h"

serialconnect::serialconnect(QWidget *parent) :
    transmission(parent),
    ui(new Ui::serialconnect)
{
    ui->setupUi(this);

    _tim_sendtask = new QTimer();
    connect(_tim_sendtask,&QTimer::timeout,this,&serialconnect::slot_send_task);

    _serialpotr = new QSerialPort();
    connect(_serialpotr,&QSerialPort::errorOccurred,this,[=](QSerialPort::SerialPortError error){
        Q_UNUSED(error)
        qDebug()<<error;
        //_serialpotr->close();
        _tim_sendtask->stop();
    });
    connect(_serialpotr,&QSerialPort::readyRead,this,&serialconnect::serial_ready_read);

    QListWidget *listSetial = new QListWidget(this);
    listSetial->setItemDelegate(new NoFocusFrameDelegate(this));

    ui->cb_serial->setModel(listSetial->model());
    ui->cb_serial->setView(listSetial);
    connect(ui->cb_serial,&ClickedComboBox::clicked,this,&serialconnect::scanSerialName);

    this->scanSerialName();
}

serialconnect::~serialconnect()
{
    delete _serialpotr;
    delete ui;
}

void serialconnect::deviceSendpack(dataPack pack)
{
    //qDebug()<<QString::number(pack.retry)<<QString::number(pack._retryCnt)<<QString::number(pack.retryTimeMs)<<QString::number(pack._retryTimeCnt);
    _sendPackList.append(pack);
}

bool serialconnect::isOpen()
{
    return _serialpotr->isOpen();
}

int serialconnect::close()
{
    if( _serialpotr->isOpen())
    {
        _serialpotr->close();
        _tim_sendtask->stop();
        ui->bn_connect->setText("Connect");
        emit sig_closed();
        return 0;
    }
    return -1;
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

void serialconnect::slot_send_task()
{
    if( _sendPackList.isEmpty() ||( !_serialpotr->isOpen())) return;
   dataPack* pack = &_sendPackList.first();

   pack->_retryTimeCnt += 10;

   if( pack->_retryTimeCnt >= pack->retryTimeMs )
   {
       _serialpotr->write(pack->toArray());
       pack->_retryTimeCnt = 0;
       pack->_retryCnt ++;
   }
   if(( pack->_retryCnt >= pack->retry )&&(!_sendPackList.isEmpty()))
   {
       qDebug()<<QString().sprintf("Send pack fault,pack id is %d",pack->id);
       _sendPackList.removeFirst();
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
            _currentRevicePack.id = data;
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
            if(( data == 0xee )&&(_currentRevicePack.check_crc() == true)){
                if(( !_sendPackList.isEmpty())&&(_currentRevicePack.id != 0xff))
                {
                    if(_sendPackList.first().id == _currentRevicePack.id )
                    {
                        _sendPackList.removeFirst();
                    }
                }
                else if(_currentRevicePack.id == 0xff)
                {
                    emit sig_packready(_currentRevicePack);
                }
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
        _tim_sendtask->stop();
        ui->bn_connect->setText("Connect");
        emit sig_closed();
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
        _tim_sendtask->start(10);
        emit sig_open();
    }
}
