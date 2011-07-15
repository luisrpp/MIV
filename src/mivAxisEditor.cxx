/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: march 2009
****************************************************************/

#include <QDesktopWidget>
#include <QMessageBox>
#include <QValidator>
#include <QDoubleValidator>
#include <QIntValidator>
#include <sys/types.h>
#include <sys/stat.h>

#include "mivAxisEditor.h"

int saveRGBImageBE(ImageType2D::Pointer image, AxisCoords c, int raio, const char* fileName) {
   RGBImageType::Pointer  localOutputImage = RGBImageType::New();

   RGBImageType::RegionType region;
   region.SetSize(image->GetLargestPossibleRegion().GetSize());
   region.SetIndex(image->GetLargestPossibleRegion().GetIndex());
   localOutputImage->SetRegions( region );
   localOutputImage->SetOrigin(image->GetOrigin());
   localOutputImage->SetSpacing(image->GetSpacing());
   localOutputImage->Allocate();

   RescaleType::Pointer scaleFilter = RescaleType::New();
   scaleFilter->SetInput(image);
   scaleFilter->SetOutputMinimum(0);
   scaleFilter->SetOutputMaximum(255);
   scaleFilter->Update();
   ChannelImageType::Pointer channelImage = scaleFilter->GetOutput();
   IterType iter(channelImage, channelImage->GetLargestPossibleRegion());

   // Copy content from input image to rgb image
   for(iter.GoToBegin(); ! iter.IsAtEnd(); ++iter) {
      ChannelType pixel = iter.Get();
      IndexType idx = iter.GetIndex();
      RGBPixelType rgbPixel;
      rgbPixel = pixel, pixel, pixel;
      localOutputImage->SetPixel(idx, rgbPixel);
   }

   RGBPixelType bluePix;
   bluePix[0] = 255;
   bluePix[1] = 10;
   bluePix[2] = 10;

   RGBImageType::IndexType localIndex;
   localIndex[0] = c.x;
   localIndex[1] = c.y;

   localOutputImage->SetPixel(localIndex, bluePix);

   ImageType2D::SizeType size = image->GetLargestPossibleRegion().GetSize();
   for(double angle = 0; angle <= 2*vnl_math::pi; angle += vnl_math::pi/60.0) {
      ImageType2D::IndexType pixelIndex;
      localIndex[0] = (int)(c.x + raio*cos(angle));
      localIndex[1] = (int)(c.y + raio*sin(angle));

      RGBImageType::RegionType c_region = localOutputImage->GetLargestPossibleRegion();
      if(c_region.IsInside(localIndex)) {
         localOutputImage->SetPixel(localIndex, bluePix);
      }
   }

   typedef RGBFlip2D::FlipAxesArrayType FlipAxesArrayType;
   FlipAxesArrayType flipArray;
   flipArray[0] = 0;
   flipArray[1] = 1;

   RGBFlip2D::Pointer flip = RGBFlip2D::New();
   flip->SetInput(localOutputImage);
   flip->SetFlipAxes(flipArray);
   flip->FlipAboutOriginOn();
   flip->Update();
   localOutputImage = flip->GetOutput();

   RGBWriterType::Pointer rgbWriter = RGBWriterType::New();
   rgbWriter->SetFileName(fileName);
   rgbWriter->SetInput(localOutputImage);
   try {
      rgbWriter->Update();
   }
   catch( itk::ExceptionObject & excep ) {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
   }

   return EXIT_SUCCESS;
}

AxisEditorState::AxisEditorState(int frame, int slice, AxisCoords coords) {
   this->frame = frame;
   this->slice = slice;
   this->coords = coords;
}

AxisEditorUndoRedo::AxisEditorUndoRedo(int stack_size) {
   this->stack_size = stack_size;
}

void AxisEditorUndoRedo::push(AxisEditorState e_state) {
   m_stack.push_back(e_state);
   if (this->stack_size < m_stack.size()) {
      m_stack.erase(m_stack.begin());
   }
}

