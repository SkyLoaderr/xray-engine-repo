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

#include "SceneObject.h"
#include "Library.h"
#include <Dialogs.hpp>
#include "ElTree.hpp"
#include "CloseBtn.hpp"
#include "RXCtrls.hpp"
#include "Placemnt.hpp"
//---------------------------------------------------------------------------
// refs
class ETextureThumbnail;
class ETextureCore;

class TfrmChoseItem : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *sbSelect;
    TExtBtn *sbCancel;
	TElTree *tvItems;
	TPanel *paImage;
	TRxLabel *lbItemName;
	TPaintBox *pbImage;
	TRxLabel *lbFileName;
	TRxLabel *RxLabel1;
	TRxLabel *RxLabel2;
	TRxLabel *RxLabel3;
	TRxLabel *lbInfo;
	TFormStorage *fsStorage;
	TPanel *paMulti;
	TElTree *tvMulti;
	TExtBtn *ebMultiUp;
	TExtBtn *ebMultiDown;
	TExtBtn *ebMultiRemove;
	TExtBtn *ebMultiClear;
    void __fastcall sbSelectClick(TObject *Sender);
    void __fastcall sbCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall tvItemsItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
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
private:	// User declarations
	static TfrmChoseItem* form;
    static AnsiString select_item;
    static AnsiString last_item;
    const char* StartFolder;

    ETextureCore* sel_tex;
    ETextureThumbnail* sel_thm;

    enum ESelectMode{
        smObject,
    	smShader,
        smPS,
        smTexture
    };
    ESelectMode Mode;

    void InitItemsList(const char* nm=0);
	TElTreeItem* FindItem(const char* s);
    TElTreeItem* FindFolder(const char* s);
    TElTreeItem* AddFolder(const char* s);
    TElTreeItem* AddItem(TElTreeItem* node, const char* name, void* obj=(void*)1);
    TElTreeItem* AddItemToFolder(const char* folder, const char* name, void* obj=(void*)1);

    bool bMultiSel;
    int iMultiSelLimit;
public:		// User declarations
    __fastcall TfrmChoseItem(TComponent* Owner);
// static function
    static LPCSTR __fastcall SelectObject	(bool bMulti=false, bool bExcludeSystem=true, LPCSTR start_folder=0, LPCSTR init_name=0);
    static LPCSTR __fastcall SelectShader	(bool bExcludeSystem=true, LPCSTR start_folder=0, LPCSTR init_name=0);
    static LPCSTR __fastcall SelectPS		(LPCSTR start_folder=0, LPCSTR init_name=0);
    static LPCSTR __fastcall SelectTexture	(bool bMulti=false, LPCSTR tex=0);
    static bool __fastcall Visible(){return !!form;}
};
//---------------------------------------------------------------------------
#endif
