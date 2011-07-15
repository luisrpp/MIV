/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: september 2008
****************************************************************/

#ifndef  __mivimage_h__
#define  __mivimage_h__

#include "mivsegmentation.h"

using namespace std;

typedef struct _AxisCoords {
   int x, y;
} AxisCoords;

typedef struct _ImageStorage
{
   ImageType3D::Pointer image3D;
   vector<ImageProperties*> sagittal;
   vector<ImageProperties*> coronal;
   vector<ImageProperties*> axial;
   bool axialSlicesSelected;
   int axialStart;
   int axialEnd;
   AxisCoords axialAxis;
   int bulls_eye_radius;
   ImageType2D::Pointer bulls_eye;
   bool bullsEyeCreated;
} ImageStorage;

class MivImage
{

public:
   MivImage();
   ~MivImage();
   void setMivSettings(MivSettingsType*);
   int writeImage(const char*);
   int writeImage4D(ImageType4D::Pointer, const char*);
   int writeImage3D(ImageType3D::Pointer, const char*);
   int writeImage2D(ImageType2D::Pointer, const char*);
   int readImage(const char*);
   int readDicom(const char*);
   int saveProject(const char*);
   int saveSegmentationMasks(const char*);
   int saveMarkers(const char*);
   int saveImage2D(int, int, int, int, const char*);
   ImageType4D::Pointer createMaskVolume();
   ImageType4D::Pointer createMarkerVolume();
   int saveRGBImage(ImageType2D::Pointer, const char*);
   vtkImageData* convertITKtoVTK(ImageType2D::Pointer);
   ImageType2D::Pointer convertVTKtoITK(vtkImageData*);
   ImageType2D::Pointer getImage2D(int, int, int);
   ImageType3D::Pointer getImage3D(int);
   vector<ImageProperties*> getImageVector(int, int);
   bool existMarker(int frame, int slice);
   bool isSegmented(int frame, int slice);
   void setAxialRegion(int frame, int slice, AxialRegion);
   AxialRegion getAxialRegion(int frame, int slice);
   void setBullsEyeRadius(int frame, int radius);
   int getBullsEyeRadius(int frame);
   bool hasAxialRegion(int frame, int slice);
   ImageType2D::Pointer getMarker(int frame, int slice);
   void setMarker(ImageType2D::Pointer, int frame, int slice);
   void createBlankMarker(int frame, int slice);
   void removeMarker(int frame, int slice);
   ImageType2D::Pointer getSegmented(int frame, int slice);
   ImageType2D::Pointer getSegmentationMask(int frame, int slice);
   int getSize(int);
   PixelType getMinimumValue();
   PixelType getMaximumValue();

   void normalization();
   void createAxialAxis();
   void defineBullsEyeRadius();
   void setAxialAxis(int, int, int);
   AxisCoords getAxialAxis(int);
   void selectAxialSlices();
   vector<int> getAxialSliceDelimitations(int);
   bool isAxialSlicesSelected(int);
   void setAxialSliceDelimitations(int, int, int);
   int findAxialCircles();
   int findAxialRings();
   int findAxialPoints();
   int performWatershed();
   int performWatershed3D();
   ImageType2D::Pointer performWatershed(ImageType2D::Pointer, ImageType2D::Pointer);
   ImageType2D::Pointer multiplyImages(ImageType2D::Pointer, ImageType2D::Pointer);
   void setBullsEyeImage(ImageType2D::Pointer, int);
   ImageType2D::Pointer getBullsEyeImage(int);
   bool isBullsEyeCreated(int);
   
private:
   ImageStorage *storage;
   ImageType4D::Pointer image4D;
   ImageType4D::SizeType size;
   PixelType minimumValue;
   PixelType maximumValue;
   MivSettingsType* settingsType;
   char filename[255];
   
   void copyToImageStorage();
   void calculateImageMinMax(ImageType4D::Pointer);
   PixelType calculateImageMaxPixel(ImageType2D::Pointer);
   ImageType3D::Pointer extractImage3D(const ImageType4D::Pointer, const int);
   ImageType2D::Pointer extractImage2D(const ImageType3D::Pointer, const int, const int);
   ImageType3D::Pointer setOrientation(ImageType3D::Pointer);
};

#endif // def __mivimage_h__
