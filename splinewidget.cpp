#include "splinewidget.h"
#include "ui_splinewidget.h"

#define CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

splinewidget::splinewidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::splinewidget)
{
    ui->setupUi(this);

    ui->label->setGeometry(0,0,this->size().width(),this->size().height());
    lab_pixmap = new QPixmap(ui->label->width(),ui->label->height());

    int pos_mid = this->height() / 2;
    _spline_points.append(QPoint(10,pos_mid));
    _spline_points.append(QPoint(this->width() - 10,pos_mid));

    drawSPLine();
    select_cnt = -1;

}

splinewidget::~splinewidget()
{
    delete ui;
}

void splinewidget::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    if( lab_pixmap != nullptr )
        delete lab_pixmap;

    _spline_points.clear();
    ui->label->setGeometry(0,0,this->size().width(),this->size().height());
    lab_pixmap = new QPixmap(ui->label->width(),ui->label->height());

    int pos_mid = this->height() / 2;
    _spline_points.append(QPoint(10,pos_mid));
    _spline_points.append(QPoint(this->width() - 10,pos_mid));

    drawSPLine();
    select_cnt = -1;
}

int splinewidget::addSPLinePoint(QPoint _p)
{
    for( int n = 0 ; n < _spline_points.size(); n++)
        if(qAbs(_spline_points.at(n).x() - _p.x()) < 30 )return n;

    _spline_points.append(_p);
    std::sort(_spline_points.begin(),_spline_points.end(),[=](QPoint p1,QPoint p2)->bool{
        return p1.x() < p2.x();
    });

    for( int n = 0 ; n < _spline_points.size(); n++)
        if(_spline_points.at(n).x() == _p.x())return n;

    return -1;
}

void splinewidget::drawCursor(QPoint _p)
{
    drawSPLine();

    QPainter painter;
    painter.begin(lab_pixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setPen(QColor(128,128,128));

    QPoint _p1 = QPoint(_p.x(),10);
    QPoint _p2 = QPoint(_p.x(),this->height() - 10 );
    painter.drawLine(_p1,_p2);

    QPoint _p3 = QPoint(10,_p.y());
    QPoint _p4 = QPoint(this->width() - 10,_p.y());
    painter.drawLine(_p3,_p4);
    painter.end();
    ui->label->setPixmap(*lab_pixmap);
}

void splinewidget::drawSPLine()
{

    lab_pixmap->fill(QColor(35,35,35));
    QPainter painter;
    painter.begin(lab_pixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setPen(QPen(QColor(200,200,200),2));

    if( _spline_points.size() > 2 )
    {
        std::vector< double > X, Y;

        foreach (QPoint p, _spline_points) {
            X.push_back(p.x());
            Y.push_back(p.y());
        }

        tk::spline s(X,Y,tk::spline::cspline,false,
                     tk::spline::second_deriv,0.0,tk::spline::first_deriv,left_first_deriv);

        left_first_deriv = s.deriv(1,10);
        for( int n = 10; n < ( this->width() - 10 ); n++ )
        {
            QPoint p1(n,int(s(n)));
            QPoint p2(n+1,int(s(n+1)));

            p1.setY(CONSTRAIN(p1.y(),10,this->height() - 10));
            p2.setY(CONSTRAIN(p2.y(),10,this->height() - 10));

            painter.drawLine(p1,p2);
        }
    }
    else
    {
        painter.drawLine(_spline_points.at(0),_spline_points.at(1));
    }
    painter.setPen(QColor(255,0,0));
    painter.setBrush(QColor(255,0,0));

    foreach (QPoint p, _spline_points){
        //painter.drawRect(QRect(p.x));
        painter.drawEllipse(p,4,4);
    }
    painter.end();
    ui->label->setPixmap(*lab_pixmap);
}

void splinewidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    QPoint pDraw = ui->label->mapFromParent(event->pos());

    if(( pDraw.x() > _spline_points.last().x())||( pDraw.x() < _spline_points.first().x())||(( pDraw.y() > (this->height() - 10) )||( pDraw.y() < 10))) return;

    select_cnt = addSPLinePoint(pDraw);
    drawCursor(_spline_points.at(select_cnt));
}

void splinewidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    select_cnt = -1;
    drawSPLine();
}

void splinewidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if(( select_cnt <= 0 )||( select_cnt >=(_spline_points.size() - 1))) return;

    int range_l = _spline_points.at(select_cnt - 1).x();
    int range_r = _spline_points.at(select_cnt + 1).x();

    QPoint pDraw = ui->label->mapFromParent(event->pos());

    if((pDraw.x() >= range_l )&&(pDraw.x() <= range_r ))
    {
        pDraw.setY(CONSTRAIN(pDraw.y(),10,(this->height() - 10)));
        pDraw.setX(CONSTRAIN(pDraw.x(),(range_l + 30),(range_r - 30 )));
        _spline_points.replace(select_cnt,pDraw);
        drawCursor(_spline_points.at(select_cnt));
    }
    else if(( pDraw.x() < range_l )||( pDraw.x() > range_r)||( pDraw.y() <= 0 )||(pDraw.y() >= ui->label->height()))
    {
        qDebug()<<"removed point"<<QString::number(select_cnt);
        _spline_points.remove(select_cnt);
        drawSPLine();
        select_cnt = -1;
    }
}
