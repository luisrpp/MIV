/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: march 2009
****************************************************************/

#include <QDesktopWidget>
#include <QMessageBox>
#include <QDoubleValidator>

#include "mivAxialRegionEditor.h"

MivAxialRegionEditor::MivAxialRegionEditor(MivImage* mivImage) {
   this->setupUi(this);

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

   this->mivi = mivImage;
   mValues = new MAREValues[mivi->getSize(3)];
   loadFromImageStorage();
   loadData(0);

   frame = 0;
   for (int i = 0; i < mivi->getSize(3); i++) {
      QString str;
      str.sprintf("Frame %d", i+1);
      cboFrame->addItem(str, 0);
   }

   int startScroll, endScroll;
   startScroll = 0;
   for (int i = 0; i < mivi->getSize(2); i++) {
      if (mivi->isSegmented(frame, i)) {
         if (startScroll == 0)
            startScroll = i+1;
         endScroll = i+1;
      }
   }

   scrollAxial->setRange(startScroll, endScroll);
   scrollAxial->setMaximum(endScroll);

   scrollSagittal->setRange(1, mivi->getSize(1));
   scrollSagittal->setMaximum(mivi->getSize(1));
   scrollSagittal->setValue((mivi->getSize(0)-1)/2);

   txtApexStart->setValidator(new QDoubleValidator(0, mivi->getSize(2), 0, txtApexStart));
   txtApexEnd->setValidator(new QDoubleValidator(0, mivi->getSize(2), 0, txtApexEnd));
   txtApicalStart->setValidator(new QDoubleValidator(0, mivi->getSize(2), 0, txtApicalStart));
   txtApicalEnd->setValidator(new QDoubleValidator(0, mivi->getSize(2), 0, txtApicalEnd));
   txtMedialStart->setValidator(new QDoubleValidator(0, mivi->getSize(2), 0, txtMedialStart));
   txtMedialEnd->setValidator(new QDoubleValidator(0, mivi->getSize(2), 0, txtMedialEnd));
   txtBasalStart->setValidator(new QDoubleValidator(0, mivi->getSize(2), 0, txtBasalStart));
   txtBasalEnd->setValidator(new QDoubleValidator(0, mivi->getSize(2), 0, txtBasalEnd));

   connect(cboFrame, SIGNAL(currentIndexChanged(int)), this, SLOT(selectFrame()));
   connect(cmdOk, SIGNAL(clicked()), this, SLOT(actionOk()));
   connect(cmdCancel, SIGNAL(clicked()), this, SLOT(actionCancel()));
   connect(scrollAxial, SIGNAL(valueChanged(int)), this, SLOT(actionMoveBar()));
   connect(scrollSagittal, SIGNAL(valueChanged(int)), this, SLOT(actionMoveBar2()));

   connect(txtApexEnd, SIGNAL(lostFocus()), this, SLOT(actionFocus2()));
   connect(txtApicalStart, SIGNAL(lostFocus()), this, SLOT(actionFocus3()));
   connect(txtApicalEnd, SIGNAL(lostFocus()), this, SLOT(actionFocus4()));
   connect(txtMedialStart, SIGNAL(lostFocus()), this, SLOT(actionFocus5()));
   connect(txtMedialEnd, SIGNAL(lostFocus()), this, SLOT(actionFocus6()));
   connect(txtBasalStart, SIGNAL(lostFocus()), this, SLOT(actionFocus7()));

   int window, level;
   window = mivi->getMaximumValue() - mivi->getMinimumValue();
   level = (0.5 * (mivi->getMaximumValue() + mivi->getMinimumValue()));

   // AXIAL
   viewer = vtkImageViewer2::New();
   viewer->SetColorLevel(level);
   viewer->SetColorWindow(window);
   viewer->SetInput(mivi->convertITKtoVTK(mivi->getSegmented(frame, scrollAxial->value()-1)));
   viewer->GetImageActor()->InterpolateOff();

   vtkWidget->SetRenderWindow(viewer->GetRenderWindow());
   vtkInteractorStyleImage *imageStyle = vtkInteractorStyleImage::New();
   viewer->SetupInteractor(vtkWidget->GetRenderWindow()->GetInteractor());
   vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(imageStyle);

   viewer->GetRenderer()->ResetCamera();
   viewer->GetRenderWindow()->Render();

   // SAGITTAL
   this->viewer2 = vtkImageViewer2::New();
   viewer2->SetColorLevel(level);
   viewer2->SetColorWindow(window);

   viewer2->SetInput(imageOverlay(imageLine(), mivi->convertITKtoVTK(mivi->getImage2D(frame, 0, scrollSagittal->value()-1)), 0.8));
   viewer2->GetImageActor()->InterpolateOff();

   vtkWidget_2->SetRenderWindow(viewer2->GetRenderWindow());
   vtkInteractorStyleImage *imageStyle2 = vtkInteractorStyleImage::New();
   viewer2->SetupInteractor(vtkWidget_2->GetRenderWindow()->GetInteractor());
   vtkWidget_2->GetRenderWindow()->GetInteractor()->SetInteractorStyle(imageStyle2);

   viewer2->GetRenderer()->ResetCamera();
   viewer2->GetRenderWindow()->Render();

   vtkWidget->show();
   vtkWidget_2->show();
}

