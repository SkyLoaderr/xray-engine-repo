//---------------------------------------------------------------------------

#ifndef ScenePropertiesH
#define ScenePropertiesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CSPIN.h"
#include "RXCtrls.hpp"
#include "RXSpin.hpp"

#include "SceneObject.h"
#include "ToolEdit.hpp"
#include <Mask.hpp>
#include "ElTree.hpp"
#include "multiobj.hpp"
#include "Placemnt.hpp"
//---------------------------------------------------------------------------
class TfrmSceneProperties : public TForm
{
__published:	// IDE-managed Components
    TPageControl *pcBuildOptions;
    TTabSheet *tsMainOptions;
    TTabSheet *tsOptimizing;
    TTabSheet *tsTesselation;
    TTabSheet *tsLightmaps;
    TEdit *edLevelName;
    TRxLabel *RxLabel1;
    TRxLabel *RxLabel2;
    TDirectoryEdit *deLevelPath;
    TTabSheet *tsBuildStages;
    TCheckBox *cbTesselation;
    TCheckBox *cbProgressive;
    TCheckBox *cbLightmaps;
    TRichEdit *mmText;
    TRxLabel *RxLabel3;
    TTabSheet *tsProgressive;
    TTabSheet *tsVertexBuffers;
    TTabSheet *tsSubdivision;
    TTabSheet *tsVisibility;
    TMultiObjSpinEdit *seMaxEdge;
    TRxLabel *RxLabel4;
    TMultiObjSpinEdit *sePixelsPerMeter;
    TRxLabel *RxLabel5;
    TMultiObjSpinEdit *seVBMaxSize;
    TRxLabel *RxLabel6;
    TMultiObjSpinEdit *seVBMaxVertices;
    TRxLabel *RxLabel7;
    TMultiObjSpinEdit *seMaxSize;
    TRxLabel *RxLabel8;
    TMultiObjSpinEdit *seRelevance;
    TRxLabel *RxLabel9;
    TMultiObjSpinEdit *seViewDist;
    TRxLabel *RxLabel10;
    TButton *btOk;
    TButton *btCancel;
    TFormStorage *fsSceneProps;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btContinueClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall cbTesselationClick(TObject *Sender);
    void __fastcall cbProgressiveClick(TObject *Sender);
    void __fastcall cbLightmapsClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TfrmSceneProperties(TComponent* Owner);
    int __fastcall Run();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSceneProperties *frmSceneProperties;
//---------------------------------------------------------------------------
#endif
