//------------------------------------------------------------------------------
// - Events:
//	- OnBeforeEdit
// 	- OnDrawValue
//------------------------------------------------------------------------------

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
    	//S когда будут все итемы удалить у каждого
/*
//s
    	for (TElTreeItem* item=node; item; item=item->GetNext()){
			PropValue* V = (PropValue*)GetItemData(item);
            if (V){
	            PropValuePairIt it=std::find(m_Values.begin(),m_Values.end(),V); VERIFY(it!=m_Values.end());
    	        if (it){
					m_Values.erase(it);
					xr_delete(V);
                }
            }
		}
*/
    }else{
        // save last selected items
        ListItemsVec items;
        GetSelected(items);
        if (!items.empty()){
            last_selected_items.clear();
            for (ListItemsIt l_it=items.begin(); l_it!=items.end(); l_it++){
                last_selected_items.push_back(AnsiString());
                AnsiString& s = last_selected_items.back();
                FHelper.MakeFullName((*l_it)->Item(),0,s);
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
    	item->Selected			= bVal;
		tvItems->EnsureVisible	(item);
		if (tvItems->OnAfterSelectionChange) tvItems->OnAfterSelectionChange(tvItems);
    }
}
//---------------------------------------------------------------------------

__fastcall TItemList::TItemList(TComponent* Owner) : TForm(Owner)
{
    DEFINE_INI		(fsStorage);
}
//---------------------------------------------------------------------------

TItemList* TItemList::CreateForm(TWinControl* parent, TAlign align, TOnItemsFocused focused, TOnCloseEvent on_close)
{
	TItemList* props 			= xr_new<TItemList>(parent);
    props->OnItemsFocused    	= focused;
    props->OnCloseEvent			= on_close;
    if (parent){
		props->Parent 			= parent;
    	props->Align 			= align;
	    props->BorderStyle 		= bsNone;
        props->ShowList	();
        props->fsStorage->Active= false;
    }
	return props;
}

TItemList* TItemList::CreateModalForm(const AnsiString& title, ListItemsVec& items, bool bFullExpand, TOnItemsFocused on_focused, TOnCloseEvent on_close)
{
	TItemList* props 			= xr_new<TItemList>((TComponent*)0);
    props->OnItemsFocused    	= on_focused;
    props->OnCloseEvent			= on_close;
    props->AssignItems			(items,bFullExpand,title);
	props->ShowListModal		();
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

void __fastcall TItemList::AssignItems(ListItemsVec& items, bool full_expand, const AnsiString& title)
{
	// begin fill mode
	LockUpdating			();
    // clear values
//    if (tvItems->Selected) FHelper.MakeFullName(tvItems->Selected,0,last_selected_item);
    ClearParams				();
    tvItems->Items->Clear	();
    // fill values
    m_Items					= items;
	for (ListItemsIt it=m_Items.begin(); it!=m_Items.end(); it++){
    	ListItem* prop		= *it;
        prop->item			= FHelper.AppendObject(tvItems,prop->key);
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

    // expand sel items
    TElList* el_list=xr_new<TElList>();
    for (AStringIt s_it=last_selected_items.begin(); s_it!=last_selected_items.end(); s_it++)
    	el_list->Add(FHelper.ExpandItem(tvItems,*s_it));

	UnlockUpdating			();

    // restore selection
    tvItems->DeselectAll	();
    for (int k=0; k<el_list->Count; k++)
	    FHelper.RestoreSelection(tvItems,(TElTreeItem*)el_list->Items[k],true);

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

void __fastcall TItemList::tvItemsItemFocused(TObject *Sender)
{
/*
	if (OnItemsFocused) 	OnItemsFocused(tvItems->Selected);
	if (tvItems->Selected){
        ListItem* prop 		= (ListItem*)tvItems->Selected->Tag;
        if (prop&&prop->OnItemFocused)prop->OnItemFocused(prop);
    }
*/
}
//---------------------------------------------------------------------------

int __fastcall TItemList::GetSelected(ListItemsVec& items)
{
    for (TElTreeItem* item = tvItems->GetNextSelected(0); item; item = tvItems->GetNextSelected(item)){
        ListItem* prop 		= (ListItem*)item->Tag;
        if (prop)			items.push_back	(prop);
    }
    return items.size();
}

void __fastcall TItemList::tvItemsAfterSelectionChange(TObject *Sender)
{
    ListItemsVec sel_items;
    GetSelected	(sel_items);
    if (OnItemsFocused) 	OnItemsFocused(sel_items);
    for (ListItemsIt it=sel_items.begin(); it!=sel_items.end(); it++)
        if ((*it)->OnItemFocused)(*it)->OnItemFocused(*it);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::FormShow(TObject *Sender)
{
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




