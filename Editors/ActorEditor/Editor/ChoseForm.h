//---------------------------------------------------------------------------

#ifndef ChoseFormH
#define ChoseFormH
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
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"
#include "ElXPThemedControl.hpp"
//---------------------------------------------------------------------------
// refs
class EImageThumbnail;

class TfrmChoseItem : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
	TElTree *tvItems;
	TFormStorage *fsStorage;
	TPanel *paMulti;
	TElTree *tvMulti;
	TExtBtn *ebMultiUp;
	TExtBtn *ebMultiDown;
	TExtBtn *ebMultiRemove;
	TExtBtn *ebMultiClear;
	TPanel *Panel2;
	TExtBtn *sbSelect;
	TExtBtn *sbCancel;
	TPanel *Panel3;
	TLabel *lbItemName;
	TLabel *lbFileName;
	TLabel *mxLabel1;
	TLabel *mxLabel2;
	TLabel *mxLabel3;
	TLabel *lbInfo;
	TPanel *paImage;
	TPaintBox *pbImage;
    void __fastcall sbSelectClick(TObject *Sender);
    void __fastcall sbCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall pbImagePaint(TObject *Sender);
	void __fastcall tvItemsDblClick(TObject *Sender);
	void __fastcall tvItemsKeyPress(TObject *Sender, char &Key);
	void __fastcall tvItemsItemChange(TObject *Sender, TElTreeItem *Item,
          TItemChangeMode ItemChangeMode);
	void __fastcall ebMultiUpClick(TObject *Sender);
	void __fastcall ebMultiDownClick(TObject *Sender);
	void __fastcall tvMultiDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall tvMultiDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall tvMultiStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall ebMultiRemoveClick(TObject *Sender);
	void __fastcall ebMultiClearClick(TObject *Sender);
	void __fastcall tvItemsItemFocused(TObject *Sender);
private:	// User declarations
	static TfrmChoseItem* form;
    static AnsiString select_item;
    const char* StartFolder;

    EImageThumbnail* m_Thm;

    void 	InitItemsList(const char* nm=0);

    bool 	bMultiSel;
    int 	iMultiSelLimit;
    bool 	bIgnoreExt;
// fill routines    
    void __fastcall FillSound		();
    void __fastcall FillObject		();
    void __fastcall FillLAnim		();
    void __fastcall FillShader		();
    void __fastcall FillShaderXRLC	();
    void __fastcall FillPS			();
    void __fastcall FillTexture		();
    void __fastcall FillEntity		();
    void __fastcall FillGameObject	();
    void __fastcall FillGameMaterial();
public:		// User declarations
    enum ESelectMode{
    	smSound = 0,
        smObject,
    	smShader,
        smShaderXRLC,
        smPS,
        smTexture,
        smEntity,
        smLAnim,
        smGameObject,
        smGameMaterial,
        smMaxMode
    };
protected:
    static ESelectMode Mode;
    static AnsiString m_LastSelection[smMaxMode];
public:		// User declarations
    __fastcall 					TfrmChoseItem	(TComponent* Owner);
    static int	 	__fastcall 	SelectItem		(ESelectMode mode, LPCSTR& dest, int sel_cnt=1, LPCSTR init_name=0, bool bIgnoreExt=false);
    static bool 	__fastcall 	Visible			(){return !!form;}
};
//---------------------------------------------------------------------------
#endif
