#-------------------------------------------------
#
# Project created by QtCreator 2016-02-11T22:56:49
# required libraries
# sudo apt-get install build-essential # compiler and debugger
# sudo apt-get install clang-tools-14
# sudo apt-get install python3-dev
# sudo apt-get install python3.11-dev -y  # or the latest version of python (replace 3.11 with appropriate tag)
# sudo apt-get install python-is-python3 -y # to make python3 recognized as python instruction
# sudo apt-get -y install libssl-dev
# sudo apt-get -y install libicu-dev
# sudo apt-get -y install libxml2-dev

# OpenGL suite
# sudo apt-get install libopengl-dev -y
# sudo apt-get install mesa-common-dev -y
# sudo apt-get install libglu1-mesa-dev -y
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



                                                                                                              ...
#QT += core gui xml
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

greaterThan(QT_MAJOR_VERSION,5): QT += core5compat

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

PYTHON = python
PYTHON_INSTALL_ACTION = CLEANCOPY
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
 INCLUDEPATH += /usr/include/x86_64-linux-gnu
 INCLUDEPATH += $$TOOLSET_ROOT/include  \
                $$TOOLSET_CONTENT\
                $$TOOLSET_INCLUDE/zbase \
                $$TOOLSET_INCLUDE/znet \
                $$TOOLSET_ROOT \
                $$TOOLSET_BASE \
                $$DEVELOPMENT_ROOT \
                $$OPENSSL_ROOT

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
    $${TOOLSET_ROOT}/config/zconfig_general.h \
    $${TOOLSET_ROOT}/config/zconfig.h \
    $$TOOLSET_CONTENT/zcontentcommon/urffield.h \
    $$TOOLSET_CONTENT/zcontentcommon/urfparser.h \
    $$TOOLSET_CONTENT/zcontentcommon/zcontentconstants.h \
    $$TOOLSET_CONTENT/zcontentcommon/zidentity.h \
    $$TOOLSET_CONTENT/zcontentcommon/zlock.h \
    $$TOOLSET_CONTENT/zcontentcommon/zoperation.h \
    $$TOOLSET_CONTENT/zcontentcommon/zresource.h \
    $$TOOLSET_CONTENT/zcontentcommon/zresult.h \
    $$TOOLSET_CONTENT/zcontentcommon/zresult.h \
    $$TOOLSET_CONTENT/zcontentutils/zentity.h \
    $$TOOLSET_CONTENT/zindexedfile/bckelement.h \
    $$TOOLSET_CONTENT/zindexedfile/zdataconversion.h \
    $$TOOLSET_CONTENT/zindexedfile/zdatatype.h \
    $$TOOLSET_CONTENT/zindexedfile/zdictionaryfile.h \
    $$TOOLSET_CONTENT/zindexedfile/zfielddescription.h \
    $$TOOLSET_CONTENT/zindexedfile/zindexcollection.h \
    $$TOOLSET_CONTENT/zindexedfile/zindexcontrolblock.h \
    $$TOOLSET_CONTENT/zindexedfile/zindexdata.h \
    $$TOOLSET_CONTENT/zindexedfile/zindexedfile_release.h \
    $$TOOLSET_CONTENT/zindexedfile/zindexfield.h \
    $$TOOLSET_CONTENT/zindexedfile/zindexitem.h \
    $$TOOLSET_CONTENT/zindexedfile/zindextable.h \
    $$TOOLSET_CONTENT/zindexedfile/zindextype.h \
    $$TOOLSET_CONTENT/zindexedfile/zjournal.h \
    $$TOOLSET_CONTENT/zindexedfile/zkeydictionary.h \
    $$TOOLSET_CONTENT/zindexedfile/zmastercontrolblock.h \
    $$TOOLSET_CONTENT/zindexedfile/zmasterfile.h \
    $$TOOLSET_CONTENT/zindexedfile/zmasterfile_utilities.h \
    $$TOOLSET_CONTENT/zindexedfile/zmasterfiletype.h \
    $$TOOLSET_CONTENT/zindexedfile/zmetadic.h \
    $$TOOLSET_CONTENT/zindexedfile/zmf_limits.h \
    $$TOOLSET_CONTENT/zindexedfile/zmfdictionary.h \
    $$TOOLSET_CONTENT/zindexedfile/zmfstats.h \
    $$TOOLSET_CONTENT/zindexedfile/znaturalfromurf.h \
    $$TOOLSET_CONTENT/zindexedfile/zrawindexfile.h \
    $$TOOLSET_CONTENT/zindexedfile/zrawmasterfile.h \
    $$TOOLSET_CONTENT/zindexedfile/zrawmasterfileutils.h \
    $$TOOLSET_CONTENT/zindexedfile/zrawrecord.h \
