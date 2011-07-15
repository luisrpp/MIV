/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: september 2008
****************************************************************/

#include <QtGui>
#include <QObject>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QString>
#include <dirent.h>
#include <string.h>
#include "mivgui.h"

#include "vtkDiskSource.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkCommand.h"
#include "vtkImageMapToColors.h"

#include "mivMarkerEditor.h"
#include "mivAxisEditor.h"
#include "mivAxialRegionEditor.h"
#include "MivSettings.h"
#include "MivSaveSlice.h"
#include "MivAdjustAxialSlice.h"
#include "MivBullsEye.h"

MivGui::MivGui(QWidget *parent): QMainWindow(parent)
{
   setupUi(this);

   QDesktopWidget *desktop = QApplication::desktop();

   // Set window settings
   int screenWidth, width;
   int screenHeight, height;
   int x, y;
   QSize windowSize;
   screenWidth = desktop->width(); // get width of screen
   screenHeight = desktop->height(); // get height of screen
   windowSize = size(); // size of our application window
   width = windowSize.width(); 
   height = windowSize.height();
   // little computations
   x = (screenWidth - width) / 2;
   y = (screenHeight - height) / 2;
   y -= 50;
   // move window to desired coordinates
   move ( x, y );

   // make connections
   connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));
   connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));
   connect(actionSave_Slice, SIGNAL(triggered()), this, SLOT(saveImage2D()));
   connect(actionSave_segmentation_mask, SIGNAL(triggered()), this, SLOT(saveMask()));
   connect(actionShow_Markers, SIGNAL(triggered()), this, SLOT(showMarkers()));
   connect(actionSave_Markers, SIGNAL(triggered()), this, SLOT(saveMarkers()));
   connect(actionShow_Segmentation, SIGNAL(triggered()), this, SLOT(showSegmentation()));
   connect(actionShow_Bulls_Eye, SIGNAL(triggered()), this, SLOT(showBullsEye()));
   connect(actionImport, SIGNAL(triggered()), this, SLOT(import()));
   connect(actionSettings, SIGNAL(triggered()), this, SLOT(settings()));
   connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
   connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
   connect(actionSelect_Axial_Slices, SIGNAL(triggered()), this, SLOT(selectAxialSlices()));
   connect(actionAdjust_Axial_Slices, SIGNAL(triggered()), this, SLOT(adjustAxialSlices()));
   connect(actionCircle_Markers_HT, SIGNAL(triggered()), this, SLOT(findCircleMarkers()));
   connect(actionRing_Markers, SIGNAL(triggered()), this, SLOT(findRingMarkers()));
   connect(actionPoint_Markers, SIGNAL(triggered()), this, SLOT(findPointMarkers()));
   connect(actionEdit_Marker, SIGNAL(triggered()), this, SLOT(editMarker()));
   connect(actionWatershed_Transform_From_Markers, SIGNAL(triggered()), this, SLOT(performWatershed()));
   connect(actionWatershed_3D, SIGNAL(triggered()), this, SLOT(performWatershed3D()));
   connect(actionNormalization, SIGNAL(triggered()), this, SLOT(normalization()));
   connect(actionAdjust_axis, SIGNAL(triggered()), this, SLOT(adjustAxis()));
   connect(actionAdjust_regions, SIGNAL(triggered()), this, SLOT(adjustRegions()));
   connect(actionGenerate_Bull_s_eye, SIGNAL(triggered()), this, SLOT(generateBullsEye()));
   connect(horizontalScrollBarX, SIGNAL(valueChanged(int)), this, SLOT(moveBarX()));
   connect(horizontalScrollBarY, SIGNAL(valueChanged(int)), this, SLOT(moveBarY()));
   connect(horizontalScrollBarZ, SIGNAL(valueChanged(int)), this, SLOT(moveBarZ()));
   connect(horizontalScrollBarF, SIGNAL(valueChanged(int)), this, SLOT(moveBarF()));
   connect(qApp, SIGNAL(lastWindowClosed()), this, SLOT(close()));

   // Default Settings
   settingsType = new MivSettingsType;
   settingsType->lut = true;
   settingsType->interpolate = false;
   settingsType->thresholdMethod = 0;
   settingsType->thresholdValue = 50;
   settingsType->axialDelimitations = true;
   settingsType->checkAxis = true;
   settingsType->useSegmentedImage = false;
   
   mivi = NULL;
   horizontalScrollBarX->setEnabled(false);
   horizontalScrollBarY->setEnabled(false);
   horizontalScrollBarZ->setEnabled(false);
   horizontalScrollBarF->setEnabled(false);

   // QT/VTK
   image_view = vtkImageViewer2::New();
   image_view2 = vtkImageViewer2::New();
   image_view3 = vtkImageViewer2::New();
   image_view4 = vtkImageViewer2::New();

   vtkImageData* blank1 = vtkImageData::New();
   createVTKBlankImage(blank1);
   blank1->Update();

   vtkImageData* blank2 = vtkImageData::New();
   createVTKBlankImage(blank2);
   blank2->Update();

   vtkImageData* blank3 = vtkImageData::New();
   createVTKBlankImage(blank3);
   blank3->Update();

   vtkImageData* blank4 = vtkImageData::New();
   createVTKBlankImage(blank4);
   blank4->Update();

   image_view->SetInput(blank1);
   image_view2->SetInput(blank2);
   image_view3->SetInput(blank3);
   image_view4->SetInput(blank4);

   image_view->GetImageActor()->InterpolateOff();
   image_view2->GetImageActor()->InterpolateOff();
   image_view3->GetImageActor()->InterpolateOff();
   image_view4->GetImageActor()->InterpolateOff();

   vtkWidget->SetRenderWindow(image_view->GetRenderWindow());
   vtkWidget_2->SetRenderWindow(image_view2->GetRenderWindow());
   vtkWidget_3->SetRenderWindow(image_view3->GetRenderWindow());
   vtkWidget_4->SetRenderWindow(image_view4->GetRenderWindow());

   image_view->SetupInteractor(vtkWidget->GetRenderWindow()->GetInteractor());
   image_view2->SetupInteractor(vtkWidget_2->GetRenderWindow()->GetInteractor());
   image_view3->SetupInteractor(vtkWidget_3->GetRenderWindow()->GetInteractor());
   image_view4->SetupInteractor(vtkWidget_4->GetRenderWindow()->GetInteractor());

   vtkInteractorStyleImage *imageStyle = vtkInteractorStyleImage::New();
   vtkInteractorStyleImage *imageStyle2 = vtkInteractorStyleImage::New();
   vtkInteractorStyleImage *imageStyle3 = vtkInteractorStyleImage::New();
   vtkInteractorStyleImage *imageStyle4 = vtkInteractorStyleImage::New();

   vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(imageStyle);
   vtkWidget_2->GetRenderWindow()->GetInteractor()->SetInteractorStyle(imageStyle2);
   vtkWidget_3->GetRenderWindow()->GetInteractor()->SetInteractorStyle(imageStyle3);
   vtkWidget_4->GetRenderWindow()->GetInteractor()->SetInteractorStyle(imageStyle4);
}

