#include "stdafx.h"
#pragma hdrstop

#include "ItemList.h"
#include <ElVCLUtils.hpp>
#include <ElTools.hpp>

#include "main.h"
#include "ShaderFunction.h"
#include "ColorPicker.h"
#include "ChoseForm.h"
#include "FolderLib.h"
#include "NumericVector.h"
#include "TextForm.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "multi_edit"
#pragma link "ElTreeStdEditors"
#pragma link "ElXPThemedControl"
#pragma link "multi_edit"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma link "ElTree"
#pragma link "ElTreeStdEditors"
#pragma link "ElXPThemedControl"
#pragma link "multi_edit"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma link "ElTreeAdvEdit"
#pragma link "ElBtnCtl"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
void TItemList::ClearParams(TElTreeItem* node)
{
	if (node){
    	THROW2("ClearParams - node");
    }else{
        // save last selected items
        ElItemsVec items;
        if (GetSelected(items)){
            last_selected_items.clear();
            for (ElItemsIt l_it=items.begin(); l_it!=items.end(); l_it++){
                AnsiString s;
                FHelper.MakeFullName(*l_it,0,s);
                last_selected_items.push_back(s);
            }
        }
        // real clear
	    for (ListItemsIt it=m_Items.begin(); it!=m_Items.end(); it++)
    		xr_delete	(*it);
		m_Items.clear();
        // fill list
        LockUpdating			();
	    tvItems->Items->Clear	();
        UnlockUpdating			();
    }
}
//---------------------------------------------------------------------------
void __fastcall TItemList::ClearList()
{
    ClearParams			();
}
//---------------------------------------------------------------------------
void __fastcall TItemList::SelectItem(const AnsiString& full_name, bool bVal, bool bLeaveSel, bool bExpand)
{
	TElTreeItem* item;
    FHelper.FindItem			(tvItems,full_name,&item);
    if (item){
    	if (!bLeaveSel)			tvItems->DeselectAll();
        if (bExpand) 			FHelper.ExpandItem(tvItems,item);
        if (tvItems->MultiSelect) 	item->Selected 		= bVal;
        else 						tvItems->Selected   = item;
		tvItems->EnsureVisible	(item); 
		if (tvItems->OnAfterSelectionChange) tvItems->OnAfterSelectionChange(0);
    }
}
//---------------------------------------------------------------------------

__fastcall TItemList::TItemList(TComponent* Owner) : TForm(Owner)
{
    DEFINE_INI		(fsStorage);
    m_Flags.zero	();
    OnItemsFocused	= 0;
    OnCloseEvent	= 0;
    OnItemRename	= 0;
}
//---------------------------------------------------------------------------

TItemList* TItemList::CreateForm(TWinControl* parent, TAlign align, u32 flags)
{
	TItemList* props 			= xr_new<TItemList>(parent);
    props->m_Flags.set			(flags);
    props->tvItems->MultiSelect	= props->m_Flags.is(ilMultiSelect);
    if (parent){
		props->Parent 			= parent;
    	props->Align 			= align;
	    props->BorderStyle 		= bsNone;
        props->ShowList	();
        props->fsStorage->Active= false;
    }
    if (props->m_Flags.is(ilDragAllowed)){
	    props->tvItems->OnStartDrag = FHelper.StartDrag;
	    props->tvItems->OnDragOver 	= FHelper.DragOver;
        props->tvItems->DragAllowed	= true;
    }else{
	    props->tvItems->OnStartDrag = 0;
	    props->tvItems->OnDragOver 	= 0;
        props->tvItems->DragAllowed	= false;
    }
	return props;
}

TItemList* TItemList::CreateModalForm(const AnsiString& title, ListItemsVec& items, u32 flags)
{
	TItemList* props 			= xr_new<TItemList>((TComponent*)0);
    props->m_Flags.set			(flags);
    props->tvItems->MultiSelect	= props->m_Flags.is(ilMultiSelect);
    props->AssignItems			(items,props->m_Flags.is(ilFullExpand),title);
	props->ShowListModal		();
    if (props->m_Flags.is(ilDragAllowed)){
	    props->tvItems->OnStartDrag = FHelper.StartDrag;
	    props->tvItems->OnDragOver 	= FHelper.DragOver;
        props->tvItems->DragAllowed	= true;
    }else{
	    props->tvItems->OnStartDrag = 0;
	    props->tvItems->OnDragOver 	= 0;
        props->tvItems->DragAllowed	= false;
    }
	return props;
}

void TItemList::DestroyForm(TItemList*& props)
{
	VERIFY(props);
    // destroy forms
	props->ClearList			();
	props->Close				();
    xr_delete					(props);
}
void __fastcall TItemList::ShowList()
{
	Show();
}