AxisEditorState AxisEditorUndoRedo::pop() {
   AxisEditorState e_state = m_stack.back();
   m_stack.pop_back();
   return e_state;
}

bool AxisEditorUndoRedo::isEmpty() {
   return m_stack.empty();
}

void AxisEditorUndoRedo::push_redo(AxisEditorState e_state) {
   m_redo_stack.push_back(e_state);
   if (this->stack_size < m_redo_stack.size()) {
      m_redo_stack.erase(m_redo_stack.begin());
   }
}

AxisEditorState AxisEditorUndoRedo::pop_redo() {
   AxisEditorState e_state = m_redo_stack.back();
   m_redo_stack.pop_back();
   return e_state;
}

bool AxisEditorUndoRedo::isRedoEmpty() {
   return m_redo_stack.empty();
}

void AxisEditorUndoRedo::clearRedo() {
   m_redo_stack.clear();
}

vtkAxisInteractionCallback::vtkAxisInteractionCallback() {
   this->Moving = 0;
}

void vtkAxisInteractionCallback::SetInteractor(vtkRenderWindowInteractor *interactor) {
   this->Interactor = interactor;
}

void vtkAxisInteractionCallback::SetImageViewer(vtkImageViewer2* viewer) {
   this->viewer = viewer;
}

void vtkAxisInteractionCallback::SetPicker(vtkPointPicker* picker) {
   this->picker = picker;
}

void vtkAxisInteractionCallback::SetEditorGui(MivAxisEditor* editor) {
   this->editorGui = editor;
}

void vtkAxisInteractionCallback::performAction() {
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

   ptMapped[1] = viewer->GetInput()->GetDimensions()[1]+ptMapped[1];

   int max_x = viewer->GetInput()->GetDimensions()[0];
   int max_y = viewer->GetInput()->GetDimensions()[1];

   if (ptMapped[0] < 0)
      ptMapped[0] = 0;
   if (ptMapped[0] >= max_x)
      ptMapped[0] = max_x - 1;
   if (ptMapped[1] < 0)
      ptMapped[1] = 0;
   if (ptMapped[1] >= max_y)
      ptMapped[1] = max_y - 1;

   editorGui->coords[editorGui->getFrame()].x = ptMapped[0];
   editorGui->coords[editorGui->getFrame()].y = ptMapped[1];

   viewer->SetInput(editorGui->imageOverlay(
                                  editorGui->drawAxis(editorGui->getFrame()),
                                  editorGui->getCurrentImage(), 0.5));
   viewer->Render();
}

void vtkAxisInteractionCallback::Execute(vtkObject *, unsigned long event, void *) {
   if (event == vtkCommand::LeftButtonPressEvent) {
      this->Moving = 1;
      this->editorGui->saveState();
      this->editorGui->clearRedo();
      performAction();
   }
   else if (event == vtkCommand::LeftButtonReleaseEvent) {
      this->Moving = 0;
      performAction();
   }
   else if (event == vtkCommand::MouseMoveEvent) {
      if (this->Moving) {
        performAction();
      }
   }
}

