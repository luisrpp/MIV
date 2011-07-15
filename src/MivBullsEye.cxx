    /****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: september 2010
****************************************************************/

#include "MivBullsEye.h"
#include "vtkRegularPolygonSource.h"

MivBullsEye::MivBullsEye(MivImage *mivi, int frame) {
   this->mivi = mivi;
   this->frame = frame;
   
   vector<int> delim = mivi->getAxialSliceDelimitations(frame);
   startSlice = delim[0];
   endSlice = delim[1];
   useSegmentedImage = false;
   delim.clear();
   
   //numberOfDirections = 360;
   numberOfDirections = 1800;

   axialAxis = mivi->getAxialAxis(frame);
}

void MivBullsEye::setStartSlice(int start_slice) {
   startSlice = start_slice;
}

void MivBullsEye::setEndSlice(int end_slice) {
   endSlice = end_slice;
}

void MivBullsEye::setAxisCoords(AxisCoords coords) {
   axialAxis = coords;
}

void MivBullsEye::setUseSegmentedImage(bool useSegImage) {
   useSegmentedImage = useSegImage;
}

void MivBullsEye::setNumberOfDirections(int n_directions) {
   numberOfDirections = n_directions;
}

void MivBullsEye::generateBullsEye() {
   vector<ImageType2D::PixelType*> be_matrix;
   
   // Build Bull's Eye Matrix
   for(int i = startSlice; i <= endSlice; i++) {
      ImageType2D::Pointer slice;
      if (useSegmentedImage) {
         slice = mivi->getSegmented(frame, i);
      }
      else {
         slice = mivi->getImage2D(frame, 2, i);
      }
      
      ImageType2D::PixelType *sectors = new ImageType2D::PixelType[numberOfDirections];
      int direction = 0;
      
      for(double angle = 0; angle < (2.0*vnl_math::pi)-2.0*vnl_math::pi/(double)numberOfDirections; angle += 2.0*vnl_math::pi/(double)numberOfDirections) {
         
         ImageType2D::IndexType pixelIndex;
         
         sectors[direction] = 0;
         
         //for (double r=0; r < 100.0; r+=0.5) {
         for (double r=0; r < (float)mivi->getBullsEyeRadius(frame); r+=0.5) {
            pixelIndex[0] = (long int)(axialAxis.x + r*cos(angle));
            pixelIndex[1] = (long int)(axialAxis.y + r*sin(angle));
            
            ImageType2D::RegionType c_region = slice->GetLargestPossibleRegion();
            
            if(c_region.IsInside(pixelIndex)) {
               if (slice->GetPixel(pixelIndex) > sectors[direction])
                  sectors[direction] = slice->GetPixel(pixelIndex);
            }
            else {
               break;
            }
         }
         direction++;
      }
      be_matrix.push_back(sectors);
   }
   
   // Build Bull's Eye Image
   ImageType2D::Pointer be_image = ImageType2D::New();
   ImageType2D::IndexType start;
   start[0] = 0;  // first index on X
   start[1] = 0;  // first index on Y
   ImageType2D::SizeType  size;
   /*size[0] = be_matrix.size()*2+2;*/  // size along X
   /*size[1] = be_matrix.size()*2+2;*/  // size along Y
   size[0] = 300;  // size along X
   size[1] = 300;  // size along Y
   ImageType2D::RegionType region;
   region.SetSize(size);
   region.SetIndex(start);
   be_image->SetRegions(region);
   be_image->Allocate();
   // Set all pixels to 0
   be_image->FillBuffer(0);

   ImageType2D::IndexType pixelIndex;
   pixelIndex[0] = (long int)(size[0]/2);
   pixelIndex[1] = (long int)(size[1]/2);


   //drawCircle(be_image, pixelIndex, be_matrix.size()-1);
   int step = (int)(140/be_matrix.size());

   // Sectors
   for(int i = 0; i < be_matrix.size(); i++) {
      ImageType2D::PixelType* sectors = be_matrix[i];
      
      int direction = 0;
      for(double angle = 0; angle < (2.0*vnl_math::pi)-2.0*vnl_math::pi/(double)numberOfDirections; angle += 2.0*vnl_math::pi/(double)numberOfDirections) {
         
         int start = step*i;
         for(int k = 0; k < step; k++) {
            pixelIndex[0] = (long int)(((size[0]/2)+1) + (start+k)*cos(angle));
            pixelIndex[1] = (long int)(((size[1]/2)+1) + (start+k)*sin(angle));

            setPixel(be_image, pixelIndex, sectors[direction]);
         }
         
         direction++;
      }
   }
   
   mivi->setBullsEyeImage(be_image, frame);
   
   // TODO: Limpar memória!!
}

