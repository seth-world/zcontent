#ifndef ZCOLLECTIONLIST_H
#define ZCOLLECTIONLIST_H

#include <QDialog>
#include <zcontent/zindexedfile/zsearchentity.h>

namespace zbs {
class ZSearchParser;
}

namespace Ui {
class ZCollectionList;
}


class ZQTableView;

class ZCollectionList : public QDialog
{
  Q_OBJECT

public:
  explicit ZCollectionList(zbs::ZSearchParser *pParser,QWidget *parent = nullptr);
  ~ZCollectionList();
  void setup();

  std::shared_ptr<ZSearchEntity> get();

  ZQTableView* CollectionListTBv=nullptr;
private:
  zbs::ZSearchParser* Parser=nullptr;
  Ui::ZCollectionList *ui;
};

#endif // ZCOLLECTIONLIST_H
