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
#include "EditObject.h"
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
#include "ElTreeStdEditors.hpp"

class CEditMesh;
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
	TLabel *RxLabel4;
	TLabel *RxLabel5;
	TTabSheet *tsSurfaces;
    TTabSheet *tsMeshes;
    TPanel *paBB;
    TStringGrid *sgBB;
    TElHeader *ElHeader1;
    TTabSheet *tsMainOptions;
    TMultiObjCheck *cbMakeDynamic;
	TLabel *RxLabel11;
    TPanel *paBottom;
	TPanel *Panel2;
	TGroupBox *GroupBox4;
	TLabel *RxLabel1;
	TLabel *RxLabel3;
	TLabel *lbVertices;
	TLabel *lbFaces;
	TExtBtn *ebDropper;
	TGroupBox *gbTemplates;
	TExtBtn *ExtBtn6;
	TExtBtn *ExtBtn7;
	TExtBtn *ExtBtn8;
	TExtBtn *ExtBtn9;
	TExtBtn *ExtBtn10;
	TExtBtn *ExtBtn3;
	TExtBtn *ExtBtn4;
	TPanel *paTex_Surf;
	TPanel *paTexture;
	TGroupBox *GroupBox2;
	TLabel *RxLabel7;
	TLabel *RxLabel8;
	TLabel *RxLabel9;
	TLabel *lbWidth;
	TLabel *lbHeight;
	TLabel *lbAlpha;
	TPanel *paImage;
	TPaintBox *pbImage;
	TPanel *paSurface;
	TGroupBox *GroupBox1;
	TLabel *RxLabel18;
	TLabel *lbSurfSideFlag;
	TLabel *RxLabel35;
	TLabel *lbSurfFaces;
	TExtBtn *ebSelectShader;
	TLabel *lbShader;
	TExtBtn *ebSelectTexture;
	TLabel *lbTexture;
	TGroupBox *gbFVF;
	TLabel *RxLabel14;
	TLabel *lbSurfFVF_TCCount;
	TLabel *RxLabel6;
	TLabel *RxLabel12;
	TLabel *RxLabel15;
	TLabel *RxLabel16;
	TLabel *RxLabel17;
	TLabel *lbSurfFVF_XYZ;
	TLabel *lbSurfFVF_Normal;
	TLabel *lbSurfFVF_Diffuse;
	TLabel *lbSurfFVF_Specular;
	TLabel *lbSurfFVF_XYZRHW;
	TPanel *Panel5;
	TExtBtn *ExtBtn1;
	TExtBtn *ExtBtn2;
	TFormStorage *fsStorage;
	TExtBtn *ebCancel;
	TLabel *RxLabel10;
	TMultiObjSpinEdit *sePositionX;
	TMultiObjSpinEdit *sePositionY;
	TMultiObjSpinEdit *sePositionZ;
	TLabel *RxLabel13;
	TLabel *RxLabel25;
	TMultiObjSpinEdit *seScaleX;
	TMultiObjSpinEdit *seScaleY;
	TMultiObjSpinEdit *seScaleZ;
	TMultiObjSpinEdit *seRotateX;
	TMultiObjSpinEdit *seRotateY;
	TMultiObjSpinEdit *seRotateZ;
	TExtBtn *ebApply;
	TExtBtn *ebSelectShaderXRLC;
	TLabel *lbShaderXRLC;
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
	void __fastcall ebSelectShaderClick(TObject *Sender);
	void __fastcall ebSelectTextureClick(TObject *Sender);
	void __fastcall ebApplyClick(TObject *Sender);
	void __fastcall seTransformChange(TObject *Sender);
	void __fastcall ebSelectShaderXRLCClick(TObject *Sender);
	void __fastcall tvMeshesItemFocused(TObject *Sender);
	void __fastcall tvSurfacesItemFocused(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
    CSMotion* 				selected_smotion;
    COMotion* 				selected_omotion;
    AnsiString   			last_name;

    bool 					bLoadMode;

	// object motion
	void __fastcall 		ebOResetActiveMotion(TElTreeItem* ignore_item);
    TElTreeItem* 			FindOMotionItem(const char* name);
	// skeleton motion
	void __fastcall 		ebSResetActiveMotion(TElTreeItem* ignore_item);
    TElTreeItem* 			FindSMotionItem(const char* name);

    // static part
	static CEditableObject* m_LibObject;
    static SurfInstVec 		m_EditSurfaces;

    static void				GetSurfacesFromObject();
    static void				SetSurfacesToObject();

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
    static void	Reset		(){m_LibObject=0;}
    static bool	IsModified	(){return form?form->ebApply->Enabled:false;}
    static void	OnPick		(const SRayPickInfo& pinf);
};
//---------------------------------------------------------------------------
#endif