vtkImageData* MivAxialRegionEditor::imageOverlay(vtkImageData* img1, vtkImageData* img2, float opacity) {
   vtkImageBlend* blend = vtkImageBlend::New();
   blend->RemoveAllInputs();
   blend->AddInput(img1);
   blend->AddInput(img2);
   blend->SetOpacity(1, opacity);
   blend->Update();
   return blend->GetOutput();
}

vtkImageData* MivAxialRegionEditor::imageLine() {
   vtkImageData *line = vtkImageData::New();
   line->DeepCopy(mivi->convertITKtoVTK(mivi->getImage2D(frame, 0, scrollSagittal->value()-1)));
   for (int x = 0; x < mivi->getSize(1); x++) {
      for (int y = 0; y < mivi->getSize(2); y++) {
         if (scrollAxial->value()-1 == y)
            line->SetScalarComponentFromDouble(x, y, 0, 0, 500);
         else
            line->SetScalarComponentFromDouble(x, y, 0, 0, 0);
      }
   }
   line->Update();
   return line;
}

void MivAxialRegionEditor::actionFocus2() {
   if (!txtApexEnd->text().isEmpty()) {
      QString str;
      str.sprintf("%d", txtApexEnd->text().toInt()+1);
      txtApicalStart->setText(str);
   }
}

void MivAxialRegionEditor::actionFocus3() {
   if (!txtApicalStart->text().isEmpty()) {
      QString str;
      str.sprintf("%d", txtApicalStart->text().toInt()-1);
      txtApexEnd->setText(str);
   }
}

void MivAxialRegionEditor::actionFocus4() {
   if (!txtApicalEnd->text().isEmpty()) {
      QString str;
      str.sprintf("%d", txtApicalEnd->text().toInt()+1);
      txtMedialStart->setText(str);
   }
}

void MivAxialRegionEditor::actionFocus5() {
   if (!txtMedialStart->text().isEmpty()) {
      QString str;
      str.sprintf("%d", txtMedialStart->text().toInt()-1);
      txtApicalEnd->setText(str);
   }
}

void MivAxialRegionEditor::actionFocus6() {
   if (!txtMedialEnd->text().isEmpty()) {
      QString str;
      str.sprintf("%d", txtMedialEnd->text().toInt()+1);
      txtBasalStart->setText(str);
   }
}

void MivAxialRegionEditor::actionFocus7() {
   if (!txtBasalStart->text().isEmpty()) {
      QString str;
      str.sprintf("%d", txtBasalStart->text().toInt()-1);
      txtMedialEnd->setText(str);
   }
}

