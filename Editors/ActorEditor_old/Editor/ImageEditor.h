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

#include "ItemList.h"
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
	TPanel *Panel3;
	TPanel *paProperties;
	TLabel *lbFileName;
	TLabel *RxLabel2;
	TLabel *RxLabel3;
	TLabel *lbInfo;
	TPanel *paItems;
	TSplitter *Splitter1;
	TBevel *Bevel2;
	TExtBtn *ebCheckAllCompliance;
	TImageList *ImageList;
	TExtBtn *ebCheckSelCompliance;
	TExtBtn *ebCancel;
	TBevel *Bevel3;
	TExtBtn *ebRemoveTexture;
	TExtBtn *ebRebuildAssociation;
	TBevel *Bevel5;
	TMxPanel *paImage;
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ebCheckAllComplianceClick(TObject *Sender);
	void __fastcall ebCheckSelComplianceClick(TObject *Sender);
	void __fastcall ebCancelClick(TObject *Sender);
	void __fastcall ebRebuildAssociationClick(TObject *Sender);
	void __fastcall ebRemoveTextureClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall paImageResize(TObject *Sender);
	void __fastcall paImagePaint(TObject *Sender);
private:
// list functions
    void 				InitItemsList		();
	void __fastcall 	OnItemsFocused		(ListItemsVec& items);
    void				DestroyTHMs			();
private:	// User declarations
	static TfrmImageLib* form;

    DEFINE_VECTOR		(ETextureThumbnail*,THMVec,THMIt);
    THMVec				m_THMs;
    TItemList*			m_ItemList;
    TProperties* 		m_ItemProps;

    void 				OnModified			();
    static FS_QueryMap	compl_map;
    static FS_QueryMap	texture_map;
    static FS_QueryMap	modif_map;
	void __fastcall 	SaveTextureParams	();
    bool 				bImportMode;
    void __fastcall 	UpdateImageLib		();
    void 				ExtractCompValue	(int val, int& A, int& M){	A = val/1000; M = val-A*1000; }
    static bool 		bFormLocked;
    static void 		LockForm			()	{ bFormLocked = true;	form->paProperties->Enabled = false; 	form->paItems->Enabled = false; }
    static void 		UnlockForm			(){ bFormLocked = false;	form->paProperties->Enabled = true; 	form->paItems->Enabled = true; 	}
public:		// User declarations
    __fastcall 			TfrmImageLib		(TComponent* Owner);
// static function
    static void __fastcall ImportTextures();
    static void __fastcall EditLib(AnsiString& title, bool bImport=false);
    static bool __fastcall HideLib();
    static bool __fastcall Visible(){return !!form;}
};
//---------------------------------------------------------------------------
#endif
