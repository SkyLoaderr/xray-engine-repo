//---------------------------------------------------------------------------

#ifndef ImageLibH
#define ImageLibH
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

#include "MXCtrls.hpp"

//---------------------------------------------------------------------------
// refs
class ETextureThumbnail;
class TProperties;
//---------------------------------------------------------------------------

class TfrmImageLib : public TForm
{
__published:	// IDE-managed Components
	TPanel *paRight;
	TFormStorage *fsStorage;
	TPanel *paCommand;
	TExtBtn *ebOk;
	TBevel *Bevel1;
	TPanel *paProperties;
	TPanel *paItems;
	TSplitter *Splitter1;
	TBevel *Bevel2;
	TImageList *ImageList;
	TExtBtn *ebCancel;
	TExtBtn *ebRemoveTexture;
	TExtBtn *ebRebuildAssociation;
	TBevel *Bevel5;
	TPanel *Panel1;
	TMxPanel *paImage;
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ebCancelClick(TObject *Sender);
	void __fastcall ebRebuildAssociationClick(TObject *Sender);
	void __fastcall ebRemoveTextureClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall paImagePaint(TObject *Sender);
private:
// list functions
    void __stdcall 		RemoveTexture		(LPCSTR fname, EItemType type, bool& res);

    void 				InitItemsList		();
	void __stdcall  	OnItemsFocused		(ListItemsVec& items);

	enum{
    	flUpdateProperties = (1<<0),
    };    
    static Flags32		m_Flags;
private:	// User declarations
	static TfrmImageLib* form;

    DEFINE_VECTOR		(ETextureThumbnail*,THMVec,THMIt);
    THMVec				m_THM_Used;
    THMVec				m_THM_Current;
    TItemList*			m_ItemList;
    TProperties* 		m_ItemProps;

    ETextureThumbnail*	FindUsedTHM			(LPCSTR name);
    void				SaveUsedTHM			();
    void				DestroyUsedTHM		();

	void __fastcall 	RegisterModifiedTHM	();
    
    void 				OnModified			();
    static FS_QueryMap	texture_map;
    static FS_QueryMap	modif_map;
    bool 				bImportMode;
    void __fastcall 	UpdateLib			();
    static bool 		bFormLocked;
    static void 		LockForm			(){ bFormLocked = true;	form->paProperties->Enabled = false; 	form->paItems->Enabled = false; }
    static void 		UnlockForm			(){ bFormLocked = false;form->paProperties->Enabled = true; 	form->paItems->Enabled = true; 	}
public:		// User declarations
    __fastcall 			TfrmImageLib		(TComponent* Owner);
// static function
    static void __fastcall ImportTextures	();
    static void __fastcall EditLib			(AnsiString& title, bool bImport=false);
    static bool __fastcall HideLib			();
    static bool __fastcall Visible			(){return !!form;}
    static void 		OnFrame				();
    static void			UpdateProperties	(){m_Flags.set(flUpdateProperties,TRUE);}
};
//---------------------------------------------------------------------------
#endif
