/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: november 2008
****************************************************************/

// Itk includes
#include "itkImage.h"
#include "itkNiftiImageIO.h"
#include "itkImageRegionIterator.h"
#include "itkNumericTraits.h"
#include "itkRGBPixel.h"
#include "itkExtractImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryThinningImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkFlipImageFilter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkGrayscaleErodeImageFilter.h"
#include "itkHoughTransform2DCirclesImageFilter.h"
#include "itkImageSeriesReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkJoinSeriesImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkMorphologicalWatershedFromMarkersImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkOrientedImage.h"
#include "itkOrientImageFilter.h"
#include "itkOtsuThresholdImageCalculator.h"
//#include "itkPasteImageFilter.h"
#include "itkPermuteAxesImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSpatialOrientation.h"
#include "itkSubtractImageFilter.h"
#include "itkComposeRGBImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkInvertIntensityImageFilter.h";

// cpp stdlib
#include <vector>

const unsigned int Dimension = 2;
typedef float PixelType;
typedef float AccumulatorPixelType;

typedef itk::Image< PixelType, 4 > ImageType4D;
typedef itk::Image< PixelType, 3 > ImageType3D;
typedef itk::Image< PixelType, 2 > ImageType2D;

typedef itk::ImageToVTKImageFilter< ImageType2D > ConnectorType;
typedef itk::VTKImageToImageFilter< ImageType2D > ConnectorType2;

typedef itk::ImageFileReader< ImageType4D  > ReaderType4D;
typedef itk::ImageSeriesReader< ImageType4D > DicomReaderType4D;
typedef itk::ImageFileReader< ImageType3D  > ReaderType3D;

typedef itk::ImageFileWriter< ImageType4D  > WriterType4D;
typedef itk::ImageFileWriter< ImageType3D  > WriterType3D;
typedef itk::ImageFileWriter< ImageType2D  > WriterType2D;

typedef itk::ExtractImageFilter< ImageType4D, ImageType3D > FilterType3D;
typedef itk::ExtractImageFilter< ImageType3D, ImageType2D > FilterType2D;

/*typedef itk::PasteImageFilter< ImageType3D > PasteFilter3D;
typedef itk::PasteImageFilter< ImageType4D > PasteFilter4D;*/
typedef itk::JoinSeriesImageFilter<ImageType2D, ImageType3D> JoinSeriesFilterType3D;
typedef itk::JoinSeriesImageFilter<ImageType3D, ImageType4D> JoinSeriesFilterType4D;

typedef itk::ImageRegionConstIteratorWithIndex< ImageType2D > IterType2D;

typedef unsigned char ChannelType;
typedef itk::Image< ChannelType, Dimension > ChannelImageType;
typedef ChannelImageType::IndexType IndexType;
typedef itk::ImageRegionConstIteratorWithIndex< ChannelImageType > IterType;
typedef itk::RescaleIntensityImageFilter< ImageType2D, ChannelImageType > RescaleType;

typedef itk::RGBPixel<ChannelType> RGBPixelType;
typedef itk::Image<RGBPixelType, Dimension> RGBImageType;
typedef itk::ImageFileWriter<RGBImageType> RGBWriterType;

typedef itk::CastImageFilter<ImageType2D, RGBImageType> CastImageFilter;

typedef itk::MinimumMaximumImageCalculator< ImageType4D > MinimumMaximumCalculatorType;
typedef itk::MinimumMaximumImageCalculator< ImageType2D > MinimumMaximumCalculatorType2;
typedef itk::MinimumMaximumImageCalculator< ImageType3D > MinimumMaximumCalculatorType3;

typedef itk::FlipImageFilter< ImageType2D > Flip2D;
typedef itk::FlipImageFilter< RGBImageType > RGBFlip2D;
typedef itk::PermuteAxesImageFilter< ImageType2D > PermuteAxesImageFilter;

typedef itk::HoughTransform2DCirclesImageFilter<PixelType, AccumulatorPixelType> HoughTransformFilterType;

typedef itk::BinaryBallStructuringElement<ImageType2D::PixelType, 2> SEBall;
typedef itk::GrayscaleDilateImageFilter<ImageType2D, ImageType2D, SEBall> DilateFilter;
typedef itk::GrayscaleErodeImageFilter<ImageType2D, ImageType2D, SEBall> ErodeFilter;
typedef itk::SubtractImageFilter<ImageType2D, ImageType2D, ImageType2D> SubtractFilter;

typedef itk::MorphologicalWatershedFromMarkersImageFilter<ImageType2D, ImageType2D> WatershedFilterType;
typedef itk::BinaryThinningImageFilter<ImageType2D, ImageType2D> ThinFilter;
typedef itk::BinaryThresholdImageFilter<ImageType2D, ImageType2D> ThreshFilter;
typedef itk::BinaryThresholdImageFilter<ImageType3D, ImageType3D> ThreshFilter3D;

typedef itk::MultiplyImageFilter< ImageType2D, ImageType2D, ImageType2D > MultiplyFilterType;


typedef itk::MorphologicalWatershedFromMarkersImageFilter<ImageType3D, ImageType3D> Watershed3DFilterType;

typedef itk::BinaryBallStructuringElement<ImageType3D::PixelType, 3> SEBall3D;
typedef itk::GrayscaleDilateImageFilter<ImageType3D, ImageType3D, SEBall3D> DilateFilter3D;
typedef itk::GrayscaleErodeImageFilter<ImageType3D, ImageType3D, SEBall3D> ErodeFilter3D;
typedef itk::SubtractImageFilter<ImageType3D, ImageType3D, ImageType3D> SubtractFilter3D;

typedef itk::OtsuThresholdImageCalculator<ImageType2D> OtsuThresholdImageCalculator;
typedef itk::OtsuThresholdImageCalculator<ImageType3D> OtsuThresholdImageCalculator3D;

typedef itk::InvertIntensityImageFilter<ImageType2D, ImageType2D> InvertIntensityImageFilter2D;

// Associates each region with a pixel value. Used to build the marker image.
typedef enum MarkerValues {
   BACKGROUND = 50,
   LEFTVENTRICLE = 100,
   OTHER = 150,
   /* It was defined for regions where we should complete the circle in
      the external marker but it is not actually used. */
   COMPLETION = 200
} MarkerValues;

typedef struct _Circle {
   int x, y;
   int r;
} Circle;

typedef enum AxialRegion {
   UNKNOWN  = 0,
   APEX = 1,
   APICAL = 2,
   MEDIAL = 3,
   BASAL = 4
} AxialRegion;

typedef struct _ImageProperties {
   ImageType2D::Pointer image;
   ImageType2D::Pointer marker;
   ImageType2D::Pointer segmented;
   ImageType2D::Pointer segmentationMask;
   bool existMarker;
   bool isSegmented;
   int type;
   int slice;
   int frame;
   Circle* circle;
   bool axialRegionDefined;
   AxialRegion region;
   char filename[255];
} ImageProperties;

typedef struct _MivSettingsType {
   bool lut;
   bool interpolate;
   bool axialDelimitations;
   bool checkAxis;
   int thresholdMethod;
   int thresholdValue;
   bool useSegmentedImage;
} MivSettingsType;

