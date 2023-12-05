#include "zcontentvisumain.h"
#include "zentrypoint.h"

#include <QApplication>
#include <zcontentcommon/zgeneralparameters.h>


int main(int argc, char *argv[])
{

  BaseParameters->addVerbose(ZVB_ZRF);
  QApplication a(argc, argv);

  ZStatus wSt=GeneralParameters.setFromArg(argc,argv);

  //ZContentVisuMain w;

  ZEntryPoint w;

  w.show();
  return a.exec();
}

