#-------------------------------------------------
#
# Project created by QtCreator 2021-11-30T23:21:07
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += svg
QT       += charts
QT       += widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cw_key
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    motordial.cpp \
    splinewidget.cpp \
    serialconnect.cpp \
    clickedcombobox.cpp \
    nofocusframedelegate.cpp \
    dynamiccharts.cpp \
    qcustomplot.cpp \
    connectwidget.cpp

HEADERS += \
        mainwindow.h \
    spline.h \
    motordial.h \
    splinewidget.h \
    serialconnect.h \
    clickedcombobox.h \
    nofocusframedelegate.h \
    dynamiccharts.h \
    qcustomplot.h \
    connectwidget.h \
    datapack.h \
    transmission.h

FORMS += \
        mainwindow.ui \
    splinewidget.ui \
    serialconnect.ui \
    connectwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
