#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPixmap>
#include <QTimer>


#include <QMouseEvent>
#include <QDebug>
#include <QTextEdit>

#include "splinewidget.h"
#include "serialconnect.h"
#include "dynamiccharts.h"
#include "qcustomplot.h"
#include "connectwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void on_te_log_textChanged();
    void on_bn_chart_stop_clicked(bool checked);
    void on_bn_clearlog_pressed();
    void on_bn_close_pressed();

private:
    Ui::MainWindow *ui;

    serialconnect *_serial;

    dynamicData<double> *_velocityCharts;
    dynamicData<double> *_angleCharts;

    QVector<double> x;
    QCustomPlot* customPlot;
    qreal _dialAngle = 0.0;

    QTimer* flush;

    bool _bk_pressed_flag = false;
    QPoint _moveStartPos;
    connectWidget *_connectWodget;

    QPropertyAnimation *pWinAnimation;

};

#endif // MAINWINDOW_H
