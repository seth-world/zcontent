#ifndef VISURAW_H
#define VISURAW_H

#include <sys/types.h> // for ssize_t

#include <qobject.h>

class ZQTableView;
class ZDataBuffer;
class QContextMenuEvent;
class QAction;

enum VisuRaw_type {
  VRTP_ZBlockHeader ,
  VRTP_URFField,
  VRTP_ZType,
  VRTP_uint16,
  VRTP_int16,
  VRTP_uint32,
  VRTP_int32,
  VRTP_uint64,
  VRTP_int64,
  VRTP_sizet,
  VRTP_float,
  VRTP_double,
  VRTP_longdouble
  };


class VisuRaw  : public QObject
{
  Q_OBJECT
public:
  explicit VisuRaw(ZQTableView* pBlockDumpTBv,const ZDataBuffer *pRawRecord);
  ~VisuRaw(){}

  static void visuBlockHeader(ZQTableView* pBlockDumpTBv,const ZDataBuffer *pRawRecord);
  static void visuURFField(ZQTableView* pBlockDumpTBv,const ZDataBuffer *pRawRecord);
  static void visuAtomic(ZQTableView* pBlockDumpTBv,const ZDataBuffer *pRawRecord,VisuRaw_type pVRTP);
  static void visuActionEvent(ZQTableView* pBlockDumpTBv,const ZDataBuffer *pRawRecord,VisuRaw_type pVRTP);

/* For error qt.qpa.xcb: QXcbConnection: XCB error: 3 (BadWindow), sequence: 909, resource id: 32688962, major code: 40 (TranslateCoords), minor code: 0
 * See https://bugreports.qt.io/browse/QTBUG-56893 */

  void VisuBvFlexMenuCallback(QContextMenuEvent *event);

private slots:
  void visuActionEvent(QAction* pAction) ;

private:
  ZQTableView* VisuTBv=nullptr;
  const ZDataBuffer *RawData=nullptr;

  /* evaluate actions */
  QAction* ZBlockHeaderQAc = nullptr;
  QAction* URFFieldQAc = nullptr;

  QAction* ZTypeQAc = nullptr;
  QAction* uint16QAc = nullptr;
  QAction* int16QAc = nullptr;
  QAction* uint32QAc = nullptr;
  QAction* int32QAc = nullptr;
  QAction* uint64QAc = nullptr;
  QAction* int64QAc = nullptr;
  QAction* sizetQAc = nullptr;
  QAction* floatQAc = nullptr;
  QAction* doubleQAc = nullptr;
  QAction* longdoubleQAc = nullptr;

};

ssize_t computeOffsetFromCoord(int pRow, int pCol);

#endif // VISURAW_H
