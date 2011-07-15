/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: march 2009
****************************************************************/

#include <QMainWindow>
#include <QDialog>
#include <QThread>
#include <QMap>
#include <vector>

#include "ui_mivaxialregion.h"

#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkCamera.h"
#include "vtkImageViewer2.h"
#include "vtkImageActor.h"
#include "vtkImageBlend.h"

#include "mivimage.h"

typedef struct _MAREValues {
   int apex_start;
   int apex_end;
   int apic_start;
   int apic_end;
   int med_start;
   int med_end;
   int bas_start;
   int bas_end;
} MAREValues;

class MivAxialRegionEditor : public QDialog, public Ui_frmAxialRegion {
   Q_OBJECT
   public:
      MivAxialRegionEditor(MivImage*);
      ~MivAxialRegionEditor();

      MAREValues *mValues;
      MivImage* mivi;
      vtkImageViewer2* viewer;
      vtkImageViewer2* viewer2;

      void saveData(int);
      void loadData(int);
      void clearFields();
      void loadFromImageStorage();
      void saveToImageStorage();

   private slots:
      void selectFrame();
      void actionOk();
      void actionCancel();
      void actionMoveBar();
      void actionMoveBar2();
      void actionFocus2();
      void actionFocus3();
      void actionFocus4();
      void actionFocus5();
      void actionFocus6();
      void actionFocus7();

   private:
      int frame;

      bool isFieldsEmpty();
      bool validateFields();
      bool validadeMAREValues();
      vtkImageData* imageLine();
      vtkImageData* imageOverlay(vtkImageData*, vtkImageData*, float);
};
