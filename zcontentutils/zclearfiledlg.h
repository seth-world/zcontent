#ifndef ZCLEARFILEDLG_H
#define ZCLEARFILEDLG_H

#include <QDialog>


class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;
namespace zbs {
class ZRawMasterFile;
class ZRandomFile;
}
using namespace zbs;

class ZClearFileDLg : public QDialog
{

public:
    explicit ZClearFileDLg(QWidget* pParent) ;

    void initLayout();
    void setup(ZRawMasterFile* pURIFile);
    void setup(ZRandomFile* pURIFile);

    bool getHighwater();
    ssize_t getSizeToKeep() ;
private slots:
    void OkBTnClicked();
    void AlignSizeCHkChanged(bool pChecked);

private:
    size_t wSizeToKeep=-1;
    //   QWidget* MainWidget=nullptr;
    QLabel* FileTypeLBl=nullptr;
    QLabel *FileDescriptionLBl = nullptr;
    QLabel* CurrentFileSizeLBl=nullptr;
    QLabel* BlockTargetSizeLBl=nullptr;
    QLabel* UsedBlocksLBl=nullptr;
    QLineEdit* SizeToKeepLEd=nullptr;
    QCheckBox* AlignSizeCHk=nullptr;
    QCheckBox* HighwaterCHk=nullptr;
    QPushButton* OkBTn=nullptr;
    QPushButton* CancelBTn=nullptr;

    ZRandomFile*    RandomFile=nullptr;
    ZRawMasterFile* RawMasterFile=nullptr;
};



#endif // ZCLEARFILEDLG_H
