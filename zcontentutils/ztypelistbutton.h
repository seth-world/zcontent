#ifndef ZTYPELISTBUTTON_H
#define ZTYPELISTBUTTON_H

#include <QPushButton>
#include <ztoolset/ztypetype.h>

class ZTypeListButton : public  QPushButton
{
public:
  explicit ZTypeListButton(QWidget* pParent):QPushButton(pParent) {}
  virtual ~ZTypeListButton() {}

  ZTypeBase ZType=ZType_Unknown;
};

#endif // ZTYPELISTBUTTON_H
