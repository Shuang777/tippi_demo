#-------------------------------------------------
#
# Project created by QtCreator 2015-04-26T10:38:10
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo
TEMPLATE = app

INCLUDEPATH += /home/shuang/project/kaldi/trunk/tools/ATLAS/include /home/shuang/project/kaldi/trunk/tools/openfst/include /home/shuang/project/kaldi/trunk/src

SOURCES += main.cpp\
        login.cpp \
    progressbar.cpp \
    signup.cpp \
    ivectorextraction.cpp \
    basics.cpp

HEADERS  += login.h \
    progressbar.h \
    portaudiocallback.h \
    basics.h \
    signup.h \
    ivectorextraction.h

FORMS    += login.ui \
    signup.ui \
    progressbar.ui

QMAKE_CXXFLAGS += -pthread -std=c++11 -msse -msse2 -rdynamic -w
QMAKE_CXXFLAGS += -DKALDI_DOUBLEPRECISION=0 -DHAVE_POSIX_MEMALIGN  -Winit-self -DHAVE_EXECINFO_H=1 -rdynamic -DHAVE_CXXABI_H -DHAVE_ATLAS

LIBS += /home/shuang/softwares/portaudio/lib/.libs/libportaudio.so \
        /home/shuang/softwares/libsndfile-1.0.25/src/.libs/libsndfile.so
LIBS += -L/home/shuang/project/kaldi/trunk/tools/openfst/lib
LIBS += /home/shuang/project/kaldi/trunk/src/ivector/kaldi-ivector.a \
        /home/shuang/project/kaldi/trunk/src/decoder/kaldi-decoder.a \
        /home/shuang/project/kaldi/trunk/src/lat/kaldi-lat.a \
        /home/shuang/project/kaldi/trunk/src/feat/kaldi-feat.a \
        /home/shuang/project/kaldi/trunk/src/hmm/kaldi-hmm.a \
        /home/shuang/project/kaldi/trunk/src/transform/kaldi-transform.a \
        /home/shuang/project/kaldi/trunk/src/gmm/kaldi-gmm.a \
        /home/shuang/project/kaldi/trunk/src/tree/kaldi-tree.a \
        /home/shuang/project/kaldi/trunk/src/thread/kaldi-thread.a \
        /home/shuang/project/kaldi/trunk/src/matrix/kaldi-matrix.a \
        /home/shuang/project/kaldi/trunk/src/util/kaldi-util.a \
        /home/shuang/project/kaldi/trunk/src/base/kaldi-base.a
LIBS += -lrt -lasound -lpthread -lm -lfst -llapack -lcblas /usr/lib/liblapack.so /usr/lib/libcblas.so /usr/lib/libatlas.so /usr/lib/libf77blas.so -ldl

RESOURCES += \
    resource.qrc


