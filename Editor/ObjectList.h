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
#include "RXCtrls.hpp"
#include "RXSpin.hpp"
#include "SceneObject.h"
#include <Buttons.hpp>
#include <ImgList.hpp>
#include "ElTree.hpp"
#include "CloseBtn.hpp"

//---------------------------------------------------------------------------
class TfrmObjectList : public TForm
{
__published:	// IDE-managed Components
	TElTree *tvItems;
    TImageList *ImageList1;
    TTimer *tmRefreshSelection;
    TPanel *Panel1;
    TExtBtn *sbClose;
    TExtBtn *sbRefreshList;
	TRadioGroup *rgSO;
	TExtBtn *ebShowSel;
	TExtBtn *ebHideSel;
	TTimer *tmRefreshList;
    void __fastcall sbCloseClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall tvItemsTryEdit(TObject *Sender, TElTreeItem *Item,
          TElHeaderSection *Section, TFieldTypes &CellType, bool &CanEdit);
    void __fastcall tvItemsItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
    void __fastcall ebHideSelClick(TObject *Sender);
    void __fastcall ebShowSelClick(TObject *Sender);
    void __fastcall sbRefreshListClick(TObject *Sender);
    void __fastcall tmRefreshListTimer(TObject *Sender);
    void __fastcall tmRefreshSelectionTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall rgSOClick(TObject *Sender);
	void __fastcall tvItemsKeyPress(TObject *Sender, char &Key);
private:	// User declarations
    int obj_count;
    EObjClass cur_cls;
    void __fastcall InitListBox();
    TElTreeItem* FindObjectByType(EObjClass type, void *obj);
    TElTreeItem* FindFolderByType(EObjClass type);
    TElTreeItem* AddFolder(EObjClass type);
    TElTreeItem* AddObject(TElTreeItem* node, LPSTR name, void* obj);
    void UpdateState();
    void UpdateSelection();
public:		// User declarations
    __fastcall TfrmObjectList(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern void frmObjectListShow();
//---------------------------------------------------------------------------
#endif