#    ../zindexedfile/zsifgeneric.h \
    $$TOOLSET_CONTENT/zindexedfile/zsjournalcontrolblock.h \
    $$TOOLSET_CONTENT/zindexedfile/zurffromnatural.h \
    $$TOOLSET_CONTENT/zrandomfile/zblock.h \
    $$TOOLSET_CONTENT/zrandomfile/zblockpool.h \
    $$TOOLSET_CONTENT/zrandomfile/zfilecontrolblock.h \
    $$TOOLSET_CONTENT/zrandomfile/zfiledescriptor.h \
    $$TOOLSET_CONTENT/zrandomfile/zheadercontrolblock.h \
    $$TOOLSET_CONTENT/zrandomfile/zrandomlockfile.h \
    $$TOOLSET_CONTENT/zrandomfile/zrfcollection.h \
    $$TOOLSET_CONTENT/zrandomfile/zrfpms.h \
    $$TOOLSET_CONTENT/zrandomfile/zrfutilities.h \
    $$TOOLSET_CONTENT/zrandomfile/zsearchargument.h \
    $$TOOLSET_CONTENT/zrandomfile/zrandomfile.h \
    $$TOOLSET_CONTENT/zrandomfile/zrandomfiletypes.h \
    $$TOOLSET_CONTENT/zrandomfile/zrecordaccesstable.h \
    $$TOOLSET_CONTENT/zrandomfile/zrfcollection.h \
    $$TOOLSET_CONTENT/zindexedfile/zicm_todo_test.h \
# documentation
# Random file documentation
    $$TOOLSET_CONTENT/zrandomfile/zrandomfile_doc.h \
    $$TOOLSET_CONTENT/zrandomfile/zrandomfile_quickstart_doc.h \
# Master file and index file documentation
    $$TOOLSET_CONTENT/zindexedfile/zmasterfile_quickstart_doc.h \
    $$TOOLSET_CONTENT/zindexedfile/zmasterfile_doc.h \
    $$TOOLSET_CONTENT/zindexedfile/zicmmain_doc.h \
    $$TOOLSET_CONTENT/zindexedfile/zindexfile_doc.h \
    $$TOOLSET_CONTENT/zindexedfile/zjournal_doc.h \                # Journalling
# configuration files
    $$TOOLSET_CONTENT/zindexedfile/zmfconfig.h \
    $$TOOLSET_BASE/config/zconfig.h \
    $$TOOLSET_BASE/config/zconfig_general.h \
    $$TOOLSET_BASE/config/zconfig_zrf.h \
    ../../config/zconfig.h \
#    ../zcontentcommon/zgeneralparameters.h \
    ../zcontentcommon/zgeneralparameters.h \
#    ../zindexedfile/zcollectionentity.h \
    ../zindexedfile/zsearch.h \
    ../zindexedfile/zsearcharithmeticterm.h \
    ../zindexedfile/zsearchcontext.h \
    ../zindexedfile/zsearchdictionary.h \
    ../zindexedfile/zsearchentity.h \
    ../zindexedfile/zsearchentitycontext.h \
    ../zindexedfile/zsearchfield.h \
    ../zindexedfile/zsearchfileentity.h \
    ../zindexedfile/zsearchhistory.h \
    ../zindexedfile/zsearchlogicalterm.h \
    ../zindexedfile/zsearchoperand.h \
    ../zindexedfile/zsearchparser.h \
    ../zindexedfile/zsearchparsertype.h \
    ../zindexedfile/zsearchsymbol.h \
    ../zindexedfile/zsearchtoken.h \
    ../zindexedfile/zsearchtokenizer.h \
    ../zindexedfile/zselclause.h