void MivGui::createLUTs() {
   lutMarker = vtkLookupTable::New();
   lutMarker->SetNumberOfColors(OTHER+1);
   lutMarker->SetRange(0.0,OTHER);
   lutMarker->SetTableValue(0,0,0,0,0);
   lutMarker->SetTableValue(BACKGROUND,1,0.2,0.2,0.5);
   lutMarker->SetTableValue(LEFTVENTRICLE,1,0,0,1);
   lutMarker->SetTableValue(OTHER,1,0.2,0.2,0.5);
   lutMarker->Build();
   
   lutGray = vtkLookupTable::New();
   lutGray->SetTableRange(mivi->getMinimumValue(), mivi->getMaximumValue());
   lutGray->SetRange(0.0,mivi->getMaximumValue());
   lutGray->SetHueRange(0,0);
   lutGray->SetSaturationRange(0,0);
   lutGray->SetValueRange(0.0,1.0);
   lutGray->Build();
   
   lutRGB = vtkLookupTable::New();
   lutRGB->SetNumberOfColors(mivi->getMaximumValue()+1);
   lutRGB->SetTableRange(mivi->getMinimumValue(), mivi->getMaximumValue());
   lutRGB->SetRange(0.0,mivi->getMaximumValue());
   lutRGB->SetHueRange(0.667,0.0);
   //lutRGB->SetHueRange(0.900,0.120);
   lutRGB->Build();
}

