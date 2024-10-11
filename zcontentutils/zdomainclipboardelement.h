#ifndef ZDOMAINCLIPBOARDELEMENT_H
#define ZDOMAINCLIPBOARDELEMENT_H

#include <QStandardItem>
#include <QList>
#include <ztoolset/zarray.h>
#include <zcontent/zcontentcommon/zcontentobjectbroker.h>

class ZDomainClipboardElement {
public:
    ZDomainClipboardElement() ;
    ZDomainClipboardElement(ZDomainClipboardElement&)=delete;
    ~ZDomainClipboardElement()
    {
        clear();
    }

    void clear()
    {
        Row.clear();
        if (DP!=nullptr)
            delete DP;
        DP=nullptr;
    }

    /**
         * @brief copyDomainPath makes a copy of pDP domain path data to clipboard element. This copy NEEDS TO BE FREED later on.
         * @param pDP
         */
    void copyDomainPath(ZDomainPath* pDP)
    {
        DP=new ZDomainPath(*pDP);
        ToBeFreed=true;
    }
    /**
         * @brief setDomainPath copies the reference  of pDP domain path to clipboard element. This copy DOES NOT need to be freed.
         * @param pDP
         */
    void setDomainPath(ZDomainPath* pDP)
    {
        DP=pDP;
        ToBeFreed=false;
    }
    /**
         * @brief getDomainPath returns the reference of domain path.
         *  to clipboard element. This copy DOES NOT need to be freed.
         * @param pDP
         */
    ZDomainPath* getDomainPath()
    {
        ToBeFreed=false;
        return DP;
    }
    ZDomainPath* useDomainPath()
    {
        return DP;
    }
    QList<QStandardItem*>   Row;
    bool HasToBeFreed() { return ToBeFreed; }
private:
    ZDomainPath*            DP=nullptr;
    bool                    ToBeFreed=false;
};


class ZDomainClipboard : public ZArray<ZDomainClipboardElement*>
{
public:
    ZDomainClipboard()=default;
    ~ZDomainClipboard()
    {
        while (count())
            delete popR();
    }
};

#endif // ZDOMAINCLIPBOARDELEMENT_H
