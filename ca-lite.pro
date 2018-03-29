#-------------------------------------------------
#
# Project created by QtCreator 2017-12-06T10:37:01
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ca_light
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


SOURCES += \
    source/main.cpp \
    source/mainwindow.cpp \
    source/csr_params.cpp \
    source/classes.cpp \
    source/work_dir.cpp \
    source/ca_cert_params.cpp \
    source/dialog.cpp

HEADERS += \
    source/mainwindow.h \
    source/csr_params.h \
    source/classes.h \
    source/work_dir.h \
    source/ca_cert_params.h \
    source/dialog.h

FORMS += \
    ui/mainwindow.ui \
    ui/csr_params.ui \
    ui/ca_cert_params.ui \
    ui/dialog.ui
