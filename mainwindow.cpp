#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
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
        if( !this->_dial_pressed_flag )
        {
            ui->dial->setFromSensor(_dialAngle);
        }
    });

    //ui->dial->blockSignals(false);

    flush->start(5);
    customPlot->addGraph();
    ui->te_log->document()->setMaximumBlockCount(2000);

    //ui->textEdit->append("<font color=\"#FF0000\">红色字体</font>");

     connect(ui->wdt_connect,&connectWidget::sig_packready,this,[=](dataPack pack){
         if(( pack.cmd >= 0x30 )&&( pack.cmd < 0x40 ))
         {
             ui->te_log->append(QString("<font color=\"#%1\">%2</font>").
                                arg(0x101010,6,16,QLatin1Char('0')).
                                arg(QString(pack.data)));
         }
         else if( pack.cmd == 0x11 )
         {
             quint32* u32value = reinterpret_cast<quint32*>(pack.data.data());
             float* fvalue = reinterpret_cast<float*>(u32value);

             _dialAngle = double(R2D(double(fvalue[0])));
             if( !_AngleFocusInFlag )
             {
                 ui->le_angle->setText(QString().sprintf("%.4f",double(R2D(double(fvalue[0])))));
             }
             ui->le_velocity->setText(QString().sprintf("%.4f",double(double(fvalue[1]))));
             _velocityCharts->append(double(fvalue[1]));
         }
     });

     connect(ui->dial,&motordial::sig_angle_change,this,[=](int circle,double angle){
         dataPack pack(dataPack::CMD_SET_ANGLE);
         pack.append(qint32(circle));
         pack.append(float(D2R(fmod(angle,360.0))));

         qDebug()<<QString::number(circle)<<QString::number(angle);
         ui->wdt_connect->send_pack(pack);
     });

     ui->le_angle->installEventFilter(this);

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

bool MainWindow::eventFilter(QObject *watched,QEvent *event)
{
    if( watched == ui->le_angle )
    {
        _AngleFocusInFlag = ( event->type() == QEvent::FocusIn ) ? true : _AngleFocusInFlag;
    }
    return QWidget::eventFilter(watched,event);
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

void MainWindow::on_bn_run_angle_clicked(bool checked)
{
    Q_UNUSED(checked);
    dataPack pack(dataPack::CMD_SET_ANGLE);
    double angle = ui->le_angle->text().toDouble();
    pack.append(qint32(angle/360.0));
    pack.append(float(D2R(fmod(angle,360.0))));

    qDebug()<<QString::number(angle)<<QString::number(qint32(angle/360.0))<<QString::number(fmod(angle,360.0));
    ui->wdt_connect->send_pack(pack);

    _AngleFocusInFlag = false;
}

void MainWindow::on_bn_run_velocity_clicked(bool checked)
{
    Q_UNUSED(checked);
    //ui->wdt_connect->send_pack();
}

void MainWindow::on_bn_run_idle_clicked(bool checked)
{
    Q_UNUSED(checked);
    dataPack pack(dataPack::CMD_SET_IDLE);
    pack.append(0.0f);
    ui->wdt_connect->send_pack(pack);
}


void MainWindow::on_dial_sig_pressed(bool pressed)
{
    qDebug()<<"Dial pressed"<<pressed;
    this->_dial_pressed_flag = pressed;
}

