#include "zcontentvisumain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  ZContentVisuMain w;
  w.show();
  return a.exec();
}
