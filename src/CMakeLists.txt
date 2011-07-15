PROJECT(Miv)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(ITK REQUIRED)
IF (ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR, "Please set ITK_DIR")
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK REQUIRED)
IF (VTK_FOUND)
    INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
    MESSAGE(FATAL_ERROR, "Please set VTK_DIR")
ENDIF(VTK_FOUND)

ADD_DEFINITIONS( -Wall -Wextra )

SET( QT_USE_QT3SUPPORT TRUE )
SET( QT_USE_QTXML TRUE )
SET (CMAKE_VERBOSE_MAKEFILE ON)
SET (CMAKE_SKIP_RPATH ON)

FIND_PACKAGE( Qt4 REQUIRED )

INCLUDE( ${QT_USE_FILE} )
QT4_WRAP_UI( QT_UI_HDRS mivgui.ui MARKEREDITOR.ui mivpreview.ui mivaxiseditor.ui mivaxialregion.ui mivsettings.ui mivsaveslice.ui mivadjustaxialslice.ui)
QT4_WRAP_CPP( QT_MOC_SRCS mivgui.h mivMarkerEditor.h mivpreview.h mivAxisEditor.h mivAxialRegionEditor.h MivBullsEye.h MivSettings.h MivSaveSlice.h MivAdjustAxialSlice.h)

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

ADD_EXECUTABLE( miv
  main.cxx
  mivgui.cxx
  mivimage.cxx
  mivsegmentation.cxx
  mivMarkerEditor.cxx
  mivpreview.cxx
  mivAxisEditor.cxx
  mivAxialRegionEditor.cxx
  MivBullsEye.cxx
  MivSettings.cxx
  MivSaveSlice.cxx
  MivAdjustAxialSlice.cxx
  ${QT_MOC_SRCS}
  ${QT_UI_HDRS} )

TARGET_LINK_LIBRARIES( miv
  QVTK
  ${QT_LIBRARIES}
  ITKBasicFilters
  ITKIO
  vtkRendering
  vtkGraphics
  vtkHybrid
  vtkImaging
  vtkIO
  vtkFiltering
  vtkCommon )
