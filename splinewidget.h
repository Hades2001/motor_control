#ifndef SPLINEWIDGET_H
#define SPLINEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDebug>
#include "spline.h"

namespace Ui {
class splinewidget;
}

class splinewidget : public QWidget
{
    Q_OBJECT

public:
    explicit splinewidget(QWidget *parent = nullptr);
    ~splinewidget();

    int addSPLinePoint(QPoint _p);
    void drawCursor(QPoint _p);
    void drawSPLine();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *e);

private slots:

private:
    QVector<QPoint> _spline_points;
    int select_cnt;
    double left_first_deriv = 0;
    QPixmap *lab_pixmap;

private:
    Ui::splinewidget *ui;
};

#endif // SPLINEWIDGET_H
