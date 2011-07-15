/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: september 2008
****************************************************************/

#include "mivimage.h"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

int saveRGBImage1(ImageType2D::Pointer img, const char* fileName) {
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

int saveRGBImageMarker1(ImageType2D::Pointer image, const char* fileName, ImageType2D::Pointer marker) {
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
         if (marker->GetPixel(midx) == BACKGROUND) {
            bluePix[0] = 90;
            bluePix[1] = 50;
            bluePix[2] = 50;
            RGBImageType::RegionType c_region = localOutputImage->GetLargestPossibleRegion();
            localIndex[0] = midx[0];
            localIndex[1] = midx[1];
            if(c_region.IsInside(localIndex)) {
               localOutputImage->SetPixel(localIndex, bluePix);
            }
         }
         if (marker->GetPixel(midx) == OTHER) {
            bluePix[0] = 50;
            bluePix[1] = 30;
            bluePix[2] = 30;
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

int saveRGBImageSelectedSlices(ImageType2D::Pointer image, int type, int start, int end, const char* fileName) {
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
   RGBImageType::IndexType localIndex, localIndex2;

   ImageType2D::SizeType size = image->GetLargestPossibleRegion().GetSize();
   for (int x=0; x < size[0]; x++) {
      for (int y=0; y < size[1]; y++) {
         if (type == 0) {
            localIndex[0] = x;
            localIndex[1] = start;
            localIndex2[0] = x;
            localIndex2[1] = end;
         }
         if (type == 1) {
            localIndex[0] = size[0]-start;
            localIndex[1] = y;
            localIndex2[0] = size[0]-end-1;
            localIndex2[1] = y;
         }
         
         bluePix[0] = 255;
         bluePix[1] = 10;
         bluePix[2] = 10;
         
         RGBImageType::RegionType c_region = localOutputImage->GetLargestPossibleRegion();
         if(c_region.IsInside(localIndex)) {
            localOutputImage->SetPixel(localIndex, bluePix);
         }
         
         if(c_region.IsInside(localIndex2)) {
            localOutputImage->SetPixel(localIndex2, bluePix);
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

MivImage::MivImage() {
}

void MivImage::setMivSettings(MivSettingsType* settings) {
   this->settingsType = settings;
}

// Read Analyze images
int MivImage::readImage(const char *file) {

   itk::NiftiImageIO::Pointer io = itk::NiftiImageIO::New();
   ReaderType4D::Pointer reader = ReaderType4D::New();
   try {
      reader->SetImageIO(io);
      reader->SetFileName(file);
      reader->Update();

      image4D = reader->GetOutput();

      ImageType4D::RegionType inputRegion = image4D->GetLargestPossibleRegion();
      size = inputRegion.GetSize();

      const char* sp = strrchr(file, '/');
      strcpy(this->filename, sp+1);
      
      copyToImageStorage();
      calculateImageMinMax(image4D);
   }
   catch (itk::ExceptionObject &exp) {
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

// Read Dicom images from a Dicom directory.
int MivImage::readDicom(const char *path) {

   DicomReaderType4D::Pointer reader = DicomReaderType4D::New();

   typedef itk::GDCMImageIO ImageIOType;
   ImageIOType::Pointer dicomIO = ImageIOType::New();

   reader->SetImageIO(dicomIO);

   typedef itk::GDCMSeriesFileNames NamesGeneratorType;
   NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
   nameGenerator->SetUseSeriesDetails(true);
   nameGenerator->AddSeriesRestriction("0008|0021");
   nameGenerator->SetDirectory(path);

   try {
      typedef std::vector<std::string> SeriesIdContainer;

      const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

      std::string seriesIdentifier;
      seriesIdentifier = seriesUID.begin()->c_str();

      typedef std::vector<std::string> FileNamesContainer;
      FileNamesContainer fileNames;

      fileNames = nameGenerator->GetFileNames(seriesIdentifier);

      reader->SetFileNames( fileNames );
      reader->Update();

      image4D = reader->GetOutput();
      ImageType4D::RegionType inputRegion = image4D->GetLargestPossibleRegion();
      size = inputRegion.GetSize();

      //TODO
      sprintf (this->filename, "unknown");

      copyToImageStorage();
      calculateImageMinMax(image4D);
   }
   catch (itk::ExceptionObject &ex) {
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

// Write an Analyze file.
int MivImage::writeImage(const char *fileName) {

   itk::NiftiImageIO::Pointer io = itk::NiftiImageIO::New();
   WriterType4D::Pointer writer = WriterType4D::New();
   try {
      writer->SetImageIO(io);
      writer->SetFileName(fileName);
      writer->SetInput(image4D);
      writer->Update();
   }
   catch (itk::ExceptionObject &ex) {
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

int MivImage::writeImage4D(ImageType4D::Pointer image4D, const char* fileName) {
   itk::NiftiImageIO::Pointer io = itk::NiftiImageIO::New();
   WriterType4D::Pointer writer = WriterType4D::New();
   try {
      writer->SetImageIO(io);
      writer->SetFileName(fileName);
      writer->SetInput(image4D);
      writer->Update();
   }
   catch (itk::ExceptionObject &ex) {
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

int MivImage::writeImage3D(ImageType3D::Pointer image3D, const char* fileName) {
   itk::NiftiImageIO::Pointer io = itk::NiftiImageIO::New();
   WriterType3D::Pointer writer = WriterType3D::New();
   try {
      writer->SetImageIO(io);
      writer->SetFileName(fileName);
      writer->SetInput(image3D);
      writer->Update();
   }
   catch (itk::ExceptionObject &ex) {
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

int MivImage::writeImage2D(ImageType2D::Pointer image2D, const char* fileName) {
   itk::NiftiImageIO::Pointer io = itk::NiftiImageIO::New();
   WriterType2D::Pointer writer = WriterType2D::New();
   try {
      writer->SetImageIO(io);
      writer->SetFileName(fileName);
      writer->SetInput(image2D);
      writer->Update();
   }
   catch (itk::ExceptionObject &ex) {
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

//TODO
int MivImage::saveProject(const char*) {
   return EXIT_FAILURE;
}

ImageType4D::Pointer MivImage::createMaskVolume() {
   JoinSeriesFilterType4D::Pointer joinSeries4D = JoinSeriesFilterType4D::New();

   for (int i = 0; i < this->getSize(3); i++) {
      ImageType3D::Pointer image3D;
      JoinSeriesFilterType3D::Pointer joinSeries3D = JoinSeriesFilterType3D::New();
      for (int j = 0; j < this->getSize(2); j++) {
         ImageType2D::Pointer image2D;
         if (this->isSegmented(i, j)) {
            image2D = this->getSegmentationMask(i, j);
         }
         else {
            image2D = copyImageStructure(this->getImage2D(i, 2, j));
         }

         typedef Flip2D::FlipAxesArrayType FlipAxesArrayType;
         FlipAxesArrayType flipArray;
         flipArray[0] = 0;
         flipArray[1] = 1;

         Flip2D::Pointer flip = Flip2D::New();
         flip->SetInput(image2D);
         flip->SetFlipAxes(flipArray);
         flip->FlipAboutOriginOn();
         flip->Update();

         joinSeries3D->PushBackInput(flip->GetOutput());
      }
      joinSeries3D->Update();
      joinSeries4D->PushBackInput(joinSeries3D->GetOutput());
   }
   joinSeries4D->Update();

   return joinSeries4D->GetOutput();
}

ImageType4D::Pointer MivImage::createMarkerVolume() {
   JoinSeriesFilterType4D::Pointer joinSeries4D = JoinSeriesFilterType4D::New();

   for (int i = 0; i < this->getSize(3); i++) {
      ImageType3D::Pointer image3D;
      JoinSeriesFilterType3D::Pointer joinSeries3D = JoinSeriesFilterType3D::New();
      for (int j = 0; j < this->getSize(2); j++) {
         ImageType2D::Pointer image2D;
         if (this->existMarker(i, j)) {
            image2D = this->getMarker(i, j);
         }
         else {
            image2D = copyImageStructure(this->getImage2D(i, 2, j));
         }

         typedef Flip2D::FlipAxesArrayType FlipAxesArrayType;
         FlipAxesArrayType flipArray;
         flipArray[0] = 0;
         flipArray[1] = 1;

         Flip2D::Pointer flip = Flip2D::New();
         flip->SetInput(image2D);
         flip->SetFlipAxes(flipArray);
         flip->FlipAboutOriginOn();
         flip->Update();

         joinSeries3D->PushBackInput(flip->GetOutput());
      }
      joinSeries3D->Update();
      joinSeries4D->PushBackInput(joinSeries3D->GetOutput());
   }
   joinSeries4D->Update();

   return joinSeries4D->GetOutput();
}

int MivImage::saveSegmentationMasks(const char* fileName) {

   itk::NiftiImageIO::Pointer io = itk::NiftiImageIO::New();
   WriterType4D::Pointer writer = WriterType4D::New();

   try {
      writer->SetImageIO(io);
      writer->SetFileName(fileName);
      writer->SetInput(createMaskVolume());
      writer->Update();
   }
   catch (itk::ExceptionObject &ex) {
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

int MivImage::saveMarkers(const char* fileName) {

   itk::NiftiImageIO::Pointer io = itk::NiftiImageIO::New();
   WriterType4D::Pointer writer = WriterType4D::New();

   try {
      writer->SetImageIO(io);
      writer->SetFileName(fileName);
      writer->SetInput(createMarkerVolume());
      writer->Update();
   }
   catch (itk::ExceptionObject &ex) {
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

int MivImage::saveRGBImage(ImageType2D::Pointer image, const char* fileName) {
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

int MivImage::saveImage2D(int orientation, int type, int frame, int slice, const char* fileName) {

   itk::NiftiImageIO::Pointer io = itk::NiftiImageIO::New();
   WriterType2D::Pointer writer = WriterType2D::New();

   try {
      writer->SetImageIO(io);
      writer->SetFileName(fileName);
      switch(type) {
         case 0:
            if (orientation == 0)
               writer->SetInput(storage[frame].sagittal[slice]->image);
            if (orientation == 1)
               writer->SetInput(storage[frame].coronal[slice]->image);
            if (orientation == 2)
               writer->SetInput(storage[frame].axial[slice]->image);
            break;
         case 1:
            if (orientation == 0)
               writer->SetInput(storage[frame].sagittal[slice]->marker);
            if (orientation == 1)
               writer->SetInput(storage[frame].coronal[slice]->marker);
            if (orientation == 2)
               writer->SetInput(storage[frame].axial[slice]->marker);
            break;
         case 2:
            if (orientation == 0)
               writer->SetInput(storage[frame].sagittal[slice]->segmentationMask);
            if (orientation == 1)
               writer->SetInput(storage[frame].coronal[slice]->segmentationMask);
            if (orientation == 2)
               writer->SetInput(storage[frame].axial[slice]->segmentationMask);
            break;
         case 3:
            if (orientation == 0)
               writer->SetInput(storage[frame].sagittal[slice]->segmented);
            if (orientation == 1)
               writer->SetInput(storage[frame].coronal[slice]->segmented);
            if (orientation == 2)
               writer->SetInput(storage[frame].axial[slice]->segmented);
            break;
      }
      writer->Update();
   }
   catch (itk::ExceptionObject &ex) {
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

// Convert images from Itk to Vtk
vtkImageData* MivImage::convertITKtoVTK(ImageType2D::Pointer itkImage) {

   ConnectorType::Pointer connector = ConnectorType::New();
   connector->SetInput(itkImage);
   connector->Update();

   vtkImageData* output = vtkImageData::New();
   output->DeepCopy(connector->GetOutput());

   return output;
}

// Convert images from Vtk to Itk
ImageType2D::Pointer MivImage::convertVTKtoITK(vtkImageData* imageData) {

   ConnectorType2::Pointer connector = ConnectorType2::New();
   connector->SetInput(imageData);
   connector->Update();

   ImageType2D::Pointer output = connector->GetOutput();

   return output;
}

void MivImage::calculateImageMinMax(ImageType4D::Pointer itkImage4D) {

   MinimumMaximumCalculatorType::Pointer calculator = MinimumMaximumCalculatorType::New();
   calculator->SetImage(itkImage4D);
   calculator->Compute();
   maximumValue = calculator->GetMaximum();
   minimumValue = calculator->GetMinimum();

}

PixelType MivImage::calculateImageMaxPixel(ImageType2D::Pointer itkImage2D) {
   MinimumMaximumCalculatorType2::Pointer calculator = MinimumMaximumCalculatorType2::New();
   calculator->SetImage(itkImage2D);
   calculator->Compute();
   return calculator->GetMaximum();
}

PixelType MivImage::getMaximumValue() {
   return maximumValue;
}

PixelType MivImage::getMinimumValue() {
   return minimumValue;
}

// Get the size of a dimension.
int MivImage::getSize(int index) {
   return (int)size[index];
}

// Copy images to an internal structure.
void MivImage::copyToImageStorage() {

   storage = new ImageStorage[size[3]];

   for (int k=0; k < (int)size[3]; k++) {

      ImageType3D::Pointer img3D = extractImage3D(image4D, k);

      storage[k].image3D = img3D;
      storage[k].axialSlicesSelected = false;
      storage[k].axialStart = 0;
      storage[k].bullsEyeCreated = false;

      for (int i=0; i < (int)size[0]; i++) {
         ImageProperties* sagittal = new ImageProperties;
         sagittal->type = 0;
         sagittal->slice = i;
         sagittal->frame = k;
         sagittal->image = extractImage2D(img3D, 0, i);
         sagittal->circle = NULL;
         sagittal->existMarker = false;
         sagittal->isSegmented = false;
         sagittal->axialRegionDefined = false;
         sprintf (sagittal->filename, "%s", filename);
         storage[k].sagittal.push_back(sagittal);
      }
      for (int i=0; i < (int)size[1]; i++) {
         ImageProperties* coronal = new ImageProperties;
         coronal->type = 1;
         coronal->slice = i;
         coronal->frame = k;
         coronal->image = extractImage2D(img3D, 1, i);
         coronal->circle = NULL;
         coronal->existMarker = false;
         coronal->isSegmented = false;
         coronal->axialRegionDefined = false;
         sprintf (coronal->filename, "%s", filename);
         storage[k].coronal.push_back(coronal);
      }
      for (int i=0; i < (int)size[2]; i++) {
         ImageProperties* axial = new ImageProperties;
         axial->type = 2;
         axial->slice = i;
         axial->frame = k;
         axial->image = extractImage2D(img3D, 2, i);
         axial->circle = NULL;
         axial->existMarker = false;
         axial->isSegmented = false;
         axial->axialRegionDefined = false;
         sprintf (axial->filename, "%s", filename);
         storage[k].axial.push_back(axial);
         storage[k].axialEnd = storage[k].axial.size()-1;
      }
   }
}

// Get a 2D image from the internal structure.
ImageType2D::Pointer MivImage::getImage2D(int frame, int plane, int slice) {

   switch (plane) {
   case 0:
      return storage[frame].sagittal[slice]->image;
   case 1:
      return storage[frame].coronal[slice]->image;
   case 2:
      return storage[frame].axial[slice]->image;
   }

   return NULL;
}

vector<ImageProperties*> MivImage::getImageVector(int frame, int plane) {
   vector<ImageProperties*> ret;

   switch (plane) {
   case 0:
      ret = storage[frame].sagittal;
      break;
   case 1:
      ret = storage[frame].coronal;
      break;
   case 2:
      ret = storage[frame].axial;
      break;
   }

   return ret;
}

bool MivImage::existMarker(int frame, int slice) {
   return storage[frame].axial[slice]->existMarker;
}

ImageType2D::Pointer MivImage::getMarker(int frame, int slice) {
   return storage[frame].axial[slice]->marker;
}

void MivImage::setMarker(ImageType2D::Pointer marker, int frame, int slice) {
   storage[frame].axial[slice]->marker = marker;
   storage[frame].axial[slice]->existMarker = true;
   storage[frame].axial[slice]->isSegmented = false;
}

void MivImage::createBlankMarker(int frame, int slice) {
   ImageType2D::Pointer blank = copyImageStructure(storage[frame].axial[slice]->image);
   setMarker(blank, frame, slice);
}

void MivImage::removeMarker(int frame, int slice) {
   storage[frame].axial[slice]->marker = NULL;
   storage[frame].axial[slice]->existMarker = false;
   storage[frame].axial[slice]->isSegmented = false;
}

bool MivImage::isSegmented(int frame, int slice) {
   return storage[frame].axial[slice]->isSegmented;
}

void MivImage::setAxialRegion(int frame, int slice, AxialRegion region) {
   if (region == UNKNOWN)
      storage[frame].axial[slice]->axialRegionDefined = false;
   else
      storage[frame].axial[slice]->axialRegionDefined = true;
   storage[frame].axial[slice]->region = region;
}

bool MivImage::hasAxialRegion(int frame, int slice) {
   return storage[frame].axial[slice]->axialRegionDefined;
}

AxialRegion MivImage::getAxialRegion(int frame, int slice) {
   return storage[frame].axial[slice]->region;
}

ImageType2D::Pointer MivImage::getSegmented(int frame, int slice) {
   return storage[frame].axial[slice]->segmented;
}

ImageType2D::Pointer MivImage::getSegmentationMask(int frame, int slice) {
   return storage[frame].axial[slice]->segmentationMask;
}

ImageType3D::Pointer MivImage::getImage3D(int frame) {
   return storage[frame].image3D;
}

// Extract a 3D image from a 4D image.
ImageType3D::Pointer MivImage::extractImage3D(const ImageType4D::Pointer img4D, const int frame) {

   FilterType3D::Pointer filter = FilterType3D::New();

   ImageType4D::RegionType inputRegion = img4D->GetLargestPossibleRegion();

   ImageType4D::SizeType size = inputRegion.GetSize();
   size[3] = 0;

   ImageType4D::IndexType start = inputRegion.GetIndex();
   const unsigned int frameNumber = frame;
   start[3] = frameNumber;

   ImageType4D::RegionType desiredRegion;
   desiredRegion.SetSize(size);
   desiredRegion.SetIndex(start);

   filter->SetExtractionRegion(desiredRegion);
   filter->SetInput(img4D);
   filter->Update();

   ImageType3D::Pointer img3D = filter->GetOutput();
   img3D->Update();

   return img3D;
}

// Extract a 2D image from a 3D image.
ImageType2D::Pointer MivImage::extractImage2D(const ImageType3D::Pointer img3D, const int plane, const int slice) {

   FilterType2D::Pointer filter = FilterType2D::New();

   ImageType3D::RegionType inputRegion = img3D->GetLargestPossibleRegion();

   ImageType3D::SizeType size = inputRegion.GetSize();
   size[plane] = 0;

   ImageType3D::IndexType start = inputRegion.GetIndex();
   const unsigned int sliceNumber = slice;
   start[plane] = sliceNumber;

   ImageType3D::RegionType desiredRegion;
   desiredRegion.SetSize(size);
   desiredRegion.SetIndex(start);

   filter->SetExtractionRegion(desiredRegion);
   filter->SetInput(img3D);
   filter->Update();

   ImageType2D::Pointer img2D;

   if (plane == 2) {
      typedef Flip2D::FlipAxesArrayType FlipAxesArrayType;
      FlipAxesArrayType flipArray;
      flipArray[0] = 0;
      flipArray[1] = 1;

      Flip2D::Pointer flip = Flip2D::New();
      flip->SetInput(filter->GetOutput());
      flip->SetFlipAxes(flipArray);
      flip->FlipAboutOriginOn();
      flip->Update();
      img2D = flip->GetOutput();
   }
   else if (plane == 1) {
      img2D = filter->GetOutput();
   }
   else {
      typedef PermuteAxesImageFilter::PermuteOrderArrayType PermuteOrderArrayType;
      PermuteOrderArrayType permuteArray;
      permuteArray[0] = 1;
      permuteArray[1] = 0;
      
      PermuteAxesImageFilter::Pointer permute = PermuteAxesImageFilter::New();
      permute->SetInput(filter->GetOutput());
      permute->SetOrder(permuteArray);
      permute->Update();
      
      typedef Flip2D::FlipAxesArrayType FlipAxesArrayType;
      FlipAxesArrayType flipArray;
      flipArray[0] = 0;
      flipArray[1] = 1;
      
      Flip2D::Pointer flip = Flip2D::New();
      flip->SetInput(permute->GetOutput());
      flip->SetFlipAxes(flipArray);
      flip->FlipAboutOriginOn();
      flip->Update();

      flipArray[0] = 1;
      flipArray[1] = 0;
      
      Flip2D::Pointer flip2 = Flip2D::New();
      flip2->SetInput(flip->GetOutput());
      flip2->SetFlipAxes(flipArray);
      flip2->FlipAboutOriginOn();
      flip2->Update();
      
      img2D = flip2->GetOutput();
   }
   img2D->Update();

   return img2D;
}

void MivImage::selectAxialSlices() {
   // Create output file
   /*char *file = new char[255];
   mkdir("/home/luisrpp/Desktop/miv", 0777);
   sprintf (file, "/home/luisrpp/Desktop/miv/%s", filename);
   mkdir(file, 0777);*/
   
   HoughTransform *ht = new HoughTransform(settingsType);
   
   for (int k=0; k < (int)size[3]; k++) {
      vector<int> lim = ht->selectAxialSlices(storage[k].axial, storage[k].coronal, storage[k].sagittal);
      storage[k].axialStart = lim[0];
      storage[k].axialEnd = lim[1];
      storage[k].axialSlicesSelected = true;
      
      //fprintf(stdout, "start: %d\t end: %d\n", lim[0], lim[1]);
      
      /*sprintf (file, "/home/luisrpp/Desktop/miv/%s/coronal_%d_%d_%d_%d.png", filename, (storage[k].coronal.size()/2)-1, k, lim[0], lim[1]);
      saveRGBImageSelectedSlices(storage[k].coronal[(storage[k].coronal.size()/2)-1]->image, 0, lim[0], lim[1], file);
      sprintf (file, "/home/luisrpp/Desktop/miv/%s/sagittal_%d_%d_%d_%d.png", filename, (storage[k].sagittal.size()/2)-1, k, lim[0], lim[1]);
      saveRGBImageSelectedSlices(storage[k].sagittal[(storage[k].sagittal.size()/2)-1]->image, 1, lim[0], lim[1], file);
      
      for (int i=0; i < (int)size[2]; i++) {
         sprintf (file, "/home/luisrpp/Desktop/miv/%s/axial_%d_%d.png", filename, k, i);
         saveRGBImage1(storage[k].axial[i]->image, file);         
      }*/

      lim.clear();
   }
}

vector<int> MivImage::getAxialSliceDelimitations(int indexFrame) {
   vector<int> result;
   result.push_back(storage[indexFrame].axialStart);
   result.push_back(storage[indexFrame].axialEnd);
   return result;
}

bool MivImage::isAxialSlicesSelected(int indexFrame) {
   return storage[indexFrame].axialSlicesSelected;
}

void MivImage::setAxialSliceDelimitations(int frame, int start, int end) {
   storage[frame].axialStart = start;
   storage[frame].axialEnd = end;
   storage[frame].axialSlicesSelected = true;
}

int MivImage::findAxialCircles() {
   typedef vector<ImageProperties*> propvector;

   HoughTransform *ht = new HoughTransform(settingsType);
   Watershed *ws = new Watershed(settingsType);

   for (int k=0; k < (int)size[3]; k++) {
      ht->findImageCircles(storage[k].axial, storage[k].axialStart, storage[k].axialEnd);
      ht->filterCircles(storage[k].axial);

      for (propvector::iterator i = storage[k].axial.begin(); i != storage[k].axial.end(); ++i) {
         ImageProperties* imgp = (*i);
         if (imgp->circle) {
            imgp->marker = ws->createCirclesMarkerImage(imgp);
            imgp->existMarker = true;
            imgp->isSegmented = false;
         }
      }

   }
   delete ht;
   delete ws;

   return EXIT_SUCCESS;
}

int MivImage::findAxialRings() {
   typedef vector<ImageProperties*> propvector;

   HoughTransform *ht = new HoughTransform(settingsType);
   Watershed *ws = new Watershed(settingsType);
   
   for (int k=0; k < (int)size[3]; k++) {
      ht->findImageCircles(storage[k].axial, storage[k].axialStart, storage[k].axialEnd);
      ht->filterCircles(storage[k].axial);

      for (propvector::iterator i = storage[k].axial.begin(); i != storage[k].axial.end(); ++i) {
         ImageProperties* imgp = (*i);
         if (imgp->circle) {
            imgp->marker = ws->createRingMarkerImage(imgp);
            imgp->existMarker = true;
            imgp->isSegmented = false;
         }
      }

   }
   delete ht;
   delete ws;

   return EXIT_SUCCESS;
}

int MivImage::findAxialPoints() {
   typedef vector<ImageProperties*> propvector;
   
   HoughTransform *ht = new HoughTransform(settingsType);
   Watershed *ws = new Watershed(settingsType);
   
   for (int k=0; k < (int)size[3]; k++) {
      ht->findImageCircles(storage[k].axial, storage[k].axialStart, storage[k].axialEnd);
      ht->filterCircles(storage[k].axial);
      
      for (propvector::iterator i = storage[k].axial.begin(); i != storage[k].axial.end(); ++i) {
         ImageProperties* imgp = (*i);
         if (imgp->circle) {
            imgp->marker = ws->createPointMarkerImage(imgp);
            imgp->existMarker = true;
            imgp->isSegmented = false;
         }
      }
      
   }
   delete ht;
   delete ws;
   
   return EXIT_SUCCESS;
}

void MivImage::createAxialAxis() {
    for (int k=0; k < (int)size[3]; k++) {
        float axialAxisX = 0;
        float axialAxisY = 0;
        float total = 0;
        for (int i=0; i < (int)size[2]; i++) {
            ImageType2D::Pointer image;
            if (storage[k].axial[i]->isSegmented) {
                image = storage[k].axial[i]->segmentationMask;
                ImageType2D::RegionType c_region = image->GetLargestPossibleRegion();

                ImageType2D::IndexType pixelIndex;
                ImageType2D::IndexType centerIndex;
                centerIndex[0] = storage[k].axial[i]->circle->x;
                centerIndex[1] = storage[k].axial[i]->circle->y;
                for(double angle = 0; angle <= 2*vnl_math::pi; angle += vnl_math::pi/60.0) {
                    for (int raio = 0; raio < storage[k].axial[i]->circle->r*2; raio++) {
                        pixelIndex[0] = (int)(storage[k].axial[i]->circle->x + raio*cos(angle));
                        pixelIndex[1] = (int)(storage[k].axial[i]->circle->y + raio*sin(angle));
                        if(c_region.IsInside(pixelIndex)) {
                            if((int)image->GetPixel(pixelIndex) > 0 && (int)image->GetPixel(centerIndex) == 0) {
                                axialAxisX = axialAxisX + pixelIndex[0];
                                axialAxisY = axialAxisY + pixelIndex[1];
                                total++;
                                break;
                            }
                        }
                    }
                }
            }
        }
        // Calculate axial axis
        storage[k].axialAxis.x = (int)(axialAxisX/total)+1;
        storage[k].axialAxis.y = (int)(axialAxisY/total)+1;
    }
}

void MivImage::setAxialAxis(int frame, int x, int y) {
   storage[frame].axialAxis.x = x;
   storage[frame].axialAxis.y = y;
}

AxisCoords MivImage::getAxialAxis(int frame) {
   return storage[frame].axialAxis;
}

void MivImage::defineBullsEyeRadius() {

   for (int k=0; k < (int)size[3]; k++) {
      int bulls_eye_radius = storage[k].axial[(int)(size[2]/2)]->circle->r;

      //fprintf(stdout, "frame: %d\t initial radius: %d\n", k, bulls_eye_radius);

      for (int raio = storage[k].axial[(int)(size[2]/2)]->circle->r; raio < (int)(size[1]/2); raio++) {
         int count = 0;
         for(double angle = 0; angle <= 2*vnl_math::pi; angle += vnl_math::pi/60.0) {
            for (int i=0; i < (int)size[2]; i++) {
               if (storage[k].axial[i]->isSegmented) {
                  
                  ImageType2D::Pointer image;
                  image = storage[k].axial[i]->segmentationMask;
                  ImageType2D::RegionType c_region = image->GetLargestPossibleRegion();
                  
                  ImageType2D::IndexType pixelIndex;

                  pixelIndex[0] = (int)(getAxialAxis(k).x + raio*cos(angle));
                  pixelIndex[1] = (int)(getAxialAxis(k).y + raio*sin(angle));

                  if(c_region.IsInside(pixelIndex)) {
                     if((int)image->GetPixel(pixelIndex) > 0)
                        count++;
                  }
               }
               else
                  continue;
            }
         }
         if(count==0) {
            bulls_eye_radius = raio-1;
            break;
         }
      }
      setBullsEyeRadius(k, bulls_eye_radius);
      /*for (int i=0; i < (int)size[2]; i++) {
         for(double angle = 0; angle <= 2*vnl_math::pi; angle += vnl_math::pi/60.0) {
            ImageType2D::IndexType pixelIndex;
            pixelIndex[0] = (int)(getAxialAxis(k).x + bulls_eye_radius*cos(angle));
            pixelIndex[1] = (int)(getAxialAxis(k).y + bulls_eye_radius*sin(angle));
            if (storage[k].axial[i]->isSegmented)
               storage[k].axial[i]->segmented->SetPixel(pixelIndex, 255);
         }
      }
      fprintf(stdout, "frame: %d\t radius: %d\n", k, bulls_eye_radius);*/
   }
}

void MivImage::setBullsEyeRadius(int frame, int radius) {
   storage[frame].bulls_eye_radius = radius;
}

int MivImage::getBullsEyeRadius(int frame) {
   return storage[frame].bulls_eye_radius;
}

ImageType2D::Pointer MivImage::multiplyImages(ImageType2D::Pointer image1, ImageType2D::Pointer image2) {
   MultiplyFilterType::Pointer mfilter = MultiplyFilterType::New();
   mfilter->SetInput1(image1);
   mfilter->SetInput2(image2);
   mfilter->Update();
   return mfilter->GetOutput();
}

int MivImage::performWatershed() {

   typedef vector<ImageProperties*> propvector;

   for (int k=0; k < (int)size[3]; k++) {
      //int cnt = 0;
      for (propvector::iterator i = storage[k].axial.begin(); i != storage[k].axial.end(); ++i) {
         ImageProperties* imgp = (*i);
         if (imgp->existMarker && !imgp->isSegmented) {
            Watershed *ws = new Watershed(settingsType);
            ws->createWSGradientImage(imgp->image);
            ws->setMarker(imgp->marker);
            imgp->segmentationMask = ws->segmentationMask(ws->performWatershed());
            imgp->segmented  = multiplyImages(imgp->image, imgp->segmentationMask);
            imgp->isSegmented = true;
            
            //debug
            /*if (k == 0 && cnt == 11) {
               saveRGBImage1(imgp->image, "/home/luisrpp/Desktop/image.png");
               saveRGBImage1(ws->getGradient(), "/home/luisrpp/Desktop/gradient.png");
               saveRGBImageMarker1(imgp->image, "/home/luisrpp/Desktop/marker.png", imgp->marker);
               saveRGBImageMarker1(ws->performWatershed(), "/home/luisrpp/Desktop/watershed.png", ws->performWatershed());
               saveRGBImage1(imgp->segmented, "/home/luisrpp/Desktop/result.png");
            }*/
            
            delete ws;
         }
         //cnt++;
      }
   }

   createAxialAxis();
   defineBullsEyeRadius();
   return EXIT_SUCCESS;
}

int MivImage::performWatershed3D() {

   for (int i=0; i < (int)size[3]; i++) {

      Watershed3D *ws = new Watershed3D(settingsType);
      ws->createWSGradientImage(storage[i].image3D);

      // Create Marker Volume
      JoinSeriesFilterType3D::Pointer joinSeries3D = JoinSeriesFilterType3D::New();
      for (int j = 0; j < this->getSize(2); j++) {
         ImageType2D::Pointer image2D;
         if (this->existMarker(i, j)) {
            image2D = this->getMarker(i, j);
         }
         else {
            image2D = copyImageStructure(this->getImage2D(i, 2, j));
         }

         typedef Flip2D::FlipAxesArrayType FlipAxesArrayType;
         FlipAxesArrayType flipArray;
         flipArray[0] = 0;
         flipArray[1] = 1;

         Flip2D::Pointer flip = Flip2D::New();
         flip->SetInput(image2D);
         flip->SetFlipAxes(flipArray);
         flip->FlipAboutOriginOn();
         flip->Update();

         joinSeries3D->PushBackInput(flip->GetOutput());
      }
      joinSeries3D->Update();

      ws->setMarker(joinSeries3D->GetOutput());
      ImageType3D::Pointer seg_volume = ws->performWatershed();

      for (int j = 0; j < this->getSize(2); j++) {
         if (this->existMarker(i, j)) {
            ImageType2D::Pointer seg_slice = extractImage2D(seg_volume, 2, j);

            storage[i].axial[j]->segmentationMask = ws->segmentationMask(seg_slice);
            storage[i].axial[j]->segmented  = multiplyImages(storage[i].axial[j]->image, storage[i].axial[j]->segmentationMask);
            storage[i].axial[j]->isSegmented = true;
         }
      }
      delete ws;
   }

   createAxialAxis();
   return EXIT_SUCCESS;
}

ImageType2D::Pointer MivImage::performWatershed(ImageType2D::Pointer image, ImageType2D::Pointer marker) {
   Watershed *ws = new Watershed(settingsType);
   ws->createWSGradientImage(image);
   ws->setMarker(marker);
   return multiplyImages(image, ws->segmentationMask(ws->performWatershed()));
}

void MivImage::setBullsEyeImage(ImageType2D::Pointer bullseye, int frame) {
   storage[frame].bullsEyeCreated = true;
   storage[frame].bulls_eye = bullseye;
}

ImageType2D::Pointer MivImage::getBullsEyeImage(int frame) {
   return storage[frame].bulls_eye;
}

bool MivImage::isBullsEyeCreated(int frame) {
   return storage[frame].bullsEyeCreated;
}

void MivImage::normalization() {
   PixelType maxValue, aux, factor;
   
   for (int i=0; i < (int)size[3]; i++) {
      maxValue = 0;
      // Get the max value for each frame
      for (int j = 0; j < this->getSize(2); j++) {
         if (storage[i].axial[j]->isSegmented) {
            aux = calculateImageMaxPixel(storage[i].axial[j]->segmented);
            if (aux > maxValue) {
               maxValue = aux;
            }
         }
      }
      // Factor to normalize the current frame
      factor = 100.0/maxValue;
      
      // Normalize the segmented left ventricle
      for (int j = 0; j < this->getSize(2); j++) {
         if (storage[i].axial[j]->isSegmented) {
            ImageType2D::Pointer aux_image = storage[i].axial[j]->segmented;
            
            ImageType2D::IndexType pixelIndex;
            for (int ii = 0;  ii < (int)aux_image->GetLargestPossibleRegion().GetSize()[0]; ii++) {
               for (int jj = 0;  jj < (int)aux_image->GetLargestPossibleRegion().GetSize()[1]; jj++) {
                  pixelIndex[0] = ii;
                  pixelIndex[1] = jj;
                  
                  ImageType2D::RegionType i_region = aux_image->GetLargestPossibleRegion();
                  if (i_region.IsInside(pixelIndex)) {
                     aux_image->SetPixel( pixelIndex, aux_image->GetPixel(pixelIndex) * factor );
                  }
               }
            }
         }
      }
   }
}

MivImage::~MivImage() {
   for (int k=0; k < (int)size[3]; k++) {
      for (int i=0; i < (int)size[0]; i++) {
         ImageProperties* sa = storage[k].sagittal[i];
         delete sa;
      }
      storage[k].sagittal.clear();

      for (int i=0; i < (int)size[1]; i++) {
         ImageProperties* cr = storage[k].coronal[i];
         delete cr;
      }
      storage[k].coronal.clear();

      for (int i=0; i < (int)size[2]; i++) {
         ImageProperties* ax = storage[k].axial[i];
         delete ax;
      }
      storage[k].axial.clear();
   }  
}
