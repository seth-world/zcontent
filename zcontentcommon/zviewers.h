#ifndef ZVIEWERS_H
#define ZVIEWERS_H

#include <ztoolset/zstatus.h>
#include <zcontent/zcontentcommon/zdomainpath.h>

#include <QWidget>

class ZViewers
{
public:
    ZViewers();
};

namespace zbs {


bool genericViewer (ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog);
void htmlViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog);
void textViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog);
bool imageViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog);
ZStatus externalViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog);
void erroredViewer(QWidget* pFather);

bool genericEditor (ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog);
ZStatus externalEditor(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog);

} // namespace zbs

#endif // ZVIEWERS_H
