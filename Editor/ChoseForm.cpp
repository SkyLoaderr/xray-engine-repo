//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ChoseForm.h"
#include "shader.h"
#include "shader_xrlc.h"
#include "texture.h"
#include "xr_trims.h"
#include "Library.h"
#include "EditObject.h"
#include "D3DUtils.h"
#include "ImageThumbnail.h"
#include "FolderLib.h"
#include "ImageManager.h"

#ifdef _LEVEL_EDITOR
#include "PSLibrary.h"
#endif
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "MXCtrls"
#pragma link "mxPlacemnt"
#pragma link "ElXPThemedControl"
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
	form->tvItems->IsUpdating		= true;
    form->tvItems->Selected 		= 0;
    form->tvItems->Items->Clear		();
    // fill object list
	AnsiString fld;
    for (LibObjIt it=Lib->FirstObj(); it!=Lib->LastObj(); it++){
		if (!start_folder||(start_folder&&(stricmp(start_folder,FOLDER::GetFolderName((*it)->GetName(),fld))==0)))
			if (!bExcludeSystem||(bExcludeSystem&&((*it)->GetName()[0]!='$')))
            	FOLDER::AppendObject(form->tvItems,(*it)->GetName());
    }
    // redraw
	form->tvItems->IsUpdating		= false;
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
	form->tvItems->IsUpdating		= true;
    form->tvItems->Selected = 0;
    form->tvItems->Items->Clear();
    // fill shaders list
    CShaderManager::BlenderMap& blenders = Device.Shader._GetBlenders();
	CShaderManager::BlenderPairIt _F = blenders.begin();
	CShaderManager::BlenderPairIt _E = blenders.end();
	for (CShaderManager::BlenderPairIt _S = _F; _S!=_E; _S++){
//		if (!start_folder||(start_folder&&(stricmp(start_folder,S->cFolder)==0)))
		if (bExcludeSystem&&(_S->first[0]!='$')||(!bExcludeSystem))
			FOLDER::AppendObject(form->tvItems,_S->first);
    }
    // redraw
	form->tvItems->IsUpdating		= false;
	// show
    if (form->ShowModal()!=mrOk) return 0;
    return select_item.c_str();
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TfrmChoseItem::SelectShaderXRLC(LPCSTR start_folder, LPCSTR init_name){
	VERIFY(!form);
	form = new TfrmChoseItem(0);
	form->Mode = smShaderXRLC;
	// init
	if (init_name) last_item = init_name;
	form->tvItems->IsUpdating		= true;
    form->tvItems->Selected = 0;
    form->tvItems->Items->Clear();
    // fill shaders list
    Shader_xrLCVec& shaders = Device.ShaderXRLC.Library();
	Shader_xrLCIt _F = shaders.begin();
	Shader_xrLCIt _E = shaders.end();
	for ( ;_F!=_E;_F++)
		FOLDER::AppendObject(form->tvItems,_F->Name);
    // redraw
	form->tvItems->IsUpdating		= false;
	// show
    if (form->ShowModal()!=mrOk) return 0;
    return select_item.c_str();
}
//---------------------------------------------------------------------------
#ifdef _LEVEL_EDITOR
LPCSTR __fastcall TfrmChoseItem::SelectPS(LPCSTR start_folder, LPCSTR init_name){
	VERIFY(!form);
	form = new TfrmChoseItem(0);
	form->Mode = smPS;
	// init
	if (init_name) last_item = init_name;
	form->tvItems->IsUpdating		= true;
    form->tvItems->Selected = 0;
    form->tvItems->Items->Clear();
	AnsiString fld;
    // fill
    for (PS::SDef* S=PSLib->FirstPS(); S!=PSLib->LastPS(); S++){
		if (!start_folder||(start_folder&&stricmp(start_folder,FOLDER::GetFolderName(S->m_Name,fld))))
			FOLDER::AppendObject(form->tvItems,S->m_Name);
    }
    // redraw
	form->tvItems->IsUpdating		= false;
	// show
    if (form->ShowModal()!=mrOk) return 0;
    return select_item.c_str();
}
#endif
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
	form->tvItems->IsUpdating		= true;
    form->tvItems->Selected = 0;
    form->tvItems->Items->Clear();
    // fill
    AStringVec lst;
    if (ImageManager.GetFiles(lst))
        for (AStringIt it=lst.begin(); it!=lst.end(); it++)
        	FOLDER::AppendObject(form->tvItems,it->c_str());
    // redraw
	form->tvItems->IsUpdating		= false;

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
//S	sel_tex = 0;
//S    sel_thm = 0;
    bMultiSel = false;
    tvItems->ShowCheckboxes = false;
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseItem::sbSelectClick(TObject *Sender)
{
	if (bMultiSel){
        select_item = "";
	    for ( TElTreeItem* node = tvMulti->Items->GetFirstNode(); node; node = node->GetNext()){
            select_item += node->Text;
            select_item += ",";
        }
        select_item.Delete(select_item.Length()-1,2);

        if (select_item.IsEmpty()){
            if (tvItems->Selected&&FOLDER::IsObject(tvItems->Selected)){
                select_item = tvItems->Selected->Text;
                Close();
                ModalResult = mrOk;
            }else{
                ELog.DlgMsg(mtInformation,"Select item first.");
            }
        }else{
            Close();
            ModalResult = mrOk;
        }
    }else{
	    if (tvItems->Selected&&FOLDER::IsObject(tvItems->Selected)){
    	    FOLDER::MakeName(tvItems->Selected,0,select_item,false);
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
            TElTreeItem* itm_node = FOLDER::FindObject(tvItems,_GetItem(last_item.c_str(),i,T));
	        TElTreeItem* fld_node = 0;
            if (itm_node){
				tvMulti->Items->Add(0,_GetItem(last_item.c_str(),i,T));
            	itm_node->Checked = true;
                tvItems->EnsureVisible(itm_node);
                fld_node=itm_node->Parent;
                if (fld_node) fld_node->Expand(false);
            }
        }
    }else{
        TElTreeItem* itm_node = FOLDER::FindObject(tvItems,last_item.c_str());
        TElTreeItem* fld_node = 0;//FOLDER::FindFolder(tvItems,last_item.c_str());;
        if (itm_node){
        	if (bMultiSel){
				tvMulti->Items->Add(0,itm_node->Text);
//            	itm_node->Checked = true;
            }
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
	_DELETE(m_Thm);
	Action = caFree;
    form = 0;
    if (tvItems->Selected) last_item=tvItems->Selected->Text;
}
//---------------------------------------------------------------------------



void __fastcall TfrmChoseItem::pbImagePaint(TObject *Sender)
{
    if (m_Thm){
        RECT r;
        r.left = 2; r.top = 2;
        float w, h;
        w = m_Thm->_Width();
        h = m_Thm->_Height();
        if (w!=h)	pbImage->Canvas->FillRect(pbImage->BoundsRect);
        if (w>h){   r.right = pbImage->Width-1; r.bottom = h/w*pbImage->Height-1;
        }else{      r.right = w/h*pbImage->Width-1; r.bottom = pbImage->Height-1;}
        m_Thm->DrawStretch(paImage->Handle, &r);
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
	    TElTreeItem *node = tvMulti->Items->LookForItem(0,Item->Text,0,0,false,true,false,true,true);
        if (node&&!Item->Checked) node->Delete();
        if (!node&&Item->Checked){
        	AnsiString fn;
            FOLDER::MakeName(Item,0,fn,false);
        	tvMulti->Items->Add(0,fn);
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
	TElTreeItem* Item = tvItems->Selected;
	_DELETE(m_Thm);
	if (Item&&FOLDER::IsObject(Item)){
        if (Mode==smTexture){
	        AnsiString nm;
        	FOLDER::MakeName		(Item,0,nm,false);
    	    m_Thm 					= new EImageThumbnail(nm.c_str(),EImageThumbnail::EITTexture);
	        if (!m_Thm->Valid())	pbImage->Repaint();
            else	 				pbImagePaint(Sender);
	        lbItemName->Caption 	= "\""+ChangeFileExt(Item->Text,"")+"\"";
    	    lbFileName->Caption 	= "\""+Item->Text+"\"";
			AnsiString temp; 		temp.sprintf("%d x %d x %s",m_Thm->_Width(),m_Thm->_Height(),m_Thm->_Alpha()?"32b":"24b");
            lbInfo->Caption			= temp;
        }else if (Mode==smObject){
	        AnsiString nm,fn;
        	FOLDER::MakeName		(Item,0,nm,false);
            fn						= ChangeFileExt(nm,".thm");
            FS.m_Objects.Update(fn);
            if (FS.Exist(fn.c_str())){
	    	    m_Thm 					= new EImageThumbnail(nm.c_str(),EImageThumbnail::EITObject);
    	        if (!m_Thm->Valid()) 	pbImage->Repaint();
        	    else				 	pbImagePaint(Sender);
            }
			CLibObject* LO			= Lib->SearchObject(nm.c_str()); R_ASSERT(LO);
			lbItemName->Caption 	= "\""+Item->Text+"\"";
			lbFileName->Caption		= "\""+AnsiString(LO->GetSrcName())+"\"";
            lbInfo->Caption			= "-";
        }else{
			lbItemName->Caption = "\""+Item->Text+"\"";
			lbFileName->Caption	= "-";
            lbInfo->Caption		= "-";
        }
    }else{
		lbItemName->Caption = "-";
		lbFileName->Caption	= "-";
		lbInfo->Caption		= "-";
    }
}
//---------------------------------------------------------------------------

