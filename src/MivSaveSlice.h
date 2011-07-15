/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: january 2011
 ****************************************************************/

#include <QMainWindow>
#include <QDialog>
#include "mivimage.h"
#include "ui_mivsaveslice.h"

class vtkEventQtSlotConnect;

class MivSaveSlice : public QDialog, public Ui_frmMivSaveSlice {
   Q_OBJECT
public:
   MivSaveSlice(MivImage*, int, int, int, int);
   
private:
   MivImage* mivi;
   int frame;
   int sliceX;
   int sliceY;
   int sliceZ;

private slots:
   void actionOk();
   void actionCancel();
};

