#include "connectwidget.h"
#include "ui_connectwidget.h"

connectWidget::connectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::connectWidget)
{

    _connect_mode = QString("unconnect");
    ui->setupUi(this);
    _serial = new serialconnect;

    connect(_serial,&serialconnect::packReady,this,[=](dataPack::transmission_pack_t pack){
        emit sig_packready(pack);
    });

    connect(_serial,&serialconnect::isConnected,this,[=](bool connect){
        _connect_mode = ( connect ) ? QString("serial") : QString("unconnect");
    });

    ui->stackedWidget->addWidget(_serial);
    ui->stackedWidget->setCurrentWidget(_serial);

    ui->label->setHidden(true);
}

connectWidget::~connectWidget()
{
    delete ui;
}

void connectWidget::send_pack(dataPack::transmission_pack_t &pack)
{
    Q_UNUSED(pack);
}

void connectWidget::on_bn_close_pressed(){}

void connectWidget::on_bn_close_released(){}

void connectWidget::on_bn_close_clicked(bool checked)
{
    QPropertyAnimation *pScaleAnimation1 = new QPropertyAnimation(this, "size");
    pScaleAnimation1->setDuration(200);
    ui->label->setHidden(!checked);
    if( checked )
    {
        pScaleAnimation1->setEasingCurve(QEasingCurve::OutCubic);
        pScaleAnimation1->setStartValue(QSize(54,54));
        pScaleAnimation1->setEndValue(QSize(350,174));
    }
    else
    {
        pScaleAnimation1->setEasingCurve(QEasingCurve::InCubic);
        pScaleAnimation1->setStartValue(QSize(350,174));
        pScaleAnimation1->setEndValue(QSize(54,54));
    }
    qDebug()<<_connect_mode;
    ui->bn_close->style()->unpolish(ui->bn_close);
    ui->bn_close->setProperty("mode", _connect_mode);
    ui->bn_close->style()->polish(ui->bn_close);

    pScaleAnimation1->start();
}
