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
#include "CSPIN.h"
#include "RXCtrls.hpp"
#include "RXSpin.hpp"

#include "SceneObject.h"
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include <Grids.hpp>
#include "ElTree.hpp"
#include "Placemnt.hpp"
#include "RxMenus.hpp"
#include <Menus.hpp>
#include "multi_color.hpp"
#include "multi_edit.hpp"

class CEditObject;
class ETexture;
class Mesh;
class ETextureCore;
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
    TRxLabel *RxLabel8;
	TTabSheet *tsEdit;
    TGroupBox *GroupBox2;
    TRxLabel *RxLabel6;
    TRxLabel *RxLabel7;
    TMultiObjSpinEdit *seBrushSize;
    TRxLabel *RxLabel9;
    TMultiObjSpinEdit *seBrushUpDepth;
    TRxLabel *RxLabel10;
    TMultiObjSpinEdit *seBrushDnDepth;
	TRxLabel *RxLabel16;
	TMultiObjColor *mc3DCursorColor;
	TTabSheet *tsCamera;
	TRadioGroup *rgCameraStyle;
	TGroupBox *GroupBox8;
	TRxLabel *RxLabel19;
	TRxLabel *RxLabel20;
	TGroupBox *GroupBox4;
	TRxLabel *RxLabel11;
	TRxLabel *RxLabel1;
	TMultiObjSpinEdit *seZNear;
	TMultiObjSpinEdit *seZFar;
	TRxLabel *RxLabel22;
	TGroupBox *GroupBox9;
	TGroupBox *GroupBox10;
	TGroupBox *GroupBox11;
	TCheckBox *cbRandomScale;
	TCheckBox *cbRandomRotation;
	TRxLabel *RxLabel3;
	TMultiObjSpinEdit *seRandomScaleMinX;
	TMultiObjSpinEdit *seRandomScaleMinY;
	TMultiObjSpinEdit *seRandomScaleMinZ;
	TRxLabel *RxLabel5;
	TMultiObjSpinEdit *seRandomScaleMaxX;
	TMultiObjSpinEdit *seRandomScaleMaxY;
	TMultiObjSpinEdit *seRandomScaleMaxZ;
	TRxLabel *RxLabel23;
	TMultiObjSpinEdit *seRandomRotateMinY;
	TMultiObjSpinEdit *seRandomRotateMinZ;
	TRxLabel *RxLabel24;
	TMultiObjSpinEdit *seRandomRotateMaxX;
	TMultiObjSpinEdit *seRandomRotateMaxY;
	TMultiObjSpinEdit *seRandomRotateMaxZ;
	TMultiObjSpinEdit *seRandomRotateMinX;
	TGroupBox *GroupBox12;
	TRxLabel *RxLabel25;
	TRxLabel *RxLabel26;
	TCheckBox *cbRandomSize;
	TMultiObjSpinEdit *seRandomSizeMax;
	TMultiObjSpinEdit *seRandomSizeMin;
	TMultiObjSpinEdit *seBrushSegment;
	TMultiObjSpinEdit *seFOV;
	TMultiObjSpinEdit *seCameraSR;
	TMultiObjSpinEdit *seCameraSM;
	TRxLabel *RxLabel2;
	TRxLabel *RxLabel4;
	TMultiObjSpinEdit *seSM;
	TMultiObjSpinEdit *seSR;
	TMultiObjSpinEdit *seSS;
	TRxPopupMenu *pmLMTextures;
	TRxLabel *RxLabel17;
	TMultiObjSpinEdit *seUndoLevels;
	TGroupBox *GroupBox7;
	TRxLabel *RxLabel18;
	TCheckBox *cbBoxPickLimitedDepth;
	TMultiObjSpinEdit *seBoxPickDepthTolerance;
	TCheckBox *cbBoxPickFaceCulling;
	TGroupBox *GroupBox13;
	TRxLabel *RxLabel28;
	TRxLabel *RxLabel29;
	TMultiObjSpinEdit *seSnapMove;
	TMultiObjSpinEdit *seSnapAngle;
	TGroupBox *GroupBox3;
	TRxLabel *RxLabel27;
	TPanel *paImage;
	TPaintBox *pbImage;
	TStaticText *stLMTextures;
	TGroupBox *GroupBox6;
	TRxLabel *RxLabel12;
	TMultiObjSpinEdit *seDOHideDistance;
	TRxLabel *RxLabel13;
	TMultiObjSpinEdit *seDORayPickHeight;
	TGroupBox *GroupBox5;
	TRxLabel *RxLabel21;
	TMultiObjSpinEdit *seCameraFlySpeed;
	TRxLabel *RxLabel15;
	TMultiObjSpinEdit *seCameraFlyAltitude;
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
	void __fastcall stLMTexturesMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall pbImagePaint(TObject *Sender);
private:	// User declarations
	ETextureCore* m_LMTexture;
    bool bActive;
public:		// User declarations
    __fastcall TfrmEditorPreferences(TComponent* Owner);
    int Run(TTabSheet* ts=0);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmEditorPreferences *frmEditorPreferences;
//---------------------------------------------------------------------------
#endif
