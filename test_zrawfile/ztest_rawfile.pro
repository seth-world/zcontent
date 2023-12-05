QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

TARGET = ztestrawfile



#------------Private to libraries build and install  (python utility <postlinkcopy.py>)-----------
#

HOME = /home/gerard


# --------ztoolset common definitions------------

DEVELOPMENT_BASE = $${HOME}/Development
TOOLSET_ROOT = $${DEVELOPMENT_BASE}/zbasetools
TOOLSET_INCLUDE = ${TOOLSET_ROOT}/include
TOOLSET_INC_ZBASE = ${TOOLSET_INCLUDE}/zbase
TOOLSET_INC_ZNET = ${TOOLSET_INCLUDE}/znet
TOOLSET_INC_ZCONTENT = ${TOOLSET_INCLUDE}/zcontent
TOOLSET_INC_ZAM = ${TOOLSET_INCLUDE}/zam
TOOLSET_INC_ZCPPPARSER = ${TOOLSET_INCLUDE}/zcppparser/zcppparser

TOOLSET_INC_ZCPPPARSER = ${TOOLSET_INCLUDE}/zcppparser/zcppparser

ZQT_ROOT = $${TOOLSET_ROOT}/zqt
ZCONTENT_ROOT = $${TOOLSET_ROOT}/zcontent
ZQT_WIDGET = $${ZQT_ROOT}/zqtwidget

# name of the root include directory

MODULE = ztestrawfile

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
LIBS += -L$${LIBRARY_REPOSITORY} -lzbase -lzcontent -lzcppparser

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

INCLUDEPATH +=/usr/include/x86_64-linux-gnu
INCLUDEPATH += $${TOOLSET_ROOT} \
                $${TOOLSET_INCLUDE}  \
                $${TOOLSET_INC_ZBASE} \
                $${TOOLSET_INC_ZNET} \
                $${TOOLSET_INC_ZCONTENT} \
                $${TOOLSET_INC_ZAM} \
                $${TOOLSET_INC_ZCPPPARSER} \
                $${OPENSSL_ROOT} \
                $${ZQT_ROOT} \
                $${ZQT_WIDGET} \
                $${ZCONTENT_ROOT}/zcontentutils \
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
    ../zcontentcommon/zidentity.cpp \
    ../zcontentcommon/zresource.cpp \
    ../zcontentutils/zentity.cpp \
    main.cpp \
    zdocphysical.cpp \
    zdocphysical_2keys.cpp \
    zdocphysical_nokey.cpp




HEADERS += \
    ../../zconfig_general.h \
    ../zcontentcommon/zresource.h \
    zdocphysical.h \
    zdocphysical_1key.h \
    zdocphysical_2keys.h \
    zdocphysical_nokey.h



#    ztypelistbutton.h \
#    ztypezqtrv.h




# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
