//---------------------------------------------------------------------------

#ifndef EditorPrefH
#define EditorPrefH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

#include "CustomObject.h"

#include "multi_color.hpp"
#include "multi_edit.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
#include <Menus.hpp>

class CEditObject;
class Mesh;
//---------------------------------------------------------------------------
class TfrmEditorPreferences : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPageControl *pcObjects;
    TPanel *paBottom;
    TTabSheet *tsMouse;
	TTabSheet *tsScene;
    TFormStorage *fsEditorPref;
    TGroupBox *GroupBox1;
    TMxLabel *RxLabel8;
	TTabSheet *tsEdit;
    TGroupBox *GroupBox2;
    TMxLabel *RxLabel6;
    TMxLabel *RxLabel7;
    TMultiObjSpinEdit *seBrushSize;
    TMxLabel *RxLabel9;
    TMultiObjSpinEdit *seBrushUpDepth;
    TMxLabel *RxLabel10;
    TMultiObjSpinEdit *seBrushDnDepth;
	TMxLabel *RxLabel16;
	TMultiObjColor *mc3DCursorColor;
	TTabSheet *tsCamera;
	TGroupBox *GroupBox8;
	TMxLabel *RxLabel19;
	TMxLabel *RxLabel20;
	TGroupBox *GroupBox4;
	TMxLabel *RxLabel11;
	TMxLabel *RxLabel1;
	TMultiObjSpinEdit *seZNear;
	TMultiObjSpinEdit *seZFar;
	TMxLabel *RxLabel22;
	TGroupBox *GroupBox9;
	TGroupBox *GroupBox10;
	TGroupBox *GroupBox11;
	TCheckBox *cbRandomScale;
	TCheckBox *cbRandomRotation;
	TMxLabel *RxLabel3;
	TMultiObjSpinEdit *seRandomScaleMinX;
	TMultiObjSpinEdit *seRandomScaleMinY;
	TMultiObjSpinEdit *seRandomScaleMinZ;
	TMxLabel *RxLabel5;
	TMultiObjSpinEdit *seRandomScaleMaxX;
	TMultiObjSpinEdit *seRandomScaleMaxY;
	TMultiObjSpinEdit *seRandomScaleMaxZ;
	TMxLabel *RxLabel23;
	TMultiObjSpinEdit *seRandomRotateMinY;
	TMultiObjSpinEdit *seRandomRotateMinZ;
	TMxLabel *RxLabel24;
	TMultiObjSpinEdit *seRandomRotateMaxX;
	TMultiObjSpinEdit *seRandomRotateMaxY;
	TMultiObjSpinEdit *seRandomRotateMaxZ;
	TMultiObjSpinEdit *seRandomRotateMinX;
	TGroupBox *GroupBox12;
	TMxLabel *RxLabel25;
	TMxLabel *RxLabel26;
	TCheckBox *cbRandomSize;
	TMultiObjSpinEdit *seRandomSizeMax;
	TMultiObjSpinEdit *seRandomSizeMin;
	TMultiObjSpinEdit *seBrushSegment;
	TMultiObjSpinEdit *seFOV;
	TMultiObjSpinEdit *seCameraSR;
	TMultiObjSpinEdit *seCameraSM;
	TMxLabel *RxLabel2;
	TMxLabel *RxLabel4;
	TMultiObjSpinEdit *seSM;
	TMultiObjSpinEdit *seSR;
	TMultiObjSpinEdit *seSS;
	TMxLabel *RxLabel17;
	TMultiObjSpinEdit *seUndoLevels;
	TGroupBox *GroupBox7;
	TMxLabel *RxLabel18;
	TCheckBox *cbBoxPickLimitedDepth;
	TMultiObjSpinEdit *seBoxPickDepthTolerance;
	TCheckBox *cbBoxPickFaceCulling;
	TGroupBox *GroupBox13;
	TMxLabel *RxLabel28;
	TMxLabel *RxLabel29;
	TMultiObjSpinEdit *seSnapMove;
	TMultiObjSpinEdit *seSnapAngle;
	TGroupBox *GroupBox6;
	TMxLabel *RxLabel12;
	TMultiObjSpinEdit *seDOHideDistance;
	TMxLabel *RxLabel13;
	TMultiObjSpinEdit *seDORayPickHeight;
	TGroupBox *GroupBox5;
	TMxLabel *RxLabel21;
	TMultiObjSpinEdit *seCameraFlySpeed;
	TMxLabel *RxLabel15;
	TMultiObjSpinEdit *seCameraFlyAltitude;
	TGroupBox *GroupBox3;
	TMxLabel *MxLabel1;
	TMxLabel *MxLabel2;
	TMultiObjSpinEdit *seGridNumberOfCells;
	TMultiObjSpinEdit *seGridSquareSize;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall fsEditorPrefRestorePlacement(TObject *Sender);
    void __fastcall mcColorDialogClick(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall seNavNetConectDistChange(TObject *Sender);
	void __fastcall stLMTexturesClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TfrmEditorPreferences(TComponent* Owner);
    int Run(TTabSheet* ts=0);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmEditorPreferences *frmEditorPreferences;
//---------------------------------------------------------------------------
#endif
