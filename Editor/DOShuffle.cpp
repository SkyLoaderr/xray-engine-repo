//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "DOShuffle.h"
#include "FileSystem.h"
#include "ChoseForm.h"
#include "Texture.h"
#include "xr_trims.h"
#include "Library.h"
#include "DOOneColor.h"
#include "Scene.h"
#include "DetailObjects.h"
#include "D3DUtils.h"
#include "PropertiesDetailObject.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Placemnt"
#pragma resource "*.dfm"
TfrmDOShuffle *TfrmDOShuffle::form=0;

static DDVec DOData;

SDOData::SDOData(){
	DOData.push_back(this);
}
//---------------------------------------------------------------------------
// Constructors
//---------------------------------------------------------------------------
bool __fastcall TfrmDOShuffle::Run(){
	VERIFY(!form);
	form = new TfrmDOShuffle(0);
	// show
    return (form->ShowModal()==mrOk);
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::FormShow(TObject *Sender)
{
    GetInfo();
}
//---------------------------------------------------------------------------

void TfrmDOShuffle::GetInfo(){
	// init
    tvItems->IsUpdating = true;
    tvItems->Selected = 0;
    tvItems->Items->Clear();
    // fill
    CDetailManager* DM=Scene->m_DetailObjects;
    VERIFY(DM);
    // objects
    for (DOIt d_it=DM->m_Objects.begin(); d_it!=DM->m_Objects.end(); d_it++){
    	SDOData* dd = new SDOData;
        dd->LO = Lib->SearchObject((*d_it)->GetName());
        VERIFY(dd->LO);
        dd->m_fMinScale 		= (*d_it)->m_fMinScale;
        dd->m_fMaxScale 		= (*d_it)->m_fMaxScale;
        dd->m_fDensityFactor 	= (*d_it)->m_fDensityFactor;
        AddItem(0,(*d_it)->GetName(),(void*)dd);
    }
    // indices
    ColorIndexPairIt S = DM->m_ColorIndices.begin();
    ColorIndexPairIt E = DM->m_ColorIndices.end();
    ColorIndexPairIt it= S;
	for(; it!=E; it++){
    	TfrmOneColor* OneColor = new TfrmOneColor(0);
		color_indices.push_back(OneColor);
		OneColor->Parent = form->sbDO;
	    OneColor->ShowIndex(this);
        OneColor->mcColor->Brush->Color = DU::rgb2bgr(it->first);
        for (d_it=it->second.begin(); d_it!=it->second.end(); d_it++)
	        OneColor->AppendObject((*d_it)->GetName());
    }
    // redraw
    tvItems->IsUpdating = false;
}

void TfrmDOShuffle::ApplyInfo(){
    CDetailManager* DM=Scene->m_DetailObjects;
    VERIFY(DM);
    // update objects
    DM->MarkAllObjectsAsDel();
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext()){
    	CDetail* DO = DM->FindObjectByName(node->Text.c_str());
    	if (DO)	DO->m_bMarkDel = false;
        else	DO = DM->AppendObject(node->Text.c_str());
        // update data
        SDOData* DD = (SDOData*)node->Data;
        DO->m_fMinScale 		= DD->m_fMinScale;
        DO->m_fMaxScale 		= DD->m_fMaxScale;
        DO->m_fDensityFactor 	= DD->m_fDensityFactor;
    }
    DM->RemoveObjects(true);
	// update indices
	DM->RemoveColorIndices();
	for (DWORD k=0; k<color_indices.size(); k++){
    	TfrmOneColor* OneColor = color_indices[k];
        DWORD clr = DU::bgr2rgb(OneColor->mcColor->Brush->Color);
	    for ( TElTreeItem* node = OneColor->tvDOList->Items->GetFirstNode(); node; node = node->GetNext())
	    	DM->AppendIndexObject(clr,node->Text.c_str(),false);
    }
}

