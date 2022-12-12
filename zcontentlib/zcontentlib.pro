#-------------------------------------------------
#
# Project created by QtCreator 2016-02-11T22:56:49
#
#-------------------------------------------------
#   DEVELOPMENTBASE (/home/gerard/Development)
#       |
#       |
#     TOOLSET_ROOT (zbasetools)
#       |
#       |
#       +---------------------------------------+-----------------------+-------------------+-------------------+
#       |                                       |                       |                   |                   |
#   TOOLSET_INCLUDE (include)                TOOLSET_LIB (bin)      TOOLSET_BASE(zbase)   TOOLSET_NET(znet)  TOOLSET_CONTENT(zcontent)
#   all library include files                libraries                  sources            sources              sources
#       |                                       |                       |                   |                   |
#       +--TOOLSET_INC_BASE (zbase)             +--TOOLSET_LIB_RELEASE  +--zbaselib         +--zconsole         +--zcontentlib
#       |                                       |   (release)           |                   |                   |
#       +--TOOLSET_INC_NET  (znet)              |                       +-- ztestzbaselib   ...                 +--ztest_zindexedfile
#       |                                       +--TOOLSET_LIB_DEBUGS   |                                       |
#       +--TOOLSET_INC_CONTENT (zcontent)           (debug)             ...                                     +--ztest_zrandomfile
#                                                                                                               |
#

PYTHON = python3.7

                                                                                                              ...
#QT += core gui xml
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += c++17 shared dll
TARGET = zcontent
DEFINES += ZCONTENT_LIBRARY

# mandatory : define home directory, where Development directory can be found

unix: HOME = /home/gerard

#------------Private to libraries build and install  (python utility <postlinkcopy.py>)-----------
#

# --------ztoolset common definitions------------

DEVELOPMENT_BASE = $${HOME}/Development
TOOLSET_ROOT = $${DEVELOPMENT_BASE}/zbasetools

MODULE = zcontent # name of the root include directory
INSTALL_ACTION = CLEANCOPY
PYTHON_PROC = $${TOOLSET_ROOT}/common/postlinkcopy.py

#defines DEVELOPMENT_BASE, TOOLSET symbols and ICU symbols

include ($$TOOLSET_ROOT/common/zbasecommon.pri)


# QMAKE_CXXFLAGS += -fpermissive

QMAKE_CXXFLAGS+= -std=c++17 -Wall -pthread

QMAKE_LFLAGS +=  -std=c++17 -lpthread

QMAKE_LFLAGS +=   -lssl
#in any cases
QMAKE_LFLAGS += -lcrypto -lcrypt
TEMPLATE = lib

unix:{
#INCLUDEPATH +=  /usr/local/include \
#                /usr/include/libxml++-2.6 \
#                /usr/include/glibmm-2.4 \
#                /usr/lib/x86_64-linux-gnu/glibmm-2.4/include \
#                /usr/include/glib-2.0 \
#                /usr/lib/x86_64-linux-gnu/glib-2.0/include

 INCLUDEPATH += $$TOOLSET_ROOT/include  \
                $$TOOLSET_INCLUDE/zbase \
                $$TOOLSET_INCLUDE/znet \
                $$ZTOOLSET_ROOT \
                $$DEVELOPMENT_ROOT \
                $$OPENSSL_ROOT \
                $$TOOLSET_CONTENT
# if using libxml2
  INCLUDEPATH +=  /usr/include/libxml2
  LIBS +=  -lxml2
} #unix:
#                /usr/include/x86_64-linux-gnu/sys \
#INCLUDEPATH +=      /usr/include          \
#                    /usr/local/include     \
#                    /usr/local/include/c++/5.3.0  \
#                $$TOOLSET_ROOT

