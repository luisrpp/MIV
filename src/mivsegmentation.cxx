/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: november 2008
 ****************************************************************/

#include "mivsegmentation.h"
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>

// Copy image structure.
ImageType2D::Pointer copyImageStructure(const ImageType2D::Pointer input) {
   
   ImageType2D::Pointer image = ImageType2D::New();
   
   ImageType2D::RegionType region;
   region.SetSize(input->GetLargestPossibleRegion().GetSize());
   region.SetIndex(input->GetLargestPossibleRegion().GetIndex());
   image->SetRegions( region );
   image->SetOrigin(input->GetOrigin());
   image->SetSpacing(input->GetSpacing());
   image->Allocate();
   /* Set all pixels to 0 */
   image->FillBuffer(0);
   
   return image;
}

int saveRGBImage(ImageType2D::Pointer img, const char* fileName) {
   RGBImageType::Pointer  localOutputImage = RGBImageType::New();

   typedef Flip2D::FlipAxesArrayType FlipAxesArrayType;
   FlipAxesArrayType flipArray;
   flipArray[0] = 0;
   flipArray[1] = 1;

   Flip2D::Pointer flip = Flip2D::New();
   flip->SetInput(img);
   flip->SetFlipAxes(flipArray);
   flip->FlipAboutOriginOn();
   flip->Update();
   ImageType2D::Pointer image = flip->GetOutput();

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

int saveRGBImageCircle(ImageType2D::Pointer image, const char* fileName, Circle* circle, bool draw_circle) {
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
   bluePix[0] = 200;
   bluePix[1] = 5;
   bluePix[2] = 5;

   RGBImageType::IndexType localIndex;

   for(double angle = 0; angle <= 2*vnl_math::pi && draw_circle; angle += vnl_math::pi/180.0) {
      localIndex[0] = (long int)(circle->x + circle->r*cos(angle));
      localIndex[1] = (long int)(circle->y + circle->r*sin(angle));

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

int saveRGBImageMarker(ImageType2D::Pointer image, const char* fileName, ImageType2D::Pointer marker) {
   RGBImageType::Pointer localOutputImage = RGBImageType::New();

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
   RGBImageType::IndexType localIndex;

   ImageType2D::SizeType size = marker->GetLargestPossibleRegion().GetSize();
   for (int x=0; x < size[0]; x++) {
      for (int y=0; y < size[1]; y++) {
         ImageType2D::IndexType midx;
         midx[0] = x;
         midx[1] = y;
         if (marker->GetPixel(midx) == LEFTVENTRICLE) {
            bluePix[0] = 255;
            bluePix[1] = 10;
            bluePix[2] = 10;
            RGBImageType::RegionType c_region = localOutputImage->GetLargestPossibleRegion();
            localIndex[0] = midx[0];
            localIndex[1] = midx[1];
            if(c_region.IsInside(localIndex)) {
               localOutputImage->SetPixel(localIndex, bluePix);
            }
         }
         if (marker->GetPixel(midx) == BACKGROUND || marker->GetPixel(midx) == OTHER) {
            bluePix[0] = 150;
            bluePix[1] = 10;
            bluePix[2] = 10;
            RGBImageType::RegionType c_region = localOutputImage->GetLargestPossibleRegion();
            localIndex[0] = midx[0];
            localIndex[1] = midx[1];
            if(c_region.IsInside(localIndex)) {
               localOutputImage->SetPixel(localIndex, bluePix);
            }
         }
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

bool compare_mw(MW_data i, MW_data j) {
   return (i.value < j.value);
}

ImageType2D::PixelType hipot(ImageType2D::PixelType a, ImageType2D::PixelType b) {
   ImageType2D::PixelType c;
   c = sqrt(a*a + b*b);
   
   return c;
}

ImageType2D::PixelType distancePoints(ImageType2D::IndexType p1, ImageType2D::IndexType p2) {
   ImageType2D::PixelType a, b;
   
   a = p1[0]-p2[0];
   b = p1[1]-p2[1];
   
   return (hipot(a, b));
}

// HoughTransform class
HoughTransform::HoughTransform(MivSettingsType *settings) {
   hp = new HoughParameters();
   settingsType = settings;
}

void HoughTransform::setHoughParameters(HoughParameters* hparams) {
   delete hp;
   hp = hparams;
}

HoughTransformFilterType::CirclesListType HoughTransform::findCircles(const ImageType2D::Pointer image) {
   
   MinimumMaximumCalculatorType2::Pointer minMaxCalculator = MinimumMaximumCalculatorType2::New();
   minMaxCalculator->SetImage(image);
   minMaxCalculator->ComputeMaximum();
   ImageType2D::PixelType max = minMaxCalculator->GetMaximum();
   
   HoughTransformFilterType::Pointer houghFilter = HoughTransformFilterType::New();
   houghFilter->SetNumberOfCircles( hp->numberOfCircles );
   houghFilter->SetMinimumRadius( hp->minimumRadius );
   houghFilter->SetMaximumRadius( hp->maximumRadius );
   
   houghFilter->SetSweepAngle( hp->sweepAngle );
   houghFilter->SetSigmaGradient( hp->sigmaGradient );
   houghFilter->SetVariance( hp->accVariance );
   houghFilter->SetDiscRadiusRatio( hp->discRadius );
   
   if (settingsType->thresholdMethod == 0) {
      OtsuThresholdImageCalculator::Pointer otsu = OtsuThresholdImageCalculator::New();
      otsu->SetImage( image );
      otsu->Compute();
      
      houghFilter->SetThreshold(otsu->GetThreshold());
   }
   else {
      houghFilter->SetThreshold(max*((double)settingsType->thresholdValue)/100.0);
   }
   
   houghFilter->SetInput( image );
   
   houghFilter->Update();
   
   HoughTransformFilterType::CirclesListType circles;
   circles = houghFilter->GetCircles( hp->numberOfCircles );
   
   return circles;
}

int HoughTransform::findImageCircles(vector<ImageProperties*> images, int start, int end) {
   
   for (int i = start; i <= end; i++) {
      HoughTransformFilterType::CirclesListType circles;
      circles = findCircles(images[i]->image);
      
      if (circles.size() == 1) {
         Circle* crc = new Circle;
         
         HoughTransformFilterType::CirclesListType::const_iterator itCircles = circles.begin();
         crc->x = (int)(*itCircles)->GetObjectToParentTransform()->GetOffset()[0];
         crc->y = (int)(*itCircles)->GetObjectToParentTransform()->GetOffset()[1];
         crc->r = (int)(*itCircles)->GetRadius()[0];
         images[i]->circle = crc;
      }
   }
   
   return EXIT_SUCCESS;
}

Circle* HoughTransform::findLargestCircle(vector<ImageProperties*> images) {
   int start, end, slice;
   start = (images.size()/2) - 4;
   end = (images.size()/2) + 4;
   
   Circle* lg_circle = new Circle;
   lg_circle->x = 0;
   lg_circle->y = 0;
   lg_circle->r = 0;
   
   for (int i = start; i < end; i++) {
      HoughTransformFilterType::CirclesListType circles;
      circles = findCircles(images[i]->image);
      
      if (circles.size() == 1) {
         HoughTransformFilterType::CirclesListType::const_iterator itCircles = circles.begin();
         if ((int)(*itCircles)->GetRadius()[0] > lg_circle->r) {
            lg_circle->x = (int)(*itCircles)->GetObjectToParentTransform()->GetOffset()[0];
            lg_circle->y = (int)(*itCircles)->GetObjectToParentTransform()->GetOffset()[1];
            lg_circle->r = (int)(*itCircles)->GetRadius()[0];
            slice = i;
         }
      }
   }
   images[slice]->circle = lg_circle;
   
   /*ImageType2D::IndexType pixelIndex;
    
    for(double angle = 0; angle <= 2*vnl_math::pi; angle += vnl_math::pi/60.0) {
    pixelIndex[0] = (long int)(lg_circle->x + lg_circle->r*cos(angle));
    pixelIndex[1] = (long int)(lg_circle->y + lg_circle->r*sin(angle));
    
    ImageType2D::RegionType c_region = images[slice]->image->GetLargestPossibleRegion();
    
    if(c_region.IsInside(pixelIndex)) {
    images[slice]->image->SetPixel( pixelIndex, 255.0 );
    }
    }*/
   
   return lg_circle;
}

bool compare_r(ImageProperties* i, ImageProperties* j) {
   return (i->circle->r >= j->circle->r);
}

vector<int> HoughTransform::selectAxialSlicesFromSagittal(vector<ImageProperties*> images, vector<ImageProperties*> axial) {
   vector<ImageProperties*> img_vect;

   // Find threshold value for all slices
   ImageType2D::PixelType max = 0;
   
   for (int i = 0; i < images.size(); i++) {
      MinimumMaximumCalculatorType2::Pointer calculator = MinimumMaximumCalculatorType2::New();
      calculator->SetImage(images[i]->image);
      calculator->Compute();
      if (max < calculator->GetMaximum()) {
         max = calculator->GetMaximum();
      }
   }
   
   // Find circles in the sagittal slices
   for (int i = (images.size()/2)-(images.size()/6); i < (images.size()/2)+(images.size()/6); i++) {
      Circle* circle = new Circle;
      circle->x = 0;
      circle->y = 0;
      circle->r = 0;
      
      HoughTransformFilterType::CirclesListType circles;
      circles = findCircles(images[i]->image);
      
      if (circles.size() == 1) {
         HoughTransformFilterType::CirclesListType::const_iterator itCircles = circles.begin();
         if ((int)(*itCircles)->GetRadius()[0] > circle->r) {
            circle->x = (int)(*itCircles)->GetObjectToParentTransform()->GetOffset()[0];
            circle->y = (int)(*itCircles)->GetObjectToParentTransform()->GetOffset()[1];
            circle->r = (int)(*itCircles)->GetRadius()[0];
         }
      }
      
      ImageType2D::IndexType pixelIndex, pixelIndex2;
      pixelIndex[0] = circle->x - circle->r;
      pixelIndex[1] = circle->y - circle->r;
      pixelIndex2[0] = circle->x + circle->r;
      pixelIndex2[1] = circle->y + circle->r;
      
      ImageType2D::RegionType c_region = images[i]->image->GetLargestPossibleRegion();
      if (c_region.IsInside(pixelIndex) && c_region.IsInside(pixelIndex2) && circle->r > 0) {
         
         MinimumMaximumCalculatorType2::Pointer calculator = MinimumMaximumCalculatorType2::New();
         calculator->SetImage(images[i]->image);
         calculator->Compute();
         ImageType2D::PixelType max_i = calculator->GetMaximum();
         
         images[i]->circle = circle;
         img_vect.push_back(images[i]);
      }
   }
   
   // Sort images by radious size
   std::stable_sort(img_vect.begin(), img_vect.end(), compare_r);

   int start_slice, end_slice;
   start_slice = axial.size() - (img_vect[0]->circle->x + img_vect[0]->circle->r) - 1;
   end_slice = axial.size() - (img_vect[0]->circle->x - img_vect[0]->circle->r) - 1;
   
   if (start_slice < 0)
      start_slice = 1;
   
   // Select slice with the biggest radious
   vector<int> result;
   result.push_back(start_slice);
   result.push_back(end_slice);
   
   return result;
}

vector<int> HoughTransform::selectAxialSlicesFromCoronal(vector<ImageProperties*> images, vector<ImageProperties*> axial) {
   vector<ImageProperties*> img_vect;
   
   // Find threshold value for all slices
   ImageType2D::PixelType max = 0;
   
   for (int i = 0; i < images.size(); i++) {
      MinimumMaximumCalculatorType2::Pointer calculator = MinimumMaximumCalculatorType2::New();
      calculator->SetImage(images[i]->image);
      calculator->Compute();
      if (max < calculator->GetMaximum()) {
         max = calculator->GetMaximum();
      }
   }
   
   // Find circles in the coronal slices
   for (int i = (images.size()/2)-(images.size()/6); i < (images.size()/2)+(images.size()/6); i++) {
      Circle* circle = new Circle;
      circle->x = 0;
      circle->y = 0;
      circle->r = 0;
      
      HoughTransformFilterType::CirclesListType circles;
      circles = findCircles(images[i]->image);
      
      if (circles.size() == 1) {
         HoughTransformFilterType::CirclesListType::const_iterator itCircles = circles.begin();
         if ((int)(*itCircles)->GetRadius()[0] > circle->r) {
            circle->x = (int)(*itCircles)->GetObjectToParentTransform()->GetOffset()[0];
            circle->y = (int)(*itCircles)->GetObjectToParentTransform()->GetOffset()[1];
            circle->r = (int)(*itCircles)->GetRadius()[0];
         }
      }
      
      ImageType2D::IndexType pixelIndex, pixelIndex2;
      pixelIndex[0] = circle->x - circle->r;
      pixelIndex[1] = circle->y - circle->r;
      pixelIndex2[0] = circle->x + circle->r;
      pixelIndex2[1] = circle->y + circle->r;
      
      ImageType2D::RegionType c_region = images[i]->image->GetLargestPossibleRegion();
      if (c_region.IsInside(pixelIndex) && c_region.IsInside(pixelIndex2) && circle->r > 0) {
         
         MinimumMaximumCalculatorType2::Pointer calculator = MinimumMaximumCalculatorType2::New();
         calculator->SetImage(images[i]->image);
         calculator->Compute();
         ImageType2D::PixelType max_i = calculator->GetMaximum();
         
         images[i]->circle = circle;
         img_vect.push_back(images[i]);
      }
   }
   
   // Sort images by radious size
   std::stable_sort(img_vect.begin(), img_vect.end(), compare_r);

   int start_slice, end_slice;
   start_slice = img_vect[0]->circle->y + img_vect[0]->circle->r;
   end_slice = img_vect[0]->circle->y - img_vect[0]->circle->r;
   
   if (start_slice < 0)
      start_slice = 1;
      
   // Select slice with the biggest radious
   vector<int> result;
   result.push_back(start_slice);
   result.push_back(end_slice);
   
   return result;
}

vector<int> HoughTransform::selectAxialSlices(vector<ImageProperties*> axial, vector<ImageProperties*> coronal, vector<ImageProperties*> sagittal) {

   vector<int> result;
   int start_slice, end_slice;
   
   vector<int> result_coronal = selectAxialSlicesFromCoronal(coronal, axial);
   vector<int> result_sagittal = selectAxialSlicesFromSagittal(sagittal, axial);
   
   if (result_coronal[0] < result_sagittal[0])
      start_slice = result_coronal[0];
   else
      start_slice = result_sagittal[0];
   
   if (result_coronal[1] > result_sagittal[1])
      end_slice = result_coronal[1];
   else
      end_slice = result_sagittal[1];
   
   result_coronal.clear();
   result_sagittal.clear();
   
   // Adjust start & end
   vector<int> result_adjust = adjustAxialSlices(axial, start_slice, end_slice);
   start_slice = result_adjust[0];
   end_slice = result_adjust[1];
   result_adjust.clear();
   
   if (start_slice < 0)
      start_slice = 1;
   
   result.push_back(start_slice);
   result.push_back(end_slice);
   
   return result;
}

vector<int> HoughTransform::adjustAxialSlices(vector<ImageProperties*> axial, int start_slice, int end_slice) {
   vector<int> result;
   int mid_slice = (start_slice + end_slice)/2;
   
   // Find mid-circle
   ImageType2D::Pointer image = axial[mid_slice]->image;
   
   Circle* circle = new Circle;
   circle->x = 0;
   circle->y = 0;
   circle->r = 0;
   
   HoughTransformFilterType::CirclesListType circles;
   circles = findCircles(image);
   
   if (circles.size() == 1) {
      HoughTransformFilterType::CirclesListType::const_iterator itCircles = circles.begin();
      if ((int)(*itCircles)->GetRadius()[0] > circle->r) {
         circle->x = (int)(*itCircles)->GetObjectToParentTransform()->GetOffset()[0];
         circle->y = (int)(*itCircles)->GetObjectToParentTransform()->GetOffset()[1];
         circle->r = (int)(*itCircles)->GetRadius()[0];
      }
   }
   
   // Get max pixel in mid-axial
   ImageType2D::IndexType pixelIndex;
   ImageType2D::PixelType max = 0;
   
   ImageType2D::RegionType c_region = image->GetLargestPossibleRegion();
   ImageType2D::SizeType size = c_region.GetSize();
   for (int x=0; x < size[0]; x++) {
      for (int y=0; y < size[1]; y++) {
         ImageType2D::IndexType center, p;
         center[0] = circle->x;
         center[1] = circle->y;
         p[0] = x;
         p[1] = y;
         if (distancePoints(center, p) <= circle->r) {
            if(c_region.IsInside(p)) {
               if (max < image->GetPixel(p))
                  max = image->GetPixel(p);
            }
         }
      }
   }
   
   // Compare max of the mid-slice with the max of start slices
   for (int i=0; i < mid_slice; i++) {
      int max_slice = 0;
      for (int x=0; x < size[0]; x++) {
         for (int y=0; y < size[1]; y++) {
            ImageType2D::IndexType center, p;
            center[0] = circle->x;
            center[1] = circle->y;
            p[0] = x;
            p[1] = y;
            if (distancePoints(center, p) <= circle->r) {
               if(c_region.IsInside(p)) {
                  if (max_slice < axial[i]->image->GetPixel(p))
                     max_slice = axial[i]->image->GetPixel(p);
               }
            }
         }
      }
      if (max_slice >= 0.65*max) {
         start_slice = i;
         break;
      }
   }

   // Compare max of the mid-slice with the max of end slices
   for (int i=axial.size()-1; i > mid_slice; i--) {
      int max_slice = 0;
      for (int x=0; x < size[0]; x++) {
         for (int y=0; y < size[1]; y++) {
            ImageType2D::IndexType center, p;
            center[0] = circle->x;
            center[1] = circle->y;
            p[0] = x;
            p[1] = y;
            if (distancePoints(center, p) <= circle->r*2) {
               if(c_region.IsInside(p)) {
                  if (max_slice < axial[i]->image->GetPixel(p))
                     max_slice = axial[i]->image->GetPixel(p);
               }
            }
         }
      }
      if (max_slice >= 0.65*max) {
         if (end_slice > i)
            end_slice = i;
         break;
      }
   }
   
   result.push_back(start_slice-1);
   result.push_back(end_slice);

   // print images
   /*circle->y = circle->y + 2;
   if (axial[start_slice]->frame == 0) {
      for (int i=0; i < axial.size(); i++) {
         ImageType2D::Pointer img = axial[i]->image;
         char filename[255];
         sprintf(filename, "/home/luisrpp/Desktop/img_%d_%d.jpg", axial[i]->frame, axial[i]->slice);
         
         saveRGBImageCircle(axial[i]->image, filename, circle, (i <= start_slice-1) || (i >= end_slice) || (i == mid_slice));
      }
   }*/
   
   return result;
}

void HoughTransform::filterCircles(vector<ImageProperties*> images) {
   
   typedef vector<ImageProperties*> propvector;
   
   /** These constants fix the limits for considering that a circle is an
    outlier. If the circle center differs from the median center more than
    these offsets, the circle is considered an outlier. */
   int const MAX_X_OFFSET = 1;
   int const MAX_Y_OFFSET = 1;
   
   // select images with circles
   vector<ImageProperties*> img_circles;
   for (propvector::iterator i = images.begin(); i != images.end(); ++i) {
      ImageProperties* imgp = (*i);
      if (imgp->circle) {
         img_circles.push_back(imgp);
      }
   }
   
   /* we use only some slices to determine the median circle */
   int num_slices = img_circles.size();
   int used_slices = num_slices/4;
   int first_used_slice = num_slices/4;
   
   std::vector<int> xcenter;
   std::vector<int> ycenter;
   std::vector<int> radii;
   
   for (propvector::iterator i = img_circles.begin()/* + first_used_slice*/;
        i != img_circles.end()/*begin() + first_used_slice + used_slices*/; ++i) {
      Circle* circle = (*i)->circle;
      xcenter.push_back(circle->x);
      ycenter.push_back(circle->y);
      radii.push_back(circle->r);
   }
   
   // calculate median X coordinate
   std::vector<int>::iterator medianIter = xcenter.begin() +
   xcenter.size()/2;
   std::nth_element(xcenter.begin(), medianIter, xcenter.end());
   int xmedian = *medianIter;
   
   // calculate median Y coordinate
   medianIter = ycenter.begin() + ycenter.size()/2;
   std::nth_element(ycenter.begin(), medianIter, ycenter.end());
   int ymedian = *medianIter;
   
   // calculate median radius
   medianIter = radii.begin() + radii.size()/2;
   std::nth_element(radii.begin(), medianIter, radii.end());
   int rmedian = *medianIter;
   
   for (propvector::iterator i = img_circles.begin(); i != img_circles.end(); ++i) {
      bool rejected = false;
      ImageProperties* ip = (*i);
      Circle* circle = ip->circle;
      // outlier filtering
      if (vnl_math_abs(circle->x - xmedian) > MAX_X_OFFSET) {
         rejected = true;
      }
      if (vnl_math_abs(circle->y - ymedian) > MAX_Y_OFFSET) {
         rejected = true;
      }
      // if circle is an outlier, we use the median circle instead
      if (rejected) {
         circle->x = xmedian;
         circle->y = ymedian;
         // TODO: need to adjust radius accordingly
         //circle->r = rmedian;
      }
   }
}

HoughTransform::~HoughTransform() {
   delete hp;
}

// MannWhitneyTest class
MannWhitneyTest::MannWhitneyTest(MivSettingsType* settings) {
   settingsType = settings;
   samplePercent = 0.5;
   iterations = 1;
}

void MannWhitneyTest::mannWhitneyTest(ImageProperties* slice, Circle* circle) {
   
   for (int rpt = 0; rpt < iterations; rpt++) {

      vector<MW_data> mw_set;

      ImageType2D::RegionType c_region = slice->image->GetLargestPossibleRegion();

      // Get data from regions 1 and 2
      ImageType2D::SizeType size = slice->image->GetLargestPossibleRegion().GetSize();
      for (int x=0; x < size[0]; x++) {
         for (int y=0; y < size[1]; y++) {
            ImageType2D::IndexType center, p;
            center[0] = circle->x;
            center[1] = circle->y;
            p[0] = x;
            p[1] = y;
            if (distancePoints(center, p) <= 2.0*circle->r) {
               if(c_region.IsInside(p)) {
                  MW_data mw_data;
                  mw_data.value = slice->image->GetPixel(p);
                  mw_data.type = 1;
                  mw_set.push_back(mw_data);
               }
            }
            else if (distancePoints(center, p) <= 3.0*circle->r) {
               if(c_region.IsInside(p)) {
                  MW_data mw_data;
                  mw_data.value = slice->image->GetPixel(p);
                  mw_data.type = 2;
                  mw_set.push_back(mw_data);
               }
            }
         }
      }

      mw_set = selectSamples(mw_set);

      // Draw Circles
      /*for(double angle = 0; angle < 2*vnl_math::pi; angle += vnl_math::pi/180.0) {
      ImageType2D::IndexType pixelIndex, pixelIndex2;

      pixelIndex[0] = (long int)(circle->x + (2.0*circle->r)*cos(angle));
      pixelIndex[1] = (long int)(circle->y + (2.0*circle->r)*sin(angle));

      pixelIndex2[0] = (long int)(circle->x + (3.0*circle->r)*cos(angle));
      pixelIndex2[1] = (long int)(circle->y + (3.0*circle->r)*sin(angle));

      if(c_region.IsInside(pixelIndex)) {
      slice->image->SetPixel( pixelIndex, 255.0 );
      }
      if(c_region.IsInside(pixelIndex2)) {
      slice->image->SetPixel( pixelIndex2, 255.0 );
      }
      }*/
            
      // Sort and rank data
      std::stable_sort(mw_set.begin(), mw_set.end(), compare_mw);
      for (int i=1; i <= mw_set.size(); i++) {
         mw_set[i-1].rank = (float)i;
      }
      // Fix Rank values
      fixRank(&mw_set);
            
      // Calculate R1, n1 and R2, n2 
      R1 = 0; R2 = 0;
      n1 = 0; n2 = 0;
      for (int i=0; i < mw_set.size(); i++) {
         if (mw_set[i].type == 1) {
            n1++;
            R1 += mw_set[i].rank;
         }
         else {
            n2++;
            R2 += mw_set[i].rank;
         }
      }
      
      // Calculate ER1 and ER2
      ER1 = R1/n1;
      ER2 = R2/n2;
      
      mw_set.clear();
         
      // Calculate U1, U2 and U
      U1 = n1*n2 + (n1*(n1+1)/2) - R1;
      U2 = n1*n2 + (n2*(n2+1)/2) - R2;
      if (U1 <= U2)
         U = U1;
      else
         U = U2;
      
      // Calculate z
      z = (U - (n1*n2/2))/sqrtf((n1*n2*(n1+n2+1))/12);
   }
}

vector<MW_data> MannWhitneyTest::selectSamples(vector<MW_data> samples) {
   vector<MW_data> sampleA, sampleB, sampleC;
   int sampleSize;
   
   for (int i=0; i < samples.size(); i++) {
      if (samples[i].type == 1) {
         sampleA.push_back(samples[i]);
      }
      else {
         sampleB.push_back(samples[i]);
      }
   }   
   
   if (sampleA.size() < sampleB.size()) {
      sampleSize = (int)(samplePercent * sampleA.size());
   }
   else {
      sampleSize = (int)(samplePercent * sampleB.size());
   }
   
   int randA, randB;
   for (int i=0; i < sampleSize; i++) {
      randA = generateRandomNumber(sampleA.size()-1);
      sampleA[randA].random = randA;
      sampleC.push_back(sampleA[randA]);
      sampleA.erase(sampleA.begin()+randA);
      
      randB = generateRandomNumber(sampleB.size()-1);
      sampleB[randB].random = randB;
      sampleC.push_back(sampleB[randB]);
      sampleB.erase(sampleB.begin()+randB);
   }
   
   samples.clear();
   sampleA.clear();
   sampleB.clear();
   
   return sampleC;
}

void MannWhitneyTest::setSamplePercent(double samplePercent) {
   this->samplePercent = samplePercent;
}

void MannWhitneyTest::setNumberOfIterations(int iterations) {
   this->iterations = iterations;
}

int MannWhitneyTest::generateRandomNumber(int max_value) {
   int random_integer;
   int lowest=0, highest=max_value;
   int range=(highest-lowest)+1;
   random_integer = lowest+int((double)range*(double)rand()/(RAND_MAX + 1.0));
   
   return random_integer;
}

// Fix Rank values for items with same pixel value.
void MannWhitneyTest::fixRank(vector<MW_data> *mw_set) {
   int start_index = 0;
   float start_rank = 0;
   ImageType2D::PixelType start_value = -1.0;
   int count = 1;
   for (int i=0; i < (*mw_set).size(); i++) {
      if (start_value != (*mw_set)[i].value) {         
         if (count != 1) {
            float median = ((*mw_set)[start_index].rank + (*mw_set)[i-1].rank)/2.0;
            for (int j = start_index; j < i; j++) {
               (*mw_set)[j].rank = median;
            }
         }
         start_index = i;
         start_rank = (*mw_set)[i].rank;
         start_value = (*mw_set)[i].value;
         count = 1;
      }
      else {
         count++;
         if ((*mw_set).size()-1 == i && (*mw_set)[i].value == (*mw_set)[i-1].value) {
            float median = ((*mw_set)[start_index].rank + (*mw_set)[i].rank)/2.0;
            for (int j = start_index; j <= i; j++) {
               (*mw_set)[j].rank = median;
            }
         }
      }
   }   
}

// Save image in RGB
void MannWhitneyTest::mmSaveRGBImage(ImageType2D::Pointer input, const char* filename) {
   
   typedef Flip2D::FlipAxesArrayType FlipAxesArrayType;
   FlipAxesArrayType flipArray;
   flipArray[0] = 0;
   flipArray[1] = 1;
   
   Flip2D::Pointer flip = Flip2D::New();
   flip->SetInput(input);
   flip->SetFlipAxes(flipArray);
   flip->FlipAboutOriginOn();
   flip->Update();
   
   CastImageFilter::Pointer castImage = CastImageFilter::New();
   castImage->SetInput(flip->GetOutput());
   castImage->Update();
   
   RGBWriterType::Pointer rgbWriter = RGBWriterType::New();
   rgbWriter->SetFileName(filename);
   rgbWriter->SetInput(castImage->GetOutput());
   try {
      rgbWriter->Update();
   }
   catch( itk::ExceptionObject & excep ) {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
   }
}

// Save Mask
void MannWhitneyTest::mmSaveMask(ImageType2D::Pointer input, const char* f1, const char* f2, Circle* circle) {
   ImageType2D::Pointer image, image2;
   image = copyImageStructure(input);
   image2 = copyImageStructure(input);
   
   ImageType2D::RegionType c_region = image->GetLargestPossibleRegion();
   
   ImageType2D::SizeType size = image->GetLargestPossibleRegion().GetSize();
   for (int x=0; x < size[0]; x++) {
      for (int y=0; y < size[1]; y++) {
         ImageType2D::IndexType center, p;
         center[0] = circle->x;
         center[1] = circle->y;
         p[0] = x;
         p[1] = y;
         if (distancePoints(center, p) <= 2.0*circle->r) {
            if(c_region.IsInside(p)) {
               image->SetPixel(p, 255.0);
            }
         }
         else if (distancePoints(center, p) <= 3.0*circle->r) {
            if(c_region.IsInside(p)) {
               image2->SetPixel(p, 255.0);
            }
         }
      }
   }
   
   mmSaveRGBImage(image, f1);
   mmSaveRGBImage(image2, f2);
}

// Watershed Class
Watershed::Watershed(MivSettingsType *settings) {
   settingsType = settings;
}

void Watershed::setMarker(ImageType2D::Pointer marker) {
   this->marker = marker;
}

ImageType2D::Pointer Watershed::getGradient() {
   return gradient;
}

// Morphological Gradient Operator
void Watershed::createWSGradientImage(ImageType2D::Pointer img2d) {
   
   DilateFilter::Pointer dilateFilter = DilateFilter::New();
   SEBall seball;
   seball.SetRadius(1);
   seball.CreateStructuringElement();
   dilateFilter->SetInput(img2d);
   dilateFilter->SetKernel(seball);

   /*ErodeFilter::Pointer erodeFilter = ErodeFilter::New();
   erodeFilter->SetInput(img2d);
   erodeFilter->SetKernel(seball);*/
   
   SubtractFilter::Pointer subFilter = SubtractFilter::New();
   subFilter->SetInput1(dilateFilter->GetOutput());
   subFilter->SetInput2(img2d);
   // output = inpu1 - input2
   subFilter->Update();
   ImageType2D::Pointer gradImage = subFilter->GetOutput();
   
   gradient = gradImage;
}

bool compare_px(PixelInfo i, PixelInfo j) {
   return (i.value < j.value);
}

// Create Marker
ImageType2D::Pointer Watershed::createCirclesMarkerImage(ImageProperties* prop) {
   
   ImageType2D::Pointer img2d = prop->image;
   Circle* circle = prop->circle;
   
   ImageType2D::Pointer image = copyImageStructure(img2d);
   
   ImageType2D::IndexType pixelIndex;
   ImageType2D::IndexType pixelIndex2;
   ImageType2D::IndexType pixelIndex3;
   
   // Get Max Pixel
   MinimumMaximumCalculatorType2::Pointer calculator = MinimumMaximumCalculatorType2::New();
   calculator->SetImage(img2d);
   calculator->Compute();
   ImageType2D::PixelType max = calculator->GetMaximum();
   
   ImageType2D::PixelType MUSCLETHRESH;
   if (settingsType->thresholdMethod == 0) {
      OtsuThresholdImageCalculator::Pointer otsu = OtsuThresholdImageCalculator::New();
      otsu->SetImage(img2d);
      otsu->Compute();
      
      MUSCLETHRESH = otsu->GetThreshold();
   }
   else {
      MUSCLETHRESH = max*((double)settingsType->thresholdValue/100.0);
   }
   
   // Threshold to find muscle region
   ThreshFilter::Pointer upThresh = ThreshFilter::New();
   upThresh->SetInput(img2d);
   upThresh->SetInsideValue(255);
   upThresh->SetOutsideValue(0);
   upThresh->SetLowerThreshold(MUSCLETHRESH);
   upThresh->Update();
   
   for(double angle = 0; angle <= 2*vnl_math::pi; angle += vnl_math::pi/60.0) {
      pixelIndex[0] = (long int)(circle->x + cos(angle));
      pixelIndex[1] = (long int)(circle->y + sin(angle));
      
      pixelIndex2[0] = (long int)(circle->x + circle->r*cos(angle));
      pixelIndex2[1] = (long int)(circle->y + circle->r*sin(angle));
      
      pixelIndex3[0] = (long int)(circle->x + ((double)(circle->r)*2)*cos(angle));
      pixelIndex3[1] = (long int)(circle->y + ((double)(circle->r)*2)*sin(angle));
      
      ImageType2D::RegionType c_region = image->GetLargestPossibleRegion();
      
      if(c_region.IsInside(pixelIndex)) {
         if (upThresh->GetOutput()->GetPixel(pixelIndex) > 0)
            image->SetPixel( pixelIndex, LEFTVENTRICLE );
         else
            image->SetPixel( pixelIndex, BACKGROUND );
      }
      if(c_region.IsInside(pixelIndex2)) {
         if (upThresh->GetOutput()->GetPixel(pixelIndex2) > 0)
            image->SetPixel( pixelIndex2, LEFTVENTRICLE );
      }
      if(c_region.IsInside(pixelIndex3)) {
         image->SetPixel( pixelIndex3, BACKGROUND );
      }
   }
   
   /*if(prop->slice == 14 && prop->frame == 0) {
      saveRGBImage(img2d, "/home/luisrpp/Desktop/original.jpg");
      saveRGBImageCircle(img2d, "/home/luisrpp/Desktop/circle.jpg", circle, true);
      saveRGBImage(upThresh->GetOutput(), "/home/luisrpp/Desktop/thrshd.jpg");
      saveRGBImageMarker(image, "/home/luisrpp/Desktop/marker.jpg", image);
      saveRGBImageMarker(img2d, "/home/luisrpp/Desktop/marker_o.jpg", image);
      saveRGBImageMarker(upThresh->GetOutput(), "/home/luisrpp/Desktop/marker_t.jpg", image);
   }*/
   
   return image;
}

/** Creates the Marker image used by the watershed algorithm. */
ImageType2D::Pointer Watershed::createRingMarkerImage(ImageProperties* prop) {

   const ImageType2D::Pointer img2d = prop->image;
   Circle* circle = prop->circle;
   int radius = circle->r;
   int x = circle->x;
   int y = circle->y;
   const int MINRADIUS = 3;

   // Get Max Pixel
   MinimumMaximumCalculatorType2::Pointer calculator = MinimumMaximumCalculatorType2::New();
   calculator->SetImage(img2d);
   calculator->Compute();
   ImageType2D::PixelType max = calculator->GetMaximum();

   ImageType2D::PixelType MUSCLETHRESH;
   
   if (settingsType->thresholdMethod == 0) {
      OtsuThresholdImageCalculator::Pointer otsu = OtsuThresholdImageCalculator::New();
      otsu->SetImage(img2d);
      otsu->Compute();
      
      MUSCLETHRESH = otsu->GetThreshold();
   }
   else {
      MUSCLETHRESH = max*((double)settingsType->thresholdValue/100.0);
   }
   
   // Threshold to find muscle region
   ThreshFilter::Pointer upThresh = ThreshFilter::New();
   upThresh->SetInput(img2d);
   upThresh->SetInsideValue(255);
   upThresh->SetOutsideValue(0);
   upThresh->SetLowerThreshold(MUSCLETHRESH);
   upThresh->Update();

   // we apply thinning to the muscle image
   ThinFilter::Pointer thinFilter = ThinFilter::New();
   thinFilter->SetInput(upThresh->GetOutput());
   thinFilter->Update();
   ImageType2D::Pointer thinImage = thinFilter->GetOutput();

   /** If we uncomment the section below, we start with a marker image
     where the regions of low intesity are assigned to the background.
     Otherwise, we start with a blank marker. **/

   //     ThreshFilter::Pointer lowThresh = ThreshFilter::New();
   //     lowThresh->SetInput(img2d);
   //     lowThresh->SetInsideValue(BACKGROUND);
   //     lowThresh->SetOutsideValue(0);
   //     lowThresh->SetUpperThreshold(BACKGROUNDTHRESH);
   //     lowThresh->Update();
   //ImageType2D::Pointer marker = lowThresh->GetOutput();
   ImageType2D::Pointer marker = copyImageStructure(img2d);


   ImageType2D::RegionType c_region = marker->GetLargestPossibleRegion();

   ImageType2D::IndexType pixelIndex;
   ImageType2D::IndexType minIndex = {{x, y}};
   ImageType2D::IndexType maxIndex = {{x, y}};

   ImageType2D::IndexType centerIndex = {{x, y}};
   ImageType2D::PixelType maxCenter = itk::NumericTraits<PixelType>::min();
   ImageType2D::PixelType minCenter = itk::NumericTraits<PixelType>::max();;

   bool nearVentricle = false;

   // scans area around center to record maximum and minimum values
   for (int i= -MINRADIUS; i <= MINRADIUS; ++i){
      for (int j= -MINRADIUS; j <= MINRADIUS; ++j) {
         pixelIndex[0] = x + i;
         pixelIndex[1] = y + j;
         if(c_region.IsInside(pixelIndex))
         {
            ImageType2D::PixelType thinPixel = thinImage->GetPixel(pixelIndex);
            // if we believe we are near the ventricle, mark
            // the corresponding pixel
            if (thinPixel > 0) {
               nearVentricle = true;
               marker->SetPixel(pixelIndex, LEFTVENTRICLE);
            }
            // searching for maximum and minimum values around center
            ImageType2D::PixelType pixel = img2d->GetPixel(pixelIndex);
            if (pixel < minCenter) {
               minCenter = pixel;
               //minIndex = pixelIndex;
            }
            if (pixel > maxCenter) {
               maxCenter = pixel;
               maxIndex = pixelIndex;
            }
         }
      }
   }

   /* If we are not near the ventricle, then the minimum pixel corresponds
   to the backgound. */
   if (!nearVentricle) {
      bool centerOn = upThresh->GetOutput()->GetPixel(centerIndex) > 0;
      if(c_region.IsInside(minIndex) && !centerOn)
         marker->SetPixel(minIndex, BACKGROUND);
      else
         marker->SetPixel(minIndex, LEFTVENTRICLE);
   } else {
      /* otherwise, we might be near the apex .*/
      PixelType delta = maxCenter - minCenter;
      if (delta > max/10) {
         /* If there is great varitions between the maximum and minimum
         values, then the minimum shoul belong to the background. */
         bool centerOn = upThresh->GetOutput()->GetPixel(centerIndex) > 0;
         if(c_region.IsInside(minIndex) && !centerOn) {
            marker->SetPixel(minIndex, BACKGROUND);
         }
         else {
            marker->SetPixel(minIndex, LEFTVENTRICLE);
         }
      } else {
         /* If we are near the ventricle and there is no great variation
         between maximum and minimum, we are probably in the apex. */
         radius = MINRADIUS - 1;
         // change the center to the maximum pixel
         x = maxIndex[0];
         y = maxIndex[1];
      }
   }

   double maxradius = radius;
   ImageType2D::IndexType lastIndex = {{x, y}};
   for(double angle = 0;angle < 2*vnl_math::pi; angle += vnl_math::pi/60.0 ) {
      // flags whether we found the ventricle
      bool found = false;
      // base radius for the outer marker
      double base= maxradius;
      // searchs for the ventricle, starting near the center and increasing
      // the radius
      for (double r = MINRADIUS; r < maxradius; r += 0.1) {
         pixelIndex[0] = (int)(x + r*cos(angle));
         pixelIndex[1] = (int)(y + r*sin(angle));
         if(c_region.IsInside(pixelIndex)) {
            // when the thin image is ON, we know we are inside
            // the ventricle
            if (thinImage->GetPixel(pixelIndex) > 0) {
               marker->SetPixel(pixelIndex, LEFTVENTRICLE);
               found = true;
            }
         }
         if (found) {
            base = r;
            lastIndex = pixelIndex;
            break;
         }
      }
      // Now we draw the external marker. We start with a radius a little
      // bigger than the last radius we used for searching the ventricle.
      // Thus we assure that the outer marker does not overlap with the
      // ventricle marker
      for (double r = base+6.5; r < maxradius+8.0; r += 0.5) {
         pixelIndex[0] = (int)(x + r*cos(angle));
         pixelIndex[1] = (int)(y + r*sin(angle));
         bool marked = marker->GetPixel(pixelIndex) > 0;
         if(!marked && c_region.IsInside(pixelIndex)) {
            if(c_region.IsInside(pixelIndex)) {
               marker->SetPixel(pixelIndex, BACKGROUND);
            }
         }
      }
      // If the ventricle was found, we search for other structures that
      // appear in the thin image and mark them as OTHER.
      if (found){
         for (double r = base+2.5; r < base + 4.0; r += 0.5) {
            pixelIndex[0] = (int)(x + r*cos(angle));
            pixelIndex[1] = (int)(y + r*sin(angle));
            if(c_region.IsInside(pixelIndex)) {
               bool inside = c_region.IsInside(pixelIndex);
               bool pixelOn = thinImage->GetPixel(pixelIndex) > 0;
               bool marked = marker->GetPixel(pixelIndex) > 0;
               bool centerOn = upThresh->GetOutput()->GetPixel(centerIndex) > 0;
               if(inside && pixelOn && !marked && !centerOn) {
                  marker->SetPixel(pixelIndex, OTHER);
               }
            }
         }
      }
   }

   return marker;
}

/** Creates the Marker image used by the watershed algorithm. */
ImageType2D::Pointer Watershed::createPointMarkerImage(ImageProperties* prop) {
   
   const ImageType2D::Pointer img2d = prop->image;
   Circle* circle = prop->circle;
   int radius = circle->r;
   ImageType2D::IndexType centerIndex = {{circle->x, circle->y}};
   const int MINRADIUS = 3;
   
   // Get Max Pixel
   MinimumMaximumCalculatorType2::Pointer calculator = MinimumMaximumCalculatorType2::New();
   calculator->SetImage(img2d);
   calculator->Compute();
   ImageType2D::PixelType max = calculator->GetMaximum();
   
   // Calculate Otsu Threshold
   OtsuThresholdImageCalculator::Pointer otsu = OtsuThresholdImageCalculator::New();
   otsu->SetImage(img2d);
   otsu->Compute();
   
   // Apply low threshold
   ThreshFilter::Pointer lowThresh = ThreshFilter::New();
   lowThresh->SetInput(img2d);
   lowThresh->SetInsideValue(0);
   lowThresh->SetOutsideValue(512);
   lowThresh->SetUpperThreshold(otsu->GetThreshold()*1.6);
   lowThresh->Update();
   
   // we apply thinning to the muscle image
   ThinFilter::Pointer thinFilter = ThinFilter::New();
   thinFilter->SetInput(lowThresh->GetOutput());
   thinFilter->Update();
   
   // Initiate marker image
   ImageType2D::Pointer marker = copyImageStructure(img2d);
   //ImageType2D::Pointer marker = thinFilter->GetOutput();
   
   ImageType2D::RegionType c_region = marker->GetLargestPossibleRegion();
   int maxradius = radius;
   ImageType2D::IndexType pixelIndex, trsh_pixel, newCenterIndex;

   // Find new center
   /*int tMarkers = 0;
   for (int i= -MINRADIUS; i <= MINRADIUS; ++i){
      for (int j= -MINRADIUS; j <= MINRADIUS; ++j) {
         int nMarkers = 0;
         for(double angle = 0;angle < 2*vnl_math::pi; angle += vnl_math::pi/15.0 ) {
            for (double r = 0; r < maxradius; r += 0.5) {
               pixelIndex[0] = (int)(centerIndex[0] + i + r*cos(angle));
               pixelIndex[1] = (int)(centerIndex[1] + j + r*sin(angle));
               
               if(c_region.IsInside(pixelIndex)) {
                  if (lowThresh->GetOutput()->GetPixel(pixelIndex) > 0) {
                     nMarkers++;
                     break;
                  }
               }
            }
         }
         if (nMarkers >= tMarkers) {
            newCenterIndex[0] = centerIndex[0] + i;
            newCenterIndex[1] = centerIndex[1] + j;
            tMarkers = nMarkers;
         }
      }
   }

   centerIndex = newCenterIndex;*/

   for (int i= -MINRADIUS; i <= MINRADIUS; ++i){
      for (int j= -MINRADIUS; j <= MINRADIUS; ++j) {
         for(double angle = 0;angle < 2*vnl_math::pi; angle += vnl_math::pi/8.0/*15.0*/) {
            double base = maxradius;
            
            trsh_pixel[0] = 0;
            trsh_pixel[1] = 0;
            for (double r = 0; r < maxradius; r += 1.0/*0.5*/) {
               pixelIndex[0] = (int)(centerIndex[0] + i + r*cos(angle));
               pixelIndex[1] = (int)(centerIndex[1] + j + r*sin(angle));
               
               if(c_region.IsInside(pixelIndex)) {
                  if (marker->GetPixel(pixelIndex) > 0) {
                     break;
                  }
                  if (lowThresh->GetOutput()->GetPixel(pixelIndex) > 0) {
                     trsh_pixel[0] = (int)(centerIndex[0] + i + (r+1)*cos(angle));
                     trsh_pixel[1] = (int)(centerIndex[1] + j + (r+1)*sin(angle));
                     marker->SetPixel(trsh_pixel, LEFTVENTRICLE);
                     break;
                  }
                  if (thinFilter->GetOutput()->GetPixel(pixelIndex) > 0) {
                     pixelIndex[0] = (int)((int)(centerIndex[0] + i + r*cos(angle)) + trsh_pixel[0])/2;
                     pixelIndex[1] = (int)((int)(centerIndex[1] + j + r*sin(angle)) + trsh_pixel[1])/2;
                     marker->SetPixel(pixelIndex, LEFTVENTRICLE);
                     base = distancePoints(centerIndex, pixelIndex);
                     break;
                  }
               }
            }
            /*for (double r = base+6.5; r < maxradius+8.0; r += 0.5) {
               pixelIndex[0] = (int)(centerIndex[0] + r*cos(angle));
               pixelIndex[1] = (int)(centerIndex[1] + r*sin(angle));
               if (c_region.IsInside(pixelIndex)) {
                  bool marked = marker->GetPixel(pixelIndex) > 0;
                  if(!marked) {
                     marker->SetPixel(pixelIndex, BACKGROUND);
                  }
               }
            }*/
         }
      }
   }
   
   return marker;
}

ImageType2D::Pointer Watershed::performWatershed() {
   
   WatershedFilterType::Pointer filter = WatershedFilterType::New();
   filter->SetInput(gradient);
   filter->SetMarkerImage(marker);
   filter->SetMarkWatershedLine(false);
   filter->SetFullyConnected(true);
   filter->Update();
   
   return filter->GetOutput();
}

ImageType2D::Pointer Watershed::segmentationMask(ImageType2D::Pointer img2d) {
   
   const ImageType2D::PixelType LEFT_INSIDE = LEFTVENTRICLE - 1;
   const ImageType2D::PixelType LEFT_OUTSIDE = LEFTVENTRICLE + 1;
   
   ThreshFilter::Pointer thresh = ThreshFilter::New();
   thresh->SetInput(img2d);
   thresh->SetInsideValue(255);
   thresh->SetOutsideValue(0);
   thresh->SetLowerThreshold(LEFT_INSIDE);
   thresh->SetUpperThreshold(LEFT_OUTSIDE);
   thresh->Update();
   
   ImageType2D::Pointer mask_image = thresh->GetOutput();
   
   ImageType2D::IndexType pixelIndex;
   for (int i = 0;  i < (int)mask_image->GetLargestPossibleRegion().GetSize()[0]; i++) {
      for (int j = 0;  j < (int)mask_image->GetLargestPossibleRegion().GetSize()[1]; j++) {
         pixelIndex[0] = i;
         pixelIndex[1] = j;
         
         ImageType2D::RegionType i_region = mask_image->GetLargestPossibleRegion();
         if (i_region.IsInside(pixelIndex)) {
            if (mask_image->GetPixel(pixelIndex) != 0) {
               mask_image->SetPixel( pixelIndex, 1 );
            }
         }
      }
   }
   
   return mask_image;
}

// Watershed 3D Class
Watershed3D::Watershed3D(MivSettingsType *settings) : Watershed(settings) {
   settingsType = settings;
}

void Watershed3D::setMarker(ImageType3D::Pointer marker) {
   this->marker = marker;
}

ImageType3D::Pointer Watershed3D::getGradient() {
   return gradient;
}

// Morphological Gradient Operator
void Watershed3D::createWSGradientImage(ImageType3D::Pointer img3d) {
   
   SEBall3D seball;
   seball.SetRadius(1);
   seball.CreateStructuringElement();
   
   DilateFilter3D::Pointer dilateFilter = DilateFilter3D::New();
   dilateFilter->SetInput(img3d);
   dilateFilter->SetKernel(seball);
   
   SubtractFilter3D::Pointer subFilter = SubtractFilter3D::New();
   subFilter->SetInput1(dilateFilter->GetOutput());
   subFilter->SetInput2(img3d);
   
   // output = inpu1 - input2
   subFilter->Update();
   
   gradient = subFilter->GetOutput();
}

ImageType3D::Pointer Watershed3D::performWatershed() {
   Watershed3DFilterType::Pointer filter = Watershed3DFilterType::New();
   filter->SetInput(gradient);
   filter->SetMarkerImage(marker);
   filter->SetMarkWatershedLine(false);
   filter->SetFullyConnected(true);
   filter->Update();
   
   return filter->GetOutput();
}