FURTHER_H =    $$TOOLSET_CONTENT/zrandomfile/zrandomlockfile.h

SOURCES += \
    $$TOOLSET_CONTENT/zcontentcommon/urffield.cpp \
    $$TOOLSET_CONTENT/zcontentcommon/urfparser.cpp \
    $$TOOLSET_CONTENT/zcontentcommon/zidentity.cpp \
    $$TOOLSET_CONTENT/zcontentcommon/zlock.cpp \
    $$TOOLSET_CONTENT/zcontentcommon/zoperation.cpp \
    $$TOOLSET_CONTENT/zcontentcommon/zresource.cpp \
    $$TOOLSET_CONTENT/zcontentcommon/zresult.cpp \
    $$TOOLSET_CONTENT/zcontentutils/zentity.cpp \
    $$TOOLSET_CONTENT/zindexedfile/bckelement.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zdataconversion.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zdatatype.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zdictionaryfile.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zfielddescription.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zindexcollection.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zindexcontrolblock.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zindexfield.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zindexitem.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zindextable.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zindextype.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zjournal.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zkeydictionary.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zmastercontrolblock.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zmasterfile.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zmetadic.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zmfdictionary.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zmfstats.cpp \
    $$TOOLSET_CONTENT/zindexedfile/znaturalfromurf.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zrawindexfile.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zrawmasterfile.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zrawmasterfileutils.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zrawrecord.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zsjournalcontrolblock.cpp \
    $$TOOLSET_CONTENT/zindexedfile/zurffromnatural.cpp \
    $$TOOLSET_CONTENT/zrandomfile/zblock.cpp \
    $$TOOLSET_CONTENT/zrandomfile/zblockpool.cpp \
    $$TOOLSET_CONTENT/zrandomfile/zfilecontrolblock.cpp \
    $$TOOLSET_CONTENT/zrandomfile/zfiledescriptor.cpp \
    $$TOOLSET_CONTENT/zrandomfile/zheadercontrolblock.cpp \
    $$TOOLSET_CONTENT/zrandomfile/zrandomfile.cpp \
    $$TOOLSET_CONTENT/zrandomfile/zrandomlockfile.cpp \
    $$TOOLSET_CONTENT/zrandomfile/zrfcollection.cpp \
    $$TOOLSET_CONTENT/zrandomfile/zrfpms.cpp \
    $$TOOLSET_CONTENT/zrandomfile/zrfutilities.cpp \
    $$TOOLSET_CONTENT/zrandomfile/zsearchargument.cpp \
    ../zcontentcommon/zcontentconstants.cpp \
#    ../zcontentcommon/zgeneralparameters.cpp \
    ../zcontentcommon/zgeneralparameters.cpp \
#    ../zindexedfile/zcollectionentity.cpp \
    ../zindexedfile/zsearch.cpp \
    ../zindexedfile/zsearcharithmeticterm.cpp \
    ../zindexedfile/zsearchcontext.cpp \
    ../zindexedfile/zsearchdictionary.cpp \
    ../zindexedfile/zsearchentity.cpp \
    ../zindexedfile/zsearchentitycontext.cpp \
    ../zindexedfile/zsearchfield.cpp \
    ../zindexedfile/zsearchfileentity.cpp \
    ../zindexedfile/zsearchhistory.cpp \
    ../zindexedfile/zsearchlogicalterm.cpp \
    ../zindexedfile/zsearchoperand.cpp \
    ../zindexedfile/zsearchparser.cpp \
    ../zindexedfile/zsearchparsertype.cpp \
    ../zindexedfile/zsearchsymbol.cpp \
    ../zindexedfile/zsearchtoken.cpp \
    ../zindexedfile/zsearchtokenizer.cpp \
    ../zindexedfile/zselclause.cpp

FURTHER_CPP=         $$TOOLSET_CONTENT/zrandomfile/zrandomlockfile.cpp

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
wLaunch+=$$PYTHON_INSTALL_ACTION
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
wLaunch+=$$PYTHON_INSTALL_ACTION
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
wLaunch+=$$PYTHON_INSTALL_ACTION
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
#message(post link $$QMAKE_POST_LINK )

