QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

TARGET = zcontentutils



#------------Private to libraries build and install  (python utility <postlinkcopy.py>)-----------
#

HOME = /home/gerard

# --------ztoolset common definitions------------

DEVELOPMENT_BASE = $${HOME}/Development
TOOLSET_ROOT = $$DEVELOPMENT_BASE/zbasetools

# name of the root include directory

MODULE = zcontentutils

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

ZQT_ROOT = $${TOOLSET_ROOT}/zqt

ZQT_WIDGET = $${ZQT_ROOT}/zqtwidget

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
    ../../zqt/zqtwidget/zdatareference.cpp \
    ../../zqt/zqtwidget/zpinboard.cpp \
    ../../zqt/zqtwidget/zqlabel.cpp \
    ../../zqt/zqtwidget/zqlineedit.cpp \
    ../../zqt/zqtwidget/zqstandarditem.cpp \
    ../../zqt/zqtwidget/zqstandarditemmodel.cpp \
    ../../zqt/zqtwidget/zqtableview.cpp \
    ../../zqt/zqtwidget/zqtreeview.cpp \
    ../../zqt/zqtwidget/zqtwidgettools.cpp \
    ../../zqt/zqtwidget/zqtwidgettypes.cpp \
    ../zcontentcommon/zresource.cpp \
    ../zindexedfile/zfielddescription.cpp \
    dicedit.cpp \
    displaymain.cpp \
    main.cpp \
    texteditmwn.cpp \
    zcontentvisumain.cpp \
    zcu_common.cpp \
    zentity.cpp \
    zexceptiondlg.cpp \
    zfielddlg.cpp \
    zkeydlg.cpp \
    zlayout.cpp \
    zscan.cpp \
    ztypezqtrv.cpp

HEADERS += \
    ../../zconfig_general.h \
    ../../zqt/zqtwidget/zdatareference.h \
    ../../zqt/zqtwidget/zpinboard.h \
    ../../zqt/zqtwidget/zqlabel.h \
    ../../zqt/zqtwidget/zqlineedit.h \
    ../../zqt/zqtwidget/zqstandarditem.h \
    ../../zqt/zqtwidget/zqstandarditemmodel.h \
    ../../zqt/zqtwidget/zqtableview.h \
    ../../zqt/zqtwidget/zqtreeview.h \
    ../../zqt/zqtwidget/zqtwidgettools.h \
    ../../zqt/zqtwidget/zqtwidgettypes.h \
    ../zcontentcommon/zresource.h \
    ../zindexedfile/zfielddescription.h \
    dicedit.h \
    displaymain.h \
    texteditmwn.h \
    zcontentvisumain.h \
    zcu_common.h \
    zentity.h \
    zexceptiondlg.h \
    zfielddlg.h \
    zkeydlg.h \
    zlayout.h \
    zscan.h \
    ztypezqtrv.h

FORMS += \
    dicedit.ui \
#    dicedit_copy.ui \
#    dicedit_old.ui \
    displaymain.ui \
    texteditmwn.ui \
    texteditmwn_copy.ui \
    zcontentvisumain.ui \
    zexceptiondlg.ui \
    zexceptiondlg_old.ui \
    zfielddlg.ui \
    zfielddlg_copy.ui \
    zkeydlg.ui \
    zscan.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
