//---------------------------------------------------------------------------

#ifndef PropertiesObjectH
#define PropertiesObjectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CSPIN.h"

#include "CustomObject.h"
#include "ElHeader.hpp"
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "multi_check.hpp"
#include "multi_edit.hpp"
#include "MXCtrls.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
#include <Grids.hpp>
#include <Menus.hpp>

class CEditMesh;
class CSurface;
class CSMotion;
class COMotion;
class EImageThumbnail;
//---------------------------------------------------------------------------
class TfrmPropertiesObject : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *ebOk;
    TPageControl *pcObjects;
    TTabSheet *tsScript;
    TMemo *mmScript;
    TTabSheet *tsInfo;
    TMxLabel *RxLabel4;
    TMxLabel *RxLabel5;
	TTabSheet *tsSurfaces;
    TTabSheet *tsMeshes;
    TPanel *paBB;
    TStringGrid *sgBB;
    TElHeader *ElHeader1;
    TTabSheet *tsMainOptions;
    TMultiObjCheck *cbMakeDynamic;
    TMxLabel *RxLabel11;
    TPanel *paBottom;
	TMxPopupMenu *pmNumericSet;
	TMenuItem *Position1;
	TMenuItem *Rotation1;
	TMenuItem *Scale1;
	TPanel *Panel2;
	TGroupBox *GroupBox4;
	TMxLabel *RxLabel1;
	TMxLabel *RxLabel3;
	TMxLabel *lbVertices;
	TMxLabel *lbFaces;
	TExtBtn *ebDropper;
	TTabSheet *tsOAnimation;
	TTabSheet *tsSAnimation;
	TGroupBox *gbOMotion;
	TMxLabel *RxLabel20;
	TMxLabel *lbOMotionName;
	TMxLabel *RxLabel23;
	TMxLabel *lbOMotionFrames;
	TMxLabel *lbOMotionFPS;
	TMxLabel *RxLabel28;
	TElTree *tvSMotions;
	TGroupBox *gbTemplates;
	TExtBtn *ExtBtn6;
	TExtBtn *ExtBtn7;
	TExtBtn *ExtBtn8;
	TExtBtn *ExtBtn9;
	TExtBtn *ExtBtn10;
	TElTree *tvOMotions;
	TGroupBox *gbSMotion;
	TMxLabel *RxLabel29;
	TMxLabel *lbSMotionName;
	TMxLabel *RxLabel24;
	TMxLabel *lbSMotionFrames;
	TMxLabel *RxLabel27;
	TMxLabel *lbSMotionFPS;
	TBevel *Bevel1;
	TMxLabel *RxLabel30;
	TMxLabel *RxLabel31;
	TMxLabel *RxLabel32;
	TMultiObjSpinEdit *seSMotionSpeed;
	TMultiObjSpinEdit *seSMotionAccrue;
	TMultiObjSpinEdit *seSMotionFalloff;
	TExtBtn *ExtBtn3;
	TExtBtn *ExtBtn4;
	TPanel *paTex_Surf;
	TPanel *paTexture;
	TGroupBox *GroupBox2;
	TMxLabel *RxLabel7;
	TMxLabel *RxLabel8;
	TMxLabel *RxLabel9;
	TMxLabel *lbWidth;
	TMxLabel *lbHeight;
	TMxLabel *lbAlpha;
	TPanel *paImage;
	TPaintBox *pbImage;
	TPanel *paSurface;
	TGroupBox *GroupBox1;
	TMxLabel *RxLabel18;
	TMxLabel *lbSurfSideFlag;
	TMxLabel *RxLabel35;
	TMxLabel *lbSurfFaces;
	TExtBtn *ebSelectShader;
	TMxLabel *lbShader;
	TExtBtn *ebSelectTexture;
	TMxLabel *lbTexture;
	TGroupBox *gbFVF;
	TMxLabel *RxLabel14;
	TMxLabel *lbSurfFVF_TCCount;
	TMxLabel *RxLabel6;
	TMxLabel *RxLabel12;
	TMxLabel *RxLabel15;
	TMxLabel *RxLabel16;
	TMxLabel *RxLabel17;
	TMxLabel *lbSurfFVF_XYZ;
	TMxLabel *lbSurfFVF_Normal;
	TMxLabel *lbSurfFVF_Diffuse;
	TMxLabel *lbSurfFVF_Specular;
	TMxLabel *lbSurfFVF_XYZRHW;
	TPanel *paSAnimBottom;
	TGroupBox *GroupBox5;
	TMxLabel *RxLabel22;
	TMxLabel *lbSMotionCount;
	TMxLabel *RxLabel26;
	TMxLabel *lbActiveSMotion;
	TMxLabel *RxLabel37;
	TMxLabel *lbSBoneCount;
	TGroupBox *gbSMotionCommand;
	TExtBtn *ebSAppendMotion;
	TExtBtn *ebSRenameMotion;
	TExtBtn *ebSDeleteMotion;
	TExtBtn *ebSMotionClear;
	TExtBtn *ebSMotionSave;
	TExtBtn *ebSMotionLoad;
	TPanel *Panel4;
	TGroupBox *GroupBox3;
	TMxLabel *RxLabel19;
	TMxLabel *lbOMotionCount;
	TMxLabel *RxLabel21;
	TMxLabel *lbActiveOMotion;
	TGroupBox *gbOMotionCommand;
	TExtBtn *ebOMotionAppend;
	TExtBtn *ebOMotionRename;
	TExtBtn *ebOMotionDelete;
	TExtBtn *ebOMotionClear;
	TExtBtn *ebOMotionSave;
	TExtBtn *ebOMotionLoad;
	TPanel *Panel5;
	TExtBtn *ExtBtn1;
	TExtBtn *ExtBtn2;
	TFormStorage *fsStorage;
	TPageControl *pcAnimType;
	TTabSheet *tsCycle;
	TMxLabel *RxLabel39;
	TComboBox *cbSBonePart;
	TTabSheet *tsFX;
	TMxLabel *RxLabel40;
	TMxLabel *RxLabel34;
	TComboBox *cbSStartMotionBone;
	TMultiObjSpinEdit *seSMotionPower;
	TMultiObjCheck *cbSMotionStopAtEnd;
	TExtBtn *ebSReloadMotion;
	TExtBtn *ebCancel;
	TMxLabel *RxLabel10;
	TMultiObjSpinEdit *sePositionX;
	TMultiObjSpinEdit *sePositionY;
	TMultiObjSpinEdit *sePositionZ;
	TMxLabel *RxLabel13;
	TMxLabel *RxLabel25;
	TMultiObjSpinEdit *seScaleX;
	TMultiObjSpinEdit *seScaleY;
	TMultiObjSpinEdit *seScaleZ;
	TMultiObjSpinEdit *seRotateX;
	TMultiObjSpinEdit *seRotateY;
	TMultiObjSpinEdit *seRotateZ;
	TExtBtn *ebApply;
	TExtBtn *ebSelectShaderXRLC;
	TMxLabel *lbShaderXRLC;
	TElTree *tvMeshes;
	TElTree *tvSurfaces;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall cbMakeDynamicClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall tsInfoShow(TObject *Sender);
    void __fastcall tsSurfacesShow(TObject *Sender);
    void __fastcall tsMeshesShow(TObject *Sender);
    void __fastcall pbImagePaint(TObject *Sender);
    void __fastcall tsMainOptionsShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall cbSurfSideFlagClick(TObject *Sender);
	void __fastcall ExtBtn1Click(TObject *Sender);
	void __fastcall ExtBtn2Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall pcObjectsChange(TObject *Sender);
	void __fastcall tsOAnimationShow(TObject *Sender);
	void __fastcall tvOMotionsItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
	void __fastcall ebOAppendMotionClick(TObject *Sender);
	void __fastcall ebODeleteMotionClick(TObject *Sender);
	void __fastcall ebORenameMotionClick(TObject *Sender);
	void __fastcall tvOMotionsItemChange(TObject *Sender, TElTreeItem *Item,
          TItemChangeMode ItemChangeMode);
	void __fastcall tsSAnimationShow(TObject *Sender);
	void __fastcall ebSAppendMotionClick(TObject *Sender);
	void __fastcall ebSDeleteMotionClick(TObject *Sender);
	void __fastcall ebSRenameMotionClick(TObject *Sender);
	void __fastcall tvSMotionsItemChange(TObject *Sender, TElTreeItem *Item,
          TItemChangeMode ItemChangeMode);
	void __fastcall tvSMotionsItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
	void __fastcall seSMotionChange(TObject *Sender);
	void __fastcall ebSMotionClearClick(TObject *Sender);
	void __fastcall ebSMotionSaveClick(TObject *Sender);
	void __fastcall ebSMotionLoadClick(TObject *Sender);
	void __fastcall ebOMotionClearClick(TObject *Sender);
	void __fastcall ebOMotionSaveClick(TObject *Sender);
	void __fastcall ebOMotionLoadClick(TObject *Sender);
	void __fastcall seSMotionSpeedKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall seSMotionSpeedLWChange(TObject *Sender, int Val);
	void __fastcall ebSelectShaderClick(TObject *Sender);
	void __fastcall ebSelectTextureClick(TObject *Sender);
	void __fastcall ebSReloadMotionClick(TObject *Sender);
	void __fastcall ebApplyClick(TObject *Sender);
	void __fastcall seTransformChange(TObject *Sender);
	void __fastcall ebSelectShaderXRLCClick(TObject *Sender);
	void __fastcall tvMeshesItemFocused(TObject *Sender);
	void __fastcall tvSurfacesItemFocused(TObject *Sender);