void MivGui::setWindowLevel(vtkImageViewer2* img_viewer) {
   int window, level;
   window = mivi->getMaximumValue() - mivi->getMinimumValue();
   level = (0.5 * (mivi->getMaximumValue() + mivi->getMinimumValue()));
   
   img_viewer->SetColorLevel(level);
   img_viewer->SetColorWindow(window);
   if (settingsType->lut)
      img_viewer->GetWindowLevel()->SetLookupTable(selectLUT());
   else
      img_viewer->GetWindowLevel()->SetLookupTable(NULL);
}

void MivGui::setInterpolation(vtkImageViewer2* img_viewer) {
   if (settingsType->interpolate)
      img_viewer->GetImageActor()->InterpolateOn();
   else
      img_viewer->GetImageActor()->InterpolateOff();
}

vtkLookupTable* MivGui::selectLUT() {
   if (settingsType->lut)
      return lutRGB;
   else
      return lutGray;
}

void MivGui::setLabelFileName(const char* file) {
   labelImageName->setText(file);
}

void MivGui::createVTKBlankImage(vtkImageData* img) {
   img->SetDimensions(1, 1, 1);
   img->AllocateScalars();
   img->SetScalarComponentFromDouble(0, 0, 0, 0, 0);
}

void MivGui::updateControls() {
   int sizex, sizey, sizez, sizef;
   sizex = mivi->getSize(0);
   sizey = mivi->getSize(1);
   sizez = mivi->getSize(2);
   sizef = mivi->getSize(3);

   horizontalScrollBarX->setRange(1, sizex);
   horizontalScrollBarX->setMaximum(sizex);
   horizontalScrollBarX->setValue(sizex/2);
   horizontalScrollBarY->setRange(1, sizey);
   horizontalScrollBarY->setMaximum(sizey);
   horizontalScrollBarY->setValue(sizey/2);
   horizontalScrollBarZ->setRange(1, sizez);
   horizontalScrollBarZ->setMaximum(sizez);
   horizontalScrollBarZ->setValue(sizez/2);
   horizontalScrollBarF->setRange(1, sizef);
   horizontalScrollBarF->setMaximum(sizef);
   horizontalScrollBarF->setValue(1);

   actionSave->setEnabled(true);
   actionSave_Slice->setEnabled(true);
   actionSave_segmentation_mask->setEnabled(true);
   actionSave_Markers->setEnabled(true);
   horizontalScrollBarX->setEnabled(true);
   horizontalScrollBarY->setEnabled(true);
   horizontalScrollBarZ->setEnabled(true);
   horizontalScrollBarF->setEnabled(true);

   actionSelect_Axial_Slices->setEnabled(true);
   actionAdjust_Axial_Slices->setEnabled(true);
   actionCircle_Markers_HT->setEnabled(true);
   actionRing_Markers->setEnabled(true);
   actionPoint_Markers->setEnabled(true);
   actionEdit_Marker->setEnabled(false);
   actionWatershed_Transform_From_Markers->setEnabled(false);
   actionShow_Markers->setEnabled(false);
   actionShow_Markers->setOn(false);
   actionShow_Segmentation->setEnabled(false);
   actionShow_Segmentation->setOn(false);
   actionShow_Bulls_Eye->setEnabled(false);
   actionShow_Bulls_Eye->setOn(false);
   menuBull_s_eye->setEnabled(false);
   
   moveBarX();
   moveBarY();
   moveBarZ();
   this->mivi->setMivSettings(settingsType);
}

void MivGui::open() {
   QString fileName = QFileDialog::getOpenFileName(this, QObject::tr("Open File"), QDir::currentPath(), QObject::tr("Analyze Header Files (*.hdr)"));
   const char *fname = ((char *) fileName.latin1());

   if (!fileName.isEmpty()) {

      delete this->mivi;
      this->mivi = new MivImage;

      if (this->mivi->readImage(fname) == EXIT_SUCCESS) {

         this->imageFrame = 0;

         this->createLUTs();
         this->updateControls();

         char ff[255];
         const char* sp = strrchr(fname, '/');
         strcpy(ff, sp+1);
         this->setLabelFileName(ff);
      }
      else {
         QMessageBox::about(this, QObject::tr("ERROR"),
            QObject::tr("<p><b>Error:</b> The image could not be loaded.</p>"));
      }
   }
}

