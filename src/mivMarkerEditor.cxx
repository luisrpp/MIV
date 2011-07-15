/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: february 2009
****************************************************************/

#include <QDesktopWidget>
#include <QMessageBox>
#include <QDoubleValidator>

#include "mivMarkerEditor.h"
#include "stdio.h"
#include "math.h"

#include "vtkLookupTable.h"
#include "vtkImageMapToWindowLevelColors.h"

vtkImageInteractionCallback::vtkImageInteractionCallback() {
   this->Slicing = 0;
}

void vtkImageInteractionCallback::SetInteractor(vtkRenderWindowInteractor *interactor) {
   this->Interactor = interactor;
}

void vtkImageInteractionCallback::SetImageViewer(vtkImageViewer2* viewer) {
   this->viewer = viewer;
}

void vtkImageInteractionCallback::SetPicker(vtkPointPicker* picker) {
   this->picker = picker;
}

void vtkImageInteractionCallback::SetEditorGui(MivMarkerEditor* editor) {
   this->editorGui = editor;
}

void vtkImageInteractionCallback::line(vtkImageData* image, int x0, int y0, int x1, int y1, double value) {
   int Dx = x1 - x0; 
   int Dy = y1 - y0;
   int steep = (abs(Dy) >= abs(Dx));
   if (steep) {
      int aux;
      aux = x0;
      x0 = y0;
      y0 = aux;
      aux = x1;
      x1 = y1;
      y1 = aux;
      // recompute Dx, Dy after swap
      Dx = x1 - x0;
      Dy = y1 - y0;
   }
   int xstep = 1;
   if (Dx < 0) {
      xstep = -1;
      Dx = -Dx;
   }
   int ystep = 1;
   if (Dy < 0) {
      ystep = -1;
      Dy = -Dy; 
   }
   int TwoDy = 2*Dy; 
   int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
   int E = TwoDy - Dx; //2*Dy - Dx
   int y = y0;
   int xDraw, yDraw;
   for (int x = x0; x != x1; x += xstep) {
      if (steep) {
         xDraw = y;
         yDraw = x;
      }
      else {
         xDraw = x;
         yDraw = y;
      }
      // plot
      //image->SetScalarComponentFromDouble(xDraw, yDraw, 0, 0, value);
      plotPoint(image, xDraw, yDraw, this->editorGui->getPointerSize(), value);
      // next
      if (E > 0) {
         E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
         y = y + ystep;
      }
      else {
         E += TwoDy; //E += 2*Dy;
      }
   }
}

void vtkImageInteractionCallback::plotPoint(vtkImageData* image, int x, int y, int size, double value) {

   double nx, ny;
   double max_x, max_y;

   max_x = image->GetDimensions()[0];
   max_y = image->GetDimensions()[1];

   if (this->editorGui->isEraserChecked())
      value = 0;

   for(double angle = 0; angle <= 2*M_PI; angle += M_PI/60.0) {
      for (double i=0; i < size; i++) {
         nx = x + i*cos(angle);
         ny = y + i*sin(angle);

         if (nx < 0)
            nx = 0;
         if (nx >= max_x)
            nx = max_x - 1;
         if (ny < 0)
            ny = 0;
         if (ny >= max_y)
            ny = max_y - 1;

         image->SetScalarComponentFromDouble(nx, ny, 0, 0, value);
      }
   }

}

void vtkImageInteractionCallback::performAction(int control) {
   if (!picker->Pick(this->Interactor->GetEventPosition()[0],
                     this->Interactor->GetEventPosition()[1],
                     0,
                     viewer->GetRenderer())) {
      return;
   }
   double ptMapped[3];
   picker->GetMapperPosition(ptMapped);

   double dSpacing[3];
   viewer->GetInput()->GetSpacing(dSpacing);
   ptMapped[2] = viewer->GetSlice() * dSpacing[2];

   // Update Image
   vtkImageData* image = vtkImageData::New();
   image->DeepCopy(this->editorGui->getImageMarker());

   int nVolIdx = viewer->GetInput()->FindPoint(ptMapped);
   unsigned short* pPix = (unsigned short*)viewer->GetInput()->GetScalarPointer();
   int usPix = (int)pPix[nVolIdx];

   ptMapped[1] = image->GetDimensions()[1]+ptMapped[1];

   if (control == 0) {
      plotPoint(image, ptMapped[0], ptMapped[1], this->editorGui->getPointerSize(), this->editorGui->getColor());
      lastPoint[0] = ptMapped[0]; lastPoint[1] = ptMapped[1];
   }
   else {
      line(image, lastPoint[0], lastPoint[1], ptMapped[0], ptMapped[1], this->editorGui->getColor());
      lastPoint[0] = ptMapped[0]; lastPoint[1] = ptMapped[1];
   }

   image->Update();

   this->editorGui->setImageMarker(image);

   viewer->SetInput(this->editorGui->imageOverlay(this->editorGui->getImageMarker(), this->editorGui->getImage(), this->editorGui->opacity));

   viewer->Render();
}

