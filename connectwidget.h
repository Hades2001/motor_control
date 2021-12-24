#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include "datapack.h"
#include "serialconnect.h"
#include "transmission.h"

namespace Ui {
class connectWidget;
}

class connectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit connectWidget(QWidget *parent = nullptr);
    ~connectWidget();
    void send_pack(dataPack &pack);
    void close_window();
    void open_window();

signals:
    void sig_closed();
    void sig_packready(dataPack pack);
private slots:

private slots:
    void on_bn_close_pressed();
    void on_bn_close_released();
    void on_bn_close_clicked(bool checked);


private:
    Ui::connectWidget *ui;

    serialconnect *_serial;
    QString _connect_mode;
    QPropertyAnimation *_pScaleAnimation;
    QList<transmission*> deviceList;
};

#endif // CONNECTWIDGET_H
