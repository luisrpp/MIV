/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: january 2011
 ****************************************************************/

#include <QMainWindow>
#include <QDialog>
#include "mivimage.h"
#include "ui_mivadjustaxialslice.h"

class vtkEventQtSlotConnect;

class MivAdjustAxialSlice : public QDialog, public Ui_frmAdjustAxialSlices {
   Q_OBJECT
public:
   MivAdjustAxialSlice(MivImage*);
   
private:
   MivImage* mivi;

private slots:
   void actionOk();
   void actionCancel();
};

