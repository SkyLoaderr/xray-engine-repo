#include "stdafx.h"
#pragma hdrstop

#include "ItemList.h"
#include <ElVCLUtils.hpp>
#include <ElTools.hpp>

#include "ShaderFunction.h"
#include "ColorPicker.h"
#include "ChoseForm.h"
#include "FolderLib.h"
#include "NumericVector.h"
#include "TextForm.h"
#include "ui_main.h"
#include "EThumbnail.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "multi_edit"
#pragma link "ElTreeStdEditors"
#pragma link "ElXPThemedControl"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma link "ElTree"
#pragma link "ElTreeStdEditors"
#pragma link "ElXPThemedControl"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma link "ElTreeAdvEdit"
#pragma link "ElBtnCtl"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TItemList::ILVec TItemList::ILForms;
//---------------------------------------------------------------------------
void TItemList::OnFormFrame()
{
}
//---------------------------------------------------------------------------

void TItemList::OnFrame()
{
	for (ILIt it=ILForms.begin(); it!=ILForms.end(); it++)
    	(*it)->OnFormFrame();
}

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
        // store
        if (m_Flags.is(ilFolderStore)&&tvItems->Items->Count){
	        FolderStore.clear();
            for (TElTreeItem* item=tvItems->Items->GetFirstNode(); item; item=item->GetNext()){
            	if (item->ChildrenCount){
                	AnsiString nm;
                	FHelper.MakeFullName(item,0,nm);
                    SFolderStore 		st_item;
                    st_item.expand		= item->Expanded;
                    FolderStore[nm]		= st_item;
                }
            }
        }
        // real clear
	    for (ListItemsIt it=m_Items.begin(); it!=m_Items.end(); it++)
    		xr_delete			(*it);
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
void __fastcall TItemList::DeselectAll()
{
    if (tvItems->MultiSelect) 	tvItems->DeselectAll();
    else 						tvItems->Selected   = 0;
}
//---------------------------------------------------------------------------
ListItem* TItemList::FindItem(LPCSTR full_name)
{
	TElTreeItem* item;              
    FHelper.FindObject			(tvItems,full_name,&item);
    return item?(ListItem*)item->Tag:0;
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
    OnItemFocused	= 0;
    OnItemsFocused	= 0;
    OnCloseEvent	= 0;
    OnItemRename	= 0;
    iLocked			= 0;
}
//---------------------------------------------------------------------------

TItemList* TItemList::CreateForm(const AnsiString& title, TWinControl* parent, TAlign align, u32 flags)
{
	TItemList* props 			= xr_new<TItemList>(parent);
    props->m_Flags.set			(flags);
    props->tvItems->MultiSelect	= props->m_Flags.is(ilMultiSelect);
    if (parent){
		props->Parent 			= parent;
    	props->Align 			= align;
	    props->BorderStyle 		= bsNone;
        props->ShowList			();
        props->fsStorage->Active= false;
    }
    if (props->m_Flags.is(ilDragAllowed)){
	    props->tvItems->OnStartDrag = FHelper.StartDrag;
	    props->tvItems->OnDragOver 	= FHelper.DragOver;
        props->tvItems->DragAllowed	= true;
    }else if (props->m_Flags.is(ilDragCustom)){
        props->tvItems->DragAllowed	= true;
    }else{
	    props->tvItems->OnStartDrag = 0;
	    props->tvItems->OnDragOver 	= 0;
        props->tvItems->DragAllowed	= false;
    }
    if (props->Parent)	props->tvItems->HeaderSections->Item[0]->Text = title;
    else				props->Caption 	= title;
    props->fsStorage->IniSection   		= title;
    props->paStatus->Visible 			= !props->m_Flags.is(ilSuppressStatus);
	ILForms.push_back(props);
	return props;
}

TItemList* TItemList::CreateModalForm(const AnsiString& title, u32 flags)
{
	return CreateForm			(title,0,alNone,flags);
}