bool MivAxialRegionEditor::isFieldsEmpty() {
   bool isEmpty = false;
   if (txtApexStart->text().isEmpty())
      isEmpty = true;
   else if (txtApexEnd->text().isEmpty())
      isEmpty = true;
   else if (txtApicalStart->text().isEmpty())
      isEmpty = true;
   else if (txtApicalEnd->text().isEmpty())
      isEmpty = true;
   else if (txtMedialStart->text().isEmpty())
      isEmpty = true;
   else if (txtMedialEnd->text().isEmpty())
      isEmpty = true;
   else if (txtBasalStart->text().isEmpty())
      isEmpty = true;
   else if (txtBasalEnd->text().isEmpty())
      isEmpty = true;

   return isEmpty;
}

bool MivAxialRegionEditor::validateFields() {
   int startValue, endValue;
   startValue = 0;
   for (int i = 0; i < mivi->getSize(2); i++) {
      if (mivi->isSegmented(this->frame, i)) {
         if (startValue == 0)
            startValue = i;
         endValue = i;
      }
   }

   if (txtApexStart->text().toInt() < startValue + 1) {
      txtApexStart->setText("");
      txtApexEnd->setText("");
      return false;
   }
   if (txtApexStart->text().toInt() > txtApexEnd->text().toInt()) {
      txtApexStart->setText("");
      txtApexEnd->setText("");
      return false;
   }
   if (txtApicalStart->text().toInt() > txtApicalEnd->text().toInt()) {
      txtApicalStart->setText("");
      txtApicalEnd->setText("");
      return false;
   }
   if (txtMedialStart->text().toInt() > txtMedialEnd->text().toInt()) {
      txtMedialStart->setText("");
      txtMedialEnd->setText("");
      return false;
   }
   if (txtBasalStart->text().toInt() > txtBasalEnd->text().toInt()) {
      txtBasalStart->setText("");
      txtBasalEnd->setText("");
      return false;
   }
   if (txtBasalEnd->text().toInt() > endValue + 1) {
      txtBasalStart->setText("");
      txtBasalEnd->setText("");
      return false;
   }
   return true;
}

void MivAxialRegionEditor::selectFrame() {
   if (isFieldsEmpty()) {
      if (cboFrame->currentIndex() != this->frame) {
         QMessageBox::warning(this, QObject::tr("Blank fields"),
             QObject::tr("Please, fill all blank fields."),
             QMessageBox::Ok);
      }
      cboFrame->setCurrentIndex(this->frame);
   }
   else if (!validateFields()) {
      if (cboFrame->currentIndex() != this->frame) {
         QMessageBox::warning(this, QObject::tr("Values out of range"),
             QObject::tr("Please, set the values in the correct range."),
             QMessageBox::Ok);
      }
      cboFrame->setCurrentIndex(this->frame);
   }
   else {
      saveData(this->frame);
      clearFields();
      this->frame = cboFrame->currentIndex();
      loadData(this->frame);
      int startScroll, endScroll;
      startScroll = 0;
      for (int i = 0; i < mivi->getSize(2); i++) {
         if (mivi->isSegmented(frame, i)) {
            if (startScroll == 0)
               startScroll = i+1;
            endScroll = i+1;
         }
      }
      scrollAxial->setRange(startScroll, endScroll);
      scrollAxial->setMaximum(endScroll);
      actionMoveBar();
   }
}

