QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

TARGET = zcontentvisu



#------------Private to libraries build and install  (python utility <postlinkcopy.py>)-----------
#

HOME = /home/gerard

# --------ztoolset common definitions------------

DEVELOPMENT_BASE = $${HOME}/Development
TOOLSET_ROOT = $$DEVELOPMENT_BASE/zbasetools

# name of the root include directory

MODULE = ztestindexfile

#defines DEVELOPMENT_BASE, TOOLSET symbols and ICU symbols

include ($$TOOLSET_ROOT/common/zbasecommon.pri)


# QMAKE_CXXFLAGS += -fpermissive

QMAKE_CXXFLAGS+= -std=c++17 -Wall -pthread

QMAKE_LFLAGS +=  -std=c++17 -lpthread

QMAKE_LFLAGS +=   -lssl

# if using libxml2
#LIBS +=/usr/local/lib/libxml2.a
QMAKE_LFLAGS += -lxml2

#in any cases
QMAKE_LFLAGS += -lcrypto -lcrypto

#
# Warning :
# 1- do not confuse -lcrypto and -lcrypt both libraries are necessary
# 2- lcrypt must be the last in library list
#


#LIBS += -L/usr/lib/
#LIBS += -ldl -static-libstdc++  #do not link libstdc in static where we link icu in dynamic because icu libraries need libstdc
LIBS += -ldl -lstdc++

unix:{
LIBS += -L$${LIBRARY_REPOSITORY} -lzbase -lzcontent

# if using libxml++  (version 2.6)
#LIBS +=/usr/lib/libxml++-2.6.a

#for icu
#LIBS += -L$${ICU_LIB} -licuuc -licudata -licui18n -licuio -licutu -licutest
LIBS +=  -licuuc -licudata -licui18n -licuio -licutu -licutest
}
#LIBS += $$ICU_BASE/lib/libicudata.a
#LIBS += $$ICU_BASE/lib/libicui18n.a
#LIBS += $$ICU_BASE/lib/libicuio.a
#LIBS += $$ICU_BASE/lib/libicutu.a
#LIBS += $$ICU_BASE/lib/libicutest.a

ZQT_ROOT = $${TOOLSET_ROOT}/zqt

ZQT_WIDGET = $${ZQT_ROOT}/zqtwidget

 INCLUDEPATH += $${TOOLSET_INCLUDE}  \
                $${TOOLSET_INC_ZBASE} \
                $${TOOLSET_INC_ZNET} \
                $${TOOLSET_INC_ZCONTENT} \
                $${TOOLSET_INC_ZAM} \
                $${OPENSSL_ROOT} \
                $${ZQT_WIDGET} \
                $${DEVELOPMENT_BASE}/zflow

#if using ICU
INCLUDEPATH +=  $${ICU_COMMON} \
                $${ICU_INCLUDE_BASE} \
                $${ICU_INCLUDE} \
                $${ICU_INCLUDEi18n}

# if using libxml2
INCLUDEPATH +=/usr/include/libxml2

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../zqt/zqtwidget/zqlabel.cpp \
    displaymain.cpp \
    main.cpp \
    texteditmwn.cpp \
    zcontentvisumain.cpp \
    zexceptiondlg.cpp \
    zscan.cpp

HEADERS += \
    ../../zqt/zqtwidget/zqlabel.h \
    displaymain.h \
    texteditmwn.h \
    zcontentvisumain.h \
    zexceptiondlg.h \
    zscan.h

FORMS += \
    displaymain.ui \
    texteditmwn.ui \
    zcontentvisumain.ui \
    zexceptiondlg.ui \
    zscan.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
