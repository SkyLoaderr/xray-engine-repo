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
#include "FolderLib.h"


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
	TElTreeInplaceAdvancedEdit *InplaceEdit;
	TMxPopupMenu *pmEdit;
	TMenuItem *MenuItem1;
	TMenuItem *MenuItem2;
	TMenuItem *MenuItem3;
	TMenuItem *MenuItem4;
	TMenuItem *MenuItem5;
	TMenuItem *N3;
	TMenuItem *RefreshForm1;
	void __fastcall 	FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall 	tvItemsClick(TObject *Sender);
	void __fastcall 	tvItemsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
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
	void __fastcall InplaceEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall miCreateFolderClick(TObject *Sender);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall tvItemsDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall Delete1Click(TObject *Sender);
	void __fastcall InplaceEditAfterOperation(TObject *Sender, bool &Accepted,
          bool &DefaultConversion);
	void __fastcall tvItemsKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall tvItemsResize(TObject *Sender);
	void __fastcall RefreshForm1Click(TObject *Sender);
public:
	DEFINE_VECTOR(TElTreeItem*,ElItemsVec,ElItemsIt);
    typedef void 	__fastcall (__closure *TOnItemsFocused)		(ListItemsVec& items);
    typedef void 	__fastcall (__closure *TOnCloseEvent)		(void);
private:	// User declarations
    AStringVec 			last_selected_items;

	void __fastcall 	RenameItem				(LPCSTR p0, LPCSTR p1);

    ListItemsVec 		m_Items;
    void 				ClearParams				(TElTreeItem* node=0);

	void 				OutBOOL					(BOOL val, TCanvas* Surface, const TRect& R);
	void 				OutText					(LPCSTR text, TCanvas* Surface, TRect R, TGraphic* g=0, bool bArrow=false);
public:
	enum{
    	// set
    	ilEditMenu		= (1<<0),              
        ilMultiSelect	= (1<<1),
        ilDragAllowed	= (1<<2),

        // internal
        ilRT_FullExpand	= (1<<30),
        ilRT_UpdateLocked=(1<<31),
    };
    Flags32				m_Flags;
	// events
    TOnItemsFocused		OnItemsFocused;
    TOnCloseEvent		OnCloseEvent;
    CFolderHelper::TOnItemRename	OnItemRename;
    CFolderHelper::TOnItemRemove	OnItemRemove;
protected:
	DEFINE_VECTOR(TItemList*,ILVec,ILIt);
	static  ILVec		ILForms;

	void 				OnFormFrame				();
public:		// User declarations
	__fastcall 			TItemList	       		(TComponent* Owner);
	static TItemList* 	CreateForm				(TWinControl* parent=0, TAlign align=alNone, u32 flags=ilMultiSelect);
	static TItemList* 	CreateModalForm			(const AnsiString& title, ListItemsVec& items, u32 flags=ilMultiSelect);
	static void 		DestroyForm				(TItemList*& props);
	static void 		OnFrame					();

    void __fastcall 	ShowListModal			();
    void __fastcall 	ShowList				();
    void __fastcall 	HideList				();
    void __fastcall 	ClearList				();
    void __fastcall 	RefreshForm				();

    void __fastcall		SelectItem				(const AnsiString& full_name, bool bVal, bool bLeaveSel, bool bExpand);
    void __fastcall 	AssignItems				(ListItemsVec& values, bool full_expand, const AnsiString& title="Item List", bool full_sort=false);
    bool __fastcall 	IsFocused				(){return tvItems->Focused();}

    int __fastcall		GetSelected				(ElItemsVec& items);
    int __fastcall		GetSelected				(LPCSTR pref, ListItemsVec& items, bool bOnlyObject);
    TElTreeItem*		GetSelected				(){return (tvItems->MultiSelect)?0:tvItems->Selected;}

    void 				LockUpdating			(){ tvItems->IsUpdating = true; m_Flags.set(ilRT_UpdateLocked,TRUE); }
    void 				UnlockUpdating			(){ tvItems->IsUpdating = false;m_Flags.set(ilRT_UpdateLocked,FALSE); }
    bool				IsLocked				(){ return m_Flags.is(ilRT_UpdateLocked); }

    void				SetImages				(TImageList* image_list){tvItems->Images=image_list;}

    void				LoadSelection			(TFormStorage* storage);
    void				SaveSelection			(TFormStorage* storage);

    void __fastcall 	SaveParams				(TFormStorage* fs)
    {
//		fs->WriteInteger(AnsiString().sprintf("%s_column0_width",Name.c_str()),tvItems->HeaderSections->Item[0]->Width);
		fs->WriteInteger(AnsiString().sprintf("%s_draw_thm",Name.c_str()),miDrawThumbnails->Checked);
    }
    void __fastcall 	LoadParams				(TFormStorage* fs)
    {                                      	
//		tvItems->HeaderSections->Item[0]->Width = fs->ReadInteger(AnsiString().sprintf("%s_column0_width",Name.c_str()),tvItems->HeaderSections->Item[0]->Width);
        miDrawThumbnails->Checked				= fs->ReadInteger(AnsiString().sprintf("%s_draw_thm",Name.c_str()),false);
        RefreshForm			();
    }
};
//---------------------------------------------------------------------------
#endif