void MivGui::save() {
   QString s = QFileDialog::getSaveFileName(
                    "./",
                    "Analyze 7.5 (*.hdr)",
                    this,
                    "save file dialog",
                    "Choose a filename to save under" );
   const char *fname = ((char *) s.latin1());

   if (!s.isEmpty()) {
      if (this->mivi->writeImage(fname) != EXIT_SUCCESS) {
         QMessageBox::about(this, QObject::tr("ERROR"),
            QObject::tr("<p><b>Error:</b> The image could not be saved.</p>"));
      }
   }
}

void MivGui::saveMask() {
   QString s = QFileDialog::getSaveFileName(
                    "./",
                    "Analyze 7.5 (*.hdr)",
                    this,
                    "save file dialog",
                    "Choose a filename to save under" );
   const char *fname = ((char *) s.latin1());

   if (!s.isEmpty()) {
      if (this->mivi->saveSegmentationMasks(fname) != EXIT_SUCCESS) {
         QMessageBox::about(this, QObject::tr("ERROR"),
            QObject::tr("<p><b>Error:</b> Unexpected error or no segmentation mask found.</p>"));
      }
   }   
}

void MivGui::saveMarkers() {
   QString s = QFileDialog::getSaveFileName(
                    "./",
                    "Analyze 7.5 (*.hdr)",
                    this,
                    "save file dialog",
                    "Choose a filename to save under" );
   const char *fname = ((char *) s.latin1());

   if (!s.isEmpty()) {
      if (this->mivi->saveMarkers(fname) != EXIT_SUCCESS) {
         QMessageBox::about(this, QObject::tr("ERROR"),
            QObject::tr("<p><b>Error:</b> Unexpected error or no segmentation mask found.</p>"));
      }
   }   
}

void MivGui::saveImage2D() {
   MivSaveSlice* saveSlice = new MivSaveSlice(mivi, this->imageFrame, horizontalScrollBarX->value()-1, horizontalScrollBarY->value()-1, horizontalScrollBarZ->value()-1);
   saveSlice->exec();
   delete(saveSlice);
}

int MivGui::isDicomDir(const char *path) {
   DIR *dp;
   struct dirent *ep;
   char *file = new char[255];

   dp = opendir (path);
   if (dp != NULL) {
      while ((ep = readdir(dp))) {
         if (strcmp(ep->d_name, ".") && strcmp(ep->d_name, "..")) {
            sprintf (file, "%s", ep->d_name);
            if (strstr(file, ".dcm")) {
               delete file;
               return 1;
            }
         }
      }
      printf ("\n");
      closedir(dp);
   }

   delete file;

   return 0;
}

void MivGui::import() {
   QString dirName = QFileDialog::getExistingDirectory(this, QObject::tr("Open DICOM Dir"), QDir::currentPath());
   const char *dname = ((char *) dirName.latin1());

   if (!dirName.isEmpty()) {

      if (isDicomDir(dname)) {

         delete this->mivi;
         this->mivi = new MivImage;

         if (this->mivi->readDicom(dname) == EXIT_SUCCESS) {
            this->imageFrame = 0;

            this->createLUTs();
            this->updateControls();
         }
         else {
            QMessageBox::about(this, QObject::tr("ERROR"),
               QObject::tr("<p><b>Error:</b> The image could not be loaded.</p>"));
         }
      }
      else {
         QMessageBox::about(this, QObject::tr("ERROR"),
            QObject::tr("<p><b>Error:</b> The image could not be loaded. <b>This is not a Dicom dir</b>.</p>"));
      }
   }
}

void MivGui::showMarkers() {
   actionShow_Segmentation->setOn(false);
   actionShow_Bulls_Eye->setOn(false);
   this->moveBarZ();
}

void MivGui::showSegmentation() {
   actionShow_Markers->setOn(false);
   actionShow_Bulls_Eye->setOn(false);
   this->moveBarZ();
}

