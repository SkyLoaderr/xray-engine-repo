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
#include "SoundManager.h"
#include "xr_ini.h"

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

AnsiString TfrmChoseItem::m_LastSelection[smMaxMode];
//---------------------------------------------------------------------------
// Constructors
//---------------------------------------------------------------------------
LPCSTR __fastcall TfrmChoseItem::SelectEntity(LPCSTR init_name)
{
	VERIFY(!form);
	form 							= new TfrmChoseItem(0);
	form->Mode 						= smEntity;
    form->bMultiSel 				= false;
	// init
	if (init_name) m_LastSelection[form->Mode] = init_name;
	form->tvItems->IsUpdating		= true;
    form->tvItems->Selected 		= 0;
    form->tvItems->Items->Clear		();
    // fill object list
	AnsiString fld;
    CInifile* sys_ini;
    AnsiString fn="system.ltx";
    Engine.FS.m_GameRoot.Update(fn);
    sys_ini = new CInifile(fn.c_str(),true);
    CInifile::Root& data = sys_ini->Sections();
    for (CInifile::RootIt it=data.begin(); it!=data.end(); it++)
    {
    	LPCSTR val;
    	if (it->LineExists("$spawn",&val))
			if (CInifile::IsBOOL(val))
            	FOLDER::AppendObject(form->tvItems,it->Name);;
    }
    _DELETE(sys_ini);
    // redraw
	form->tvItems->IsUpdating		= false;
	// show
    if (form->ShowModal()!=mrOk) return 0;
    return select_item.c_str();
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TfrmChoseItem::SelectEntityCLSID(LPCSTR init_name)
{
	VERIFY(!form);
	form 							= new TfrmChoseItem(0);
	form->Mode 						= smEntityCLSID;
    form->bMultiSel 				= false;
	// init
	if (init_name) m_LastSelection[form->Mode] = init_name;
	form->tvItems->IsUpdating		= true;
    form->tvItems->Selected 		= 0;
    form->tvItems->Items->Clear		();
    // fill object list
	AnsiString fld;
    CInifile* sys_ini;
    AnsiString fn="system.ltx";
    Engine.FS.m_GameRoot.Update(fn);
    sys_ini = new CInifile(fn.c_str(),true);
    CInifile::Root& data = sys_ini->Sections();
	FOLDER::AppendObject(form->tvItems,"O_ACTOR");
    for (CInifile::RootIt it=data.begin(); it!=data.end(); it++)
    {
    	AnsiString sect=it->Name;
        if (1==sect.Pos("m_"))
        	if (sys_ini->LineExists(it->Name,"class")){
            	LPCSTR N=sys_ini->ReadSTRING(it->Name,"class");
        		FOLDER::AppendObject(form->tvItems,N);
            }
    }
    _DELETE(sys_ini);
    // redraw
	form->tvItems->IsUpdating		= false;
	// show
    if (form->ShowModal()!=mrOk) return 0;
    return select_item.c_str();
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TfrmChoseItem::SelectSound(bool bMulti, LPCSTR init_name, bool bIgnoreExt){
	VERIFY(!form);
	form 							= new TfrmChoseItem(0);
	form->Mode 						= smSound;
    form->bMultiSel 				= bMulti;
    form->iMultiSelLimit 			= 32;
    form->bIgnoreExt 				= bIgnoreExt;
	// init
	if (init_name) m_LastSelection[form->Mode] = init_name;
	form->tvItems->IsUpdating	= true;
    form->tvItems->Selected 	= 0;
    form->tvItems->Items->Clear	();
    // fill
    FileMap lst;
    if (SoundManager.GetSounds(lst)){
	    FilePairIt it=lst.begin();
    	FilePairIt _E=lst.end();   // check without extension
	    for (; it!=_E; it++)
        	FOLDER::AppendObject(form->tvItems,it->first.c_str());
    }
    // redraw
	form->tvItems->IsUpdating		= false;

	// show
    if (form->ShowModal()!=mrOk) return 0;
    if (bIgnoreExt) select_item = ChangeFileExt(select_item,"");
    return select_item.c_str();
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TfrmChoseItem::SelectObject(bool bMulti, LPCSTR start_folder, LPCSTR start_name){
	VERIFY(!form);
	form 							= new TfrmChoseItem(0);
	form->Mode 						= smObject;
    form->bMultiSel 				= bMulti;
    form->iMultiSelLimit 			= 32;
	// init
	if (start_name) m_LastSelection[form->Mode] = start_name;
	form->tvItems->IsUpdating		= true;
    form->tvItems->Selected 		= 0;
    form->tvItems->Items->Clear		();
    // fill object list
	AnsiString fld;
    FileMap& lst = Lib.Objects();
    FilePairIt it=lst.begin();
    FilePairIt _E=lst.end();   		// check without extension
    for (; it!=_E; it++)
		if (!start_folder||(start_folder&&(stricmp(start_folder,FOLDER::GetFolderName(it->first.c_str(),fld))==0))){
        	TElTreeItem* node=FOLDER::AppendObject(form->tvItems,it->first.c_str());
            node->CheckBoxEnabled 	= bMulti;
            node->ShowCheckBox 		= bMulti;
        }
    // redraw
	form->tvItems->IsUpdating		= false;
	// show
    if (form->ShowModal()!=mrOk) return 0;
    return select_item.c_str();
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TfrmChoseItem::SelectShader(LPCSTR init_name){
	VERIFY(!form);
	form = new TfrmChoseItem(0);
	form->Mode = smShader;
	// init
	if (init_name) m_LastSelection[form->Mode] = init_name;
	form->tvItems->IsUpdating	= true;
    form->tvItems->Selected 	= 0;
    form->tvItems->Items->Clear	();
    // fill shaders list
    CShaderManager::BlenderMap& blenders = Device.Shader._GetBlenders();
	CShaderManager::BlenderPairIt _F = blenders.begin();
	CShaderManager::BlenderPairIt _E = blenders.end();
	for (CShaderManager::BlenderPairIt _S = _F; _S!=_E; _S++)
		FOLDER::AppendObject(form->tvItems,_S->first);
    // redraw
	form->tvItems->IsUpdating		= false;
	// show
    if (form->ShowModal()!=mrOk) return 0;
    return select_item.c_str();
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TfrmChoseItem::SelectShaderXRLC(LPCSTR init_name){
	VERIFY(!form);
	form = new TfrmChoseItem(0);
	form->Mode = smShaderXRLC;
	// init
	if (init_name) m_LastSelection[form->Mode] = init_name;
	form->tvItems->IsUpdating	= true;
    form->tvItems->Selected 	= 0;
    form->tvItems->Items->Clear	();
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
	if (init_name) m_LastSelection[form->Mode] = init_name;
	form->tvItems->IsUpdating	= true;
    form->tvItems->Selected 	= 0;
    form->tvItems->Items->Clear	();
	AnsiString fld;
    // fill
    for (PS::SDef* S=PSLib.FirstPS(); S!=PSLib.LastPS(); S++){
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
LPCSTR __fastcall TfrmChoseItem::SelectTexture(bool msel, LPCSTR init_name, bool bIgnoreExt){
	VERIFY(!form);
	form = new TfrmChoseItem(0);
	form->Mode = smTexture;
    form->bMultiSel = msel;
    form->iMultiSelLimit = 8;
    form->bIgnoreExt = bIgnoreExt;
	// init
	if (init_name) m_LastSelection[form->Mode] = init_name;
	form->tvItems->IsUpdating	= true;
    form->tvItems->Selected 	= 0;
    form->tvItems->Items->Clear	();
    // fill
    FileMap lst;
    if (ImageManager.GetTextures(lst)){
	    FilePairIt it=lst.begin();
    	FilePairIt _E=lst.end();   // check without extension
	    for (; it!=_E; it++)
        	FOLDER::AppendObject(form->tvItems,it->first.c_str());
    }
    // redraw
	form->tvItems->IsUpdating		= false;

	// show
    if (form->ShowModal()!=mrOk) return 0;
    if (bIgnoreExt) select_item = ChangeFileExt(select_item,"");
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
    bMultiSel = false;
    tvItems->ShowCheckboxes = false;
	DEFINE_INI(fsStorage);
    bIgnoreExt = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseItem::sbSelectClick(TObject *Sender)
{
	if (bMultiSel){
        select_item = "";
        AnsiString nm;
	    for ( TElTreeItem* node = tvMulti->Items->GetFirstNode(); node; node = node->GetNext()){
    	    FOLDER::MakeName(node,0,nm,false);
            select_item += nm+AnsiString(",");
        }
        select_item.Delete(select_item.Length(),1);

        if (select_item.IsEmpty()){
            if (tvItems->Selected&&FOLDER::IsObject(tvItems->Selected)){
	    	    FOLDER::MakeName(tvItems->Selected,0,select_item,false);
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
    tvItems->ShowCheckboxes 	= bMultiSel;
	int itm_cnt = _GetItemCount(m_LastSelection[form->Mode].c_str());
	if (bMultiSel&&(itm_cnt>1)){
	    char T[MAX_OBJ_NAME];
        for (int i=0; i<itm_cnt; i++){
            TElTreeItem* itm_node = FOLDER::FindObject(tvItems,_GetItem(m_LastSelection[form->Mode].LowerCase().c_str(),i,T),0,0,bIgnoreExt);
	        TElTreeItem* fld_node = 0;
            if (itm_node){
				tvMulti->Items->AddObject(0,_GetItem(m_LastSelection[form->Mode].c_str(),i,T),(void*)FOLDER::TYPE_OBJECT);
            	itm_node->Checked = true;
                tvItems->EnsureVisible(itm_node);
                fld_node=itm_node->Parent;
                if (fld_node) fld_node->Expand(false);
            }
        }
    }else{
        TElTreeItem* itm_node = FOLDER::FindObject(tvItems,m_LastSelection[form->Mode].LowerCase().c_str(),0,0,bIgnoreExt);
        TElTreeItem* fld_node = 0;
        if (itm_node){
        	if (bMultiSel){
				tvMulti->Items->AddObject(0,itm_node->Text,(void*)FOLDER::TYPE_OBJECT);
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
    if (tvItems->Selected) m_LastSelection[form->Mode]=tvItems->Selected->Text;
	_DELETE(m_Thm);
	Action = caFree;
    form = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::pbImagePaint(TObject *Sender)
{
    if (m_Thm) m_Thm->Draw(paImage,pbImage,true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmChoseItem::tvItemsDblClick(TObject *Sender)
{
	if (!bMultiSel&&FOLDER::IsObject(tvItems->Selected)) sbSelectClick(Sender);
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
        FOLDER::MakeName(Item,0,fn,false);
	    TElTreeItem *node = tvMulti->Items->LookForItem(0,fn.c_str(),0,0,false,true,false,true,true);
        if (node&&!Item->Checked) node->Delete();
        if (!node&&Item->Checked) tvMulti->Items->AddObject(0,fn,(void*)FOLDER::TYPE_OBJECT);
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
            Engine.FS.m_Objects.Update(fn);
            if (Engine.FS.Exist(fn.c_str())){
	    	    m_Thm 					= new EImageThumbnail(nm.c_str(),EImageThumbnail::EITObject);
    	        if (!m_Thm->Valid()) 	pbImage->Repaint();
        	    else				 	pbImagePaint(Sender);
            }else						pbImage->Repaint();
			lbItemName->Caption 	= "\""+Item->Text+"\"";
			lbFileName->Caption		= "\""+Item->Text+".object\"";
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

