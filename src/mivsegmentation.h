/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: november 2008
 ****************************************************************/

#include "mivdefinitions.h"

using namespace std;

// Function to copy image structure.
ImageType2D::Pointer copyImageStructure(const ImageType2D::Pointer input);

typedef struct _MW_data {
   ImageType2D::PixelType value;
   int type;
   float rank;
   int random;
} MW_data;

// This is only a container for Hough Transform parameters.
class HoughParameters {
   
public:
   //The constructor, with some default values.
   HoughParameters(int numberOfCircles = 1, float minimumRadius = 2.0,
                   float maximumRadius = 14.0, float sweepAngle = 0.0,
                   int sigmaGradient = 2, float accVariance = 5.0,
                   float discRadius = 1.2, float thresholdRate = 0.6) {
      this->numberOfCircles = numberOfCircles;
      this->minimumRadius = minimumRadius;
      this->maximumRadius = maximumRadius;
      this->sweepAngle = sweepAngle;
      this->sigmaGradient = sigmaGradient;
      this->accVariance = accVariance;
      this->discRadius = discRadius;
      this->thresholdRate = thresholdRate;
   }
   int numberOfCircles;
   float minimumRadius;
   float maximumRadius;
   float sweepAngle;
   int sigmaGradient;
   float accVariance;
   float discRadius;
   float thresholdRate;
};

// HoughTransform to find circles.
class HoughTransform {
private:
   MivSettingsType *settingsType;
   HoughParameters *hp;
public:
   HoughTransform(MivSettingsType*);
   ~HoughTransform();
   void setHoughParameters(HoughParameters*);
   HoughTransformFilterType::CirclesListType findCircles(const ImageType2D::Pointer);
   int findImageCircles(vector<ImageProperties*>, int, int);
   Circle* findLargestCircle(vector<ImageProperties*>);
   void filterCircles(vector<ImageProperties*>);
   vector<int> selectAxialSlicesFromSagittal(vector<ImageProperties*>, vector<ImageProperties*>);
   vector<int> selectAxialSlicesFromCoronal(vector<ImageProperties*>, vector<ImageProperties*>);
   vector<int> selectAxialSlices(vector<ImageProperties*>, vector<ImageProperties*>, vector<ImageProperties*>);
   vector<int> adjustAxialSlices(vector<ImageProperties*>, int, int);
};

class MannWhitneyTest {
private:
   MivSettingsType* settingsType;
   
   double U, U1, U2;
   double R1, R2, ER1, ER2;
   double n1, n2;
   double z;
   double samplePercent;
   int iterations;
   
   int generateRandomNumber(int);
   
public:
   MannWhitneyTest(MivSettingsType*);
   void mannWhitneyTest(ImageProperties*, Circle*);
   vector<MW_data> selectSamples(vector<MW_data>);
   void fixRank(vector<MW_data>*);
   void setSamplePercent(double);
   void setNumberOfIterations(int);
   
   void mmSaveRGBImage(ImageType2D::Pointer, const char*);
   void mmSaveMask(ImageType2D::Pointer, const char*, const char*, Circle*);
};

typedef struct _PixelInfo {
   ImageType2D::PixelType value;
   ImageType2D::IndexType index;
} PixelInfo;

// Morphological Watershed Class
class Watershed {
private:
   ImageType2D::Pointer marker;
   ImageType2D::Pointer gradient;
   MivSettingsType *settingsType;
public:
   Watershed(MivSettingsType*);
   void setMarker(ImageType2D::Pointer);
   ImageType2D::Pointer getGradient();
   void createWSGradientImage(ImageType2D::Pointer);
   ImageType2D::Pointer createCirclesMarkerImage(ImageProperties* prop);
   ImageType2D::Pointer createRingMarkerImage(ImageProperties* prop);
   ImageType2D::Pointer createPointMarkerImage(ImageProperties* prop);
   ImageType2D::Pointer performWatershed();
   ImageType2D::Pointer segmentationMask(ImageType2D::Pointer);
};

class Watershed3D : public Watershed {
private:
   ImageType3D::Pointer marker;
   ImageType3D::Pointer gradient;
   MivSettingsType *settingsType;
public:
   Watershed3D(MivSettingsType*);
   void setMarker(ImageType3D::Pointer);
   ImageType3D::Pointer getGradient();
   void createWSGradientImage(ImageType3D::Pointer);
   ImageType3D::Pointer performWatershed();
};
