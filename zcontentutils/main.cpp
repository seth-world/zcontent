#include "zcontentvisumain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  ZVerbose |= ZVB_ZRF;

  QApplication a(argc, argv);

  utf8VaryingString wStr;
  wStr="5";

  unsigned long wL=wStr.toULong();

  ZContentVisuMain w;
  w.show();
  return a.exec();
}

