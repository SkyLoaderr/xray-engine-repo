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
#include "RXCtrls.hpp"
#include "RXSpin.hpp"

#include "SceneObject.h"
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include <Grids.hpp>
#include "ElTree.hpp"
#include "RxMenus.hpp"
#include <Menus.hpp>
#include "multi_check.hpp"
#include "multi_edit.hpp"
#include "Placemnt.hpp"

class CEditObject;
class ETextureCore;
class CEditMesh;
struct st_Surface;
class CSMotion;
class COMotion;
//---------------------------------------------------------------------------
class TfrmPropertiesObject : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TOpenDialog *odMesh;
    TPageControl *pcObjects;
    TTabSheet *tsScript;
    TMemo *mmScript;
    TTabSheet *tsInfo;
    TRxLabel *RxLabel4;
    TRxLabel *RxLabel5;
	TTabSheet *tsSurfaces;
    TTabSheet *tsMeshes;
	TElTree *tvSurfaces;
    TPanel *paMatrix;
	TStringGrid *sgTransform;
    TElHeader *ElHeader2;
    TPanel *paBB;
    TStringGrid *sgBB;
    TElHeader *ElHeader1;
    TTabSheet *tsMainOptions;
    TEdit *edName;
    TMultiObjCheck *cbMakeDynamic;
    TRxLabel *RxLabel2;
    TBevel *Bevel2;
    TRxLabel *RxLabel10;
    TRxLabel *RxLabel11;
    TPanel *paBottom;
    TRxLabel *lbLibRef;
    TElTree *tvMeshes;
	TRxPopupMenu *pmNumericSet;
	TMenuItem *Position1;
	TMenuItem *Rotation1;
	TMenuItem *Scale1;
	TPanel *Panel2;
	TGroupBox *GroupBox4;
	TRxLabel *RxLabel1;
	TRxLabel *RxLabel3;
	TRxLabel *lbVertices;
	TRxLabel *lbFaces;
	TExtBtn *ebDropper;
	TStaticText *stNumericSet;
	TTabSheet *tsOAnimation;
	TTabSheet *tsSAnimation;
	TGroupBox *gbOMotion;
	TRxLabel *RxLabel20;
	TRxLabel *lbOMotionName;
	TRxLabel *RxLabel23;
	TRxLabel *lbOMotionFrames;
	TRxLabel *lbOMotionFPS;
	TRxLabel *RxLabel28;
	TElTree *tvSMotions;
	TGroupBox *gbTemplates;
	TExtBtn *ExtBtn6;
	TExtBtn *ExtBtn7;
	TExtBtn *ExtBtn8;
	TExtBtn *ExtBtn9;
	TExtBtn *ExtBtn10;
	TElTree *tvOMotions;
	TGroupBox *gbSMotion;
	TRxLabel *RxLabel29;
	TRxLabel *lbSMotionName;
	TRxLabel *RxLabel24;
	TRxLabel *lbSMotionFrames;
	TRxLabel *RxLabel27;
	TRxLabel *lbSMotionFPS;
	TBevel *Bevel1;
	TRxLabel *RxLabel30;
	TRxLabel *RxLabel31;
	TRxLabel *RxLabel32;
	TMultiObjSpinEdit *seSMotionSpeed;
	TMultiObjSpinEdit *seSMotionAccrue;
	TMultiObjSpinEdit *seSMotionFalloff;
	TExtBtn *ExtBtn3;
	TExtBtn *ExtBtn4;
	TPanel *paTex_Surf;
	TPanel *paTexture;
	TGroupBox *GroupBox2;
	TRxLabel *RxLabel7;
	TRxLabel *RxLabel8;
	TRxLabel *RxLabel9;
	TRxLabel *lbWidth;
	TRxLabel *lbHeight;
	TRxLabel *lbAlpha;
	TPanel *paImage;
	TPaintBox *im;
	TPanel *paSurface;
	TGroupBox *GroupBox1;
	TRxLabel *RxLabel18;
	TRxLabel *lbSurfSideFlag;
	TRxLabel *RxLabel35;
	TRxLabel *lbSurfFaces;
	TExtBtn *ebSelectShader;
	TRxLabel *lbShader;
	TExtBtn *ebSelectTexture;
	TRxLabel *lbTexture;
	TGroupBox *gbFVF;
	TRxLabel *RxLabel14;
	TRxLabel *lbSurfFVF_TCCount;
	TRxLabel *RxLabel6;
	TRxLabel *RxLabel12;
	TRxLabel *RxLabel15;
	TRxLabel *RxLabel16;
	TRxLabel *RxLabel17;
	TRxLabel *lbSurfFVF_XYZ;
	TRxLabel *lbSurfFVF_Normal;
	TRxLabel *lbSurfFVF_Diffuse;
	TRxLabel *lbSurfFVF_Specular;
	TRxLabel *lbSurfFVF_XYZRHW;
	TPanel *paSAnimBottom;
	TGroupBox *GroupBox5;
	TRxLabel *RxLabel22;
	TRxLabel *lbSMotionCount;
	TRxLabel *RxLabel26;
	TRxLabel *lbActiveSMotion;
	TRxLabel *RxLabel37;
	TRxLabel *lbSBoneCount;
	TGroupBox *gbSMotionCommand;
	TExtBtn *ebSAppendMotion;
	TExtBtn *ebSRenameMotion;
	TExtBtn *ebSDeleteMotion;
	TExtBtn *ebSMotionClear;
	TExtBtn *ebSMotionSave;
	TExtBtn *ebSMotionLoad;
	TPanel *Panel4;
	TGroupBox *GroupBox3;
	TRxLabel *RxLabel19;
	TRxLabel *lbOMotionCount;
	TRxLabel *RxLabel21;
	TRxLabel *lbActiveOMotion;
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
	TRxLabel *RxLabel39;
	TComboBox *cbSBonePart;
	TTabSheet *tsFX;
	TRxLabel *RxLabel40;
	TRxLabel *RxLabel34;
	TComboBox *cbSStartMotionBone;
	TMultiObjSpinEdit *seSMotionPower;
	TMultiObjCheck *cbSMotionStopAtEnd;
	TExtBtn *ebSReloadMotion;
    void __fastcall ebEditMeshClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall ebAddMeshClick(TObject *Sender);
    void __fastcall ebDeleteMeshClick(TObject *Sender);
    void __fastcall cbMakeDynamicClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall tsInfoShow(TObject *Sender);
    void __fastcall tsSurfacesShow(TObject *Sender);
    void __fastcall tsMeshesShow(TObject *Sender);
    void __fastcall tvSurfacesItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
    void __fastcall imPaint(TObject *Sender);
    void __fastcall tsMainOptionsShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
    void __fastcall ebMergeMeshClick(TObject *Sender);
	void __fastcall tvMeshesItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
	void __fastcall cbSurfSideFlagClick(TObject *Sender);
	void __fastcall ExtBtn1Click(TObject *Sender);
	void __fastcall ExtBtn2Click(TObject *Sender);
	void __fastcall Position1Click(TObject *Sender);
	void __fastcall Rotation1Click(TObject *Sender);
	void __fastcall Scale1Click(TObject *Sender);
	void __fastcall ebNumericSetMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall pcObjectsChange(TObject *Sender);
	void __fastcall tsOAnimationShow(TObject *Sender);
	void __fastcall tvOMotionsItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
	void __fastcall ebOAppendMotionClick(TObject *Sender);
	void __fastcall ebODeleteMotionClick(TObject *Sender);
	void __fastcall ebORenameMotionClick(TObject *Sender);
	void __fastcall tvOMotionsTryEdit(TObject *Sender, TElTreeItem *Item,
          TElHeaderSection *Section, TFieldTypes &CellType, bool &CanEdit);
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
	void __fastcall tvSMotionsTryEdit(TObject *Sender, TElTreeItem *Item,
          TElHeaderSection *Section, TFieldTypes &CellType, bool &CanEdit);
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
private:	// User declarations
	bool bMultiSelection;
    ETextureCore* tx_selected;
    st_Surface* surf_selected;
	CEditObject *m_EditObject;
    DEFINE_VECTOR(AnsiString,SH_NamesList,SH_NamesIt);
    SH_NamesList SH_Names;

    list<SceneObject*> m_Objects;

    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();

    CSMotion* selected_smotion;
    COMotion* selected_omotion;
    AnsiString   last_name;
    TElTreeItem* FEditNode;
	// object motion
	void __fastcall ebOResetActiveMotion(TElTreeItem* ignore_item);
    TElTreeItem* FindOMotionItem(const char* name);
	// skeleton motion
	void __fastcall ebSResetActiveMotion(TElTreeItem* ignore_item);
    TElTreeItem* FindSMotionItem(const char* name);
public:		// User declarations
    __fastcall TfrmPropertiesObject(TComponent* Owner);
    int __fastcall Run(ObjectList* pObjects, bool& bChange);

    void OnIdle();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPropertiesObject* frmPropertiesObject;
int frmPropertiesObjectRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