void MivAxialRegionEditor::saveData(int m_frame) {
   int value = -1;
   if (txtApexStart->text().isEmpty())
      value = -1;
   else
      value = txtApexStart->text().toInt()-1;
   mValues[m_frame].apex_start = value;

   if (txtApexEnd->text().isEmpty())
      value = -1;
   else
      value = txtApexEnd->text().toInt()-1;
   mValues[m_frame].apex_end = value;

   if (txtApicalStart->text().isEmpty())
      value = -1;
   else
      value = txtApicalStart->text().toInt()-1;
   mValues[m_frame].apic_start = value;

   if (txtApicalEnd->text().isEmpty())
      value = -1;
   else
      value = txtApicalEnd->text().toInt()-1;
   mValues[m_frame].apic_end = value;

   if (txtMedialStart->text().isEmpty())
      value = -1;
   else
      value = txtMedialStart->text().toInt()-1;
   mValues[m_frame].med_start = value;

   if (txtMedialEnd->text().isEmpty())
      value = -1;
   else
      value = txtMedialEnd->text().toInt()-1;
   mValues[m_frame].med_end = value;

   if (txtBasalStart->text().isEmpty())
      value = -1;
   else
      value = txtBasalStart->text().toInt()-1;
   mValues[m_frame].bas_start = value;

   if (txtBasalEnd->text().isEmpty())
      value = -1;
   else
      value = txtBasalEnd->text().toInt()-1;
   mValues[m_frame].bas_end = value;
}

void MivAxialRegionEditor::loadData(int m_frame) {
   QString str;
   if (mValues[m_frame].apex_start == -1)
      txtApexStart->setText("");
   else {
      str.sprintf("%d", mValues[m_frame].apex_start+1);
      txtApexStart->setText(str);
   }

   if (mValues[m_frame].apex_end == -1)
      txtApexEnd->setText("");
   else {
      str.sprintf("%d", mValues[m_frame].apex_end+1);
      txtApexEnd->setText(str);
   }

   if (mValues[m_frame].apic_start == -1)
      txtApicalStart->setText("");
   else {
      str.sprintf("%d", mValues[m_frame].apic_start+1);
      txtApicalStart->setText(str);
   }

   if (mValues[m_frame].apic_end == -1)
      txtApicalEnd->setText("");
   else {
      str.sprintf("%d", mValues[m_frame].apic_end+1);
      txtApicalEnd->setText(str);
   }

   if (mValues[m_frame].med_start == -1)
      txtMedialStart->setText("");
   else {
      str.sprintf("%d", mValues[m_frame].med_start+1);
      txtMedialStart->setText(str);
   }

   if (mValues[m_frame].med_end == -1)
      txtMedialEnd->setText("");
   else {
      str.sprintf("%d", mValues[m_frame].med_end+1);
      txtMedialEnd->setText(str);
   }

   if (mValues[m_frame].bas_start == -1)
      txtBasalStart->setText("");
   else {
      str.sprintf("%d", mValues[m_frame].bas_start+1);
      txtBasalStart->setText(str);
   }

   if (mValues[m_frame].bas_end == -1)
      txtBasalEnd->setText("");
   else {
      str.sprintf("%d", mValues[m_frame].bas_end+1);
      txtBasalEnd->setText(str);
   }
}

void MivAxialRegionEditor::clearFields() {
   txtApexStart->setText("");
   txtApexEnd->setText("");
   txtApicalStart->setText("");
   txtApicalEnd->setText("");
   txtMedialStart->setText("");
   txtMedialEnd->setText("");
   txtBasalStart->setText("");
   txtBasalEnd->setText("");
}

void MivAxialRegionEditor::loadFromImageStorage() {
   for (int i = 0; i < mivi->getSize(3); i++) {
      int apex_start = -1;
      int apex_end = -1;
      int apic_start = -1;
      int apic_end = -1;
      int med_start = -1;
      int med_end = -1;
      int bas_start = -1;
      int bas_end = -1;
      for (int j = 0; j < mivi->getSize(2); j++) {
         if (mivi->hasAxialRegion(i, j)) {
            if (mivi->getAxialRegion(i, j) == APEX) {
               if (apex_start == -1)
                  apex_start = j;
               apex_end = j;
            }
            if (mivi->getAxialRegion(i, j) == APICAL) {
               if (apic_start == -1)
                  apic_start = j;
               apic_end = j;
            }
            if (mivi->getAxialRegion(i, j) == MEDIAL) {
               if (med_start == -1)
                  med_start = j;
               med_end = j;
            }
            if (mivi->getAxialRegion(i, j) == BASAL) {
               if (bas_start == -1)
                  bas_start = j;
               bas_end = j;
            }
         }
      }
      mValues[i].apex_start = apex_start;
      mValues[i].apex_end = apex_end;
      mValues[i].apic_start = apic_start;
      mValues[i].apic_end = apic_end;
      mValues[i].med_start = med_start;
      mValues[i].med_end = med_end;
      mValues[i].bas_start = bas_start;
      mValues[i].bas_end = bas_end;
   }
}