void vtkImageInteractionCallback::Execute(vtkObject *, unsigned long event, void *) {
   if (event == vtkCommand::LeftButtonPressEvent) {
      this->Slicing = 1;
      this->editorGui->saveState();
      this->editorGui->clearRedo();
      performAction(0);
   }
   else if (event == vtkCommand::LeftButtonReleaseEvent) {
      this->Slicing = 0;
      performAction(1);
   }
   else if (event == vtkCommand::MouseMoveEvent) {
      if (this->Slicing) {
        performAction(2);
      }
   }
}

EditorState::EditorState(vtkImageData* image) {
   this->image = image;
}

EditorUndoRedo::EditorUndoRedo(int stack_size) {
   this->stack_size = stack_size;
}

void EditorUndoRedo::push(EditorState e_state) {
   m_stack.push_back(e_state);
   if (this->stack_size < m_stack.size()) {
      m_stack.erase(m_stack.begin());
   }
}

EditorState EditorUndoRedo::pop() {
   EditorState e_state = m_stack.back();
   m_stack.pop_back();
   return e_state;
}

bool EditorUndoRedo::isEmpty() {
   return m_stack.empty();
}

void EditorUndoRedo::push_redo(EditorState e_state) {
   m_redo_stack.push_back(e_state);
   if (this->stack_size < m_redo_stack.size()) {
      m_redo_stack.erase(m_redo_stack.begin());
   }
}

EditorState EditorUndoRedo::pop_redo() {
   EditorState e_state = m_redo_stack.back();
   m_redo_stack.pop_back();
   return e_state;
}

bool EditorUndoRedo::isRedoEmpty() {
   return m_redo_stack.empty();
}

void EditorUndoRedo::clearRedo() {
   m_redo_stack.clear();
}

