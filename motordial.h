#ifndef MOTORDIAL_H
#define MOTORDIAL_H

#include <QObject>
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
    Q_OBJECT
public:
    explicit motordial(QWidget *parent = nullptr);
    ~motordial();

    void setFromSensor(double angle);
    double getAngle();
    int getCircle();

    enum{
        kMotorImgTop = 0,
        kMotorImgMid = 1,
        kMotorImgButtom = 2
    };

signals:
    void sig_pressed(bool);
    void sig_angle_change(int,double);
private:
    QPixmap rotateImageWithPainter(const QPixmap &src, double angle);
protected:
    void paintEvent(QPaintEvent *pe);
    void resizeEvent(QResizeEvent *e);
    void sliderChange(QAbstractSlider::SliderChange change);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    int _img_size = 10;
    double _angle = 0.0;
    double _angle_ole = 0.0;
    int _circle = 0;
    QSvgRenderer *svgimg[3];
    QPixmap *piximg[3];
    QTimer *notifyAngleTimer;
    bool isPressed = false;
};

#endif // MOTORDIAL_H
