//---------------------------------------------------------------------------
#ifndef FrameDetObjH
#define FrameDetObjH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
#include "RXCtrls.hpp"

#include "SceneObject.h"
#include "ui_customtools.h"
#include "multi_edit.hpp"
#include "Placemnt.hpp"
// refs
struct SDOClusterDef;
//---------------------------------------------------------------------------
class TfraDetailObject : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paSelect;
	TExtBtn *ebDORandomScale;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TExtBtn *ebRandomRotate;
	TGroupBox *GroupBox2;
	TRxLabel *RxLabel5;
	TMultiObjSpinEdit *seScaleMinY;
	TMultiObjSpinEdit *seScaleMaxY;
	TRxLabel *RxLabel6;
	TRxLabel *RxLabel7;
	TMultiObjSpinEdit *seScaleMinXZ;
	TMultiObjSpinEdit *seScaleMaxXZ;
	TRxLabel *RxLabel8;
	TFormStorage *fsStorage;
	TPanel *paCommand;
	TExtBtn *ebModeCluster;
	TLabel *Label1;
	TExtBtn *ExtBtn3;
	TExtBtn *ebModeObjects;
	TPanel *paClusterBrush;
	TLabel *Label3;
	TExtBtn *ExtBtn8;
	TRxLabel *RxLabel1;
	TMultiObjSpinEdit *seClusterDensity;
	TPanel *paObjectBrush;
	TLabel *Label2;
	TExtBtn *ExtBtn4;
	TExtBtn *ebObjectName;
	TRxLabel *lbObjectName;
	TRxLabel *RxLabel3;
	TRxLabel *RxLabel10;
	TExtBtn *ebObjectAdd;
	TExtBtn *ebObjectClear;
	TBevel *Bevel1;
	TMultiObjSpinEdit *seObjectBrushSize;
	TCheckBox *cbObjectUseSize;
	TMultiObjSpinEdit *seObjectPressure;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebObjectNameClick(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall ebModeClusterClick(TObject *Sender);
	void __fastcall ebModeObjectsClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraDetailObject(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