HEADERS += \
    ../zcontentcommon/urfparser.h \
    ../zcontentcommon/zcontentconstants.h \
    ../zcontentcommon/zidentity.h \
    ../zcontentcommon/zlock.h \
    ../zcontentcommon/zoperation.h \
    ../zcontentcommon/zresource.h \
    ../zcontentcommon/zresult.h \
    ../zcontentcommon/zresult.h \
    ../zindexedfile/zdataconversion.h \
    ../zindexedfile/zdatatype.h \
    ../zindexedfile/zdictionaryfile.h \
    ../zindexedfile/zfielddescription.h \
    ../zindexedfile/zindexcollection.h \
    ../zindexedfile/zindexcontrolblock.h \
    ../zindexedfile/zindexdata.h \
    ../zindexedfile/zindexedfile_release.h \
    ../zindexedfile/zindexfield.h \
    ../zindexedfile/zindexitem.h \
    ../zindexedfile/zindextable.h \
    ../zindexedfile/zindextype.h \
    ../zindexedfile/zjournal.h \
    ../zindexedfile/zkeydictionary.h \
    ../zindexedfile/zmastercontrolblock.h \
    ../zindexedfile/zmasterfile_utilities.h \
    ../zindexedfile/zmasterfiletype.h \
    ../zindexedfile/zmetadic.h \
    ../zindexedfile/zmf_limits.h \
    ../zindexedfile/zmfdictionary.h \
    ../zindexedfile/zmfstats.h \
    ../zindexedfile/znaturalfromurf.h \
    ../zindexedfile/zrawindexfile.h \
    ../zindexedfile/zrawmasterfile.h \
    ../zindexedfile/zrawmasterfileutils.h \
    ../zindexedfile/zrawrecord.h \
#    ../zindexedfile/zsifgeneric.h \
    ../zindexedfile/zsjournalcontrolblock.h \
    ../zindexedfile/zurffromnatural.h \
    ../zrandomfile/zblock.h \
    ../zrandomfile/zfilecontrolblock.h \
    ../zrandomfile/zfiledescriptor.h \
    ../zrandomfile/zheadercontrolblock.h \
    ../zrandomfile/zrandomlockfile.h \
    ../zrandomfile/zrfcollection.h \
    ../zrandomfile/zrfpms.h \
    ../zrandomfile/zsearchargument.h \
    ../zrandomfile/zrandomfile.h \
    ../zrandomfile/zrandomfiletypes.h \
    ../zrandomfile/zrecordaccesstable.h \
    ../zrandomfile/zrfcollection.h \
    ../zrandomfile/zrfconfig.h \
    ../zindexedfile/zicm_todo_test.h \
# documentation
# Random file documentation
    ../zrandomfile/zrandomfile_doc.h \
    ../zrandomfile/zrandomfile_quickstart_doc.h \
# Master file and index file documentation
    ../zindexedfile/zmasterfile_quickstart_doc.h \
    ../zindexedfile/zmasterfile_doc.h \
    ../zindexedfile/zicmmain_doc.h \
    ../zindexedfile/zindexfile_doc.h \
    ../zindexedfile/zjournal_doc.h \                # Journalling
# configuration files
    zconfig.h \
    ../zindexedfile/zmfconfig.h

FURTHER_H =     ../zrandomfile/zrandomlockfile.h

SOURCES += \
    ../zcontentcommon/urfparser.cpp \
    ../zcontentcommon/zidentity.cpp \
    ../zcontentcommon/zlock.cpp \
    ../zcontentcommon/zoperation.cpp \
    ../zcontentcommon/zresource.cpp \
    ../zcontentcommon/zresult.cpp \
    ../zindexedfile/zdataconversion.cpp \
    ../zindexedfile/zdatatype.cpp \
    ../zindexedfile/zdictionaryfile.cpp \
    ../zindexedfile/zfielddescription.cpp \
    ../zindexedfile/zindexcollection.cpp \
    ../zindexedfile/zindexcontrolblock.cpp \
    ../zindexedfile/zindexfield.cpp \
    ../zindexedfile/zindexitem.cpp \
    ../zindexedfile/zindextable.cpp \
    ../zindexedfile/zindextype.cpp \
    ../zindexedfile/zjournal.cpp \
    ../zindexedfile/zkeydictionary.cpp \
    ../zindexedfile/zmastercontrolblock.cpp \
    ../zindexedfile/zmetadic.cpp \
    ../zindexedfile/zmfdictionary.cpp \
    ../zindexedfile/zmfstats.cpp \
    ../zindexedfile/znaturalfromurf.cpp \
    ../zindexedfile/zrawindexfile.cpp \
    ../zindexedfile/zrawmasterfile.cpp \
    ../zindexedfile/zrawrecord.cpp \
    ../zindexedfile/zsjournalcontrolblock.cpp \
    ../zindexedfile/zurffromnatural.cpp \
    ../zrandomfile/zblock.cpp \
    ../zrandomfile/zfilecontrolblock.cpp \
    ../zrandomfile/zfiledescriptor.cpp \
    ../zrandomfile/zheadercontrolblock.cpp \
    ../zrandomfile/zrandomfile.cpp \
    ../zrandomfile/zrandomlockfile.cpp \
    ../zrandomfile/zrfcollection.cpp \
    ../zrandomfile/zrfpms.cpp \
    ../zrandomfile/zsearchargument.cpp

