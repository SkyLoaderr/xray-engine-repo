//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ChoseForm.h"
#include "EThumbnail.h"
#include "PropertiesList.h"               
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "MXCtrls"
#pragma link "mxPlacemnt"
#pragma link "ElXPThemedControl"
#pragma link "Gradient"
#pragma resource "*.dfm"
TfrmChoseItem*				TfrmChoseItem::form			= 0;
AnsiString 					TfrmChoseItem::select_item	= "";
TfrmChoseItem::EventsMap	TfrmChoseItem::m_Events;
TOnChooseFillEvents 		TfrmChoseItem::fill_events	= 0;      
AnsiString 					TfrmChoseItem::m_LastSelection; 

//---------------------------------------------------------------------------
SChooseEvents* TfrmChoseItem::GetEvents	(u32 choose_ID)
{
	EventsMapIt it 	= m_Events.find(choose_ID);
    if (it!=m_Events.end()){
    	return &it->second;
    }else return 0;
}
void TfrmChoseItem::AppendEvents(u32 choose_ID, LPCSTR caption, TOnChooseFill on_fill, TOnChooseSelect on_sel, bool bTHM)
{
	EventsMapIt it 	= m_Events.find(choose_ID); VERIFY(it==m_Events.end());
    m_Events.insert	(std::make_pair(choose_ID,SChooseEvents(caption,on_fill,on_sel,bTHM)));
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseItem::FormCreate(TObject *Sender)
{
    m_Props = TProperties::CreateForm("Info",paInfo,alClient);
    m_Thm	= 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::FormDestroy(TObject *Sender)
{
	TProperties::DestroyForm		(m_Props);
}
//---------------------------------------------------------------------------

int __fastcall TfrmChoseItem::SelectItem(u32 choose_ID, LPCSTR& dest, int sel_cnt, LPCSTR init_name, TOnChooseFillProp item_fill, TOnChooseSelect item_select)
{
	if (m_Events.empty()){
    	VERIFY						(fill_events);
	    fill_events					();
    }
	VERIFY(!form);
	form 							= xr_new<TfrmChoseItem>((TComponent*)0);
    form->bMultiSel 				= sel_cnt>1;
    form->iMultiSelLimit 			= sel_cnt;

	// init
	if (init_name&&init_name[0]) 
    	m_LastSelection 			= init_name;
    form->tvItems->Selected 		= 0;

    // fill items
    if (item_fill){
    	// custom
        item_fill					(form->m_Items);
	    form->Caption				= "Select Item";
	    form->item_select_event		= item_select;
    }else{
    	SChooseEvents* E			= GetEvents(choose_ID); VERIFY(E);
        E->on_fill					(form->m_Items);
	    form->Caption				= E->caption;
	    form->item_select_event		= item_select?item_select:E->on_sel;
    }
    form->FillItems					();
    
	form->paItemsCount->Caption		= AnsiString(" Items in list: ")+AnsiString(form->tvItems->Items->Count);

	// show
    bool bRes 						= (form->ShowModal()==mrOk);
    dest							= 0;
    if (bRes){
		int item_cnt				= _GetItemCount(select_item.c_str(),',');
    	dest 						= (select_item==NONE_CAPTION)?0:select_item.c_str();
	    m_LastSelection				= select_item;
        return 						item_cnt;
    }
    return 0;
}
// Constructors
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::AppendItem(SChooseItem* item)
{
    TElTreeItem* node				= FHelper.AppendObject(form->tvItems,item->name,false,true);
    node->Tag						= (int)item;
    node->Hint						= item->hint.Length()?item->hint:AnsiString("-");
    node->CheckBoxEnabled 			= form->bMultiSel;
    node->ShowCheckBox 				= form->bMultiSel;
}

void __fastcall TfrmChoseItem::FillItems()
{
	form->tvItems->IsUpdating		= true;
    tvItems->Items->Clear			();
    FHelper.AppendObject			(tvItems,NONE_CAPTION,false,true);
    ChooseItemVecIt  it				= m_Items.begin();
    ChooseItemVecIt  _E				= m_Items.end();
    for (; it!=_E; it++)			AppendItem(&(*it));
    form->tvItems->Sort				(true);
	form->tvItems->IsUpdating 		= false;
}

//---------------------------------------------------------------------------
// implementation
//---------------------------------------------------------------------------
__fastcall TfrmChoseItem::TfrmChoseItem(TComponent* Owner)
    : TForm(Owner)
{
	tvItems->MultiSelect = false;
    bMultiSel = false;
    tvItems->ShowCheckboxes = false;
    grdFon->Caption = "";
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseItem::sbSelectClick(TObject *Sender)
{
	if (bMultiSel){
        select_item = "";
        AnsiString nm;
	    for ( TElTreeItem* node = tvMulti->Items->GetFirstNode(); node; node = node->GetNext()){
    	    FHelper.MakeName(node,0,nm,false);
            select_item += nm+AnsiString(",");
        }
        select_item.Delete(select_item.Length(),1);

        if (select_item.IsEmpty()){
            if (tvItems->Selected&&FHelper.IsObject(tvItems->Selected)){
	    	    FHelper.MakeName(tvItems->Selected,0,select_item,false);
                Close();
                ModalResult = mrOk;
            }else{
				Msg		("#Select item first.");
            }
        }else{
            Close();
            ModalResult = mrOk;
        }
    }else{
	    if (tvItems->Selected&&FHelper.IsObject(tvItems->Selected)){
    	    FHelper.MakeName(tvItems->Selected,0,select_item,false);
	        Close();
    	    ModalResult = mrOk;
	    }else{
			Msg			("#Select item first.");
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseItem::sbCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseItem::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) sbCancel->Click();
    if (Key==VK_RETURN) sbSelect->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::FormShow(TObject *Sender)
{
    tvItems->ShowCheckboxes 	= bMultiSel;
	int itm_cnt = _GetItemCount(m_LastSelection.c_str());
	if (bMultiSel){
	    string256 T;
        for (int i=0; i<itm_cnt; i++){
            TElTreeItem* itm_node = FHelper.FindObject(tvItems,_GetItem(m_LastSelection.LowerCase().c_str(),i,T),0,0);//,bIgnoreExt);
	        TElTreeItem* fld_node = 0;
            if (itm_node){
				tvMulti->Items->AddObject(0,_GetItem(m_LastSelection.c_str(),i,T),(void*)TYPE_OBJECT);
            	itm_node->Checked = true;
                tvItems->EnsureVisible(itm_node);
                fld_node=itm_node->Parent;
                if (fld_node) fld_node->Expand(false);
            }
        }
    }else{
        TElTreeItem* itm_node = FHelper.FindItem(tvItems,m_LastSelection.LowerCase().c_str(),0,0);//,bIgnoreExt);
        TElTreeItem* fld_node = 0;
        if (itm_node){
            tvItems->Selected = itm_node;
            tvItems->EnsureVisible(itm_node);
            fld_node=itm_node->Parent;
            if (fld_node) fld_node->Expand(false);
        }else if (fld_node){
            tvItems->EnsureVisible(fld_node);
            fld_node->Expand(false);
            tvItems->Selected = fld_node;
        }
    }
    paMulti->Visible = bMultiSel;
	// check window position
	CheckWindowPos	(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (tvItems->Selected) m_LastSelection=tvItems->Selected->Text;
	Action 			= caFree;
    form 			= 0;
}
//---------------------------------------------------------------------------


void __fastcall TfrmChoseItem::tvItemsDblClick(TObject *Sender)
{
	if (!bMultiSel&&FHelper.IsObject(tvItems->Selected)) sbSelectClick(Sender);
}
//---------------------------------------------------------------------------

bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails){
    char s1 = *(char*)SearchDetails;
    char s2 = *AnsiString(Item->Text).c_str();
	return (s1==tolower(s2));
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::tvItemsKeyPress(TObject *Sender, char &Key)
{
	TElTreeItem* node = tvItems->Items->LookForItemEx(tvItems->Selected,-1,false,false,false,&Key,LookupFunc);
    if (!node) node = tvItems->Items->LookForItemEx(0,-1,false,false,false,&Key,LookupFunc);
    if (node){
    	tvItems->Selected = node;
		tvItems->EnsureVisible(node);
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmChoseItem::tvItemsItemChange(TObject *Sender,
      TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
	if (Item&&(ItemChangeMode==icmCheckState)){
        AnsiString fn;
        FHelper.MakeName(Item,0,fn,false);
	    TElTreeItem *node = tvMulti->Items->LookForItem(0,fn.c_str(),0,0,false,true,false,true,true);
        if (node&&!Item->Checked) node->Delete();
        if (!node&&Item->Checked){
	    	if ((tvMulti->Items->Count+1)<=iMultiSelLimit){
    	        tvMulti->Items->AddObject(0,fn,(void*)TYPE_OBJECT);
            }else{
            	Item->Checked 	= false;
	        	Msg				("#Limit %d item(s).",iMultiSelLimit);
            }
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::ebMultiUpClick(TObject *Sender)
{
    if (tvMulti->Selected&&tvMulti->Selected->GetPrev())
    	tvMulti->Selected->MoveToIns(0,tvMulti->Selected->Index-1);
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::ebMultiDownClick(TObject *Sender)
{
    if (tvMulti->Selected&&tvMulti->Selected->GetNext())
    	tvMulti->Selected->MoveToIns(0,tvMulti->Selected->Index+1);
}
//---------------------------------------------------------------------------
static TElTreeItem* DragItem=0;
void __fastcall TfrmChoseItem::tvMultiDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	TElTreeItem* node = ((TElTree*)Sender)->GetItemAtY(Y);
    if (node)
		DragItem->MoveToIns(0,node->Index);
	DragItem = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::tvMultiDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	Accept = false;
	TElTreeItem* node = ((TElTree*)Sender)->GetItemAtY(Y);
	if ((Sender==Source)&&(node!=DragItem)) Accept=true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::tvMultiStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
	DragItem = ((TElTree*)Sender)->Selected;
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::ebMultiRemoveClick(TObject *Sender)
{
	if (tvMulti->Selected){
    	TElTreeItem *cur = tvMulti->Selected;
	    TElTreeItem *node = tvItems->Items->LookForItem(0,cur->Text,0,0,false,true,false,true,true);
        if (node) node->Checked = false;
        tvMulti->Selected = cur->GetNext()?cur->GetNext():cur->GetPrev();
    	cur->Delete();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::ebMultiClearClick(TObject *Sender)
{
	tvMulti->Items->Clear();
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
    	if (node->Checked) node->Checked=false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::tvItemsItemFocused(TObject *Sender)
{
	TElTreeItem* Item 	= tvItems->Selected;
    PropItemVec 		items;
	xr_delete			(m_Thm);
	m_Snd.destroy		();
	if (Item&&FHelper.IsObject(Item)&&Item->Tag){
    	if (ebExt->Down&&item_select_event)	item_select_event((SChooseItem*)Item->Tag,m_Thm,m_Snd,items);
        lbItemName->Caption 	= Item->Text;
        lbHint->Caption 		= Item->Hint;
    }
    if (m_Thm)			m_Thm->FillInfo	(items);
    if (m_Snd.handle)	m_Snd.play(0,sm_2D);
    m_Props->AssignItems		(items);
    paInfo->Visible				= !items.empty();
	paImage->Repaint			();
/*    
    	switch (Mode){
        case smTexture:{
            if (ebExt->Down){
                AnsiString nm;
                FHelper.MakeName		(Item,0,nm,false);
                if (nm!=NONE_CAPTION)	m_Thm	= xr_new<ETextureThumbnail>(nm.c_str());
                lbItemName->Caption 	= "\""+ChangeFileExt(Item->Text,"")+"\"";
            }
        }break;
        case smObject:{
            if (ebExt->Down){
                AnsiString nm,fn;
                FHelper.MakeName		(Item,0,nm,false);
                fn						= ChangeFileExt(nm,".thm");
                FS.update_path			(_objects_,fn);
                if (FS.exist(fn.c_str())){
                    m_Thm 				= xr_new<EObjectThumbnail>(nm.c_str());
                }
            }
			lbItemName->Caption 	= "\""+Item->Text+"\"";
			lbHint->Caption			= "\""+Item->Text+".object\"";
        }break;
        case smSoundSource:{
            if (ebExt->Down){
                AnsiString fn;
                FHelper.MakeName		(Item,0,fn,false);
                fn						= ChangeFileExt(fn,".ogg");
                const CLocatorAPI::file* file	= FS.exist(_game_sounds_,fn.c_str());
                if (file){
                    m_Snd.create		(TRUE,fn.c_str());
                    m_Snd.play			(0,sm_2D);
                }
            }
			lbItemName->Caption 	= "\""+Item->Text+"\"";
        }break;
        case smGameObject:{
	        AnsiString fn;
        	FHelper.MakeName		(Item,0,fn,false);
            fn						= ChangeFileExt(fn,".ogf");
            if (ebExt->Down){
                IRender_Visual* visual	= ::Render->model_Create(fn.c_str());
                if (visual){
                    PHelper.CreateCaption	(items, 	"Source File",	*visual->desc.source_file?*visual->desc.source_file:"unknown");
                    PHelper.CreateCaption	(items, 	"Creator Name",	*visual->desc.create_name?*visual->desc.create_name:"unknown");
                    PHelper.CreateCaption	(items, 	"Creator Time",	Trim(AnsiString(ctime(&visual->desc.create_time))));
                    PHelper.CreateCaption	(items, 	"Modif Name",	*visual->desc.modif_name ?*visual->desc.modif_name :"unknown");
                    PHelper.CreateCaption	(items, 	"Modif Time",	Trim(AnsiString(ctime(&visual->desc.modif_time))));
                    PHelper.CreateCaption	(items, 	"Build Name",	*visual->desc.build_name ?*visual->desc.build_name :"unknown");
                    PHelper.CreateCaption	(items, 	"Build Time",	Trim(AnsiString(ctime(&visual->desc.build_time))));
                }
                ::Render->model_Delete(visual);
            }
			lbItemName->Caption 	= "\""+Item->Text+"\"";
        }break;
        default:
			lbItemName->Caption = "\""+Item->Text+"\"";
        }
        lbHint->Caption 		= Item->Hint;
    }else{
		lbItemName->Caption = "-";
		lbHint->Caption		= "-";
    }
    if (m_Thm)	m_Thm->FillInfo		(items);
    m_Props->AssignItems			(items);
    paInfo->Visible					= !items.empty();
	paImage->Repaint				();
*/
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::paImagePaint(TObject *Sender)
{
	if (m_Thm&&m_Thm->IsClass(ECustomThumbnail::ETTexture)) 
    	static_cast<EImageThumbnail*>(m_Thm)->Draw(paImage,false);
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::ebExtClick(TObject *Sender)
{
	tvItemsItemFocused				(0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::fsStorageRestorePlacement(TObject *Sender)
{
	m_Props->RestoreParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::fsStorageSavePlacement(TObject *Sender)
{
	m_Props->SaveParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::tvItemsCompareItems(TObject *Sender,
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


