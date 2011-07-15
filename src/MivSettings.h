/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: july 2010
 ****************************************************************/

#include <QMainWindow>
#include <QDialog>
#include "mivimage.h"
#include "ui_mivsettings.h"

class vtkEventQtSlotConnect;

class MivSettings : public QDialog, public Ui_frmMivSettings {
   Q_OBJECT
public:
   MivSettings(MivSettingsType*);
   
private:
   MivSettingsType* settings;
   
private slots:
   void actionOk();
   void actionCancel();
   void actionRadioBox();
};
