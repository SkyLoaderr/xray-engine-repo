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
	TFormStorage *fsStorage;
    TGroupBox *GroupBox1;
	TLabel *RxLabel8;
	TTabSheet *tsEdit;
    TGroupBox *GroupBox2;
	TLabel *RxLabel6;
	TLabel *RxLabel7;
    TMultiObjSpinEdit *seBrushSize;
	TLabel *RxLabel9;
    TMultiObjSpinEdit *seBrushUpDepth;
	TLabel *RxLabel10;
    TMultiObjSpinEdit *seBrushDnDepth;
	TLabel *RxLabel16;
	TMultiObjColor *mc3DCursorColor;
	TTabSheet *tsCamera;
	TGroupBox *GroupBox8;
	TLabel *RxLabel19;
	TLabel *RxLabel20;
	TGroupBox *GroupBox4;
	TLabel *RxLabel11;
	TLabel *RxLabel1;
	TMultiObjSpinEdit *seZNear;
	TMultiObjSpinEdit *seZFar;
	TLabel *RxLabel22;
	TGroupBox *GroupBox9;
	TGroupBox *GroupBox10;
	TGroupBox *GroupBox11;
	TCheckBox *cbRandomScale;
	TCheckBox *cbRandomRotation;
	TLabel *RxLabel3;
	TMultiObjSpinEdit *seRandomScaleMinX;
	TMultiObjSpinEdit *seRandomScaleMinY;
	TMultiObjSpinEdit *seRandomScaleMinZ;
	TLabel *RxLabel5;
	TMultiObjSpinEdit *seRandomScaleMaxX;
	TMultiObjSpinEdit *seRandomScaleMaxY;
	TMultiObjSpinEdit *seRandomScaleMaxZ;
	TLabel *RxLabel23;
	TMultiObjSpinEdit *seRandomRotateMinY;
	TMultiObjSpinEdit *seRandomRotateMinZ;
	TLabel *RxLabel24;
	TMultiObjSpinEdit *seRandomRotateMaxX;
	TMultiObjSpinEdit *seRandomRotateMaxY;
	TMultiObjSpinEdit *seRandomRotateMaxZ;
	TMultiObjSpinEdit *seRandomRotateMinX;
	TGroupBox *GroupBox12;
	TLabel *RxLabel25;
	TLabel *RxLabel26;
	TCheckBox *cbRandomSize;
	TMultiObjSpinEdit *seRandomSizeMax;
	TMultiObjSpinEdit *seRandomSizeMin;
	TMultiObjSpinEdit *seBrushSegment;
	TMultiObjSpinEdit *seFOV;
	TMultiObjSpinEdit *seCameraSR;
	TMultiObjSpinEdit *seCameraSM;
	TLabel *RxLabel2;
	TLabel *RxLabel4;
	TMultiObjSpinEdit *seSM;
	TMultiObjSpinEdit *seSR;
	TMultiObjSpinEdit *seSS;
	TLabel *RxLabel17;
	TMultiObjSpinEdit *seUndoLevels;
	TGroupBox *GroupBox7;
	TLabel *RxLabel18;
	TCheckBox *cbBoxPickLimitedDepth;
	TMultiObjSpinEdit *seBoxPickDepthTolerance;
	TCheckBox *cbBoxPickFaceCulling;
	TGroupBox *GroupBox13;
	TLabel *RxLabel28;
	TLabel *RxLabel29;
	TMultiObjSpinEdit *seSnapMove;
	TMultiObjSpinEdit *seSnapAngle;
	TGroupBox *GroupBox5;
	TLabel *RxLabel21;
	TMultiObjSpinEdit *seCameraFlySpeed;
	TLabel *RxLabel15;
	TMultiObjSpinEdit *seCameraFlyAltitude;
	TGroupBox *GroupBox3;
	TLabel *MxLabel1;
	TLabel *MxLabel2;
	TMultiObjSpinEdit *seGridNumberOfCells;
	TMultiObjSpinEdit *seGridSquareSize;
	TLabel *Label1;
	TMultiObjSpinEdit *seSnapMoveTo;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall fsStorageRestorePlacement(TObject *Sender);
    void __fastcall mcColorDialogClick(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall seNavNetConectDistChange(TObject *Sender);
	void __fastcall stLMTexturesClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TfrmEditorPreferences(TComponent* Owner);
    int Run(TTabSheet* ts=0);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmEditorPreferences *frmEditorPreferences;
//---------------------------------------------------------------------------
#endif
