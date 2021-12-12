#ifndef MOTORDIAL_H
#define MOTORDIAL_H

#include <QWidget>
#include <QDial>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QtSvg>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QTimer>

class motordial : public QDial
{
public:
    explicit motordial(QWidget *parent = nullptr);
    ~motordial();

    enum{
        kMotorImgTop = 0,
        kMotorImgMid = 1,
        kMotorImgButtom = 2
    };
private:
    QPixmap rotateImageWithPainter(const QPixmap &src, int angle);
protected:
    void paintEvent(QPaintEvent *pe);
    void resizeEvent(QResizeEvent *e);
    void sliderChange(QAbstractSlider::SliderChange change);

    int _img_size = 10;
    QSvgRenderer *svgimg[3];
    QPixmap *piximg[3];
};

#endif // MOTORDIAL_H
