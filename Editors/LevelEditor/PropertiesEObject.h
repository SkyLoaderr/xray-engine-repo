//---------------------------------------------------------------------------

#ifndef PropertiesEObjectH
#define PropertiesEObjectH
//---------------------------------------------------------------------------

#include "PropertiesList.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include "mxPlacemnt.hpp"
#include "ExtBtn.hpp"
#include <ComCtrls.hpp>
#include <StdCtrls.hpp>
#include "ElPgCtl.hpp"
#include "ElXPThemedControl.hpp"
#include "MXCtrls.hpp"
//refs
class CSceneObject;
class EImageThumbnail;
struct SRayPickInfo;

class TfrmPropertiesEObject : public TForm
{
__published:	// IDE-managed Components
	TFormStorage *fsStorage;
	TElPageControl *ElPageControl1;
	TElTabSheet *tsBasic;
	TElTabSheet *tsSurfaces;
	TPanel *paBasic;
	TPanel *paSurfaces;
	TPanel *Panel2;
	TGroupBox *gbTexture;
	TLabel *RxLabel7;
	TLabel *RxLabel8;
	TLabel *RxLabel9;
	TLabel *lbWidth;
	TLabel *lbHeight;
	TLabel *lbAlpha;
	TPanel *paImage;
	TPaintBox *pbImage;
	TGroupBox *GroupBox1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *lbXYZ;
	TLabel *lbNormal;
	TLabel *lbDiffuse;
	TLabel *Label8;
	TLabel *lbUVs;
	TLabel *Label10;
	TLabel *lb2Sided;
	TLabel *Label5;
	TLabel *lbSurfFaces;
	TPanel *Panel1;
	TExtBtn *ebSortByImage;
	TExtBtn *ebSortByName;
	TExtBtn *ebDropper;
	TExtBtn *ebSortByShader;
	TLabel *Label1;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall pbImagePaint(TObject *Sender);
private:	// User declarations
    TProperties* 	m_Basic;
    TProperties* 	m_Surfaces;
    bool			bModified;
    TOnModifiedEvent OnModifiedEvent;
    void __fastcall	OnSurfaceFocused	   	(TElTreeItem* item);
    EImageThumbnail*m_Thumbnail;
    CSceneObject* 	m_pEditObject;
    void			ResetSurfInfo			(bool imageinfo, bool surfinfo);
    void			FillBasicProps			();
    void			FillSurfProps			();
	void __fastcall OnAfterShaderEdit		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall OnAfterTextureEdit		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall OnAfterTransformation	(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall RotateOnAfterEdit	 	(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall RotateOnBeforeEdit		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall RotateOnDraw			(PropValue* sender, LPVOID draw_val);
public:
public:		// User declarations
    __fastcall 		TfrmPropertiesEObject	(TComponent* Owner);
	static TfrmPropertiesEObject* CreateProperties	(TWinControl* parent=0, TAlign align=alNone, TOnModifiedEvent modif=0);
	static void 	DestroyProperties		(TfrmPropertiesEObject*& props);
	void 			UpdateProperties		(CSceneObject* S);
    void			ShowProperties			(){Show();}
    void			HideProperties			(){Hide();}
    bool __fastcall IsModified				(){return (m_Basic->IsModified()||m_Surfaces->IsModified());}
	void __fastcall OnPick					(const SRayPickInfo& pinf);
};
//---------------------------------------------------------------------------
#endif