private:	// User declarations
    EImageThumbnail*		m_Thm;
    CSurface* 				m_Surf;

    CSMotion* 				selected_smotion;
    COMotion* 				selected_omotion;
    AnsiString   			last_name;
    TElTreeItem* 			FEditNode;

    bool 					bLoadMode;

	// object motion
	void __fastcall 		ebOResetActiveMotion(TElTreeItem* ignore_item);
    TElTreeItem* 			FindOMotionItem(const char* name);
	// skeleton motion
	void __fastcall 		ebSResetActiveMotion(TElTreeItem* ignore_item);
    TElTreeItem* 			FindSMotionItem(const char* name);

    // static part
	static CEditableObject* m_EditObject;
	static 	TfrmPropertiesObject* form;

    // object init&save
    void 					GetObjectsInfo     ();
    void 					ApplyObjectsInfo   ();
public:		// User declarations
    __fastcall 				TfrmPropertiesObject(TComponent* Owner);
    static void __fastcall 	ShowProperties();
    static void __fastcall 	HideProperties();
    static bool __fastcall 	Visible(){return !!form;}
    static void	SetCurrent	(CEditableObject* object);
    static void	Reset		(){m_EditObject=0;}
    static bool	IsModified	(){return form?form->ebOk->Enabled:false;}
    static void	OnPick		(const SRayPickInfo& pinf);
};
//---------------------------------------------------------------------------
#endif