void MivGui::showBullsEye() {
   actionShow_Segmentation->setOn(false);
   actionShow_Markers->setOn(false);
   this->moveBarZ();
}

void MivGui::moveBarX() {
   ImageType2D::Pointer imgx = this->mivi->getImage2D(this->imageFrame, 0,
                                                      horizontalScrollBarX->value()-1);

   if (this->mivi->isAxialSlicesSelected(this->imageFrame) && settingsType->axialDelimitations) {
      vtkImageData* img_x = vtkImageData::New();
      img_x->DeepCopy(this->mivi->convertITKtoVTK(imgx));
      vector<int> delim = mivi->getAxialSliceDelimitations(this->imageFrame);

      for (int i=0; i < mivi->getSize(1); i++) {
         img_x->SetScalarComponentFromDouble(img_x->GetDimensions()[0] - delim[0] - 1, i, 0, 0, mivi->getMaximumValue());
         img_x->SetScalarComponentFromDouble(img_x->GetDimensions()[0] - delim[1] - 1, i, 0, 0, mivi->getMaximumValue());
         delim.clear();
      }
      img_x->Update();
      this->image_view2->SetInput(img_x);
   }
   else {
      this->image_view2->SetInput(this->mivi->convertITKtoVTK(imgx));
   }

   //this->image_view2->SetInput(this->mivi->convertITKtoVTK(imgx));
   setWindowLevel(this->image_view2);
   setInterpolation(this->image_view2);
   this->image_view2->Render();
   this->image_view2->GetRenderer()->ResetCamera();
}

void MivGui::moveBarY() {
   ImageType2D::Pointer imgy = this->mivi->getImage2D(this->imageFrame, 1,
                                                      horizontalScrollBarY->value()-1);

   
   if (this->mivi->isAxialSlicesSelected(this->imageFrame) && settingsType->axialDelimitations) {
      vtkImageData* img_y = vtkImageData::New();
      img_y->DeepCopy(this->mivi->convertITKtoVTK(imgy));
      vector<int> delim = mivi->getAxialSliceDelimitations(this->imageFrame);

      for (int i=0; i < mivi->getSize(0); i++) {
         img_y->SetScalarComponentFromDouble(i, delim[0], 0, 0, mivi->getMaximumValue());
         img_y->SetScalarComponentFromDouble(i, delim[1], 0, 0, mivi->getMaximumValue());
         delim.clear();
      }
      img_y->Update();
      this->image_view->SetInput(img_y);
   }
   else {
      this->image_view->SetInput(this->mivi->convertITKtoVTK(imgy));
   }
   
   //this->image_view->SetInput(this->mivi->convertITKtoVTK(imgy));
   setWindowLevel(this->image_view);
   setInterpolation(this->image_view);
   this->image_view->Render();
   this->image_view->GetRenderer()->ResetCamera();
}

void MivGui::moveBarZ() {
   ImageType2D::Pointer imgz = this->mivi->getImage2D(this->imageFrame, 2,
                                                      horizontalScrollBarZ->value()-1);
   
   this->image_view3->SetInput(this->mivi->convertITKtoVTK(imgz));
   setWindowLevel(this->image_view3);
   setInterpolation(this->image_view3);
   this->image_view3->Render();
   this->image_view3->GetRenderer()->ResetCamera();
   
   // Show the segmented image.
   if (mivi->isSegmented(this->imageFrame, horizontalScrollBarZ->value()-1) && actionShow_Segmentation->isOn()) {
      ImageType2D::Pointer segmented = this->mivi->getSegmented(this->imageFrame,
                                                                horizontalScrollBarZ->value()-1);
      
      this->image_view4->SetInput(convertToRGB(this->mivi->convertITKtoVTK(segmented),lutGray));
   }
   // Show Marker Image if the segementation was not performed.
   else if (mivi->existMarker(this->imageFrame, horizontalScrollBarZ->value()-1) && actionShow_Markers->isOn()) {
      ImageType2D::Pointer marker = this->mivi->getMarker(this->imageFrame,
                                                          horizontalScrollBarZ->value()-1);
      
      this->image_view4->SetInput(this->imageOverlay(this->mivi->convertITKtoVTK(imgz), lutGray,
                                                     this->mivi->convertITKtoVTK(marker), lutMarker));
   }
   else if (mivi->isBullsEyeCreated(horizontalScrollBarF->value()-1) && actionShow_Bulls_Eye->isOn()) {
      this->image_view4->SetInput(convertToRGB(this->mivi->convertITKtoVTK(this->mivi->getBullsEyeImage(horizontalScrollBarF->value()-1)), lutRGB));
   }
   else {
      vtkImageData* blank = vtkImageData::New();
      createVTKBlankImage(blank);
      blank->Update();
      this->image_view4->SetInput(blank);
   }
   // Due to some bug in the update of the widget, Render and ResetCamera are called twice.
   setInterpolation(this->image_view4);
   this->image_view4->Render();
   this->image_view4->GetRenderer()->ResetCamera();
   this->image_view4->Render();
   this->image_view4->GetRenderer()->ResetCamera();
}

