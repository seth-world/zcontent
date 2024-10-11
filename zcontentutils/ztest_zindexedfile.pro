QT += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE += console
CONFIG += console c++17
TARGET = ztestindexfile

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



 INCLUDEPATH += $${TOOLSET_INCLUDE}  \
                $${TOOLSET_INC_ZBASE} \
                $${TOOLSET_INC_ZNET} \
                $${TOOLSET_INC_ZCONTENT} \
                $${TOOLSET_INC_ZAM} \
                $${OPENSSL_ROOT} \
                $${DEVELOPMENT_BASE}/zflow


#if using ICU
INCLUDEPATH +=  $${ICU_COMMON} \
                $${ICU_INCLUDE_BASE} \
                $${ICU_INCLUDE} \
                $${ICU_INCLUDEi18n}

# if using libxml2
INCLUDEPATH +=/usr/include/libxml2

# if using libxml++ (version 2.6)  : uncomment
#-----------------------------------
#INCLUDEPATH +=  /usr/include/libxml++-2.6 \
#                /usr/include/libxml++-2.6/libxml++ \
#                /usr/lib/libxml++-2.6/include \
#                /usr/include/glibmm-2.4 \
#                /usr/lib/x86_64-linux-gnu/glibmm-2.4/include


HEADERS_OLD += \
    zconfig.h \
    ../ztoolset/zarray.h \
    ../ztoolset/ztoolset_common.h \
    ../ztoolset/zerror.h \
    ../zio/zdir.h \
    ../zthread/zthread.h \
    ../zrandomfile/zrandomfiletypes.h \
    ../zrandomfile/zrandomfile.h \
    ../ztoolset/zexceptionmin.h \
    ../ztoolset/zbasedatatypes.h \
    ../zindexedfile/zifgeneric.h \
    ../zrandomfile/zrandomfile_doc.h \
    ../zindexedfile/zmasterfile_doc.h \
    ../zindexedfile/zindexfile_doc.h \
    ../ztoolset/ztoolset_doc.h \
    ../zindexedfile/zmasterfile_quickstart_doc.h \
    ../zindexedfile/zicm_todo_test.h \
    ../zindexedfile/zicmmain_doc.h \
    ../zindexedfile/zjournal.h \
    ../zxml/qxmlutilities.h \
    ../zrandomfile/zrfcollection.h \
    ../zrandomfile/zrandomfile_quickstart_doc.h \
    ../ztoolset/zdatabuffer.h \
    ../ztoolset/zdate.h \
    ../ztoolset/uristring.h \
    ../ztoolset/ztime.h \
    ../zremote/zrpc/zrpc.h \
 #   ../ztoolset/zstrings.h \  #renamed as zstrings_old.h
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
    ../zindexedfile/zmasterfile_utilities.h \
    ../zrandomfile/zssearchargument.h \
    ../ztoolset/templatewstring.h \
    ../ztoolset/zfunctions.h \
 #   ../ztoolset/templatestring.h \
#    ../ztoolset/zwstrings.h \
    ../ztoolset/zatomicconvert.h \
    ../zindexedfile/zurffromnatural.h \
    ../zindexedfile/znaturalfromurf.h \
    ../ztoolset/zmodulestack.h \
    ../ztoolset/zerror_min.h \
    ../ztoolset/zerror.h \
    ../zxml/zxmldoc.h \
    ../zxml/zxmlerror.h \
    ../zxml/zxmlnode.h \
    ../zxml/zxmlattribute.h \
    ../zxml/zxmlnamespace.h \
    ../zxml/zxml-documentation.h \
    ../zxml/zxml.h \
    ../ztoolset/zlimit.h \
    ../ztoolset/encodedString.h \
    ../ztoolset/zcharset.h \
    ../ztoolset/utftemplatestring.h \
    ../ztoolset/zutfstrings.h \
    ../ztoolset/zutfstrings.h \
    ../ztoolset/exceptionstring.h \
    ../ztoolset/utfvsprintf.h \
    ../ztoolset/utfutils.h \
    ../ztoolset/utfsprintf.h \
    ../ztoolset/utffixedstring.h \
    ../ztoolset/utfstringcommon.h \
    ../ztoolset/cfixedstring.h \
    ../ztoolset/utfvtemplatestring.h \
    ../ztoolset/utfvaryingstring.h \
    ../ztoolset/zutfstrings.h \
    ../ztoolset/charfixedstring.h \
    ../ztoolset/charvaryingstring.h \
    ../zcrypt/zcrypt.h \
    ../zcrypt/checksum.h \
    ../zcrypt/md5.h \
    ../ztoolset/zmem.h \
    ../ztoolset/utfkey.h \
    ../ztoolset/zicuconverter.h \
    ../ztoolset/utfconverter.h \
    ../ztoolset/utfstrctx.h \
    ../ztoolset/cnvcallback.h \
    ../ztoolset/zicu.h \
    ../ztoolset/zlocale.h \
    ../ztoolset/zsymbols.h \
    ../ztoolset/ztypetype.h
