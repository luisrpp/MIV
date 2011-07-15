/****************************************************************
 * Software: Medical Image Viewer
 * Author: Luis Roberto Pereira de Paula
 * Date: march 2009
****************************************************************/

#include <QMainWindow>
#include <QDialog>
#include <QThread>
#include <QMap>
#include <vector>

#include "ui_mivaxiseditor.h"

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

typedef QMap<QString, vtkImageData*> SliceMap;

class AxisEditorState {
   public:
      AxisEditorState(int, int, AxisCoords);
      int frame;
      int slice;
      AxisCoords coords;
};

class AxisEditorUndoRedo {
   public:
      AxisEditorUndoRedo(int);
      void push(AxisEditorState);
      AxisEditorState pop();
      bool isEmpty();
      void push_redo(AxisEditorState);
      AxisEditorState pop_redo();
      void clearRedo();
      bool isRedoEmpty();

   private:
      unsigned int stack_size;
      vector<AxisEditorState> m_stack;
      vector<AxisEditorState> m_redo_stack;
};

class MivAxisEditor : public QDialog, public Ui_frmAxisEditor {
   Q_OBJECT
   public:
      MivAxisEditor(MivImage*);
      ~MivAxisEditor();

      MivImage* mivi;
      vtkImageViewer2* viewer;
      vector<AxisCoords> coords;
      vector<int> radius;

      void loadCoords();
      vtkImageData* drawAxis(int);
      vtkImageData* imageOverlay(vtkImageData*, vtkImageData*, float);
      vtkImageData* getCurrentImage();
      int getFrame();
      void enableUndo();
      void saveState();
      void clearRedo();
      void changeCbox(int, int);

   private slots:
      void selectFrame();
      void selectSlice();
      void actionUndo();
      void actionRedo();
      void actionSave();
      void actionCancel();
      void actionSetRadius();

   private:
      AxisEditorUndoRedo* undoRedo;
      vtkImageData* currentImage;
      SliceMap smap;
      int frame;
};

class vtkAxisInteractionCallback : public vtkCommand {
   public:

      static vtkAxisInteractionCallback* New() {
         return new vtkAxisInteractionCallback;
      }
 
      void SetInteractor(vtkRenderWindowInteractor*);
      void SetImageViewer(vtkImageViewer2*);
      void SetPicker(vtkPointPicker*);
      void SetEditorGui(MivAxisEditor*);
      void performAction();

      virtual void Execute(vtkObject *, unsigned long event, void *);
 
   private:
      vtkAxisInteractionCallback();

      int Moving;
      vtkRenderWindowInteractor* Interactor;
      vtkPointPicker* picker;
      vtkImageViewer2* viewer;

      MivAxisEditor* editorGui;
};
