#ifndef VISULINECOL_H
#define VISULINECOL_H
#include <stdint.h>
#include <QModelIndex>

class VisuLineCol {
public:
  VisuLineCol();
  int line=0;
  int col=0;
  VisuLineCol& compute(ssize_t wOffset) {
    /* highlight searched result in table view
   * start block :
     offset / 16 -> gives the line

    offset - (offset / 16) gives the column
                     */

                 double wOf = wOffset;
    line = int(wOf / 16.0);

    col = int(wOf - (double (line)* 16.0));

    col = col + (col / 4);

    return *this;
  }
  QModelIndex getIndex() {
    QModelIndex wReturn;
    wReturn.sibling(line,col);
    return wReturn;
  }
};



#endif // VISULINECOL_H