MivAxisEditor::MivAxisEditor(MivImage* mivImage) {
   this->setupUi(this);

   QDesktopWidget *desktop = QApplication::desktop();
   QValidator *validator = new QIntValidator(100, 999, this);
   txtRadius->setValidator(validator);

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
   this->undoRedo = new AxisEditorUndoRedo(15);

   for (int i = 0; i < mivi->getSize(3); i++) {
      QString str;
      str.sprintf("Frame %d", i+1);
      cboFrame->addItem(str, 0);
   }

   // Erase this!!
   /*char *file = new char[255];
   mkdir("/home/luisrpp/Desktop/miv", 0777);
   sprintf (file, "/home/luisrpp/Desktop/miv");
   mkdir(file, 0777);*/
   // End Erase this!!

   frame = cboFrame->currentIndex();
   for (int i = 0; i < mivi->getSize(2); i++) {
      if (mivi->isSegmented(frame, i)) {
         QString str;
         str.sprintf("Slice %d", i+1);
         smap[str] = mivi->convertITKtoVTK(mivi->getSegmented(frame, i));
         cboSlice->addItem(str, 0);
      }


      // Erase this!!
      /*if (mivi->isSegmented(frame, i)) {
         sprintf (file, "/home/luisrpp/Desktop/miv/img_%d_%d.png", frame, i);
         saveRGBImageBE(mivi->getSegmented(frame, i), mivi->getAxialAxis(frame), mivi->getBullsEyeRadius(frame), file);
      }*/
      // End Erase this!!
   }

   connect(cboFrame, SIGNAL(currentIndexChanged(int)), this, SLOT(selectFrame()));
   connect(cboSlice, SIGNAL(currentIndexChanged(int)), this, SLOT(selectSlice()));
   connect(cmdUndo, SIGNAL(clicked()), this, SLOT(actionUndo()));
   connect(cmdRedo, SIGNAL(clicked()), this, SLOT(actionRedo()));
   connect(cmdSave, SIGNAL(clicked()), this, SLOT(actionSave()));
   connect(cmdCancel, SIGNAL(clicked()), this, SLOT(actionCancel()));
   connect(cmdSetRadius, SIGNAL(clicked()), this, SLOT(actionSetRadius()));

   int window, level;
   window = mivi->getMaximumValue() - mivi->getMinimumValue();
   level = (0.5 * (mivi->getMaximumValue() + mivi->getMinimumValue()));

   loadCoords();
   QString radius_str;
   radius_str.sprintf("%d", radius[frame]);
   txtRadius->setText(radius_str);

   this->currentImage = smap[cboSlice->currentText()];

   QString initialSlice;
   viewer = vtkImageViewer2::New();
   viewer->SetColorLevel(level);
   viewer->SetColorWindow(window);
   viewer->SetInput(imageOverlay(drawAxis(cboFrame->currentIndex()), this->currentImage, 0.5));
   viewer->GetImageActor()->InterpolateOff();

   vtkWidget->SetRenderWindow(viewer->GetRenderWindow());

   // Set up the interaction
   vtkInteractorStyleImage *imageStyle = vtkInteractorStyleImage::New();

   vtkPointPicker* picker = vtkPointPicker::New();
   vtkWidget->GetRenderWindow()->GetInteractor()->SetPicker(picker);

   vtkAxisInteractionCallback *callback = vtkAxisInteractionCallback::New();
   callback->SetInteractor(vtkWidget->GetRenderWindow()->GetInteractor());
   callback->SetPicker(picker);
   callback->SetImageViewer(viewer);
   callback->SetEditorGui(this);
   imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
   imageStyle->AddObserver(vtkCommand::LeftButtonPressEvent, callback);
   imageStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, callback);

   viewer->SetupInteractor(vtkWidget->GetRenderWindow()->GetInteractor());
   vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(imageStyle);

   viewer->GetRenderer()->ResetCamera();
   viewer->GetRenderWindow()->Render();

   vtkWidget->show();
}

void MivAxisEditor::enableUndo() {
   cmdUndo->setEnabled(true);
}

void MivAxisEditor::saveState() {
   enableUndo();
   AxisEditorState e_state(cboFrame->currentIndex(), cboSlice->currentIndex(), coords[frame]);
   undoRedo->push(e_state);
}

void MivAxisEditor::clearRedo() {
   undoRedo->clearRedo();
   cmdRedo->setEnabled(false);
}

void MivAxisEditor::actionUndo() {
   AxisEditorState current_state(cboFrame->currentIndex(), cboSlice->currentIndex(), coords[frame]);
   undoRedo->push_redo(current_state);
   AxisEditorState e_state = undoRedo->pop();
   coords[e_state.frame] = e_state.coords;
   changeCbox(e_state.frame, e_state.slice);
   if (undoRedo->isEmpty()) {
      cmdUndo->setEnabled(false);
   }
   cmdRedo->setEnabled(true);
}

