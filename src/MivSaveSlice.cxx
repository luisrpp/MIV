/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: january 2011
 ****************************************************************/

#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <stdio.h>

#include "MivSaveSlice.h"

MivSaveSlice::MivSaveSlice(MivImage* mivi, int frame, int sliceX, int sliceY, int sliceZ) {
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
   
   connect(this->buttonBox, SIGNAL(accepted()), this, SLOT(actionOk()));
   connect(this->buttonBox, SIGNAL(rejected()), this, SLOT(actionCancel()));
   
   this->mivi = mivi;
   this->frame = frame;
   this->sliceX = sliceX;
   this->sliceY = sliceY;
   this->sliceZ = sliceZ;
}

void MivSaveSlice::actionOk() {

   int orientation = 0;
   int type = 0;
   int slice = 0;
   
   if (radioAxial->isChecked()) {
      orientation = 2;
      slice = sliceZ;
   }
   if (radioCoronal->isChecked()) {
      orientation = 1;
      slice = sliceY;
   }
   if (radioSagittal->isChecked()) {
      orientation = 0;
      slice = sliceX;
   }
   
   if (radioOriginal->isChecked()) {
      type = 0;
   }
   if (radioMarker->isChecked()) {
      type = 1;
   }
   if (radioSegmented->isChecked()) {
      type = 3;
   }
   if (radioMask->isChecked()) {
      type = 2;
   }
   
   QString s = QFileDialog::getSaveFileName(
                    "./",
                    "Analyze 7.5 (*.hdr)",
                    this,
                    "save file dialog",
                    "Choose a filename to save under" );
   const char *fname = ((char *) s.latin1());

   if (!s.isEmpty()) {
      if (mivi->saveImage2D(orientation, type, frame, slice, fname) != EXIT_SUCCESS) {
         QMessageBox::about(this, QObject::tr("ERROR"),
            QObject::tr("<p><b>Error:</b> Unexpected error or no image found.</p>"));
      }
      else
         this->accept();
   }  
}

void MivSaveSlice::actionCancel() {
   this->reject();
}