void TItemList::DestroyForm(TItemList*& props)
{
	VERIFY(props);
    ILIt it						= std::find(ILForms.begin(),ILForms.end(),props); VERIFY(it!=ILForms.end());
	ILForms.erase				(it);
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

void __fastcall TItemList::AssignItems(ListItemsVec& items, bool full_expand, bool full_sort)
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
        if (!prop->key.IsEmpty()&&(prop->key[prop->key.Length()]=='\\')){
        	TElTreeItem* I	= FHelper.AppendFolder(tvItems,prop->key,!m_Flags.is(ilSuppressIcon));
            I->UseStyles				= true;
            I->MainStyle->TextColor 	= prop->prop_color;         
            I->MainStyle->Style			= ElhsOwnerDraw;
        }else{
            prop->item			= FHelper.AppendObject(tvItems,prop->key,false,!m_Flags.is(ilSuppressIcon));
            if (!prop->item){
                ELog.DlgMsg		(mtError,"Duplicate item name found: '%s'",prop->key);
                break;
            }
            prop->item->ImageIndex	= prop->icon_index;
            prop->item->Tag	    	= (int)prop;
            prop->item->UseStyles	= true;
            prop->item->CheckBoxEnabled = prop->m_Flags.is(ListItem::flShowCB);
            prop->item->ShowCheckBox 	= prop->m_Flags.is(ListItem::flShowCB);
            prop->item->CheckBoxState 	= (TCheckBoxState)prop->m_Flags.is(ListItem::flCBChecked);

	        prop->item->MainStyle->OwnerProps 	= false;
    	    prop->item->MainStyle->TextColor	= clRed;
            // set flags                                        
            if (prop->m_Flags.is(ListItem::flDrawThumbnail)){
                prop->item->Height 		= 64;
                prop->item->OwnerHeight = !miDrawThumbnails->Checked;
            }
            // set style
            prop->item->MainStyle->OwnerProps 	= true;
            prop->item->MainStyle->Style 		= ElhsOwnerDraw;
        }
    }

    // end fill mode
	if (full_expand) tvItems->FullExpand();

    // folder restore
    if (m_Flags.is(ilFolderStore)&&!FolderStore.empty()){
        for (TElTreeItem* item=tvItems->Items->GetFirstNode(); item; item=item->GetNext()){
            if (item->ChildrenCount){
                AnsiString nm;
                FHelper.MakeFullName		(item,0,nm);
                FolderStorePairIt it 		= FolderStore.find(nm);
                if (it!=FolderStore.end()){
                    SFolderStore& st_item 	= it->second;
                    if (st_item.expand) 	item->Expand	(false);
                    else					item->Collapse	(false);
                }
            }
        }
    }

    // sorting
    if (full_sort){
        tvItems->ShowColumns	= false;
    	tvItems->Sort			(true);
        tvItems->SortMode 		= smAdd;
        tvItems->ShowColumns	= true;
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

    // check size
	tvItemsResize			(0);

    paStatus->Caption		= AnsiString(" Items count: ")+m_Items.size();
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
    if (OnItemFocused)		OnItemFocused(GetSelected());
    if (OnItemsFocused) 	OnItemsFocused(sel_items);
    for (ListItemsIt it=sel_items.begin(); it!=sel_items.end(); it++){
        if ((*it)->OnItemFocused)(*it)->OnItemFocused(*it);
    }
}
//---------------------------------------------------------------------------