void MivAxisEditor::actionRedo() {
   AxisEditorState current_state(cboFrame->currentIndex(), cboSlice->currentIndex(), coords[frame]);
   undoRedo->push(current_state);
   AxisEditorState e_state = undoRedo->pop_redo();
   coords[e_state.frame] = e_state.coords;
   changeCbox(e_state.frame, e_state.slice);
   if (undoRedo->isRedoEmpty()) {
      cmdRedo->setEnabled(false);
   }
   cmdUndo->setEnabled(true);
}

void MivAxisEditor::actionSetRadius() {
   bool result;
   radius[cboFrame->currentIndex()] = txtRadius->text().toInt(&result, 10);
   selectSlice();
}

void MivAxisEditor::actionCancel() {
   this->reject();
}

void MivAxisEditor::actionSave() {
   for (int i = 0; i < mivi->getSize(3); i++) {
      mivi->setAxialAxis(i, coords[i].x, coords[i].y);
      mivi->setBullsEyeRadius(i, radius[i]);
   }
   this->accept();
}

int MivAxisEditor::getFrame() {
   return frame;
}

vtkImageData* MivAxisEditor::getCurrentImage() {
   return this->currentImage;
}

void MivAxisEditor::loadCoords() {
   for (int i = 0; i < mivi->getSize(3); i++) {
      coords.push_back(mivi->getAxialAxis(i));
      radius.push_back(mivi->getBullsEyeRadius(i));
   }
}

vtkImageData* MivAxisEditor::drawAxis(int frame) {
   vtkImageData* img1 = this->currentImage;
   vtkImageData* axisImage = vtkImageData::New();
   axisImage->DeepCopy(img1);
   axisImage->SetScalarComponentFromDouble(coords[frame].x, coords[frame].y, 0, 0, 1000);

   for(double angle = 0; angle <= 2*vnl_math::pi; angle += vnl_math::pi/60.0) {
      axisImage->SetScalarComponentFromDouble(coords[frame].x + radius[frame]*cos(angle), coords[frame].y + radius[frame]*sin(angle), 0, 0, 1000);
   }

   axisImage->Update();

   return axisImage;
}

void MivAxisEditor::changeCbox(int frame, int slice) {
   cboFrame->setCurrentIndex(frame);
   selectFrame();
   cboSlice->setCurrentIndex(slice);
   selectSlice();
}

void MivAxisEditor::selectFrame() {
   smap.clear();
   cboSlice->clear();
   frame = cboFrame->currentIndex();
   int initial = 0;
   for (int i = 0; i < mivi->getSize(2); i++) {
      if (mivi->isSegmented(cboFrame->currentIndex(), i)) {
         if (initial == 0) {
            this->currentImage = mivi->convertITKtoVTK(mivi->getSegmented(cboFrame->currentIndex(), i));
            initial = 1;
         }
         QString str;
         str.sprintf("Slice %d", i+1);
         smap[str] = mivi->convertITKtoVTK(mivi->getSegmented(cboFrame->currentIndex(), i));
         cboSlice->addItem(str, 0);
      }
   }
   QString radius_str;
   radius_str.sprintf("%d", radius[frame]);
   txtRadius->setText(radius_str);
}

void MivAxisEditor::selectSlice() {
   if (cboSlice->currentText().count() != 0) {
      this->currentImage = smap[cboSlice->currentText()];
      viewer->SetInput(imageOverlay(drawAxis(cboFrame->currentIndex()), this->currentImage, 0.5));
      viewer->GetRenderWindow()->Render();
   }
}

vtkImageData* MivAxisEditor::imageOverlay(vtkImageData* img1, vtkImageData* img2, float opacity) {

   vtkImageBlend* blend = vtkImageBlend::New();
   blend->RemoveAllInputs();
   blend->AddInput(img1);
   blend->AddInput(img2);
   blend->SetOpacity(1, opacity);
   blend->Update();
   return blend->GetOutput();
}

MivAxisEditor::~MivAxisEditor() {
   delete(undoRedo);
}