void MivAxialRegionEditor::saveToImageStorage() {
   for (int i = 0; i < mivi->getSize(3); i++) {
      for (int j = 0; j < mValues[i].apex_start; j++) {
         mivi->setAxialRegion(i, j, UNKNOWN);
      }
      for (int j = mValues[i].apex_start; j <= mValues[i].apex_end; j++) {
         mivi->setAxialRegion(i, j, APEX);
      }
      for (int j = mValues[i].apic_start; j <= mValues[i].apic_end; j++) {
         mivi->setAxialRegion(i, j, APICAL);
      }
      for (int j = mValues[i].med_start; j <= mValues[i].med_end; j++) {
         mivi->setAxialRegion(i, j, MEDIAL);
      }
      for (int j = mValues[i].bas_start; j <= mValues[i].bas_end; j++) {
         mivi->setAxialRegion(i, j, BASAL);
      }
      for (int j = mValues[i].bas_end + 1; j < mivi->getSize(2); j++) {
         mivi->setAxialRegion(i, j, UNKNOWN);
      }
   }
}

bool MivAxialRegionEditor::validadeMAREValues() {
   bool isOk = true;
   for (int i = 0; i < mivi->getSize(3); i++) {
      if (mValues[i].apex_start == -1 || mValues[i].apex_end == -1 ||
          mValues[i].apic_start == -1 || mValues[i].apic_end == -1 ||
          mValues[i].med_start == -1 || mValues[i].med_end == -1 ||
          mValues[i].bas_start == -1 || mValues[i].bas_end == -1) {
         isOk = false;
      }
   }
   return isOk;
}

void MivAxialRegionEditor::actionMoveBar() {
   if (mivi->isSegmented(frame, scrollAxial->value()-1)) {
      viewer->SetInput(mivi->convertITKtoVTK(mivi->getSegmented(frame, scrollAxial->value()-1)));
      viewer->GetRenderWindow()->Render();
      viewer2->SetInput(imageOverlay(imageLine(), mivi->convertITKtoVTK(mivi->getImage2D(frame, 0, scrollSagittal->value()-1)), 0.8));
      viewer2->GetRenderWindow()->Render();
   }
}

void MivAxialRegionEditor::actionMoveBar2() {
   viewer2->SetInput(imageOverlay(imageLine(), mivi->convertITKtoVTK(mivi->getImage2D(frame, 0, scrollSagittal->value()-1)), 0.8));
   viewer2->GetRenderWindow()->Render();
}

void MivAxialRegionEditor::actionOk() {
   saveData(this->frame);
   bool mareVal = validadeMAREValues();
   bool valFields = validateFields();
   if (mareVal && valFields) {
      saveToImageStorage();
      this->accept();
   }
   else {
      if (!mareVal) {
         QMessageBox::warning(this, QObject::tr("Blank fields"),
             QObject::tr("Please, fill all blank fields from all frames."),
             QMessageBox::Ok);
      }
      else if (!valFields) {
         QMessageBox::warning(this, QObject::tr("Values out of range"),
             QObject::tr("Please, set the values in the correct range."),
             QMessageBox::Ok);
      }
   }
}

void MivAxialRegionEditor::actionCancel() {
   this->reject();
}

MivAxialRegionEditor::~MivAxialRegionEditor() {
   delete(mValues);
}
