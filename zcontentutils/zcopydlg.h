#ifndef ZCOPYDLG_H
#define ZCOPYDLG_H

#include <QDialog>
#include <stdint.h>

#include <ztoolset/uristring.h>
#include <zcontent/zindexedfile/zmasterfile.h>
#include <zio/zioutils.h>

//#include "bckelement.h"

class ZCopyElement : public uriString {
public:
    ZCopyElement()=default;
    ZCopyElement(const uriString pUri) {
        uriString::_copyFrom(pUri);
        Size = getFileSize();
    }
    ZCopyElement(const ZCopyElement& pIn) {_copyFrom(pIn);}
    ZCopyElement& _copyFrom(const ZCopyElement& pIn) {
        uriString::_copyFrom(pIn);
        Size=pIn.Size;
        return *this;
    }
    size_t Size=0;
};

class QLabel;
class QLineEdit;
class ZQTableView;
class QProgressBar;
class QComboBox;
class QCheckBox;

class textEditMWn;
class ZaiErrors;

#include "zmfprogressmwn.h"

class ZCopyDLg : public QDialog, public ZProgressBase
{
Q_OBJECT

public:
    explicit ZCopyDLg(ZaiErrors *pErrorLog, QWidget *pParent);
    ~ZCopyDLg();

    void initLayout();

    //  ZStatus dataSetup(uriString &pURIMaster, const uriString &pTargetDir="");
    /*
  ZStatus XmlSaveBackupset(uriString& pXmlFile, bool pComment=true);
  ZStatus XmlLoadBackupset(const uriString &pXmlFile, ZDateFull &pBackupDate);
*/
    //  BckElement getElt(const uriString& pSource, const uriString& pTargetDir=uriString());

    ZArray<uriString> getSource() { return SourceList; }
    uriString getTarget() { return TargetPath; }

    ZMNP_type getFlags();

private slots:
    bool searchFileClicked();
    void targetClicked();
    //  void bckextChanged();
    void doCopy();


private:
//  ZMasterFile       MasterFile;

//  ZArray<BckElement> Files;

  uriString         URIMasterFile;
  utf8VaryingString MasterBaseName;
  ZArray<uriString> SourceList;
  uriString         TargetPath;

  QLabel*       URISourceLBl=nullptr;
  QLabel*       IndexPathLBl=nullptr;
  QLabel*       URITargetLBl=nullptr;
//  QLineEdit*    BckNameLEd=nullptr;
//  ZQTableView*  BckTBv=nullptr;

  QComboBox*    FlagCBx=nullptr;
  QCheckBox*    PrivCHk=nullptr;
  QCheckBox*    FailCHk=nullptr;

  QComboBox*    IncludeOptCBx=nullptr;



  QPushButton* SearchZMFBTn=nullptr;
  QPushButton* TargetBTn=nullptr;
  QPushButton* OkBTn=nullptr;
  QPushButton* CancelBTn=nullptr;

  /* progress */



  ZaiErrors* ErrorLog=nullptr;
//  textEditMWn* ComLog=nullptr;
};

#endif // ZCOPYDLG_H