void __fastcall TItemList::ShowListModal()
{
	ShowModal();
}

void __fastcall TItemList::HideList()
{
	Hide();
}

void __fastcall TItemList::FormClose(TObject *Sender,
      TCloseAction &Action)
{

    if (OnCloseEvent) 	OnCloseEvent();
	ClearParams			();
}
//---------------------------------------------------------------------------

void __fastcall TItemList::AssignItems(ListItemsVec& items, bool full_expand, const AnsiString& title, bool full_sort)
{
	// begin fill mode
	LockUpdating			();
    // clear values
//    if (tvItems->Selected) FHelper.MakeFullName(tvItems->Selected,0,last_selected_item);
    ClearParams				();
    // fill values
    m_Items					= items;
	for (ListItemsIt it=m_Items.begin(); it!=m_Items.end(); it++){
    	ListItem* prop		= *it;
        prop->item			= FHelper.AppendObject(tvItems,prop->key);
        prop->item->ImageIndex= prop->icon_index;
        prop->item->Tag	    = (int)prop;
        prop->item->UseStyles=true;
        prop->item->CheckBoxEnabled = prop->m_Flags.is(ListItem::flShowCB);
        prop->item->ShowCheckBox 	= prop->m_Flags.is(ListItem::flShowCB);
        prop->item->CheckBoxState 	= (TCheckBoxState)prop->m_Flags.is(ListItem::flCBChecked);
        // set flags                                        
        if (prop->m_Flags.is(ListItem::flDrawThumbnail)){
        	prop->item->Height 		= 64;
        	prop->item->OwnerHeight = !miDrawThumbnails->Checked;
        }
    }

    // end fill mode
	if (full_expand) tvItems->FullExpand();

    // sorting
    if (full_sort){
    	tvItems->Sort		(true);
        tvItems->SortMode 	= smAdd;
    }else{
        for (ListItemsIt it=m_Items.begin(); it!=m_Items.end(); it++){
            ListItem* prop		= *it;
            if (prop->m_Flags.is(ListItem::flSorted)) prop->item->Sort(true);
        }
    }

    // expand sel items
    ElItemsVec el_list;
    for (AStringIt s_it=last_selected_items.begin(); s_it!=last_selected_items.end(); s_it++)
    	el_list.push_back(FHelper.ExpandItem(tvItems,*s_it));

	UnlockUpdating			();

    // restore selection
    tvItems->DeselectAll	();
	for (ElItemsIt el_it=el_list.begin(); el_it!=el_list.end(); el_it++)
	    FHelper.RestoreSelection(tvItems,*el_it,true);

    Caption = title;
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsClick(TObject *Sender)
{
	TElTreeItem* item = dynamic_cast<TElTreeItem*>(Sender);
  	if (item){
        ListItem* prop = (ListItem*)item->Tag;
        if (prop&&prop->OnClickEvent) prop->OnClickEvent(prop);
    };
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	TSTItemPart 	IP=(TSTItemPart)0;
    int				HC=0;
	TElTreeItem* item = tvItems->GetItemAt(X,Y,IP,HC);
  	if (item){
        if (Button==mbRight){
            TPoint P; P.x = X; P.y = Y;
            P=tvItems->ClientToScreen(P);
            pmItems->Popup(P.x,P.y-10);
        }
    };
    if (m_Flags.is(ilEditMenu)){
        if (Button==mbRight){
            TPoint P; P.x = X; P.y = Y;
            P=tvItems->ClientToScreen(P);
            pmEdit->Popup(P.x,P.y-10);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
//
}
//---------------------------------------------------------------------------

int __fastcall TItemList::GetSelected(ElItemsVec& items)
{
    for (TElTreeItem* item = tvItems->GetNextSelected(0); item; item = tvItems->GetNextSelected(item))
        items.push_back	(item);
    return items.size();
}

int __fastcall TItemList::GetSelected(LPCSTR pref, ListItemsVec& items, bool bOnlyObject)
{
    for (TElTreeItem* item = tvItems->GetNextSelected(0); item; item = tvItems->GetNextSelected(item)){
        ListItem* prop 		= (ListItem*)item->Tag;
        if (prop&&(!bOnlyObject||(bOnlyObject&&prop->m_Object))){
        	if (pref){
            	if (1==prop->key.Pos(pref))
                	items.push_back	(prop);
            }else
				items.push_back	(prop);
        }
    }
    return items.size();
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsAfterSelectionChange(TObject *Sender)
{
	if (IsLocked()) return;
    ListItemsVec sel_items;
    GetSelected	(0,sel_items,false);
    if (OnItemsFocused) 	OnItemsFocused(sel_items);
    for (ListItemsIt it=sel_items.begin(); it!=sel_items.end(); it++)
        if ((*it)->OnItemFocused)(*it)->OnItemFocused(*it);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::FormShow(TObject *Sender)
{
    InplaceEdit->Editor->Color		= TColor(0x00A0A0A0);
    InplaceEdit->Editor->BorderStyle= bsNone;
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsItemChange(TObject *Sender,
      TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
	if (Item&&(icmCheckState==ItemChangeMode)){
		ListItem* prop 			= (ListItem*)Item->Tag;
	    if (prop){
        	prop->m_Flags.set	(ListItem::flCBChecked,Item->Checked);
//			prop->OnChange		();
//			Modified			();
    	}
	    tvItems->Refresh	();
    }
}
//---------------------------------------------------------------------------

void __fastcall TItemList::ExpandAll1Click(TObject *Sender)
{
	tvItems->FullExpand();
}
//---------------------------------------------------------------------------

void __fastcall TItemList::CollapseAll1Click(TObject *Sender)
{
	tvItems->FullCollapse();
}
//---------------------------------------------------------------------------

void __fastcall TItemList::miDrawThumbnailsClick(TObject *Sender)
{
	RefreshForm();
}
//---------------------------------------------------------------------------

void __fastcall TItemList::RefreshForm()
{
    LockUpdating			();
    for (ListItemsIt it=m_Items.begin(); it!=m_Items.end(); it++){
    	ListItem* prop = *it;
    	if (prop->m_Flags.is(ListItem::flDrawThumbnail)) 
        	prop->item->OwnerHeight = !miDrawThumbnails->Checked;
    }
	UnlockUpdating			();
	tvItems->Repaint		();
}

void __fastcall TItemList::tvItemsItemDraw(TObject *Sender,
      TElTreeItem *Item, TCanvas *Surface, TRect &R, int SectionIndex)
{
//	
}
//---------------------------------------------------------------------------

void __fastcall TItemList::InplaceEditValidateResult(
      TObject *Sender, bool &InputValid)
{
	R_ASSERT(m_Flags.is(ilEditMenu));
	TElTreeInplaceAdvancedEdit* IE	= InplaceEdit;
    AnsiString new_text 			= AnsiString(IE->Editor->Text).LowerCase();
    IE->Editor->Text 				= new_text;
	AnsiString old_name,new_name;
	FHelper.MakeName				(IE->Item,0,old_name,false);
    _ReplaceItem					(old_name.c_str(),IE->Item->Level,new_text.c_str(),new_name,'\\');
    TElTreeItem* find_item			= FHelper.FindItem(tvItems,new_name);
    InputValid 						= (find_item==IE->Item)||(!find_item);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::InplaceEditAfterOperation(TObject *Sender,
      bool &Accepted, bool &DefaultConversion)
{
	R_ASSERT(m_Flags.is(ilEditMenu));
	TElTreeInplaceAdvancedEdit* IE	= InplaceEdit;
    AnsiString new_text 			= AnsiString(IE->Editor->Text).LowerCase();
    FHelper.RenameItem				(tvItems,IE->Item,new_text,OnItemRename);
	if (tvItems->OnAfterSelectionChange) tvItems->OnAfterSelectionChange(0);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::miCreateFolderClick(TObject *Sender)
{
	R_ASSERT(m_Flags.is(ilEditMenu));
	AnsiString folder;
    AnsiString start_folder;
    FHelper.MakeName(tvItems->Selected,0,start_folder,true);
    FHelper.GenerateFolderName(tvItems,tvItems->Selected,folder);
    folder = start_folder+folder;
	TElTreeItem* node = FHelper.AppendFolder(tvItems,folder.c_str());
    if (tvItems->Selected) tvItems->Selected->Expand(false);
    tvItems->EditItem(node,-1);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::Rename1Click(TObject *Sender)
{
	R_ASSERT(m_Flags.is(ilEditMenu));
    ElItemsVec sel_items;
    if (1==GetSelected(sel_items))
		if (sel_items.back()) tvItems->EditItem(sel_items.back(),-1);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	R_ASSERT(m_Flags.is(ilEditMenu));
	FHelper.DragDrop(Sender,Source,X,Y,OnItemRename);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::Delete1Click(TObject *Sender)
{
	R_ASSERT(m_Flags.is(ilEditMenu));
    ElItemsVec sel_items;
    if (GetSelected(sel_items))
        for (ElItemsIt it=sel_items.begin(); it!=sel_items.end(); it++)
			FHelper.RemoveItem(tvItems,*it,OnItemRemove);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (m_Flags.is(ilEditMenu)){
		if (Key==VK_DELETE) 
			FHelper.RemoveItem(tvItems,tvItems->Selected,OnItemRemove);
    }
}
//---------------------------------------------------------------------------

