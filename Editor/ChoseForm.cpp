//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ChoseForm.h"
#include "shader.h"
#include "FileSystem.h"
#include "texture.h"
#include "PSLibrary.h"
#include "xr_trims.h"
#include "Library.h"
#include "EditObject.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Placemnt"
#pragma resource "*.dfm"
TfrmChoseItem *TfrmChoseItem::form=0;
AnsiString TfrmChoseItem::select_item="";
AnsiString TfrmChoseItem::last_item="";
//---------------------------------------------------------------------------
// Constructors
//---------------------------------------------------------------------------
LPCSTR __fastcall TfrmChoseItem::SelectObject(bool bMulti, bool bExcludeSystem, LPCSTR start_folder, LPCSTR start_name){
	VERIFY(!form);
	form 							= new TfrmChoseItem(0);
	form->Mode 						= smObject; 
    form->bMultiSel 				= bMulti;
    form->iMultiSelLimit 			= 8;
    form->tvItems->ShowCheckboxes 	= bMulti;
	// init
	if (start_name) last_item 		= start_name;
	SendMessage(form->tvItems->Handle,WM_SETREDRAW,0,0);
    form->tvItems->Selected 		= 0;
    form->tvItems->Items->Clear		();
    // fill object list
    for (LibObjIt it=Lib->FirstObj(); it!=Lib->LastObj(); it++){
		if (!start_folder||(start_folder&&(stricmp(start_folder,(*it)->GetFolderName())==0)))
			if (!bExcludeSystem||(bExcludeSystem&&((*it)->GetRefName()[0]!='$')))
    	    	form->AddItemToFolder((*it)->GetFolderName(),(*it)->GetRefName(),(*it));
    }
    // redraw
	SendMessage(form->tvItems->Handle,WM_SETREDRAW,1,0);
	form->tvItems->Repaint			();
	// show    
    if (form->ShowModal()!=mrOk) return 0;
    return select_item.c_str();
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TfrmChoseItem::SelectShader(bool bExcludeSystem, LPCSTR start_folder, LPCSTR init_name){
	VERIFY(!form);
	form = new TfrmChoseItem(0);
	form->Mode = smShader; 
	// init
	if (init_name) last_item = init_name;
	SendMessage(form->tvItems->Handle,WM_SETREDRAW,0,0);
    form->tvItems->Selected = 0;
    form->tvItems->Items->Clear();
    // fill shaders list
    for (SH_ShaderDef* S=SHLib->FirstShader(); S!=SHLib->LastShader(); S++){
//		if (!start_folder||(start_folder&&(stricmp(start_folder,S->cFolder)==0)))
		if (bExcludeSystem&&(S->cName[0]!='$')||(!bExcludeSystem))
        	form->AddItemToFolder("Shaders",S->cName);
    }
    // redraw
	SendMessage(form->tvItems->Handle,WM_SETREDRAW,1,0);
	form->tvItems->Repaint();
	// show    
    if (form->ShowModal()!=mrOk) return 0;
    return select_item.c_str();
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TfrmChoseItem::SelectPS(LPCSTR start_folder, LPCSTR init_name){
	VERIFY(!form);
	form = new TfrmChoseItem(0);
	form->Mode = smPS; 
	// init
	if (init_name) last_item = init_name;
	SendMessage(form->tvItems->Handle,WM_SETREDRAW,0,0);
    form->tvItems->Selected = 0;
    form->tvItems->Items->Clear();
    // fill
    for (PS::SDef* S=PSLib->FirstPS(); S!=PSLib->LastPS(); S++){
		if (!start_folder||(start_folder&&stricmp(start_folder,S->m_Folder)))
        	form->AddItemToFolder(S->m_Folder,S->m_Name);
    }
    // redraw
	SendMessage(form->tvItems->Handle,WM_SETREDRAW,1,0);
	form->tvItems->Repaint();
	// show    
    if (form->ShowModal()!=mrOk) return 0;
    return select_item.c_str();
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TfrmChoseItem::SelectTexture(bool msel, LPCSTR init_name){
	VERIFY(!form);
	form = new TfrmChoseItem(0);
	form->Mode = smTexture; 
    form->bMultiSel = msel;
    form->iMultiSelLimit = 8;
    form->tvItems->ShowCheckboxes = msel;
	// init
	if (init_name) last_item = init_name;
	SendMessage(form->tvItems->Handle,WM_SETREDRAW,0,0);
    form->tvItems->Selected = 0;
    form->tvItems->Items->Clear();
    // fill
    TElTreeItem* fld = form->AddFolder("Pictures");
	AnsiString tt;
    AnsiString nm = "*.tga";
    FS.m_Textures.Update(nm);
    LPCSTR T=0;
	if (T=FS.FindFirst(nm.c_str())){
    	do{ 
			tt = ChangeFileExt(AnsiString(T),"");
        	form->AddItem(fld,tt.c_str());
        }while(T=FS.FindNext());
	}
    nm = "*.bmp";
    FS.m_Textures.Update(nm);
	if (T=FS.FindFirst(nm.c_str())){
    	do{ 
			tt = ChangeFileExt(AnsiString(T),"");
        	form->AddItem(fld,tt.c_str());
        }while(T=FS.FindNext());
	}
    // redraw
	SendMessage(form->tvItems->Handle,WM_SETREDRAW,1,0);
	form->tvItems->Repaint();

	// show    
    if (form->ShowModal()!=mrOk) return 0;
    return select_item.c_str();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// implementation
//---------------------------------------------------------------------------
__fastcall TfrmChoseItem::TfrmChoseItem(TComponent* Owner)
    : TForm(Owner)
{
	tvItems->MultiSelect = false;
	sel_tex = 0;
    sel_thm = 0;
    bMultiSel = false;
    tvItems->ShowCheckboxes = false;
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmChoseItem::FindFolder(const char* s)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Data && (node->Text == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmChoseItem::FindItem(const char* s)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (node->Data && (node->Text == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmChoseItem::AddFolder(const char* s)
{
    TElTreeItem* node = 0;
    if (s[0]!=0){
        node = tvItems->Items->AddObject(0,s,0);
        node->ParentStyle = false;
        node->Bold = true;
    }
    return node;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmChoseItem::AddItem(TElTreeItem* node, const char* name, void* obj)
{
    TElTreeItem* obj_node = tvItems->Items->AddChildObject(node, name, obj);
    obj_node->ParentStyle = false;
    obj_node->Bold = false;
	obj_node->ShowCheckBox = bMultiSel;
//	obj_node->CheckBoxType = ectCheckBox;
    obj_node->CheckBoxEnabled = bMultiSel;
    return obj_node;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmChoseItem::AddItemToFolder(const char* folder, const char* name, void* obj){
	TElTreeItem* node = FindFolder(folder);
    if (!node) node = AddFolder(folder);
	return AddItem(node,name,obj);
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::sbSelectClick(TObject *Sender)
{
	if (bMultiSel){
        select_item = "";
	    for ( TElTreeItem* node = tvMulti->Items->GetFirstNode(); node; node = node->GetNext()){
            select_item += node->Text;
            select_item += ", ";
        }
        select_item.Delete(select_item.Length()-1,2);
    
        if (select_item.IsEmpty()){
            if (tvItems->Selected&&tvItems->Selected->Data){
                select_item = tvItems->Selected->Text;
                Close();
                ModalResult = mrOk;
            }else{
                Log->DlgMsg(mtInformation,"Select item first.");
            }
        }else{
            Close();
            ModalResult = mrOk;
        }
    }else{
	    if (tvItems->Selected&&tvItems->Selected->Data){
    	    select_item = tvItems->Selected->Text;
	        Close();
    	    ModalResult = mrOk;
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
	int itm_cnt = _GetItemCount(last_item.c_str());
	if (bMultiSel&&(itm_cnt>1)){
	    char T[MAX_OBJ_NAME];
        for (int i=0; i<itm_cnt; i++){
            TElTreeItem* itm_node = FindItem(_GetItem(last_item.c_str(),i,T));
	        TElTreeItem* fld_node = 0;
            if (itm_node){ 
				tvMulti->Items->Add(0,itm_node->Text);
            	itm_node->Checked = true;
                tvItems->EnsureVisible(itm_node);
                fld_node=itm_node->Parent;
                if (fld_node) fld_node->Expand(false);
            }
        }
    }else{
        TElTreeItem* itm_node = FindItem(last_item.c_str());
        TElTreeItem* fld_node = FindFolder(last_item.c_str());
        if (itm_node){ 
        	if (bMultiSel) itm_node->Checked = true;
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
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::FormClose(TObject *Sender, TCloseAction &Action)
{
	_DELETE(sel_tex);
    _DELETE(sel_thm);
	Action = caFree;
    form = 0;
    if (tvItems->Selected) last_item=tvItems->Selected->Text;
}
//---------------------------------------------------------------------------


void __fastcall TfrmChoseItem::tvItemsItemSelectedChange(TObject *Sender, TElTreeItem *Item)
{
	if (Item==tvItems->Selected) return;
	_DELETE(sel_tex);
    _DELETE(sel_thm);
	if (Item&&Item->Data){
        if (Mode==smTexture){
	        AnsiString nm = ChangeFileExt(Item->Text,"");
    	    sel_tex = new ETextureCore(nm.c_str());
	        if (!sel_tex->Valid())	pbImage->Repaint();
            else	 				pbImagePaint(Sender);
	        lbItemName->Caption 	= "\""+nm+"\"";
    	    lbFileName->Caption 	= "\""+Item->Text+"\"";
			AnsiString temp; 		temp.sprintf("%d x %d x %s",sel_tex->width(),sel_tex->height(),sel_tex->alpha()?"32b":"24b");
            lbInfo->Caption			= temp;
        }else if (Mode==smObject){
	        AnsiString nm = AnsiString("$O_")+Item->Text;
    	    sel_thm 				= new ETextureThumbnail(nm.c_str());
            if (!sel_thm->Load()) 	pbImage->Repaint();
            else				 	pbImagePaint(Sender);
            CLibObject* LO			= (CLibObject*)Item->Data;
			lbItemName->Caption 	= "\""+AnsiString(LO->GetRefName())+"\"";
			lbFileName->Caption		= "\""+AnsiString(LO->GetFileName())+"\"";
//			AnsiString temp; 		temp.sprintf("Vertices: %d\nFaces: %d",LO->GetReference()->GetVertexCount(),LO->GetReference()->GetFaceCount());
            lbInfo->Caption			= "";//temp;
        }else{
			lbItemName->Caption = "\""+Item->Text+"\"";
			lbFileName->Caption	= "...";
            lbInfo->Caption		= "...";
        }
    }else{
		lbItemName->Caption = "...";
		lbFileName->Caption	= "...";
		lbInfo->Caption		= "...";
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::pbImagePaint(TObject *Sender)
{
    if (sel_tex){
        RECT r; 
        r.left = 2; r.top = 2;
        float w, h;
        w = sel_tex->width();
        h = sel_tex->height();
        if (w!=h)	pbImage->Canvas->FillRect(pbImage->BoundsRect);
        if (w>h){   r.right = pbImage->Width; r.bottom = h/w*pbImage->Height;
        }else{      r.right = w/h*pbImage->Width; r.bottom = pbImage->Height;}
        sel_tex->StretchThumbnail(paImage->Handle, &r);
    }else if (sel_thm){
        RECT r; r.left = 2; r.top = 2;
        float w, h;
        w = THUMB_WIDTH;
        h = THUMB_HEIGHT;
        if (w>h){   r.right = pbImage->Width; r.bottom = h/w*pbImage->Height;
        }else{      r.right = w/h*pbImage->Width; r.bottom = pbImage->Height;}
        sel_thm->DrawStretch(paImage->Handle, &r);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::tvItemsDblClick(TObject *Sender)
{
	if (!bMultiSel) sbSelectClick(Sender);
}
//---------------------------------------------------------------------------

bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails){
    char s1 = *(char*)SearchDetails;
    char s2 = *Item->Text.c_str();
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
	    TElTreeItem *node = tvMulti->Items->LookForItem(0,Item->Text,0,0,false,true,false,true,true);
        if (node&&!Item->Checked) node->Delete();
        if (!node&&Item->Checked) tvMulti->Items->Add(0,Item->Text);
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

