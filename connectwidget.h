#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include "datapack.h"
#include "serialconnect.h"

namespace Ui {
class connectWidget;
}

class connectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit connectWidget(QWidget *parent = nullptr);
    ~connectWidget();

    void send_pack(dataPack::transmission_pack_t &pack);

signals:
    void sig_closed();
    void sig_packready(dataPack::transmission_pack_t pack);

private slots:
    void on_bn_close_pressed();
    void on_bn_close_released();
    void on_bn_close_clicked(bool checked);

private:
    Ui::connectWidget *ui;

    serialconnect *_serial;
    QString _connect_mode;
};

#endif // CONNECTWIDGET_H
