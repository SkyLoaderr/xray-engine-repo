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
#include "multiobj.hpp"
#include "SceneObject.h"
#include <Buttons.hpp>
#include <ImgList.hpp>
#include "ElTree.hpp"
#include "CloseBtn.hpp"

//---------------------------------------------------------------------------
class TfrmObjectList : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *sbHideAll;
    TExtBtn *sbShowAll;
    TExtBtn *sbHideSel;
    TExtBtn *sbClose;
    TExtBtn *sbProperties;
    TElTree *tvObjects;
    TExtBtn *sbShowSel;
    TExtBtn *sbRefreshList;
    TImageList *ImageList1;
    TTimer *tmRefreshSelection;
    TExtBtn *sbHideGroup;
    TExtBtn *sbShowGroup;
    TCheckBox *cbShowHidden;
    void __fastcall sbCloseClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall tvObjectsTryEdit(TObject *Sender, TElTreeItem *Item,
          TElHeaderSection *Section, TFieldTypes &CellType, bool &CanEdit);
    void __fastcall tvObjectsItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
    void __fastcall sbHideSelClick(TObject *Sender);
    void __fastcall sbShowSelClick(TObject *Sender);
    void __fastcall sbHideAllClick(TObject *Sender);
    void __fastcall sbShowAllClick(TObject *Sender);
    void __fastcall sbPropertiesClick(TObject *Sender);
    void __fastcall sbRefreshListClick(TObject *Sender);
    void __fastcall tmRefreshListTimer(TObject *Sender);
    void __fastcall tmRefreshSelectionTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall sbHideGroupClick(TObject *Sender);
    void __fastcall sbShowGroupClick(TObject *Sender);
    void __fastcall cbShowHiddenClick(TObject *Sender);
    void __fastcall tvObjectsDblClick(TObject *Sender);
private:	// User declarations
    int obj_count;
    void __fastcall InitListBox();
    void __fastcall UpdateListBox();
    TElTreeItem* FindObjectByType(DWORD type, void *obj);
    TElTreeItem* FindFolderByType(DWORD type);
    TElTreeItem* AddFolder(DWORD type);
    TElTreeItem* AddObject(TElTreeItem* node, LPSTR name, void* obj);
    void UpdateState();
    void UpdateSelection();
public:		// User declarations
    __fastcall TfrmObjectList(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmObjectList *frmObjectList;
//---------------------------------------------------------------------------
#endif
