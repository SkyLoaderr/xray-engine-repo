//---------------------------------------------------------------------------
#ifndef ItemListH
#define ItemListH
//---------------------------------------------------------------------------

#include "ElTree.hpp"
#include "ElTreeStdEditors.hpp"
#include "ElXPThemedControl.hpp"
#include "multi_edit.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include "ElTreeAdvEdit.hpp"
#include <Mask.hpp>

#include "ItemListHelper.h"


class TItemList: public TForm
{
__published:	// IDE-managed Components
	TElTree *tvItems;
	TFormStorage 		*fsStorage;
	TBevel 				*Bevel1;
	TMxPopupMenu *pmSystem;
	TMenuItem *ExpandAll1;
	TMenuItem *N1;
	TMenuItem *CollapseAll1;
	TMenuItem *N2;
	TMenuItem *miDrawThumbnails;
	TMxPopupMenu *pmItems;
	void __fastcall 	FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall 	tvItemsClick(TObject *Sender);
	void __fastcall 	tvItemsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall 	tvItemsItemFocused(TObject *Sender);
	void __fastcall 	FormShow(TObject *Sender);
	void __fastcall 	tvItemsItemChange(TObject *Sender, TElTreeItem *Item, TItemChangeMode ItemChangeMode);
	void __fastcall 	CollapseAll1Click(TObject *Sender);
	void __fastcall 	ExpandAll1Click(TObject *Sender);
	void __fastcall 	miDrawThumbnailsClick(TObject *Sender);
	void __fastcall 	tvItemsMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall 	tvItemsMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall tvItemsAfterSelectionChange(TObject *Sender);
	void __fastcall tvItemsItemDraw(TObject *Sender, TElTreeItem *Item,
          TCanvas *Surface, TRect &R, int SectionIndex);
public:
    typedef void 	__fastcall (__closure *TOnItemsFocused)		(ListItemsVec& items);
    typedef void 	__fastcall (__closure *TOnCloseEvent)		(void);
private:	// User declarations
    AStringVec 			last_selected_items;

    ListItemsVec 		m_Items;
    TOnItemsFocused		OnItemsFocused;
    TOnCloseEvent		OnCloseEvent;
    void 				ClearParams				(TElTreeItem* node=0);

	void 				OutBOOL					(BOOL val, TCanvas* Surface, const TRect& R);
	void 				OutText					(LPCSTR text, TCanvas* Surface, TRect R, TGraphic* g=0, bool bArrow=false);
public:		// User declarations
	__fastcall 			TItemList	       		(TComponent* Owner);
	static TItemList* 	CreateForm				(TWinControl* parent=0, TAlign align=alNone, TOnItemsFocused focused=0, TOnCloseEvent close=0, bool allow_multiselect=true);
	static TItemList* 	CreateModalForm			(const AnsiString& title, ListItemsVec& items, bool bFullExpand=true, TOnItemsFocused focused=0, TOnCloseEvent close=0, bool allow_multiselect=true);
	static void 		DestroyForm				(TItemList*& props);
    void __fastcall 	ShowListModal			();
    void __fastcall 	ShowList				();
    void __fastcall 	HideList				();
    void __fastcall 	ClearList				();
    void __fastcall 	RefreshForm				();

    void __fastcall		SelectItem				(const AnsiString& full_name, bool bVal, bool bLeaveSel, bool bExpand);
    void __fastcall 	AssignItems				(ListItemsVec& values, bool full_expand, const AnsiString& title="Item List", bool full_sort=false);
    bool __fastcall 	IsFocused				(){return tvItems->Focused();}

    int __fastcall		GetSelected				(ListItemsVec& items);
    TElTreeItem*		GetSelected				(){return (tvItems->MultiSelect)?0:tvItems->Selected;}

    void 				LockUpdating			(){ tvItems->IsUpdating = true; }
    void 				UnlockUpdating			(){ tvItems->IsUpdating = false; }

    void				SetImages				(TImageList* image_list){tvItems->Images=image_list;}
};
//---------------------------------------------------------------------------
#endif
