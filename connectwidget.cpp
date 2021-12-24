#include "connectwidget.h"
#include "ui_connectwidget.h"

connectWidget::connectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::connectWidget)
{
    _connect_mode = QString("unconnect");
    ui->setupUi(this);

    deviceList.clear();

    _serial = new serialconnect;
    deviceList.append(_serial);

    foreach(transmission* device,deviceList)
    {
        connect(device,&serialconnect::sig_packready,this,[=](dataPack pack){
            emit sig_packready(pack);
        });
    }

    ui->stackedWidget->addWidget(_serial);
    ui->stackedWidget->setCurrentWidget(_serial);

    ui->label->setHidden(true);

    _pScaleAnimation = new QPropertyAnimation(this, "size");
    _pScaleAnimation->setDuration(200);
}

connectWidget::~connectWidget()
{
    delete ui;
}

void connectWidget::send_pack(dataPack &pack)
{
    Q_UNUSED(pack);
    foreach (transmission* device, deviceList) {
        device->sendpack(pack);
    }
}

void connectWidget::close_window()
{
    ui->label->setHidden(true);
    _pScaleAnimation->setEasingCurve(QEasingCurve::InCubic);
    _pScaleAnimation->setStartValue(QSize(350,174));
    _pScaleAnimation->setEndValue(QSize(54,54));

    ui->bn_close->style()->unpolish(ui->bn_close);
    ui->bn_close->setProperty("mode", _connect_mode);
    ui->bn_close->style()->polish(ui->bn_close);
    _pScaleAnimation->start();
}
void connectWidget::open_window()
{
    ui->label->setHidden(false);
    _pScaleAnimation->setEasingCurve(QEasingCurve::OutCubic);
    _pScaleAnimation->setStartValue(QSize(54,54));
    _pScaleAnimation->setEndValue(QSize(350,174));

    ui->bn_close->style()->unpolish(ui->bn_close);
    ui->bn_close->setProperty("mode", QString("unconnect"));
    ui->bn_close->style()->polish(ui->bn_close);
    _pScaleAnimation->start();
}

void connectWidget::on_bn_close_pressed(){}

void connectWidget::on_bn_close_released(){}

void connectWidget::on_bn_close_clicked(bool checked)
{
    if( checked )
    {
        open_window();
    }
    else
    {
        close_window();
    }
}