MivMarkerEditor::MivMarkerEditor(MivImage* mivImage, int frame, int plane, int slice, OutputGui* output) {
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

   undoRedo = new EditorUndoRedo(15);

   this->mivi = mivImage;
   this->frame = frame;
   this->slice = slice;
   this->opacity = ((float)overlaySlider->value())/100;
   this->outputGui = output;

   cboColor->addItem("Background (50)", 0);
   cboColor->addItem("Left Ventricle (100)", 0);
   cboColor->addItem("Other (150)", 0);

   cboSize->addItem("1 px", 0 );
   cboSize->addItem("2 px", 0 );
   cboSize->addItem("3 px", 0 );
   cboSize->addItem("4 px", 0 );
   cboSize->addItem("5 px", 0 );
   cboSize->addItem("6 px", 0 );
   cboSize->addItem("7 px", 0 );
   cboSize->addItem("8 px", 0 );
   cboSize->addItem("9 px", 0 );
   cboSize->addItem("10 px", 0 );

   for (int i = 0; i < mivi->getSize(3); i++) {
      QString str;
      str.sprintf("Frame %d", i+1);
      cboMarkerFrame->addItem(str, 0);
   }

   for (int i = 0; i < mivi->getSize(2); i++) {
      if (mivi->existMarker(frame, i)) {
         QString str;
         str.sprintf("Slice %d", i+1);
         smap[str] = mivi->convertITKtoVTK(mivi->getMarker(frame, i));
         cboMarkerSlice->addItem(str, 0);
      }
   }

   QString str_frame;
   str_frame.sprintf("Frame %d", frame+1);
   cboMarkerFrame->setCurrentIndex(cboMarkerFrame->findText(str_frame, Qt::MatchExactly));

   QString str_slice;
   str_slice.sprintf("Slice %d", slice+1);
   cboMarkerSlice->setCurrentIndex(cboMarkerSlice->findText(str_slice, Qt::MatchExactly));

   // connections
   connect(cmdEraser, SIGNAL(clicked()), this, SLOT(actionEraser()));
   connect(cmdSave, SIGNAL(clicked()), this, SLOT(actionSave()));
   connect(cmdCancel, SIGNAL(clicked()), this, SLOT(actionCancel()));
   connect(cmdPreview, SIGNAL(clicked()), this, SLOT(actionPreview()));
   connect(cmdUndo, SIGNAL(clicked()), this, SLOT(actionUndo()));
   connect(cmdRedo, SIGNAL(clicked()), this, SLOT(actionRedo()));
   connect(cmdRemoveMarker, SIGNAL(clicked()), this, SLOT(actionRemove()));
   connect(boxInterpolation, SIGNAL(clicked()), this, SLOT(interpolate()));
   connect(overlaySlider, SIGNAL(valueChanged(int)), this, SLOT(overlayControl()));
   connect(cboColor, SIGNAL(currentIndexChanged(int)), this, SLOT(changeColor()));
   connect(cboMarkerFrame, SIGNAL(currentIndexChanged(int)), this, SLOT(selectFrame()));
   connect(cboMarkerSlice, SIGNAL(currentIndexChanged(int)), this, SLOT(selectSlice()));
   
   // Create LUTs
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

   // Prepare images
   setImage(mivi->convertITKtoVTK(mivi->getImage2D(frame, plane, slice)));
   setImageMarker(mivi->convertITKtoVTK(mivi->getMarker(frame, slice)));

   // Create Viewer
   viewer = vtkImageViewer2::New();
   viewer->SetInput(imageOverlay(marker, image, this->opacity));
   viewer->SetupInteractor(vtkWidget->GetRenderWindow()->GetInteractor());
   
   // Turn off interpolation
   viewer->GetImageActor()->InterpolateOff();
   
   // Set Render Window
   vtkWidget->SetRenderWindow(viewer->GetRenderWindow());

   // Set up the interaction
   vtkInteractorStyleImage *imageStyle = vtkInteractorStyleImage::New();
   vtkPointPicker* picker = vtkPointPicker::New();
   vtkWidget->GetRenderWindow()->GetInteractor()->SetPicker(picker);

   // Create callback
   vtkImageInteractionCallback *callback = vtkImageInteractionCallback::New();
   callback->SetInteractor(vtkWidget->GetRenderWindow()->GetInteractor());
   callback->SetPicker(picker);
   callback->SetImageViewer(viewer);
   callback->SetEditorGui(this);
   imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
   imageStyle->AddObserver(vtkCommand::LeftButtonPressEvent, callback);
   imageStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, callback);
   
   vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(imageStyle);
   
   viewer->GetRenderer()->ResetCamera();
}

vtkImageData* MivMarkerEditor::convertToRGB(vtkImageData* img, vtkLookupTable* lut) {
   vtkImageMapToColors *img_rgb = vtkImageMapToColors::New();
   img_rgb->SetInput(img);
   img_rgb->SetLookupTable(lut);
   img_rgb->SetOutputFormatToRGBA();
   img_rgb->Update();
   return img_rgb->GetOutput();
}

void MivMarkerEditor::enableUndo() {
   cmdUndo->setEnabled(true);
}

int MivMarkerEditor::getPointerSize() {
   return cboSize->currentIndex()+1;
}

int MivMarkerEditor::isEraserChecked() {
   return cmdEraser->isChecked();
}

void MivMarkerEditor::actionEraser() {
   if (cmdEraser->isChecked()) {
      vtkWidget->setCursor(QCursor(Qt::OpenHandCursor));
   }
   else {
      vtkWidget->setCursor(QCursor(Qt::ArrowCursor));
   }
}

void MivMarkerEditor::actionSave() {
   this->outputGui->setImage(this->marker);
   this->outputGui->remove = false;
   this->accept();
}

void MivMarkerEditor::changeColor() {
   cmdEraser->setChecked(false);
   if (cmdEraser->isChecked()) {
      vtkWidget->setCursor(QCursor(Qt::OpenHandCursor));
   }
   else {
      vtkWidget->setCursor(QCursor(Qt::ArrowCursor));
   }
}

void MivMarkerEditor::actionCancel() {
   this->reject();
}

void MivMarkerEditor::actionPreview() {
   int window, level;
   window = mivi->getMaximumValue() - mivi->getMinimumValue();
   level = (0.5 * (mivi->getMaximumValue() + mivi->getMinimumValue()));

   vtkImageData* sprev = this->mivi->convertITKtoVTK(
                                  this->mivi->performWatershed(
                                     this->mivi->convertVTKtoITK(image),
                                     this->mivi->convertVTKtoITK(marker)));

   MivPreview* preview = new MivPreview(imageOverlay(sprev, getImage(), 0.5), window, level);
   preview->exec();

   delete(preview);
}

