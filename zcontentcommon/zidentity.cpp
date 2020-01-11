#include <zcontentcommon/zidentity.h>
#include <ztoolset/charman.h>

#ifdef QT_CORE_LIB
QString     ZIdentity::toQString(void)
{
    char Buffer[15];
    sprintf(Buffer,"%06ld",id);
    return(QString(Buffer));
}
QString     ZIdentity::toQStringShort(void)
{
    char Buffer[15];
    sprintf(Buffer,"%06ld",id);
    return(QString(Buffer));
}
QString     ZIdentity::toQStringLong(void)
{
    char Buffer[15];
    sprintf(Buffer,"%012ld",id);
    return(QString(Buffer));
}

ZIdentity& ZIdentity::fromQString (const QString pInput)
{
    bool ConvError;
    id = pInput.toInt(&ConvError);
    if (ConvError)
        fprintf(stderr,"%s-E-CONVERROR There were error converting QString %s to Docid_struct\n",
                _GET_FUNCTION_NAME_,
                pInput.toStdString().c_str());
    return(*this);
}
#endif // QT_CORE_LIB
CharMan ZIdentity::toStr()
{
    CharMan wId;
    sprintf(wId.content, "%ld", id);
    return wId;
}
CharMan ZIdentity::toHexa()
{
    CharMan wId;
    sprintf(wId.content, "%lXd", id);
    return wId;
}
