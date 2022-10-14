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


OLDHEADERS += \
    zconfig.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zarray.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zsortedarray.h \
    $$ZBASESYSTEM_ROOT/ztoolset/ztoolset_common.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zerror.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zsaatomic.h \
    $$ZBASESYSTEM_ROOT/zio/zdir.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zsastring.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zsacommon.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zsafiltered.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zfilter.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zfind.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zsahelpers.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zsageneric.h\
    $$ZBASESYSTEM_ROOT/ztoolset/zjoin.h \
    $$ZBASESYSTEM_ROOT/zthread/zthread.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zam.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zjoindyn.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zjoinstatic.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zam_include.h \
    $$ZBASESYSTEM_ROOT/zcrypt/zcrypt.h \
    $$ZBASESYSTEM_ROOT/zmail/zmailcommon.h \
    $$ZBASESYSTEM_ROOT/zxml/qxmlutilities.h \
    $$ZBASESYSTEM_ROOT/ztoolset/ztypes.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zflowutility.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zlockmanager.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zexceptionmin.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zbasedatatypes.h \
#    $$ZBASESYSTEM_ROOT/treedialog.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zauthorization.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zstrings.h \
    $$ZBASESYSTEM_ROOT/ztoolset/ztemplatestring.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zdatabuffer.h \
    $$ZBASESYSTEM_ROOT/ztoolset/uristring.h \
    $$ZBASESYSTEM_ROOT/zthread/zarglist.h \
    $$ZBASESYSTEM_ROOT/zthread/zmutex.h \
    $$ZBASESYSTEM_ROOT/zthread/zmutex_pthread.h \
    $$ZBASESYSTEM_ROOT/zthread/zmutex_std.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zdate.h \
    $$ZBASESYSTEM_ROOT/ztoolset/cescapedstring.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zmodulestack.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zlimit.h \
    $$ZBASESYSTEM_ROOT/ztoolset/ZSystemUser.h \
    $$ZBASESYSTEM_ROOT/ztoolset/checksum.h \
    $$ZBASESYSTEM_ROOT/ztoolset/ztime.h \
    $$ZBASESYSTEM_ROOT/zthread/zthreadexithandler.h \
    $$ZBASESYSTEM_ROOT/ztoolset/zfunctions.h \
    ../zrandomfile/zrandomfile.h \
    ../zrandomfile/zrandomfile_doc.h \
    ../zrandomfile/zrandomfile_quickstart_doc.h \
    ../zrandomfile/zrandomfiletypes.h \
    ../zrandomfile/zrecordaccesstable.h \
    ../zrandomfile/zrfcollection.h \
    ../zrandomfile/zrfconfig.h \
    ../zrandomfile/zsearchargument.h \
    ../zindexedfile/zicm_todo_test.h \
    ../zindexedfile/zicmmain_doc.h \
    ../zindexedfile/zifgeneric.h \
    ../zindexedfile/zindexcollection.h \
    ../zindexedfile/zindexfile.h \
    ../zindexedfile/zindexfile_doc.h \
    ../zindexedfile/zindextype.h \
    ../zindexedfile/zjournal.h \
    ../zindexedfile/zjournal_doc.h \
    ../zindexedfile/zkey.h \
    ../zindexedfile/zmasterfile.h \
    ../zindexedfile/zmasterfile_doc.h \
    ../zindexedfile/zmasterfile_old.h \
    ../zindexedfile/zmasterfile_quickstart_doc.h \
    ../zindexedfile/zmfconfig.h \
    zconfig.h

