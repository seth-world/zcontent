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

MODULE = zcontentutils

#defines DEVELOPMENT_BASE, TOOLSET symbols and ICU symbols

include ($$TOOLSET_ROOT/common/zbasecommon.pri)


# QMAKE_CXXFLAGS += -fpermissive

QMAKE_CXXFLAGS+= -std=c17 -Wall -pthread

QMAKE_LFLAGS +=  -std=c17 -lpthread

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

INCLUDEPATH +=  $${OPENSSL_BASE} \
                $${OPENSSL_INCLUDEPATH} \
                $${TOOLSET_ROOT} \
                $${TOOLSET_INCLUDE}  \
                $${TOOLSET_INC_ZBASE} \
                $${TOOLSET_INC_ZNET} \
                $${TOOLSET_INC_ZCONTENT} \
                $${TOOLSET_INC_ZAM} \
                $${TOOLSET_INC_ZCPPPARSER} \
                $${ZQT_ROOT} \
                $${ZQT_WIDGET} \
                $${ZCONTENT_ROOT}/zcontentutils

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
    $${ZQT_WIDGET}/zdatareference.cpp \
    $${ZQT_WIDGET}/zpinboard.cpp \
    $${ZQT_WIDGET}/zqlabel.cpp \
    $${ZQT_WIDGET}/zqlineedit.cpp \
    $${ZQT_WIDGET}/zqstandarditem.cpp \
    $${ZQT_WIDGET}/zqstandarditemmodel.cpp \
    $${ZQT_WIDGET}/zqtableview.cpp \
    $${ZQT_WIDGET}/zqtreeview.cpp \
    $${ZQT_WIDGET}/zqtwidgettools.cpp \
    $${ZQT_WIDGET}/zqtwidgettypes.cpp \
    $${ZQT_WIDGET}/zqplaintextedit.cpp \
#    $${ZQT_WIDGET}/zqpixlabelchecked.cpp \
    ../../zcppparser/zcppparsertype.cpp \
    ../../zqt/zqtwidget/zqgraphicscene.cpp \
    ../../zqt/zqtwidget/zqrectangle.cpp \
    ../zcontentcommon/urffield.cpp \
    ../zcontentcommon/urfparser.cpp \
\
    $${ZCONTENT_ROOT}/zcontentcommon/zresource.cpp \
    $${ZCONTENT_ROOT}/zindexedfile/zfielddescription.cpp \
\
    $${ZCONTENT_ROOT}/zcontentutils/dicedit.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/displaymain.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zcu_common.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zdisplayedfield.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zparsingrules.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zparsingrulesdlg.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zstdlistdlg.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/ztypedlg.cpp \
\
    $${TOOLSET_ROOT}/zcppparser/zcppparserutils/rawfields.cpp \
#    ../../zcppparser/zcppparserutils/rawfields.cpp \
#/home/gerard/Development/zbasetools/zcppparser/zcppparserutils/rawfields.h
    $${ZCONTENT_ROOT}/zcontentutils/texteditmwn.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zcontentvisumain.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zentity.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zexceptiondlg.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zfielddlg.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zkeydlg.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zlayout.cpp \
    $${ZCONTENT_ROOT}/zcontentutils/zscan.cpp \
    bckelement.cpp \
    editblockdlg.cpp \
    filegeneratedlg.cpp \
    keydata.cpp \
    main.cpp \
    masterfilevalues.cpp \
    poolvisu.cpp \
    visulinecol.cpp \
    visuraw.cpp \
    zbackupdlg.cpp \
    zcellformatdlg.cpp \
    zchangerecord.cpp \
    zcollectionlist.cpp \
    zcppgenerate.cpp \
    zfilegenerate.cpp \
    zkeylistdlg.cpp \
    zrawkeylistdlg.cpp \
    zrawmasterfilevisu.cpp \
    zrestoredlg.cpp \
    zsearchquerymwd.cpp



HEADERS += \
    ../../zconfig_general.h \
    $${ZQT_WIDGET}/zdatareference.h \
    $${ZQT_WIDGET}/zpinboard.h \
    $${ZQT_WIDGET}/zqlabel.h \
    $${ZQT_WIDGET}/zqlineedit.h \
    $${ZQT_WIDGET}/zqstandarditem.h \
    $${ZQT_WIDGET}/zqstandarditemmodel.h \
    $${ZQT_WIDGET}/zqtableview.h \
    $${ZQT_WIDGET}/zqtreeview.h \
    $${ZQT_WIDGET}/zqtwidgettools.h \
    $${ZQT_WIDGET}/zqtwidgettypes.h \
    $${ZQT_WIDGET}/zqplaintextedit.h \
#    $${ZQT_WIDGET}/zqpixlabelchecked.h \
    ../../zcppparser/zcppparsertype.h \
    ../../zqt/zqtwidget/zqgraphicscene.h \
    ../../zqt/zqtwidget/zqrectangle.h \
    ../../zqt/zqtwidget/zqtutils.h \
    ../../zqt/zqtwidget/zqtwidgets_release.h \
    ../zcontentcommon/urffield.h \
    ../zcontentcommon/urfparser.h \
\
    $${ZCONTENT_ROOT}/zcontentcommon/zresource.h \
    $${ZCONTENT_ROOT}/zindexedfile/zfielddescription.h \
