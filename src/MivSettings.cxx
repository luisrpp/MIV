/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: july 2010
 ****************************************************************/

#include <QDesktopWidget>
#include <stdio.h>

#include "MivSettings.h"

MivSettings::MivSettings(MivSettingsType* settingsType) {
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
   connect(this->radioOtsu, SIGNAL(clicked()), this, SLOT(actionRadioBox()));
   connect(this->radioSimple, SIGNAL(clicked()), this, SLOT(actionRadioBox()));
   
   this->settings = settingsType;
   
   if (this->settings->lut) {
      this->cboLUT->setCheckState(Qt::Checked);
   }
   else {
      this->cboLUT->setCheckState(Qt::Unchecked);
   }
   
   if (this->settings->interpolate) {
      this->cboInterpolate->setCheckState(Qt::Checked);
   }
   else {
      this->cboInterpolate->setCheckState(Qt::Unchecked);
   }
   
   if (this->settings->axialDelimitations) {
      this->cboDelim->setCheckState(Qt::Checked);
   }
   else {
      this->cboDelim->setCheckState(Qt::Unchecked);
   }
   
   if (this->settings->checkAxis) {
      this->cboAxis->setCheckState(Qt::Checked);
   }
   else {
      this->cboAxis->setCheckState(Qt::Unchecked);
   }
   
   if (this->settings->thresholdMethod == 0) {
      this->lblPercentage->setEnabled(true);
      this->horizontalSlider->setEnabled(true);
      this->lcdNumber->setEnabled(true);
      this->radioOtsu->setChecked(true);
      this->lblPercentage->setEnabled(false);
      this->horizontalSlider->setEnabled(false);
      this->lcdNumber->setEnabled(false);
   }
   if (this->settings->thresholdMethod == 1) {
      this->radioSimple->setChecked(true);
      this->lblPercentage->setEnabled(true);
      this->horizontalSlider->setEnabled(true);
      this->lcdNumber->setEnabled(true);
      this->horizontalSlider->setValue(this->settings->thresholdValue);
   }
   
   if (this->settings->useSegmentedImage) {
      this->cboSegmentedImage->setCheckState(Qt::Checked);
   }
   else {
      this->cboSegmentedImage->setCheckState(Qt::Unchecked);
   }
}

void MivSettings::actionOk() {
   if (this->cboLUT->checkState() == Qt::Checked)
      this->settings->lut = true;
   else
      this->settings->lut = false;

   if (this->cboInterpolate->checkState() == Qt::Checked)
      this->settings->interpolate = true;
   else
      this->settings->interpolate = false;

   if (this->cboDelim->checkState() == Qt::Checked)
      this->settings->axialDelimitations = true;
   else
      this->settings->axialDelimitations = false;
   
   if (this->cboAxis->checkState() == Qt::Checked)
      this->settings->checkAxis = true;
   else
      this->settings->checkAxis = false;
   
   if (this->radioSimple->isChecked()) {
      this->settings->thresholdMethod = 1;
      this->settings->thresholdValue = this->horizontalSlider->value();
   }
   else {
      this->settings->thresholdMethod = 0;
   }
   
   if (this->cboSegmentedImage->checkState() == Qt::Checked)
      this->settings->useSegmentedImage = true;
   else 
      this->settings->useSegmentedImage = false;
   
   this->accept();
}

void MivSettings::actionCancel() {
   this->reject();
}

void MivSettings::actionRadioBox(){
   if (this->radioSimple->isChecked()) {
      this->lblPercentage->setEnabled(true);
      this->horizontalSlider->setEnabled(true);
      this->lcdNumber->setEnabled(true);
   }
   else {
      this->lblPercentage->setEnabled(false);
      this->horizontalSlider->setEnabled(false);
      this->lcdNumber->setEnabled(false);
   }
}
