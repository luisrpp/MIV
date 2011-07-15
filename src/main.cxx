/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: september 2008
****************************************************************/

#include <QApplication>
#include "mivgui.h"

int main(int argc, char *argv[])
{

   QApplication app(argc, argv);
   MivGui* window = new MivGui;
   window->show();

   return app.exec();

}