\
    $${ZCONTENT_ROOT}/zcontentutils/dicedit.h \
    $${ZCONTENT_ROOT}/zcontentutils/displaymain.h \
    $${ZCONTENT_ROOT}/zcontentutils/zcu_common.h \
    $${ZCONTENT_ROOT}/zcontentutils/zcontentutilsparams.h \
    $${ZCONTENT_ROOT}/zcontentutils/zdisplayedfield.h \
    $${ZCONTENT_ROOT}/zcontentutils/zparsingrules.h \
    $${ZCONTENT_ROOT}/zcontentutils/zparsingrulesdlg.h \
    $${ZCONTENT_ROOT}/zcontentutils/zstdlistdlg.h \
    $${ZCONTENT_ROOT}/zcontentutils/ztypedlg.h \
    $${ZCONTENT_ROOT}/zcontentutils/texteditmwn.h \
    $${ZCONTENT_ROOT}/zcontentutils/zcontentvisumain.h \
    $${ZCONTENT_ROOT}/zcontentutils/zentity.h \
    $${ZCONTENT_ROOT}/zcontentutils/zexceptiondlg.h \
    $${ZCONTENT_ROOT}/zcontentutils/zfielddlg.h \
    $${ZCONTENT_ROOT}/zcontentutils/zkeydlg.h \
    $${ZCONTENT_ROOT}/zcontentutils/zlayout.h \
    $${ZCONTENT_ROOT}/zcontentutils/zscan.h \
\
    $${TOOLSET_ROOT}/zcppparser/zcppparserutils/rawfields.h \
    bckelement.h \
    editblockdlg.h \
    filegeneratedlg.h \
    keydata.h \
    masterfilevalues.h \
    poolvisu.h \
    visulinecol.h \
    visuraw.h \
    zbackupdlg.h \
    zcellformatdlg.h \
    zchangerecord.h \
    zcollectionlist.h \
    zcontentutils_release.h \
    zcppgenerate.h \
    zfilegenerate.h \
    zkeylistdlg.h \
    zrawkeylistdlg.h \
    zrawmasterfilevisu.h \
    zrestoredlg.h \
    zsearchquerymwd.h


#    ztypelistbutton.h \
#    ztypezqtrv.h



FORMS += \
#    ../../zcppparser/zcppparserutils/rawfields.ui \
    $${ZCONTENT_ROOT}/zcontentutils/displaymain.ui \
    $${ZCONTENT_ROOT}/zcontentutils/dicedit.ui \
    $${ZCONTENT_ROOT}/zcontentutils/zparsingrules.ui \
    $${ZCONTENT_ROOT}/zcontentutils/zparsingrulesdlg.ui \
    $${ZCONTENT_ROOT}/zcontentutils/texteditmwn.ui \
    $${ZCONTENT_ROOT}/zcontentutils/zcontentvisumain.ui \
    $${ZCONTENT_ROOT}/zcontentutils/zexceptiondlg.ui \
    $${ZCONTENT_ROOT}/zcontentutils/zfielddlg.ui \
    $${ZCONTENT_ROOT}/zcontentutils/zkeydlg.ui \
    $${ZCONTENT_ROOT}/zcontentutils/zscan.ui \
    ../../zcppparser/zcppparserutils/rawfields.ui \
    filegeneratedlg.ui \
    poolvisu.ui \
    poolvisuDLg.ui \
    zcellformatdlg.ui \
    zcollectionlist.ui \
    zrawmasterfilevisu.ui \
    zsearchquerymwd.ui
    $${TOOLSET_ROOT}/zcppparser/zcppparserutils/rawfields.ui


OLDHEADERS += \
    ../../zconfig_general.h \
    $${ZQT_WIDGET}/zdatareference.h \
    $${ZQT_WIDGET}/zpinboard.h \
    $${ZQT_WIDGET}/zqlabel.h \
    $${ZQT_WIDGET}/zqlineedit.h \
    $${ZQT_WIDGET}/zqstandarditem.h \
    $${ZQT_WIDGET}/zqstandarditemmodel.h \
    $${ZQT_WIDGET}/zqtableview.h \
    $${ZQT_WIDGET}/zqtreeview.h \
    $${ZQT_WIDGET}/zqtwidgettools.h \
    $${ZQT_WIDGET}/zqtwidgettypes.h \
    $${ZQT_WIDGET}/zqplaintextedit.h \
    $${ZCONTENT_ROOT}/zcontentcommon/zresource.h \
    $${ZCONTENT_ROOT}/zindexedfile/zfielddescription.h \
    $${ZCONTENT_ROOT}/zcontentutils/dicedit.h \
    $${ZCONTENT_ROOT}/zcontentutils/displaymain.h \
    $${ZCONTENT_ROOT}/zcontentutils/zcu_common.h \
    $${ZCONTENT_ROOT}/zcontentutils/zcontentutilsparams.h \
    $${ZCONTENT_ROOT}/zcontentutils/zdisplayedfield.h \
    $${ZCONTENT_ROOT}/zcontentutils/zparsingrules.h \
    $${ZCONTENT_ROOT}/zcontentutils/zparsingrulesdlg.h \
    $${ZCONTENT_ROOT}/zcontentutils/zstdlistdlg.h \
    $${ZCONTENT_ROOT}/zcontentutils/ztypedlg.h \
    rawfields.h \
    $${ZCONTENT_ROOT}/zcontentutils/texteditmwn.h \
    $${ZCONTENT_ROOT}/zcontentutils/zcontentvisumain.h \
    $${ZCONTENT_ROOT}/zcontentutils/zentity.h \
    $${ZCONTENT_ROOT}/zcontentutils/zexceptiondlg.h \
    $${ZCONTENT_ROOT}/zcontentutils/zfielddlg.h \
    $${ZCONTENT_ROOT}/zcontentutils/zkeydlg.h \
    $${ZCONTENT_ROOT}/zcontentutils/zlayout.h \
    $${ZCONTENT_ROOT}/zcontentutils/zscan.h




# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../zqt/icons/backward.gif \
    ../../zqt/icons/forward.gif \
    ../../zqt/icons/tobegin.gif \
    ../../zqt/icons/toend.gif
