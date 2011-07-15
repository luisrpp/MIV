/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: january 2011
 ****************************************************************/

#include <QDesktopWidget>
#include <stdio.h>

#include "MivAdjustAxialSlice.h"

MivAdjustAxialSlice::MivAdjustAxialSlice(MivImage* mivi) {
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
   
   for (int i = 0; i < mivi->getSize(3); i++) {
      QString str;
      str.sprintf("%d", i+1);
      cboFrame->addItem(str, 0);
   }

   for (int i = 0; i < mivi->getSize(2); i++) {
      QString str;
      str.sprintf("%d", i+1);
      cboStartSlice->addItem(str, 0);
      cboEndSlice->addItem(str, 0);
   }
   
   this->mivi = mivi;
}

void MivAdjustAxialSlice::actionOk() {
   mivi->setAxialSliceDelimitations(cboFrame->currentIndex(), cboStartSlice->currentIndex(), cboEndSlice->currentIndex());
   this->accept();  
}

void MivAdjustAxialSlice::actionCancel() {
   this->reject();
}

