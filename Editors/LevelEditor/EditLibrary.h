//---------------------------------------------------------------------------

#ifndef EditLibraryH
#define EditLibraryH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>

#include "Library.h"
#include <Dialogs.hpp>
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
#include <Menus.hpp>
#include "PropertiesEObject.h"
#include "MxMenus.hpp"
#include "ElTreeAdvEdit.hpp"
#include "ItemList.h"
#include "MXCtrls.hpp"
//---------------------------------------------------------------------------
// refs
class TObjectPreview;
class EObjectThumbnail;
//---------------------------------------------------------------------------

class TfrmEditLibrary : public TForm
{
__published:	// IDE-managed Components
	TPanel *paCommands;
	TExtBtn *ebMakeThm;
	TFormStorage *fsStorage;
	TExtBtn *ebProperties;
	TCheckBox *cbPreview;
	TExtBtn *ebMakeLOD;
	TPanel *Panel3;
	TLabel *lbFaces;
	TLabel *RxLabel2;
	TLabel *RxLabel3;
	TLabel *lbVertices;
	TBevel *Bevel4;
	TPanel *paItems;
	TPanel *paControl;
	TExtBtn *ebImport;
	TExtBtn *ebMakeLWO;
	TExtBtn *ebSave;
	TExtBtn *ebCancel;
	TExtBtn *ExtBtn1;
	TExtBtn *ExtBtn2;
	TMxPanel *paImage;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebPropertiesClick(TObject *Sender);
    void __fastcall tvItemsDblClick(TObject *Sender);
    void __fastcall cbPreviewClick(TObject *Sender);
	void __fastcall ebMakeThmClick(TObject *Sender);
	void __fastcall paImagePaint(TObject *Sender);
	void __fastcall ebImportClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall ebMakeLODClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall ebMakeLWOClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall ExtBtn1Click(TObject *Sender);
	void __fastcall ExtBtn2Click(TObject *Sender);
private:	// User declarations
    void InitObjects();
    EObjectThumbnail* 		m_Thm;
	static TfrmEditLibrary*	form;
    CSceneObject* 			m_pEditObject;
    static FS_QueryMap		modif_map;
    static bool 			bFinalExit;
    static bool 			bExitResult;
    TfrmPropertiesEObject*  m_Props;
    void					UpdateObjectProperties();
    void					ChangeReference	(LPCSTR new_name);

	void __fastcall 		OnObjectRename	(LPCSTR p0, LPCSTR p1, EItemType type);

    TItemList*				m_Items;
	void __fastcall 		OnItemFocused	(TElTreeItem* item);
	bool 					GenerateLOD		(TElTreeItem* node);
    bool 					bFormLocked;
    void 					LockForm		()	{ bFormLocked = true;	paCommands->Enabled = false; 	paItems->Enabled = false; 	}
    void 					UnlockForm		()	{ bFormLocked = false;	paCommands->Enabled = true; 	paItems->Enabled = true;	}
public:		// User declarations
    void __fastcall 		OnModified		();
    __fastcall 				TfrmEditLibrary	(TComponent* Owner);
    static bool 			FinalClose		();
    static void __fastcall 	OnRender		();
    static void __fastcall 	ZoomObject		();
	static CSceneObject* __fastcall RayPick	(const Fvector& start, const Fvector& direction, SRayPickInfo* pinf);
    static void __fastcall 	ShowEditor		();
    static void __fastcall 	ResetSelected	();
    static void __fastcall 	RefreshSelected	();
};
//---------------------------------------------------------------------------
#endif
