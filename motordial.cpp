#include "motordial.h"

motordial::motordial(QWidget *parent) :
    QDial(parent)
{
    Q_UNUSED(parent)
    svgimg[kMotorImgTop] = new QSvgRenderer(QString(":/svg/icons/motor_top.svg"));
    svgimg[kMotorImgMid] = new QSvgRenderer(QString(":/svg/icons/motor_mid.svg"));
    svgimg[kMotorImgButtom] = new QSvgRenderer(QString(":/svg/icons/motor_bottom.svg"));

    _img_size = qMin(this->height(),this->width());
    piximg[kMotorImgTop] = new QPixmap(_img_size,_img_size);
    piximg[kMotorImgMid] = new QPixmap(_img_size,_img_size);
    piximg[kMotorImgButtom] = new QPixmap(_img_size,_img_size);

    piximg[kMotorImgTop]->fill(Qt::transparent);
    piximg[kMotorImgMid]->fill(Qt::transparent);
    piximg[kMotorImgButtom]->fill(Qt::transparent);

    QPainter drawTopImg(piximg[kMotorImgTop]);
    QPainter drawTopMid(piximg[kMotorImgMid]);
    QPainter drawButtomImg(piximg[kMotorImgButtom]);

    drawTopImg.setRenderHint(QPainter::SmoothPixmapTransform);
    drawTopMid.setRenderHint(QPainter::SmoothPixmapTransform);
    drawButtomImg.setRenderHint(QPainter::SmoothPixmapTransform);

    svgimg[kMotorImgTop]->render(&drawTopImg);
    svgimg[kMotorImgMid]->render(&drawTopMid);
    svgimg[kMotorImgButtom]->render(&drawButtomImg);


    QTimer *rrrr = new QTimer();
    connect(rrrr,&QTimer::timeout,this,[=](){
        int value = this->value() + 1;
        value = ( value > 359 ) ? 0 : value;
        this->setValue(value);
    });
    //rrrr->start(5);

}

motordial::~motordial()
{

}

QPixmap motordial::rotateImageWithPainter(const QPixmap &src, int angle)
{
    QPixmap pix(src.size());
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setBrush(Qt::transparent);
    painter.setPen(Qt::transparent);
    QRect srcRect = src.rect();
    painter.translate(srcRect.center());
    painter.rotate(angle);

    int cubeWidth = qMin(src.width(),src.height())/2;
    painter.drawEllipse(QPoint(0,0),cubeWidth,cubeWidth);

    painter.translate(-srcRect.center());
    painter.drawPixmap(QPoint(-1,-1),src);

    return pix;
}

void motordial::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)

    _img_size = int(qMin(this->height(),this->width()));

    piximg[kMotorImgTop] = new QPixmap(_img_size,_img_size);
    piximg[kMotorImgMid] = new QPixmap(_img_size,_img_size);
    piximg[kMotorImgButtom] = new QPixmap(_img_size,_img_size);

    piximg[kMotorImgTop]->fill(Qt::transparent);
    piximg[kMotorImgMid]->fill(Qt::transparent);
    piximg[kMotorImgButtom]->fill(Qt::transparent);

    QPainter drawTopImg(piximg[kMotorImgTop]);
    QPainter drawTopMid(piximg[kMotorImgMid]);
    QPainter drawButtomImg(piximg[kMotorImgButtom]);

    drawTopImg.setRenderHint(QPainter::SmoothPixmapTransform);
    drawTopMid.setRenderHint(QPainter::SmoothPixmapTransform);
    drawButtomImg.setRenderHint(QPainter::SmoothPixmapTransform);

    svgimg[kMotorImgTop]->render(&drawTopImg);
    svgimg[kMotorImgMid]->render(&drawTopMid);
    svgimg[kMotorImgButtom]->render(&drawButtomImg);

}

void motordial::paintEvent(QPaintEvent *pe)
{
    Q_UNUSED(pe)

    QPainter p(this);

    p.drawPixmap(QRect(0,0,_img_size,_img_size),rotateImageWithPainter(*piximg[kMotorImgButtom],0),QRect(0,0,_img_size,_img_size));
    p.drawPixmap(QRect(0,0,_img_size,_img_size),rotateImageWithPainter(*piximg[kMotorImgMid],this->value()),QRect(0,0,_img_size,_img_size));
    p.drawPixmap(QRect(0,0,_img_size,_img_size),rotateImageWithPainter(*piximg[kMotorImgTop],0),QRect(0,0,_img_size,_img_size));
    p.end();

}

void motordial::sliderChange(QAbstractSlider::SliderChange change)
{
    if( change == QAbstractSlider::SliderChange::SliderValueChange )
    {
        //qDebug()<<"Value : "<<this->value();
        this->update();
    }
}
