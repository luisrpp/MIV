/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: february 2009
****************************************************************/

#include <QDesktopWidget>
#include "mivpreview.h"

MivPreview::MivPreview(vtkImageData* image, int window, int level) {

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

   vtkImageViewer2 *viewer = vtkImageViewer2::New();
   viewer->SetColorLevel(level);
   viewer->SetColorWindow(window);
   viewer->SetInput(image);
   viewer->GetImageActor()->InterpolateOff();

   vtkInteractorStyleImage *imageStyle = vtkInteractorStyleImage::New();

   vtkWidget->SetRenderWindow(viewer->GetRenderWindow());
   viewer->SetupInteractor(vtkWidget->GetRenderWindow()->GetInteractor());
   vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(imageStyle);

   viewer->GetRenderer()->ResetCamera();
   viewer->GetRenderWindow()->Render();

   vtkWidget->show();
}