//---------------------------------------------------------------------------
// implementation
//---------------------------------------------------------------------------
__fastcall TfrmDOShuffle::TfrmDOShuffle(TComponent* Owner)
    : TForm(Owner)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmDOShuffle::FindFolder(const char* s)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Data && (node->Text == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmDOShuffle::FindItem(const char* s)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (node->Data && (node->Text == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmDOShuffle::AddFolder(const char* s)
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
TElTreeItem* TfrmDOShuffle::AddItem(TElTreeItem* node, const char* name, void* obj)
{
    TElTreeItem* obj_node = tvItems->Items->AddChildObject(node, name, obj);
    obj_node->ParentStyle = false;
    obj_node->Bold = false;
    return obj_node;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmDOShuffle::AddItemToFolder(const char* folder, const char* name, void* obj){
	TElTreeItem* node = FindFolder(folder);
    if (!node) node = AddFolder(folder);
	return AddItem(node,name,obj);
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::ebOkClick(TObject *Sender)
{
	ApplyInfo();
	Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfrmDOShuffle::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfrmDOShuffle::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Key==VK_RETURN) ebOk->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::FormClose(TObject *Sender, TCloseAction &Action)
{
	for (DDIt it=DOData.begin(); it!=DOData.end(); it++)
		_DELETE(*it);
    DOData.clear();
	for (DWORD k=0; k<color_indices.size(); k++)
    	_DELETE(color_indices[k]);
    color_indices.clear();
    _DELETE(sel_thm);

    if (ModalResult==mrOk)
		Scene->m_DetailObjects->InvalidateCache();

	Action = caFree;
    form = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::tvItemsItemSelectedChange(TObject *Sender, TElTreeItem *Item)
{
	if (Item==tvItems->Selected) return;
    _DELETE(sel_thm);
	if (Item&&Item->Data){
		AnsiString nm = AnsiString("$O_")+Item->Text;
    	sel_thm 				= new ETextureThumbnail(nm.c_str());
        if (!sel_thm->Load()) 	pbImage->Repaint();
        else				 	pbImagePaint(Sender);
        SDOData* dd				= (SDOData*)Item->Data;
		lbItemName->Caption 	= "\""+AnsiString(dd->LO->GetName())+"\"";
		AnsiString temp; 		temp.sprintf("Density: %1.2f\nScale: [%3.1f, %3.1f)",dd->m_fDensityFactor,dd->m_fMinScale,dd->m_fMaxScale);
        lbInfo->Caption			= temp;
    }else{
		lbItemName->Caption = "...";
		lbInfo->Caption		= "...";
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::pbImagePaint(TObject *Sender)
{
	if (sel_thm){
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


bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails){
    char s1 = *(char*)SearchDetails;
    char s2 = *Item->Text.c_str();
	return (s1==tolower(s2));
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::tvItemsKeyPress(TObject *Sender, char &Key)
{
	TElTreeItem* node = tvItems->Items->LookForItemEx(tvItems->Selected,-1,false,false,false,&Key,LookupFunc);
    if (!node) node = tvItems->Items->LookForItemEx(0,-1,false,false,false,&Key,LookupFunc);    
    if (node){
    	tvItems->Selected = node;
		tvItems->EnsureVisible(node);
    }
}
//---------------------------------------------------------------------------

static TElTreeItem* DragItem=0;
void __fastcall TfrmDOShuffle::tvMultiDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	TElTreeItem* node = ((TElTree*)Sender)->GetItemAtY(Y);
    if (node)
		DragItem->MoveToIns(0,node->Index);
	DragItem = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::tvMultiDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	Accept = false;
	TElTreeItem* node = ((TElTree*)Sender)->GetItemAtY(Y);
	if ((Sender==Source)&&(node!=DragItem)) Accept=true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::tvMultiStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
	DragItem = ((TElTree*)Sender)->Selected;
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::ebAddObjectClick(TObject *Sender)
{
	LPCSTR S = TfrmChoseItem::SelectObject(true,true,0,0);
    if (S){
	    AStringVec lst;
		SequenceToList(lst, S);
        for (AStringIt s_it=lst.begin(); s_it!=lst.end(); s_it++)
        	if (!FindItem(s_it->c_str())){
                SDOData* dd = new SDOData;
                dd->LO = Lib->SearchObject(s_it->c_str());
                VERIFY(dd->LO);
                dd->m_fMinScale 		= 0.5f;
                dd->m_fMaxScale 		= 2.f;
                dd->m_fDensityFactor 	= 1.f;
             	AddItem(0,s_it->c_str(),(void*)dd);
            }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::ebDelObjectClick(TObject *Sender)
{
	if (tvItems->Selected){
		for (DWORD k=0; k<color_indices.size(); k++)
    		color_indices[k]->RemoveObject(tvItems->Selected->Text);
        tvItems->Selected->Delete();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::ebAppendIndexClick(TObject *Sender)
{
	color_indices.push_back(new TfrmOneColor(0));
	color_indices.back()->Parent = sbDO;
    color_indices.back()->ShowIndex(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::RemoveColorIndex(TfrmOneColor* p){
	form->color_indices.erase(find(form->color_indices.begin(),form->color_indices.end(),p));
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::ebMultiClearClick(TObject *Sender)
{
	for (DWORD k=0; k<color_indices.size(); k++)
    	_DELETE(color_indices[k]);
    color_indices.clear();
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::tvItemsDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	TfrmOneColor* OneColor = (TfrmOneColor*)((TElTree*)Source)->Parent;
    if (OneColor&&OneColor->FDragItem) OneColor->FDragItem->Delete();
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::tvItemsDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
    Accept = false;
    if (Source == tvItems) return;
    TElTreeItem* node;
    Accept = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::tvItemsStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
    FDragItem = tvItems->ItemFocused;
}
//---------------------------------------------------------------------------


void __fastcall TfrmDOShuffle::ebDOPropertiesClick(TObject *Sender)
{
	if (tvItems->Selected&&tvItems->Selected->Data){
		bool bChange;
		DDVec lst;
	    lst.push_back(((SDOData*)tvItems->Selected->Data));
        TElTreeItem* node = tvItems->Selected;
		if (frmPropertiesSectorRun(lst,bChange)==mrOk){
			tvItems->Selected = 0;
			tvItems->Selected = node;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::tvItemsDblClick(TObject *Sender)
{
	ebDOProperties->Click();
}
//---------------------------------------------------------------------------