void MivGui::moveBarF() {
   this->imageFrame = horizontalScrollBarF->value()-1;
   this->moveBarX();
   this->moveBarY();
   this->moveBarZ();
}

void MivGui::selectAxialSlices() {
   this->mivi->selectAxialSlices();
   this->moveBarY();
   this->moveBarX();
}

void MivGui::adjustAxialSlices() {
   MivAdjustAxialSlice* adjustSlice = new MivAdjustAxialSlice(mivi);
   adjustSlice->exec();
   delete(adjustSlice);
   this->moveBarY();
   this->moveBarX();
}

void MivGui::findCircleMarkers() {
   this->mivi->findAxialCircles();
   actionEdit_Marker->setEnabled(true);
   actionWatershed_Transform_From_Markers->setEnabled(true);
   actionWatershed_3D->setEnabled(true);
   actionShow_Markers->setEnabled(true);
   actionShow_Markers->setOn(true);
   actionShow_Segmentation->setOn(false);
   actionShow_Bulls_Eye->setOn(false);
   menuBull_s_eye->setEnabled(false);
   this->moveBarZ();
}

void MivGui::findRingMarkers() {
   this->mivi->findAxialRings();
   actionEdit_Marker->setEnabled(true);
   actionWatershed_Transform_From_Markers->setEnabled(true);
   actionWatershed_3D->setEnabled(true);
   actionShow_Markers->setEnabled(true);
   actionShow_Markers->setOn(true);
   actionShow_Segmentation->setOn(false);
   actionShow_Bulls_Eye->setOn(false);
   menuBull_s_eye->setEnabled(false);
   this->moveBarZ();
}

void MivGui::findPointMarkers() {
   this->mivi->findAxialPoints();
   actionEdit_Marker->setEnabled(true);
   actionWatershed_Transform_From_Markers->setEnabled(true);
   actionWatershed_3D->setEnabled(true);
   actionShow_Markers->setEnabled(true);
   actionShow_Markers->setOn(true);
   actionShow_Segmentation->setOn(false);
   actionShow_Bulls_Eye->setOn(false);
   menuBull_s_eye->setEnabled(false);
   this->moveBarZ();
}

void MivGui::editMarker() {
   bool is_new = false;
   if (!this->mivi->existMarker(this->imageFrame, horizontalScrollBarZ->value()-1)) {
      int ret = QMessageBox::question(this, QObject::tr("Create marker"),
                QObject::tr("No marker found for this slice. Create a new marker?"),
                QMessageBox::Yes | QMessageBox::No);

      is_new = true;
      if (ret == QMessageBox::Yes)
         this->mivi->createBlankMarker(this->imageFrame, horizontalScrollBarZ->value()-1);
   }

   if (this->mivi->existMarker(this->imageFrame, horizontalScrollBarZ->value()-1)) {

      OutputGui* output = new OutputGui;
      MivMarkerEditor* widget = new MivMarkerEditor(mivi, this->imageFrame, 2,
                                                    horizontalScrollBarZ->value()-1, output);

      if (widget->exec() == QDialog::Accepted) {

         if (!output->remove) {
            this->mivi->setMarker(this->mivi->convertVTKtoITK(output->getImage()), this->imageFrame,          
                                                              horizontalScrollBarZ->value()-1);
         }

         actionEdit_Marker->setEnabled(true);
         actionWatershed_Transform_From_Markers->setEnabled(true);
         actionShow_Markers->setEnabled(true);
         actionShow_Markers->setOn(true);
         actionShow_Segmentation->setOn(false);
         actionShow_Bulls_Eye->setOn(false);
         moveBarZ();
      }

      delete(widget);
      delete(output);
   }
}

