#ifndef QRLABEL_H
#define QRLABEL_H

#include <QObject>
#include <QLabel>
#include <qrcodegen.h>
#include <QColor>
#include <ztoolset/zutfstrings.h>
#include <qpainter.h>

using namespace qrcodegen;

class QRLabel : public QLabel
{
  Q_OBJECT
public:
  explicit QRLabel(QWidget *parent = nullptr);
  ~QRLabel()
  {
  //  delete Painter;
  }

  void setQRString(const utf8String& pString,const QColor& pBg,const QColor& pFg);
  void setQRData(const uint8_t* pData,int pDataLength,const QColor& pBg,const QColor& pFg);


signals:

public slots:
//  void paintEvent_1(QPaintEvent *pPaintEvent) ;
 // void paintEvent_2(QPaintEvent *pPaintEvent) ;

protected:
// void paintEvent(QPaintEvent *) override;
private:
  QColor  Fg = QColor(Qt::black);
  QColor  Bg = QColor(Qt::white);
  utf8String Content;

//  QPainter* Painter=nullptr;
};

#endif // QRLABEL_H
