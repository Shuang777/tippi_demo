#-------------------------------------------------
#
# Project created by QtCreator 2015-04-26T10:38:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo
TEMPLATE = app


SOURCES += main.cpp\
        login.cpp \
    signin.cpp \
    progressbar.cpp \
    incprogressbar.cpp

HEADERS  += login.h \
    signin.h \
    progressbar.h \
    incprogressbar.h \
    portaudiocallback.h \
    basics.h

FORMS    += login.ui \
    signin.ui \
    progressbar.ui

QMAKE_CXXFLAGS += -pthread -std=c++11

LIBS += /home/shuang/softwares/portaudio/lib/.libs/libportaudio.so /home/shuang/softwares/libsndfile-1.0.25/src/.libs/libsndfile.so -lrt -lasound -lpthread -lm