void __fastcall TItemList::FormShow(TObject *Sender)
{
    InplaceEdit->Editor->Color		= TColor(0x00A0A0A0);
    InplaceEdit->Editor->AutoSelect	= true;
	// check window position
	UI->CheckWindowPos(this);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsItemChange(TObject *Sender,
      TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
	if (Item){
    	if (icmCheckState==ItemChangeMode){
            ListItem* prop 			= (ListItem*)Item->Tag;
            if (prop){
                prop->m_Flags.set	(ListItem::flCBChecked,Item->Checked);
    //			prop->OnChange		();
    //			Modified			();
            }
	        tvItems->Refresh		();
	    }
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
        else
        	prop->item->OwnerHeight = true;
    }
	UnlockUpdating			();
	tvItems->Repaint		();
}

void __fastcall TItemList::tvItemsItemDraw(TObject *Sender,
      TElTreeItem *Item, TCanvas *Surface, TRect &R, int SectionIndex)
{
    ListItem* prop 			= (ListItem*)Item->Tag;
    if (prop){
    	Surface->Font->Color= prop->prop_color;
        DrawText			(Surface->Handle, AnsiString(Item->Text).c_str(), -1, &R, DT_LEFT | DT_SINGLELINE);
        if (miDrawThumbnails->Checked&&prop->m_Flags.is(ListItem::flDrawThumbnail)){ 
            R.top			+= tvItems->LineHeight-4;
            if (prop->OnDrawThumbnail)
            	prop->OnDrawThumbnail(prop,Surface,R);
        }
    }else{
    	Surface->Font->Color= Item->MainStyle->TextColor;
        DrawText			(Surface->Handle, AnsiString(Item->Text).c_str(), -1, &R, DT_LEFT | DT_SINGLELINE);
    }
}
//---------------------------------------------------------------------------

void __fastcall TItemList::InplaceEditValidateResult(
      TObject *Sender, bool &InputValid)
{
	R_ASSERT(m_Flags.is(ilEditMenu));
	TElTreeInplaceAdvancedEdit* IE	= InplaceEdit;
    AnsiString new_text 			= AnsiString(IE->Editor->Text).LowerCase();
    InputValid						= false;
    if (!new_text.IsEmpty()){
	    IE->Editor->Text 			= new_text;
		AnsiString old_name, new_name;
		FHelper.MakeName			(IE->Item,0,old_name,false);
	    _ReplaceItem				(old_name.c_str(),IE->Item->Level,new_text.c_str(),new_name,'\\');
	    TElTreeItem* find_item		= FHelper.FindItem(tvItems,new_name);
    	InputValid 					= (find_item==IE->Item)||(!find_item);//.(!find_item); нужно для того чтобы принимало 
    }
}
//---------------------------------------------------------------------------

void __fastcall TItemList::InplaceEditAfterOperation(TObject *Sender,
      bool &Accepted, bool &DefaultConversion)
{
	if (Accepted){
        R_ASSERT(m_Flags.is(ilEditMenu));
        TElTreeInplaceAdvancedEdit* IE	= InplaceEdit;
        AnsiString new_text 			= AnsiString(IE->Editor->Text).LowerCase();
        bool bRes						= FHelper.RenameItem(tvItems,IE->Item,new_text,RenameItem); 
        if (bRes){
	        if (tvItems->OnAfterSelectionChange)tvItems->OnAfterSelectionChange(0);
            if (OnModifiedEvent)		OnModifiedEvent();
            // ensure visible
            IE->Item->Text				= new_text;
			tvItems->EnsureVisible		(IE->Item); 
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall	TItemList::RenameItem(LPCSTR fn0, LPCSTR fn1, EItemType type)
{
	if (OnItemRename) OnItemRename	(fn0,fn1,type);
    if (type==TYPE_OBJECT){
        TElTreeItem* item0			= FHelper.FindObject(tvItems,fn0); 	VERIFY(item0);
        ListItem* prop				= (ListItem*)item0->Tag; 			VERIFY(prop);
		prop->SetName				(fn1);
        TElTreeItem* item1			= FHelper.FindObject(tvItems,fn1);
        if (item1) prop->item		= item1;
    }
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	R_ASSERT(m_Flags.is(ilEditMenu));
	FHelper.DragDrop(Sender,Source,X,Y,RenameItem);
	if (tvItems->OnAfterSelectionChange) tvItems->OnAfterSelectionChange(0);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::miCreateFolderClick(TObject *Sender)
{
	FHelper.CreateNewFolder(tvItems,true);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::Rename1Click(TObject *Sender)
{
	RenameSelItem();
}
//---------------------------------------------------------------------------

void __fastcall TItemList::Delete1Click(TObject *Sender)
{
	RemoveSelItems();
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (m_Flags.is(ilEditMenu)){
		if (Key==VK_DELETE) 
			if (FHelper.RemoveItem(tvItems,tvItems->Selected,OnItemRemove))
            	if (OnModifiedEvent) OnModifiedEvent();
    }
}
//---------------------------------------------------------------------------

void TItemList::LoadSelection(TFormStorage* storage)
{
	last_selected_items.clear();
    for (int k=0; ;k++){
    	AnsiString tmp = storage->ReadString(AnsiString().sprintf("sel%d",k),"");
        if (tmp.IsEmpty()) break;
        last_selected_items.push_back(tmp);
    }
}
//---------------------------------------------------------------------------

void TItemList::SaveSelection(TFormStorage* storage)
{
    ElItemsVec items;
    if (GetSelected(items)){
        for (ElItemsIt l_it=items.begin(); l_it!=items.end(); l_it++){
            AnsiString s;
            FHelper.MakeFullName(*l_it,0,s);
	    	storage->WriteString(AnsiString().sprintf("sel%d",l_it-items.begin()),s);
        }
    }
//    for (AStringIt s_it=last_selected_items.begin(); s_it!=last_selected_items.end(); s_it++)
//    	storage->WriteString(AnsiString().sprintf("sel%d",s_it-last_selected_items.begin()),*s_it);
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsResize(TObject *Sender)
{
    tvItems->HeaderSections->Item[0]->Width = tvItems->Width;
    if (tvItems->VertScrollBarVisible)
    	tvItems->HeaderSections->Item[0]->Width -= tvItems->VertScrollBarStyles->Width;
}
//---------------------------------------------------------------------------

void __fastcall TItemList::RefreshForm1Click(TObject *Sender)
{
	RefreshForm();	
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsHeaderResize(TObject *Sender)
{
    tvItems->HeaderSections->Item[0]->Width = tvItems->Width;
    if (tvItems->VertScrollBarVisible)
    	tvItems->HeaderSections->Item[0]->Width -= tvItems->VertScrollBarStyles->Width;
}
//---------------------------------------------------------------------------

void TItemList::RemoveSelItems(CFolderHelper::TOnItemRemove on_remove)
{
	R_ASSERT(m_Flags.is(ilEditMenu)||on_remove);
    ElItemsVec sel_items;
    if (GetSelected(sel_items)){
    	tvItems->IsUpdating = true; // LockUpdating нельзя
    	DeselectAll					();
        tvItemsAfterSelectionChange	(0);
        bool bSelChanged=false;
        bool bRes=false;
        for (ElItemsIt it=sel_items.begin(); it!=sel_items.end(); it++)
			if (!FHelper.RemoveItem(tvItems,*it,on_remove?on_remove:OnItemRemove)){
            	AnsiString fn;
                FHelper.MakeFullName(*it,0,fn);
            	SelectItem(fn,true,true,false);
                bSelChanged=true;
            }else{
            	bRes = true;
            }
        if (bSelChanged||bRes){
            tvItemsAfterSelectionChange	(0);
            if (bRes&&OnModifiedEvent)	OnModifiedEvent(); 
        }
    	tvItems->IsUpdating 		= false;
    }
}
//---------------------------------------------------------------------------

void TItemList::RenameSelItem()
{
	R_ASSERT(m_Flags.is(ilEditMenu));
    ElItemsVec sel_items;
    if (1==GetSelected(sel_items)){
		if (sel_items.back()) tvItems->EditItem(sel_items.back(),-1);
        tvItemsAfterSelectionChange	(0);
    }
}
//---------------------------------------------------------------------------

void __fastcall TItemList::tvItemsCompareItems(TObject *Sender,
      TElTreeItem *Item1, TElTreeItem *Item2, int &res)
{
	u32 type1 = (u32)Item1->Data;
	u32 type2 = (u32)Item2->Data;
    if (type1==type2){
        if (Item1->Text<Item2->Text) 		res = -1;
        else if (Item1->Text>Item2->Text) 	res =  1;
        else if (Item1->Text==Item2->Text) 	res =  0;
    }else if (type1==TYPE_FOLDER)	    	res = -1;
    else if (type2==TYPE_FOLDER)	    	res =  1;
}
//---------------------------------------------------------------------------

void TItemList::FireOnItemFocused()
{
	tvItemsAfterSelectionChange(0);
}
//---------------------------------------------------------------------------

