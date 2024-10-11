DEVELOPMENT_BASE = /home/gerard/Development
TOOLSET_BASE = /home/gerard/Development/zbasesystem

QT += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ztestrandomfile
CONFIG += console
#CONFIG += staticlib
#CONFIG -= app_bundle

CONFIG += c++14

# QMAKE_CXXFLAGS += -fpermissive

#
# Warning :
# 1- do not confuse -lcrypto and -lcrypt both libraries are necessary
# 2- lcrypt must be the last in library list
#

QMAKE_CXXFLAGS+= -std=c++14 -Wall -pthread

#if not using ldap-lssl
QMAKE_LFLAGS +=  -std=c++14 -lpthread
#if using ldap
QMAKE_LFLAGS +=  -lldap
#if using lssl
QMAKE_LFLAGS +=   -lssl
#in any cases
QMAKE_LFLAGS += -lcrypto -lcrypt

LIBS += -ldl -static-libstdc++
LIBS += /home/gerard/Development/zbasesystem/libzbs_debug/libzbasesystem.a
#LIBS += /home/gerard/Development/openssl-1.0.2d/libssl.a
#LIBS += /home/gerard/Development/openssl-1.0.2d/libcrypto.a
#LIBS += /usr/local/ssl/lib/libssl.a
#LIBS += /usr/local/ssl/lib/libcrypto.a



#TEMPLATE = lib
TEMPLATE = app

#INCLUDEPATH +=  /usr/local/include      /home/gerard/Development/zbasesystem
INCLUDEPATH +=  /usr/local/include \
                /usr/include/libxml++-2.6 \
                /usr/include/glibmm-2.4 \
                /usr/lib/x86_64-linux-gnu/glibmm-2.4/include \
                /usr/include/glib-2.0 \
                /usr/lib/x86_64-linux-gnu/glib-2.0/include \
                $$TOOLSET_BASE  \
                $$DEVELOPMENT_BASE

HEADERS += \
    zconfig.h \
    ../ztoolset/zarray.h \
    ../ztoolset/ztoolset_common.h \
    ../ztoolset/zerror.h \
    ../zio/zdir.h \
    ../zthread/zthread.h \
    ../zrandomfile/zrandomfiletypes.h \
    ../zrandomfile/zrandomfile.h \
    ../ztoolset/zexceptionmin.h \
    ../ztoolset/ztemplatestring.h \
    ../ztoolset/zbasedatatypes.h \
    ../zindexedfile/zifgeneric.h \
    ../zindexedfile/zmasterfile.h \
    ../zindexedfile/zindexfile.h \
    ../zindexedfile/zindextype.h \
    ../zrandomfile/zrandomfile_doc.h \
    ../zindexedfile/zmasterfile_doc.h \
    ../zindexedfile/zindexfile_doc.h \
    ../ztoolset/ztoolset_doc.h \
    ../zindexedfile/zkey.h \
    ../zindexedfile/zmasterfile_quickstart_doc.h \
    ../zindexedfile/zicm_todo_test.h \
    ../zindexedfile/zicmmain_doc.h \
    ../zindexedfile/zjournal.h \
    ../zxml/qxmlutilities.h \
    ../zrandomfile/zrfcollection.h \
    ../zrandomfile/zrandomfile_quickstart_doc.h \
    ../zindexedfile/zindexcollection.h \
    ../zrandomfile/zsearchargument.h \
    ../zindexedfile/zjournal_doc.h \
    ../ztoolset/zdatabuffer.h \
    ../ztoolset/zdate.h \
    ../ztoolset/uristring.h \
    ../ztoolset/ztime.h \
    ../ztoolset/checksum.h \
    ../zremote/zrpc/zrpc.h \
    ../ztoolset/zstrings.h \
    ../zremote/zserver.h \
    ../zremote/zbasenet/zsocket.h \
    ../zremote/znetcommon.h \
    ../zremote/zconfig_network.h \
    ../zremote/zbasenet/zsslfunctions.h \
    ../zremote/zbasenet/zclientsocket.h \
    ../zremote/zbasenet/zsslbase.h \
    ../zremote/zbasenet/zsslclientsocket.h \
    ../zconfig_general.h \
    ../zthread/zmutex.h \
    ../ztoolset/cescapedstring.h \
    ../ztoolset/ztimer.h \
    ../zthread/zthreadexithandler.h \
    ../ztoolset/ztokenizer.h \
    ../ztoolset/zuser.h \
    ../zindexedfile/zsmasterfile.h \
    ../zindexedfile/zmetadic.h \
    ../zindexedfile/zdatatype.h \
    ../zindexedfile/zrecord.h \
    ../zindexedfile/zmf_limits.h \
    ../zindexedfile/zsindextype.h \
    ../zindexedfile/zsifgeneric.h \
    ../zindexedfile/zsindexfile.h \
    ../zindexedfile/zskey.h \
    ../zindexedfile/zsjournal.h \
    ../ztoolset/zbitset.h \
    ../zindexedfile/zmasterfile_utilities.h


SOURCES += main.cpp \
    ../ztoolset/zfunctions.cpp \
    ../zio/zdir.cpp \
    ../ztoolset/zbasedatatypes.cpp \
    ../zrandomfile/zrandomfile.cpp \
    ../ztoolset/zexceptionmin.cpp \
    ../zindexedfile/zmasterfile.cpp \
    ../zindexedfile/zindexfile.cpp \
    ../zindexedfile/zindextype.cpp \
    ../zindexedfile/zkey.cpp \
    ../zindexedfile/zjournal.cpp \
    ../zxml/qxmlutilities.cpp \
    ../zrandomfile/zrfcollection.cpp \
    ../zindexedfile/zindexcollection.cpp \
    ../zrandomfile/zsearchargument.cpp \
    ../zthread/zthread.cpp \
    ../ztoolset/zdatabuffer.cpp \
    ../ztoolset/zdate.cpp \
    ../ztoolset/uristring.cpp \
    ../ztoolset/ztemplatestring.cpp \
    ../ztoolset/ztime.cpp \
    ../ztoolset/checksum.cpp \
    ../zremote/zrpc/zrpc.cpp \
    ../ztoolset/zstrings.cpp \
    ../zremote/zserver.cpp \
    ../zremote/zbasenet/zsocket.cpp \
    ../zremote/znetcommon.cpp \
    ../ztoolset/zerror.cpp \
    ../zremote/zbasenet/zsslfunctions.cpp \
    ../zremote/zbasenet/zclientsocket.cpp \
    ../zremote/zbasenet/zsslbase.cpp \
    ../zremote/zbasenet/zsslclientsocket.cpp \
    ../ztoolset/cescapedstring.cpp \
    ../zthread/zmutex.cpp \
    ../ztoolset/ztimer.cpp \
    ../zthread/zthreadexithandler.cpp \
    ../ztoolset/ztokenizer.cpp \
    ../ztoolset/zuser.cpp \
    ../zindexedfile/zsmasterfile.cpp \
    ../zindexedfile/zmetadic.cpp \
    ../zindexedfile/zdatatype.cpp \
    ../zindexedfile/zrecord.cpp \
    ../zindexedfile/zsindextype.cpp \
    ../zindexedfile/zsindexcollection.cpp \
    ../zindexedfile/zsindexfile.cpp \
    ../zindexedfile/zskey.cpp \
    ../zindexedfile/zsjournal.cpp \
    ../ztoolset/zbitset.cpp \
    ../zindexedfile/zmasterfile_utilities.cpp
