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

//---------------------------------------------------------------------------
// refs
class EImageThumbnail;
class TProperties;
//---------------------------------------------------------------------------

class TfrmImageLib : public TForm
{
__published:	// IDE-managed Components
	TPanel *paRight;
	TFormStorage *fsStorage;
	TPanel *paCommand;
	TExtBtn *ebOk;
	TPanel *paImage;
	TPaintBox *pbImage;
	TBevel *Bevel1;
	TPanel *Panel3;
	TPanel *paProperties;
	TLabel *lbFileName;
	TLabel *RxLabel2;
	TLabel *RxLabel3;
	TLabel *lbInfo;
	TPanel *paLeft;
	TPanel *paTextureCount;
	TElTree *tvItems;
	TSplitter *Splitter1;
	TBevel *Bevel2;
	TExtBtn *ebCheckAllCompliance;
	TImageList *ImageList1;
	TExtBtn *ebCheckSelCompliance;
	TExtBtn *ebCancel;
	TBevel *Bevel3;
	TBevel *Bevel4;
	TExtBtn *ebRemoveTexture;
	TExtBtn *ebRebuildAssociation;
	TBevel *Bevel5;
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall pbImagePaint(TObject *Sender);
	void __fastcall tvItemsKeyPress(TObject *Sender, char &Key);
	void __fastcall tvItemsItemFocused(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall tvItemsKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall ebCheckAllComplianceClick(TObject *Sender);
	void __fastcall ebCheckSelComplianceClick(TObject *Sender);
	void __fastcall ebCancelClick(TObject *Sender);
	void __fastcall ebRebuildAssociationClick(TObject *Sender);
	void __fastcall ebRemoveTextureClick(TObject *Sender);
	void __fastcall ExtBtn1Click(TObject *Sender);
private:
// list functions
    void InitItemsList(const char* nm=0);
private:	// User declarations
	static TfrmImageLib* form;

	EImageThumbnail* 	m_Thm;
    AnsiString          m_SelectedName;

    Fvector init_cam_hpb;
    Fvector init_cam_pos;
    void OnModified();
    static FS_QueryMap	compl_map;
    static FS_QueryMap	texture_map;
    static FS_QueryMap	modif_map;
	void __fastcall SaveTextureParams();
    bool bImportMode;
    TProperties* ImageProps;
    static AnsiString m_LastSelection;
    void __fastcall UpdateImageLib();
    void ExtractCompValue(int val, int& A, int& M){	A = val/1000; M = val-A*1000; }
    static bool bFormLocked;
    static void LockForm()	{ bFormLocked = true;	form->paProperties->Enabled = false; 	form->paLeft->Enabled = false; }
    static void UnlockForm(){ bFormLocked = false;	form->paProperties->Enabled = true; 	form->paLeft->Enabled = true; 	}
public:		// User declarations
    __fastcall TfrmImageLib(TComponent* Owner);
// static function
    static void __fastcall ImportTextures();
    static void __fastcall EditImageLib(AnsiString& title, bool bImport=false);
    static bool __fastcall HideImageLib();
    static bool __fastcall Visible(){return !!form;}
};
//---------------------------------------------------------------------------
#endif