void MivBullsEye::drawCircle(ImageType2D::Pointer img, ImageType2D::IndexType idx, int radius) {
   int x0 = idx[0];
   int y0 = idx[1];
   int f = 1 - radius;
   int ddF_x = 1;
   int ddF_y = -2 * radius;
   int x = 0;
   int y = radius;

   ImageType2D::IndexType pixIndex = {{x0, y0 + radius}};
   setPixel(img, pixIndex, 255);
   ImageType2D::IndexType pixIndex1 = {{x0, y0 - radius}};
   setPixel(img, pixIndex1, 255);
   ImageType2D::IndexType pixIndex2 = {{x0 + radius, y0}};
   setPixel(img, pixIndex2, 255);
   ImageType2D::IndexType pixIndex3 = {{x0 - radius, y0}};
   setPixel(img, pixIndex3, 255);

   while(x < y) {
      ddF_x == 2 * x + 1;
      ddF_y == -2 * y;
      f == x*x + y*y - radius*radius + 2*x - y + 1;
      if(f >= 0) {
         y--;
         ddF_y += 2;
         f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x;
      ImageType2D::IndexType pixIndex5 = {{x0 + x, y0 + y}};
      setPixel(img, pixIndex5, 255);
      ImageType2D::IndexType pixIndex6 = {{x0 - x, y0 + y}};
      setPixel(img, pixIndex6, 255);
      ImageType2D::IndexType pixIndex7 = {{x0 + x, y0 - y}};
      setPixel(img, pixIndex7, 255);
      ImageType2D::IndexType pixIndex8 = {{x0 - x, y0 - y}};
      setPixel(img, pixIndex8, 255);
      ImageType2D::IndexType pixIndex9 = {{x0 + y, y0 + x}};
      setPixel(img, pixIndex9, 255);
      ImageType2D::IndexType pixIndex10 = {{x0 - y, y0 + x}};
      setPixel(img, pixIndex10, 255);
      ImageType2D::IndexType pixIndex11 = {{x0 + y, y0 - x}};
      setPixel(img, pixIndex11, 255);
      ImageType2D::IndexType pixIndex12 = {{x0 - y, y0 - x}};
      setPixel(img, pixIndex12, 255);
   }
}

void MivBullsEye::setPixel(ImageType2D::Pointer image, ImageType2D::IndexType pixelIndex, ImageType2D::PixelType pix) {
   ImageType2D::RegionType c_region = image->GetLargestPossibleRegion();

   if (c_region.IsInside(pixelIndex)) {
      image->SetPixel(pixelIndex, pix);
   }
}

/*void MivBullsEye::setSectorValues(ImageType2D::Pointer image, ImageType2D::IndexType center, vector<ImageType2D::PixelType*> be_matrix) {
   
   ImageType2D::SizeType size = image->GetLargestPossibleRegion().GetSize();
   int real_length = be_matrix.size();

   for (int i=0; i < size[0]; i++) {
      for (int j=0; j < size[1]; j++) {
         ImageType2D::IndexType index = {{i, j}};
         if (image->GetPixel(index) == 255) {
            // Get slice number
            int n_slice = (int)(distance(index, center)*real_length/100);
            if (n_slice >= real_length)
               n_slice = real_length-1;

            // Get the image
            ImageType2D::Pointer slice;
            if (useSegmentedImage) {
               slice = mivi->getSegmented(frame, n_slice);
            }
            else {
               slice = mivi->getImage2D(frame, 2, n_slice);
            }

            // Get pixel value
            double cos_a = (double)(index[0] - center[0])/(double)n_slice;
            double sin_a = (double)(index[1] - center[1])/(double)n_slice;

            ImageType2D::PixelType max_pix = 0;
            ImageType2D::IndexType slice_idx;
            for (double r=0; r < 100.0; r=r+0.01) {
               slice_idx[0] = (long int)(axialAxis.x + r*cos_a);
               slice_idx[1] = (long int)(axialAxis.y + r*sin_a);
               
               ImageType2D::RegionType c_region = slice->GetLargestPossibleRegion();
               
               if(c_region.IsInside(slice_idx)) {
                  if (slice->GetPixel(slice_idx) > max_pix)
                     max_pix = slice->GetPixel(slice_idx);
               }
               else {
                  break;
               }
            }
            image->SetPixel(index, max_pix);
         }
      }
   }
}*/

ImageType2D::PixelType MivBullsEye::distance(ImageType2D::IndexType p1, ImageType2D::IndexType p2) {
   ImageType2D::PixelType a, b, c;
   
   a = p1[0]-p2[0];
   b = p1[1]-p2[1];
   c = sqrt(a*a + b*b);
   
   return c;
}

MivBullsEye::~MivBullsEye() {
}
