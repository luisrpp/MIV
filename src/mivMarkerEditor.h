/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: february 2009
****************************************************************/

#include <QMainWindow>
#include <QDialog>
#include <QThread>
#include <QMap>
#include <vector>

#include "ui_MARKEREDITOR.h"

#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkCamera.h"
#include "vtkPointPicker.h"
#include "vtkImageViewer2.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkImageBlend.h"
#include "vtkImageActor.h"

#include "mivimage.h"
#include "mivpreview.h"

typedef QMap<QString, vtkImageData*> SliceMap;

class vtkRenderer;
class vtkEventQtSlotConnect;
class vtkObject;
class vtkCommand;

class OutputGui {
   public:
      OutputGui();

      void setImage(vtkImageData*);
      vtkImageData* getImage();
      bool remove;
      
   private:
      vtkImageData* image;
};

class EditorState {
   public:
      EditorState(vtkImageData*);
      vtkImageData* image;
};

class EditorUndoRedo {
   public:
      EditorUndoRedo(int);
      void push(EditorState);
      EditorState pop();
      bool isEmpty();
      void push_redo(EditorState);
      EditorState pop_redo();
      void clearRedo();
      bool isRedoEmpty();

   private:
      unsigned int stack_size;
      vector<EditorState> m_stack;
      vector<EditorState> m_redo_stack;
};

class MivMarkerEditor : public QDialog, public Ui_frmMarker {
   Q_OBJECT
   public:
      MivMarkerEditor(MivImage*, int, int, int, OutputGui*);
      ~MivMarkerEditor();

      void setImage(vtkImageData*);
      void setImageMarker(vtkImageData*);
      vtkImageData* getImage();
      vtkImageData* getImageMarker();
      float getColor();
      vtkImageData* imageOverlay(vtkImageData*, vtkImageData*, float);
      int getPointerSize();
      int isEraserChecked();
      void enableUndo();
      void saveState();
      void clearRedo();
      vtkImageData* convertToRGB(vtkImageData*, vtkLookupTable*);

      void run();

      MivImage* mivi;
      float opacity;
      vtkImageViewer2* viewer;
      vtkImageData* image;
      vtkImageData* marker;
      EditorUndoRedo* undoRedo;
      
      vtkLookupTable *lutGray;
      vtkLookupTable *lutMarker;

      OutputGui* outputGui;

   public slots:
      void actionEraser();
      void actionSave();
      void actionCancel();
      void actionPreview();
      void actionUndo();
      void actionRedo();
      void actionRemove();
      void interpolate();
      void overlayControl();
      void selectFrame();
      void selectSlice();
      void changeColor();

   protected:

   private:
      SliceMap smap;
      int frame;
      int slice;
};

class vtkImageInteractionCallback : public vtkCommand {
   public:

      static vtkImageInteractionCallback* New() {
         return new vtkImageInteractionCallback;
      }
 
      vtkImageInteractionCallback();

      void SetInteractor(vtkRenderWindowInteractor*);
      void SetImageViewer(vtkImageViewer2*);
      void SetPicker(vtkPointPicker*);
      void SetEditorGui(MivMarkerEditor*);

      void line(vtkImageData*, int, int, int, int, double);
      void plotPoint(vtkImageData*, int, int, int, double);

      void performAction(int);

      virtual void Execute(vtkObject *, unsigned long event, void *);
 
   private:
      int Slicing;
      vtkRenderWindowInteractor* Interactor;
      vtkPointPicker* picker;
      vtkImageViewer2* viewer;
      int lastPoint[2];
      MivMarkerEditor* editorGui;
};