HEADERS_OLD1 += \
    ../common/zcontentconstants.h \
    ../common/zidentity.h \
    ../common/zidentity.h \
    ../common/zlock.h \
    ../common/zlock.h \
    ../common/zresult.h \
    ../zindexedfile/zdatatype.h \
    ../zindexedfile/zmasterfile_utilities.h \
    ../zindexedfile/zmetadic.h \
    ../zindexedfile/zmf_limits.h \
    ../zindexedfile/znaturalfromurf.h \
    ../zindexedfile/znaturalfromurf_old.h \
    ../zindexedfile/zrecord.h \
    ../zindexedfile/zsifgeneric.h \
    ../zindexedfile/zsindexcollection.h \
    ../zindexedfile/zindexfile.h \
    ../zindexedfile/zsindextype.h \
    ../zindexedfile/zsjournal.h \
    ../zindexedfile/zskey.h \
    ../zindexedfile/zmasterfile.h \
    ../zindexedfile/zurffromnatural.h \
    ../zindexedfile/zurffromnatural_old.h \
    ../zrandomfile/zrandomlockfile.h \
    ../zrandomfile/zsrfcollection.h \
    ../zrandomfile/zssearchargument.h \
    ../zrandomfile/zrandomfile.h \
    ../zrandomfile/zrandomfile_doc.h \
    ../zrandomfile/zrandomfile_quickstart_doc.h \
    ../zrandomfile/zrandomfiletypes.h \
    ../zrandomfile/zrecordaccesstable.h \
    ../zrandomfile/zrfcollection.h \
    ../zrandomfile/zrfconfig.h \
    ../zrandomfile/zsearchargument.h \
    ../zindexedfile/zicm_todo_test.h \
    ../zindexedfile/zicmmain_doc.h \
#    ../zindexedfile/zifgeneric.h \
#    ../zindexedfile/zindexcollection.h \
#    ../zindexedfile/zindexfile.h \
#    ../zindexedfile/zindextype.h \
#    ../zindexedfile/zjournal.h \
#    ../zindexedfile/zkey.h \
#    ../zindexedfile/zmasterfile.h \
    ../zindexedfile/zindexfile_doc.h \
    ../zindexedfile/zmasterfile_doc.h \
    ../zindexedfile/zjournal_doc.h \
    ../zindexedfile/zmasterfile_quickstart_doc.h \
    zconfig.h \
    ../zindexedfile/zmfconfig.h

HEADERS += \
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
    ../zindexedfile/zfullindexfield.h \
    ../zindexedfile/zindexcontrolblock.h \
    ../zindexedfile/zindexdata.h \
    ../zindexedfile/zindexfield.h \
    ../zindexedfile/zindexfile.h \
    ../zindexedfile/zkeydictionary.h \
    ../zindexedfile/zmastercontrolblock.h \
    ../zindexedfile/zmasterfile.h \
    ../zindexedfile/zmasterfile_utilities.h \
    ../zindexedfile/zmasterfiletype.h \
    ../zindexedfile/zmetadic.h \
    ../zindexedfile/zmf_limits.h \
    ../zindexedfile/zmfdictionary.h \
    ../zindexedfile/zmfstats.h \
    ../zindexedfile/znaturalfromurf.h \
    ../zindexedfile/zrawindexfile.h \
    ../zindexedfile/zrawmasterfile.h \
    ../zindexedfile/zrawrecord.h \
    ../zindexedfile/zrecord.h \
#    ../zindexedfile/zsifgeneric.h \
    ../zindexedfile/zsindexcollection.h \
    ../zindexedfile/zsindexitem.h \
    ../zindexedfile/zsindextype.h \
    ../zindexedfile/zsjournal.h \
    ../zindexedfile/zsjournalcontrolblock.h \
    ../zindexedfile/zskey.h \
    ../zindexedfile/zurffromnatural.h \
    ../zrandomfile/zblock.h \
    ../zrandomfile/zfilecontrolblock.h \
    ../zrandomfile/zfiledescriptor.h \
    ../zrandomfile/zheadercontrolblock.h \
    ../zrandomfile/zrandomlockfile.h \
    ../zrandomfile/zrfpms.h \
    ../zrandomfile/zsrfcollection.h \
    ../zrandomfile/zssearchargument.h \
    ../zrandomfile/zrandomfile.h \
    ../zrandomfile/zrandomfiletypes.h \
    ../zrandomfile/zrecordaccesstable.h \
    ../zrandomfile/zrfcollection.h \
    ../zrandomfile/zrfconfig.h \
    ../zrandomfile/zsearchargument.h \
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

OLDSOURCE +=  \
    $$ZBASESYSTEM_ROOT/ztoolset/zfunctions.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/zam.cpp \
    $$ZBASESYSTEM_ROOT/zcrypt/zcrypt.cpp \
    $$ZBASESYSTEM_ROOT/zmail/zmailcommon.cpp \
    $$ZBASESYSTEM_ROOT/zxml/qxmlutilities.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/ztypes.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/zlockmanager.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/zexceptionmin.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/zbasedatatypes.cpp \