HEADERS += \
    ../../../zflow/common/zentity.h \
    ../../../zflow/common/zresourcecode.h \
    zconfig.h




SOURCES_OLD += main.cpp \
    ../ztoolset/zfunctions.cpp \   #  see zlibzbasesystem.a
    ../zio/zdir.cpp \
    ../ztoolset/zbasedatatypes.cpp \
    ../zrandomfile/zrandomfile.cpp \
    ../ztoolset/zexceptionmin.cpp \
    ../zxml/qxmlutilities.cpp \
    ../zrandomfile/zrfcollection.cpp \
    ../zthread/zthread.cpp \
    ../ztoolset/zdatabuffer.cpp \
    ../ztoolset/zdate.cpp \
    ../ztoolset/uristring.cpp \
    ../ztoolset/ztime.cpp \
    ../zremote/zrpc/zrpc.cpp \
#    ../ztoolset/zstrings.cpp \  #renamed as zstrings_old.cpp
    ../zremote/zserver.cpp \
    ../zremote/zbasenet/zsocket.cpp \
    ../zremote/znetcommon.cpp \
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
    ../zindexedfile/zmasterfile_utilities.cpp \
    ../zrandomfile/zssearchargument.cpp \
    ../ztoolset/templatewstring.cpp \
#    ../ztoolset/templatestring.cpp \
#    ../ztoolset/zwstrings.cpp \
    ../zindexedfile/zurffromnatural.cpp \
    ../zindexedfile/znaturalfromurf.cpp \
    ../ztoolset/zmodulestack.cpp \
#    ../ztoolset/zerror_min.cpp
    ../ztoolset/zerror_min.cpp \
    ../zxml/zxmldoc.cpp \
    ../zxml/zxmlerror.cpp \
    ../zxml/zxmlnode.cpp \
    ../zxml/zxmlattribute.cpp \
    ../zxml/zxmlnamespace.cpp \
    ../ztoolset/encodedString.cpp \
    ../ztoolset/zcharset.cpp \
    ../ztoolset/utftemplatestring.cpp \
    ../ztoolset/zutfstrings.cpp \
    ../ztoolset/utfsprintf.cpp \
    ../ztoolset/zatomicconvert.cpp \
    ../ztoolset/utfstringcommon.cpp \
    ../ztoolset/exceptionstring.cpp \
    ../ztoolset/utfvtemplatestring.cpp \
    ../ztoolset/utfvaryingstring.cpp \
    ../ztoolset/charvaryingstring.cpp \
    ../zcrypt/zcrypt.cpp \
    ../zcrypt/checksum.cpp \
    ../zcrypt/md5.cpp \
    ../ztoolset/zmem.cpp \
    ../ztoolset/utfkey.cpp \
    ../ztoolset/zicuconverter.cpp \
    ../ztoolset/utfconverter.cpp \
    ../ztoolset/utfstrctx.cpp \
    ../ztoolset/zlocale.cpp \
    ../ztoolset/ztypetype.cpp

SOURCES += main.cpp \
    ../../../zflow/common/zentity.cpp \
    ../../../zflow/common/zresourcecode.cpp



#for(var, $$list($$enumerate_vars())) {
#    message($$var)
#    message($$eval($$var))
#}