void MivMarkerEditor::actionUndo() {
   undoRedo->push_redo(getImageMarker());
   EditorState e_state = undoRedo->pop();
   setImageMarker(e_state.image);
   viewer->SetInput(imageOverlay(getImageMarker(), getImage(), opacity));
   viewer->Render();
   if (undoRedo->isEmpty()) {
      cmdUndo->setEnabled(false);
   }
   cmdRedo->setEnabled(true);
}

void MivMarkerEditor::actionRedo() {
   undoRedo->push(getImageMarker());
   EditorState e_state = undoRedo->pop_redo();
   setImageMarker(e_state.image);
   viewer->SetInput(imageOverlay(getImageMarker(), getImage(), opacity));
   viewer->Render();
   if (undoRedo->isRedoEmpty()) {
      cmdRedo->setEnabled(false);
   }
   cmdUndo->setEnabled(true);
}

void MivMarkerEditor::actionRemove() {
   int ret = QMessageBox::question(this, QObject::tr("Remove marker"),
             QObject::tr("Are you sure you want to remove this marker?"),
             QMessageBox::Yes | QMessageBox::No);
   if (ret == QMessageBox::Yes) {
      this->outputGui->remove = true;
      this->mivi->removeMarker(frame, slice);
      this->accept();
   }
}

void MivMarkerEditor::saveState() {
   enableUndo();
   EditorState e_state(getImageMarker());
   undoRedo->push(e_state);
}

void MivMarkerEditor::clearRedo() {
   undoRedo->clearRedo();
   cmdRedo->setEnabled(false);
}

void MivMarkerEditor::interpolate() {
   if (boxInterpolation->isChecked()) {
      viewer->GetImageActor()->InterpolateOn();
   }
   else {
      viewer->GetImageActor()->InterpolateOff();
   }
   viewer->GetRenderWindow()->Render();
}

void MivMarkerEditor::overlayControl() {
   this->opacity = ((float)overlaySlider->value())/100;
   viewer->SetInput(imageOverlay(getImageMarker(), getImage(), this->opacity));
   viewer->GetRenderWindow()->Render();
}

void MivMarkerEditor::selectFrame() {
   smap.clear();
   cboMarkerSlice->clear();
   for (int i = 0; i < mivi->getSize(2); i++) {
      if (mivi->existMarker(cboMarkerFrame->currentIndex(), i)) {
         QString str;
         str.sprintf("Slice %d", i+1);
         smap[str] = mivi->convertITKtoVTK(mivi->getMarker(cboMarkerFrame->currentIndex(), i));
         cboMarkerSlice->addItem(str, 0);
      }
   }
}

void MivMarkerEditor::selectSlice() {
   saveState();
   setImageMarker(smap[cboMarkerSlice->currentText()]);
   viewer->SetInput(imageOverlay(marker, image, this->opacity));
   viewer->GetRenderWindow()->Render();
}

float MivMarkerEditor::getColor() {
   float color = 0;
   switch (cboColor->currentIndex()) {
      case 0:
         color = BACKGROUND;
         break;
      case 1:
         color = LEFTVENTRICLE;
         break;
      case 2:
         color = OTHER;
         break;
   }
   return color;
}

vtkImageData* MivMarkerEditor::imageOverlay(vtkImageData* img1, vtkImageData* img2, float opacity) {

   vtkImageBlend* blend = vtkImageBlend::New();
   blend->RemoveAllInputs();
   blend->AddInput(convertToRGB(img2,lutGray));
   blend->AddInput(convertToRGB(img1, lutMarker));
   blend->SetBlendModeToNormal();
   blend->Update();
   return blend->GetOutput();
}

void MivMarkerEditor::setImage(vtkImageData* Image) {
   this->image = Image;
}

void MivMarkerEditor::setImageMarker(vtkImageData* iMarker) {
   this->marker = iMarker;
}

vtkImageData* MivMarkerEditor::getImage() {
   return this->image;
}

vtkImageData* MivMarkerEditor::getImageMarker() {
   return this->marker;
}

MivMarkerEditor::~MivMarkerEditor()
{
}

OutputGui::OutputGui() {
   //TODO
}

void OutputGui::setImage(vtkImageData* image) {
   this->image = image;
}

vtkImageData* OutputGui::getImage() {
   return this->image;
}