void MivGui::performWatershed() {
   this->mivi->performWatershed();
   actionShow_Segmentation->setEnabled(true);
   actionShow_Segmentation->setOn(true);
   actionShow_Markers->setOn(false);
   actionShow_Bulls_Eye->setOn(false);
   menuBull_s_eye->setEnabled(true);
   this->moveBarZ();
}

void MivGui::performWatershed3D() {
   this->mivi->performWatershed3D();
   actionShow_Segmentation->setEnabled(true);
   actionShow_Segmentation->setOn(true);
   actionShow_Markers->setOn(false);
   actionShow_Bulls_Eye->setOn(false);
   menuBull_s_eye->setEnabled(true);
   this->moveBarZ();
}

void MivGui::normalization() {
   this->mivi->normalization();
   this->moveBarZ();
}

void MivGui::adjustAxis() {
   MivAxisEditor *aEditor = new MivAxisEditor(mivi);
   aEditor->exec();
   delete(aEditor);
}

void MivGui::adjustRegions() {
   // TODO
}

void MivGui::generateBullsEye() {
   for (int i=0; i < mivi->getSize(3); i++) {
      MivBullsEye* beye = new MivBullsEye(mivi, i);
      if (settingsType->useSegmentedImage) {
         beye->setUseSegmentedImage(true);
      }
      else {
         beye->setUseSegmentedImage(false);
      }
      beye->generateBullsEye();
      delete(beye);
   }
   actionShow_Segmentation->setOn(false);
   actionShow_Markers->setOn(false);
   actionShow_Bulls_Eye->setEnabled(true);
   actionShow_Bulls_Eye->setOn(true);
   this->moveBarZ();
}

vtkImageData* MivGui::convertToRGB(vtkImageData* img, vtkLookupTable* lut) {
   vtkImageMapToColors *img_rgb = vtkImageMapToColors::New();
   img_rgb->SetInput(img);
   img_rgb->SetLookupTable(lut);
   img_rgb->SetOutputFormatToRGBA();
   img_rgb->Update();
   return img_rgb->GetOutput();
}

vtkImageData* MivGui::imageOverlay(vtkImageData* img1, vtkLookupTable* mrk1, vtkImageData* img2, vtkLookupTable* mrk2) {
   vtkImageBlend* blend = vtkImageBlend::New();
   blend->RemoveAllInputs();
   blend->AddInput(convertToRGB(img1, mrk1));
   blend->AddInput(convertToRGB(img2, mrk2));
   blend->SetBlendModeToNormal();
   blend->Update();
   return blend->GetOutput();
}

void MivGui::settings() {
   MivSettings *settings = new MivSettings(settingsType);
   if (settings->exec() == QDialog::Accepted) {
      if (horizontalScrollBarX->isEnabled()) {
         this->moveBarX();
         this->moveBarY();
         this->moveBarZ();
         this->mivi->setMivSettings(settingsType);
      }
   }
   delete(settings);
}

void MivGui::about() {
   QMessageBox::about(this, QObject::tr("About Medical Image Visualizer"),
      QObject::tr("<p>The <b>Medical Image Visualizer</b> is a free software that uses "
      "the libraries ITK, VTK and QT to load, segment and visualize cardiac images (SPECT) in Analyze "
      "format. This program also creates a Bull's Eye graphic for cardiovascular perfusion exams.<p/><br><b>Author:</b> Luis Roberto P. de Paula<br>"
      "<b>e-mail:</b> luisrpp@vision.ime.usp.br"));
}

void MivGui::close() {
   image_view->Delete();
   image_view2->Delete();
   image_view3->Delete();
   image_view4->Delete();
   delete vtkWidget;
   delete vtkWidget_2;
   delete vtkWidget_3;
   delete vtkWidget_4;
   delete mivi;
   delete settingsType;

   QApplication::exit();
}

MivGui::~MivGui() {
}
