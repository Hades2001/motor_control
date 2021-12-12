#include "mainwindow.h"
#include "ui_mainwindow.h"

#define R2D(_R) (_R * 57.295779f)



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _serial = new serialconnect();
    ui->tw_connect->clear();
    ui->tw_connect->addTab(_serial,"SERIAL");
    ui->tw_connect->addTab(new QWidget(),"USB HID");
    ui->tw_connect->addTab(new QWidget(),"RS485");

    this->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏

    //this->setWindowOpacity(0.7);//设置透明1-全体透明
    this->setAttribute(Qt::WA_TranslucentBackground, true);


    customPlot = ui->graphicsView_charts;

    _velocityCharts = new dynamicData<double>(customPlot->addGraph(),5000);
    dynamicData<double>* _velref_Charts = new dynamicData<double>(customPlot->addGraph(),5000);
    _angleCharts = new dynamicData<double>(customPlot->addGraph(),5000);

    _velocityCharts->setPen(QColor(255,0,0));
    _velref_Charts->setPen(QPen(QColor(0,0,200,128),2,Qt::SolidLine));
    _angleCharts->setPen(QColor(0,255,0));

    customPlot->xAxis->setRange(0,5000);
    customPlot->yAxis->setRange(0,120);

    for( int i = 0; i < 1000; i++)
    {
        _velref_Charts->append(double(20.0f));
    }

    flush = new QTimer();
    connect(flush,&QTimer::timeout,this,[=](){
        customPlot->replot();
        ui->dial->setValue(int(_dialAngle));
    });

    flush->start(17);
    customPlot->addGraph();
    ui->te_log->document()->setMaximumBlockCount(2000);


    //ui->bn_connect->setProperty("mode", "serial");
    //ui->textEdit->append("<font color=\"#FF0000\">红色字体</font>");

     connect(ui->wdt_connect,&connectWidget::sig_packready,this,[=](dataPack::transmission_pack_t pack){
         if(( pack.cmd >= 0x30 )&&( pack.cmd < 0x40 ))
         {
             ui->te_log->append(QString("<font color=\"#%1\">%2</font>").
                                arg(0xdedede,6,16,QLatin1Char('0')).
                                arg(QString(pack.data)));
         }
         else if( pack.cmd == 0x11 )
         {
             //qDebug()<<"parm Length = "<<QString::number(pack.length);

             quint32* u32value = reinterpret_cast<quint32*>(pack.data.data());
             float* fvalue = reinterpret_cast<float*>(u32value);

             qreal angle = double(R2D(fvalue[0]));
             angle = fmod(angle,360.0);
             _dialAngle = ( angle < 0 ) ? ( 360.0 - fabs(angle)) : angle;
             ui->le_angle->setText(QString().sprintf("%.4f",double(R2D(fvalue[0]))));
             ui->le_velocity->setText(QString().sprintf("%.4f",double(fvalue[1])));
             _velocityCharts->append(double(fvalue[1]));
         }
     });

     pWinAnimation = new QPropertyAnimation(this, "size");

     pWinAnimation->setDuration(400);
     pWinAnimation->setEasingCurve(QEasingCurve::InCubic);
     pWinAnimation->setStartValue(QSize(16,430));
     pWinAnimation->setEndValue(QSize(1000,430));
     pWinAnimation->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    //ui->scoreStandardBtn->geometry().contains(this->mapFromGlobal(QCursor::pos()))
    if(!ui->lab_background->geometry().contains((this->mapFromGlobal(event->pos()))))
    {
        this->_bk_pressed_flag = true;
        _moveStartPos = event->globalPos() - this->pos();
    }

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->_bk_pressed_flag = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if(_bk_pressed_flag)
    {
        this->move(event->globalPos() - _moveStartPos);
    }
}

void MainWindow::on_te_log_textChanged()
{
    ui->te_log->moveCursor(QTextCursor::End);
}

void MainWindow::on_bn_chart_stop_clicked(bool checked)
{
    if(checked)  flush->stop();
    else flush->start(17);
}

void MainWindow::on_bn_clearlog_pressed()
{
    ui->te_log->clear();
}

void MainWindow::on_bn_close_pressed()
{
    pWinAnimation->setDuration(400);
    pWinAnimation->setEasingCurve(QEasingCurve::OutCubic);
    pWinAnimation->setStartValue(QSize(1000,430));
    pWinAnimation->setEndValue(QSize(16,430));

    connect(pWinAnimation,&QPropertyAnimation::finished,this,[=](){
        QApplication::exit();
    });
    pWinAnimation->start();
}