#    $$ZBASESYSTEM_ROOT/treedialog.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/zstrings.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/ztemplatestring.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/zdatabuffer.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/uristring.cpp \
    $$ZBASESYSTEM_ROOT/zthread/zmutex.cpp \
    $$ZBASESYSTEM_ROOT/zthread/zthread.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/zdate.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/cescapedstring.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/zmodulestack.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/ZSystemUser.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/zerror.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/checksum.cpp \
    $$ZBASESYSTEM_ROOT/zio/zdir.cpp \
    $$ZBASESYSTEM_ROOT/ztoolset/ztime.cpp \
    $$ZBASESYSTEM_ROOT/zthread/zthreadexithandler.cpp

SOURCES_OLD += \
    ../zcontentcommon/zlock.cpp \
    ../zindexedfile/zdatatype.cpp \
    ../zindexedfile/zmasterfile_utilities.cpp \
    ../zindexedfile/zmetadic.cpp \
    ../zindexedfile/znaturalfromurf.cpp \
    ../zindexedfile/znaturalfromurf_old.cpp \
    ../zindexedfile/zrecord.cpp \
    ../zindexedfile/zsindexcollection.cpp \
    ../zindexedfile/zindexfile.cpp \
    ../zindexedfile/zsindextype.cpp \
    ../zindexedfile/zsjournal.cpp \
    ../zindexedfile/zskey.cpp \
    ../zindexedfile/zsmasterfile.cpp \
    ../zindexedfile/zurffromnatural.cpp \
    ../zindexedfile/zurffromnatural_old.cpp \
    ../zrandomfile/zrandomfile.cpp \
    ../zrandomfile/zrandomlockfile.cpp \
    ../zrandomfile/zrfcollection.cpp \
    ../zrandomfile/zsearchargument.cpp \
    ../zindexedfile/zindexcollection.cpp \
    ../zindexedfile/zindexfile.cpp \
    ../zindexedfile/zindextype.cpp \
    ../zindexedfile/zjournal.cpp \
    ../zindexedfile/zkey.cpp \
    ../zindexedfile/zmasterfile.cpp \
    ../zrandomfile/zssearchargument.cpp
SOURCES += \
    ../zcontentcommon/zidentity.cpp \
    ../zcontentcommon/zlock.cpp \
    ../zcontentcommon/zoperation.cpp \
    ../zcontentcommon/zresource.cpp \
    ../zcontentcommon/zresult.cpp \
    ../zindexedfile/zdataconversion.cpp \
    ../zindexedfile/zdatatype.cpp \
    ../zindexedfile/zdictionaryfile.cpp \
    ../zindexedfile/zfielddescription.cpp \
    ../zindexedfile/zfullindexfield.cpp \
    ../zindexedfile/zindexcontrolblock.cpp \
    ../zindexedfile/zindexfield.cpp \
    ../zindexedfile/zindexfile.cpp \
    ../zindexedfile/zkeydictionary.cpp \
    ../zindexedfile/zmastercontrolblock.cpp \
    ../zindexedfile/zmasterfile.cpp \
    ../zindexedfile/zmasterfile_utilities.cpp \
    ../zindexedfile/zmetadic.cpp \
    ../zindexedfile/zmfdictionary.cpp \
    ../zindexedfile/zmfstats.cpp \
    ../zindexedfile/znaturalfromurf.cpp \
    ../zindexedfile/zrawindexfile.cpp \
    ../zindexedfile/zrawmasterfile.cpp \
    ../zindexedfile/zrawrecord.cpp \
    ../zindexedfile/zrecord.cpp \
    ../zindexedfile/zsindexcollection.cpp \
    ../zindexedfile/zsindexitem.cpp \
    ../zindexedfile/zsindextype.cpp \
    ../zindexedfile/zsjournal.cpp \
    ../zindexedfile/zsjournalcontrolblock.cpp \
    ../zindexedfile/zskey.cpp \
    ../zindexedfile/zurffromnatural.cpp \
    ../zrandomfile/zblock.cpp \
    ../zrandomfile/zfilecontrolblock.cpp \
    ../zrandomfile/zfiledescriptor.cpp \
    ../zrandomfile/zheadercontrolblock.cpp \
    ../zrandomfile/zrandomfile.cpp \
    ../zrandomfile/zrandomlockfile.cpp \
    ../zrandomfile/zrfcollection.cpp \
    ../zrandomfile/zrfpms.cpp \
    ../zrandomfile/zssearchargument.cpp

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


