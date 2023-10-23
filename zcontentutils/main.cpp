#include "zcontentvisumain.h"

#include <QApplication>
#include <zcontentcommon/zgeneralparameters.h>


int main(int argc, char *argv[])
{
  ZVerbose |= ZVB_ZRF;
  QApplication a(argc, argv);

  ZStatus wSt=GeneralParameters.setFromArg(argc,argv);

  ZContentVisuMain w;
  w.show();
  return a.exec();
}

