#ifndef DYNAMICCHARTS_H
#define DYNAMICCHARTS_H

#include <QObject>
#include <QDebug>
#include "qcustomplot.h"

template<class T> class dynamicData
{
public:
    dynamicData(QCPGraph *graph,qint32 maxSize):_graph(graph) , _maxSize(maxSize)
    {

    }

    void append(T data)
    {
        if(_dataList.size() >= _maxSize ) _dataList.removeFirst();
        _dataList.append(data);

        if(_dataAxisX.size() < _maxSize )
        _dataAxisX.append(_dataList.size() - 1);

        if( _cnt < 10 )
        {
            _cnt ++;
            return ;
        }
        _cnt = 0;
        _graph->setData(_dataAxisX,_dataList);

    }
    void setColor(QColor color){_graph->setPen(color);}
    void setPen(QPen pen){_graph->setPen(pen);}
    void setBrush(QBrush brush){_graph->setBrush(brush);}

private:

    QVector<T> _dataList;
    QVector<T> _dataAxisX;
    QCPGraph *_graph;
    qint32 _maxSize = 0;

    quint16 _cnt = 0;
};

#endif // DYNAMICCHARTS_H
