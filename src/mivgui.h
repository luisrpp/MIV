/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: september 2008
 ****************************************************************/

#include "ui_mivgui.h"
#include "mivimage.h"

#include "vtkImageViewer2.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkActor2D.h"
#include "vtkImageMapper.h"
#include "vtkLookupTable.h"
#include "vtkPointPicker.h"
#include "vtkPNGReader.h"
#include "vtkImageBlend.h"

#include <QScrollBar>
#include <QApplication>

class vtkEventQtSlotConnect;

class MivGui : public QMainWindow, private Ui::MivGui {
   Q_OBJECT
   
public:
   MivGui(QWidget *parent = 0);
   ~MivGui();
   
   int imageFrame;
   MivImage* mivi;
   MivSettingsType* settingsType;
   vtkImageViewer2* image_view;
   vtkImageViewer2* image_view2;
   vtkImageViewer2* image_view3;
   vtkImageViewer2* image_view4;
   vtkEventQtSlotConnect* connections;
   
   vtkLookupTable *lutGray;
   vtkLookupTable *lutRGB;
   vtkLookupTable *lutMarker;
   
   void updateControls();
   int isDicomDir(const char*);
   void setLabelFileName(const char*);
   vtkImageData* imageOverlay(vtkImageData*, vtkLookupTable*, vtkImageData*, vtkLookupTable*);
   void createVTKBlankImage(vtkImageData*);
   vtkImageData* convertToRGB(vtkImageData*, vtkLookupTable*);
   void setWindowLevel(vtkImageViewer2*);
   void setInterpolation(vtkImageViewer2*);
   void createLUTs();
   vtkLookupTable* selectLUT();
   
   public slots:
   void moveBarX();
   void moveBarY();
   void moveBarZ();
   
   private slots:
   void open();
   void save();
   void saveMask();
   void saveMarkers();
   void saveImage2D();
   void showMarkers();
   void showSegmentation();
   void showBullsEye();
   void import();
   void settings();
   void about();
   void moveBarF();
   void close();
   void selectAxialSlices();
   void adjustAxialSlices();
   void findCircleMarkers();
   void findRingMarkers();
   void findPointMarkers();
   void editMarker();
   void performWatershed();
   void performWatershed3D();
   void normalization();
   void adjustAxis();
   void adjustRegions();
   void generateBullsEye();
};
