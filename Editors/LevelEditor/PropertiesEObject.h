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
	TPanel *Panel1;
	TExtBtn *ebSortByImage;
	TExtBtn *ebSortByName;
	TExtBtn *ebDropper;
	TExtBtn *ebSortByShader;
	TLabel *Label1;
	TTimer *tmIdle;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall pbImagePaint(TObject *Sender);
	void __fastcall tmIdleTimer(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
private:	// User declarations
    TProperties* 	m_BasicProp;
    TProperties* 	m_SurfProp;
    bool			bModified;
    TOnModifiedEvent OnModifiedEvent;
    void __fastcall	OnSurfaceFocused	   	(TElTreeItem* item);
    EImageThumbnail*m_Thumbnail;
    CSceneObject* 	m_pEditObject;
    void			FillBasicProps			();
    void			FillSurfProps			();
	void __fastcall OnChangeShader			(PropValue* sender);
	void __fastcall OnChangeTransform		(PropValue* sender);

    bool			m_bNeedRefreshShaders;
    void			RefreshShaders			(){m_bNeedRefreshShaders = true;}
public:
public:		// User declarations
    __fastcall 		TfrmPropertiesEObject	(TComponent* Owner);
	static TfrmPropertiesEObject* CreateProperties	(TWinControl* parent=0, TAlign align=alNone, TOnModifiedEvent modif=0);
	static void 	DestroyProperties		(TfrmPropertiesEObject*& props);
	void 			UpdateProperties		(CSceneObject* S);
    void			ShowProperties			(){Show();}
    void			HideProperties			(){Hide();}
    bool __fastcall IsModified				(){return (m_BasicProp->IsModified()||m_SurfProp->IsModified());}
	void __fastcall OnPick					(const SRayPickInfo& pinf);
};
//---------------------------------------------------------------------------
#endif
