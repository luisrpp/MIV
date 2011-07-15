/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: february 2009
****************************************************************/

#include <QMainWindow>
#include <QDialog>
#include "ui_mivpreview.h"

#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkCamera.h"
#include "vtkImageViewer2.h"
#include "vtkImageActor.h"

class MivPreview : public QDialog, public Ui_frmPreview {
   Q_OBJECT
   public:
      MivPreview(vtkImageData*, int, int);
};
