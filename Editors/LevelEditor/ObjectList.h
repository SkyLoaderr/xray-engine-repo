//---------------------------------------------------------------------------

#ifndef ObjectListH
#define ObjectListH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CSPIN.h"
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include <ImgList.hpp>
#include "mxPlacemnt.hpp"

//---------------------------------------------------------------------------
class TfrmObjectList : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *sbClose;
    TExtBtn *sbRefreshList;
	TRadioGroup *rgSO;
	TExtBtn *ebShowSel;
	TExtBtn *ebHideSel;
	TElTree *tvItems;
	TFormStorage *fsStorage;
    void __fastcall sbCloseClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ebHideSelClick(TObject *Sender);
    void __fastcall ebShowSelClick(TObject *Sender);
    void __fastcall sbRefreshListClick(TObject *Sender);
    void __fastcall tmRefreshListTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall rgSOClick(TObject *Sender);
	void __fastcall tvItemsKeyPress(TObject *Sender, char &Key);
	void __fastcall tvItemsItemFocused(TObject *Sender);
private:	// User declarations
    int obj_count;
    int cur_cls;
    void __fastcall InitListBox();
    TElTreeItem* FindObjectByType(int type, void *obj);
    TElTreeItem* FindFolderByType(int type);
    TElTreeItem* AddFolder(EObjClass type);
    TElTreeItem* AddObject(TElTreeItem* node, LPSTR name, void* obj);
    void UpdateState();
    void UpdateSelection();
    bool bLockUpdate;
public:		// User declarations
    __fastcall 				TfrmObjectList		(TComponent* Owner);
	static TfrmObjectList* 	CreateForm			(TWinControl* parent=0);
	static void 			DestroyForm			(TfrmObjectList*& obj_list);
    void __fastcall 		ShowObjectListModal	();
    void __fastcall 		ShowObjectList		();
	void __fastcall 		HideObjectList		();
	void __fastcall 		UpdateObjectList	();
};
//---------------------------------------------------------------------------
extern void frmObjectListShow();
//---------------------------------------------------------------------------
#endif
