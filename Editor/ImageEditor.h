//---------------------------------------------------------------------------

#ifndef ImageLibH
#define ImageLibH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>

#include <Dialogs.hpp>
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "multi_color.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"
//---------------------------------------------------------------------------
// refs
class EImageThumbnail;
class TfrmProperties;
//---------------------------------------------------------------------------

class TfrmImageLib : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
	TElTree *tvItems;
	TFormStorage *fsStorage;
	TPanel *Panel2;
	TExtBtn *ebClose;
	TPanel *paImage;
	TPaintBox *pbImage;
	TExtBtn *ebConvert;
	TBevel *Bevel1;
	TPanel *Panel3;
	TPanel *paProperties;
	TMxLabel *lbFileName;
	TMxLabel *RxLabel2;
	TMxLabel *RxLabel3;
	TMxLabel *lbInfo;
    void __fastcall ebCloseClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall pbImagePaint(TObject *Sender);
	void __fastcall ebConvertClick(TObject *Sender);
	void __fastcall tvItemsKeyPress(TObject *Sender, char &Key);
	void __fastcall tvItemsItemFocused(TObject *Sender);
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
    static FileMap texture_map;
    static FileMap modif_map;
	void __fastcall SaveTextureParams();
    bool bCheckMode;
    TfrmProperties* ImageProps;
    static AnsiString m_LastSelection;
public:		// User declarations
    __fastcall TfrmImageLib(TComponent* Owner);
// static function
    static void __fastcall CheckImageLib();
    static void __fastcall EditImageLib(AnsiString& title, bool bCheck=false);
    static bool __fastcall HideImageLib();
    static bool __fastcall Visible(){return !!form;}
};
//---------------------------------------------------------------------------
#endif
