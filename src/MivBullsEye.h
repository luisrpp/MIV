/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: september 2010
 ****************************************************************/

#include "mivimage.h"

class MivBullsEye {
public:
   MivBullsEye(MivImage*, int);
   ~MivBullsEye();
   
   void setStartSlice(int);
   void setEndSlice(int);
   void setAxisCoords(AxisCoords);
   void setUseSegmentedImage(bool);
   
   void setNumberOfDirections(int);
   
   void generateBullsEye();
   
private:
   MivImage* mivi;
   int frame;
   
   int startSlice;
   int endSlice;
   AxisCoords axialAxis;
   bool useSegmentedImage;
   
   int numberOfDirections;
   
   void setPixel(ImageType2D::Pointer, ImageType2D::IndexType, ImageType2D::PixelType);
   void setSectorValues(ImageType2D::Pointer,ImageType2D::IndexType, vector<ImageType2D::PixelType*>);
   void drawCircle(ImageType2D::Pointer, ImageType2D::IndexType, int);

   ImageType2D::PixelType distance(ImageType2D::IndexType, ImageType2D::IndexType);
};
