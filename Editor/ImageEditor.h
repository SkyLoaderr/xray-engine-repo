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
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include "RXCtrls.hpp"
#include <Grids.hpp>
#include "RxMenus.hpp"
#include <Menus.hpp>
#include "Placemnt.hpp"
#include "multi_color.hpp"
//---------------------------------------------------------------------------
// refs
class ETextureCore;
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
	TGroupBox *gbOptions;
	TRadioGroup *rgSaveFormat;
	TCheckBox *cbDither;
	TCheckBox *cbAlphaZeroBorder;
	TCheckBox *cbBinaryAlpha;
	TCheckBox *cbNormalMap;
	TGroupBox *GroupBox2;
	TCheckBox *cbAllowFade;
	TComboBox *cbNumFadeMips;
	TLabel *Label1;
	TExtBtn *ebConvert;
	TBevel *Bevel1;
	TMultiObjColor *mcFadeColor;
	TRadioGroup *rgMIPmaps;
	TPanel *Panel3;
	TRxLabel *lbFileName;
	TRxLabel *RxLabel2;
	TRxLabel *RxLabel3;
	TRxLabel *lbInfo;
	TCheckBox *cbImplicitLighted;
    void __fastcall ebCloseClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall mcFadeColorMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall tvItemsItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
	void __fastcall pbImagePaint(TObject *Sender);
	void __fastcall rgChangeClick(TObject *Sender);
	void __fastcall pbImageDblClick(TObject *Sender);
	void __fastcall ebConvertClick(TObject *Sender);
	void __fastcall tvItemsKeyPress(TObject *Sender, char &Key);
private:	
// list functions
    void InitItemsList(const char* nm=0);
	TElTreeItem* FindItem(const char* s);
    TElTreeItem* FindFolder(const char* s);
    TElTreeItem* AddFolder(const char* s);
    TElTreeItem* AddItem(TElTreeItem* node, const char* name);
    TElTreeItem* AddItemToFolder(const char* folder, const char* name);
private:	// User declarations
	static TfrmImageLib* form;

    Fvector init_cam_hpb;
    Fvector init_cam_pos;
    void OnModified();
    static AStringVec check_tex_list;
    ETextureCore* sel_tex;
    bool bModified;
    bool bSetMode;
	void __fastcall SaveExportParams();
    bool bCheckMode;
public:		// User declarations
    __fastcall TfrmImageLib(TComponent* Owner);

    static void __fastcall OnRender();
    static void __fastcall OnIdle();
// static function
    static void __fastcall CheckImageLib();
    static void __fastcall EditImageLib(AnsiString& title, bool bCheck=false);
    static void __fastcall HideImageLib();
    static bool __fastcall Visible(){return !!form;}
};
//---------------------------------------------------------------------------
#endif