FURTHER_CPP=         ../zrandomfile/zrandomlockfile.cpp

# ---------------Manage copy files to target ------------------

DISTFILES += \
    $$TOOLSET_ROOT/common/postlinkcopy.py \
    $$TOOLSET_ROOT/common/zbasecommon.pri


# need to resolve local files path : QMake runs in a different directory and needs absolute paths
HEADERS2PY=""
for(wH , HEADERS) : HEADERS2PY += $$absolute_path($${wH})

message(PROFILE Building library <$$TARGET> module <$$MODULE> for installing within <$$LIBRARY_REPOSITORY> from <$$DESTDIR>)

wLaunch="$${PYTHON} "$$quote($${PYTHON_PROC})
wLaunch+=" "
wLaunch+=$$INSTALL_ACTION
wLaunch+=" "
wLaunch+=$$quote($${MODULE})
wLaunch+=" "
wLaunch+="'$$quote($${TOOLSET_CONTENT})'"
wLaunch+=" "
wLaunch+="'$$quote($${TOOLSET_INCLUDE})'"
wLaunch+=" "
wLaunch+="'$$quote($${HEADERS2PY})'"
wLaunch+=" "
wLaunch+="'$$quote($${LIBRARY_REPOSITORY})'"
wLaunch+=" "
#wLaunch+="'$$quote(lib$${TARGET}.so)'"
wLaunch+="'$$quote($${TARGET})'"

QMAKE_POST_LINK +="$$wLaunch"
#system($$wLaunch)> from <$$DESTDIR>)

wLaunch="$${PYTHON} "$$quote($${PYTHON_PROC})
wLaunch+=" "
wLaunch+=$$INSTALL_ACTION
wLaunch+=" "
wLaunch+=$$quote($${MODULE})
wLaunch+=" "
wLaunch+="'$$quote($${TOOLSET_CONTENT})'"
wLaunch+=" "
wLaunch+="'$$quote($${TOOLSET_INCLUDE})'"
wLaunch+=" "
wLaunch+="'$$quote($${HEADERS2PY})'"
wLaunch+=" "
wLaunch+="'$$quote($${DESTINATION_LIB})'"
wLaunch+=" "
#wLaunch+="'$$quote(lib$${TARGET}.so)'"
wLaunch+="'$$quote($${TARGET})'"

QMAKE_POST_LINK +="$$wLaunch"
#system($$wLaunch)> from <$$DESTDIR>)

wLaunch="$${PYTHON} "$$quote($${PYTHON_PROC})
wLaunch+=" "
wLaunch+=$$INSTALL_ACTION
wLaunch+=" "
wLaunch+=$$quote($${MODULE})
wLaunch+=" "
wLaunch+="'$$quote($${TOOLSET_CONTENT})'"
wLaunch+=" "
wLaunch+="'$$quote($${TOOLSET_INCLUDE})'"
wLaunch+=" "
wLaunch+="'$$quote($${HEADERS2PY})'"
wLaunch+=" "
wLaunch+="'$$quote($${DESTINATION_LIB})'"
wLaunch+=" "
#wLaunch+="'$$quote(lib$${TARGET}.so)'"
wLaunch+="'$$quote($${TARGET})'"

QMAKE_POST_LINK +="$$wLaunch"
#system($$wLaunch)


