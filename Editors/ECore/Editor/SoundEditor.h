//---------------------------------------------------------------------------

#ifndef SoundLibH
#define SoundLibH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
#include <ImgList.hpp>

#include "ItemList.h"
#include "MXCtrls.hpp"

//---------------------------------------------------------------------------
// refs
class ESoundThumbnail;
class TProperties;
//---------------------------------------------------------------------------

class TfrmSoundLib : public TForm
{
__published:	// IDE-managed Components
	TPanel *paRight;
	TFormStorage *fsStorage;
	TPanel *paCommand;
	TExtBtn *ebOk;
	TPanel *paProperties;
	TPanel *paItems;
	TSplitter *Splitter1;
	TImageList *ImageList;
	TExtBtn *ebCancel;
	TExtBtn *ebRemoveCurrent;
	TExtBtn *ebImportSound;
	TExtBtn *ebRenameCurrent;
	TPanel *Panel1;
	TPanel *Panel2;
	TPanel *Panel3;
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ebCancelClick(TObject *Sender);
	void __fastcall ebRemoveCurrentClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ebImportSoundClick(TObject *Sender);
	void __fastcall ebRenameCurrentClick(TObject *Sender);
private:
// list functions
    void 				InitItemsList		();
	void __fastcall 	OnItemsFocused		(ListItemsVec& items);

    BOOL __fastcall		RemoveSound			(LPCSTR fname, EItemType type);
	void __fastcall 	RenameSound			(LPCSTR p0, LPCSTR p1, EItemType type);

	enum{
    	flUpdateProperties = (1<<0),
    };    
    static Flags32		m_Flags;
private:	// User declarations
	static TfrmSoundLib* form;

    DEFINE_VECTOR		(ESoundThumbnail*,THMVec,THMIt);
    THMVec				m_THM_Used;
    THMVec				m_THM_Current;
    TItemList*			m_ItemList;
    TProperties* 		m_ItemProps;

    ESoundThumbnail*	FindUsedTHM			(LPCSTR name);
    void				SaveUsedTHM			();
    void				DestroyUsedTHM		();

	void __fastcall 	RegisterModifiedTHM	();

    void 				OnModified			();
    void __fastcall 	UpdateLib			();

    bool 				bFormLocked;
    BOOL				bAutoPlay;

    void 				LockForm			(){ bFormLocked = true;		form->paProperties->Enabled = false; 	form->paItems->Enabled = false; }
    void 				UnlockForm			(){ bFormLocked = false;	form->paProperties->Enabled = true; 	form->paItems->Enabled = true; 	}

    static FS_QueryMap	modif_map;
	ref_sound			m_Snd;
    void				PlaySound			(LPCSTR name, u32& size, u32& time);
	void __fastcall 	OnControlClick		(PropValue* sender, bool& bModif, bool& bSafe);
	void __fastcall 	OnControl2Click		(PropValue* sender, bool& bModif, bool& bSafe);

    void				AppendModif			(LPCSTR nm);
public:		// User declarations
    __fastcall 			TfrmSoundLib		(TComponent* Owner);
// static function
    static void __fastcall EditLib			(AnsiString& title, bool bImport=false);
    static bool __fastcall HideLib			();
    static bool __fastcall Visible			(){return !!form;}
    static void 		OnFrame				();
    static void			UpdateProperties	(){m_Flags.set(flUpdateProperties,TRUE);}
};
//---------------------------------------------------------------------------
#endif
