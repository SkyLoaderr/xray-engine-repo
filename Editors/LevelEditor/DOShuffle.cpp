//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "DOShuffle.h"
#include "ChoseForm.h"
#include "ImageThumbnail.h"
#include "xr_trims.h"
#include "Library.h"
#include "DOOneColor.h"
#include "EditObject.h"
#include "Scene.h"
#include "folderlib.h"
#include "DetailObjects.h"
#include "PropertiesDetailObject.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
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
	bColorIndModif = false;
    GetInfo();
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------

void TfrmDOShuffle::GetInfo(){
	// init
    tvItems->IsUpdating = true;
    tvItems->Selected = 0;
    tvItems->Items->Clear();
    // fill
    EDetailManager* DM=Scene.m_DetailObjects;
    VERIFY(DM);
    // objects
    for (DOIt d_it=DM->objects.begin(); d_it!=DM->objects.end(); d_it++){
    	SDOData* dd 			= new SDOData;
        dd->m_RefName			= (*d_it)->GetName();
        dd->m_fMinScale 		= (*d_it)->s_min;
        dd->m_fMaxScale 		= (*d_it)->s_max;
        dd->m_fDensityFactor 	= (*d_it)->m_fDensityFactor;
        dd->m_dwFlags			= (*d_it)->flags;
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
        OneColor->mcColor->Brush->Color = (TColor)rgb2bgr(it->first);
        for (d_it=it->second.begin(); d_it!=it->second.end(); d_it++)
	        OneColor->AppendObject((*d_it)->GetName());
    }
    // redraw
    tvItems->IsUpdating = false;
}
//---------------------------------------------------------------------------

void TfrmDOShuffle::ApplyInfo(){
    EDetailManager* DM=Scene.m_DetailObjects;
    VERIFY(DM);
    bool bNeedUpdate = false;
    // update objects
    DM->MarkAllObjectsAsDel();
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext()){
    	CDetail* DO = DM->FindObjectByName(AnsiString(node->Text).c_str());
    	if (DO)	DO->m_bMarkDel = false;
        else{
        	DO = DM->AppendObject(AnsiString(node->Text).c_str());
            bNeedUpdate = true;
        }
        // update data
        SDOData* DD 		= (SDOData*)node->Data;
        DO->s_min		 	= DD->m_fMinScale;
        DO->s_max		 	= DD->m_fMaxScale;
        DO->m_fDensityFactor= DD->m_fDensityFactor;
        DO->flags			= DD->m_dwFlags;
    }
    if (DM->RemoveObjects(true)) bNeedUpdate=true;
	// update indices
	DM->RemoveColorIndices();
	for (DWORD k=0; k<color_indices.size(); k++){
    	TfrmOneColor* OneColor = color_indices[k];
        DWORD clr = bgr2rgb(OneColor->mcColor->Brush->Color);
	    for ( TElTreeItem* node = OneColor->tvDOList->Items->GetFirstNode(); node; node = node->GetNext())
	    	DM->AppendIndexObject(clr,AnsiString(node->Text).c_str(),false);
    }
    if (bNeedUpdate||bColorIndModif){
        ELog.DlgMsg(mtInformation,"Object list changed. Update objects needed!");
        DM->InvalidateSlots();
    }
}
//---------------------------------------------------------------------------

void TfrmDOShuffle::ClearInfo()
{
	for (DDIt it=DOData.begin(); it!=DOData.end(); it++)
		_DELETE(*it);
    DOData.clear();
	for (DWORD k=0; k<color_indices.size(); k++)
    	_DELETE(color_indices[k]);
    color_indices.clear();
    _DELETE(m_Thm);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// implementation
//---------------------------------------------------------------------------
__fastcall TfrmDOShuffle::TfrmDOShuffle(TComponent* Owner)
    : TForm(Owner)
{
    DEFINE_INI(fsStorage);
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmDOShuffle::FindItem(const char* s)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (node->Data && (AnsiString(node->Text) == s)) return node;
    return 0;
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
	ClearInfo();

    if (ModalResult==mrOk)
		Scene.m_DetailObjects->InvalidateCache();
 
	Action = caFree;
    form = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::tvItemsItemFocused(TObject *Sender)
{
	TElTreeItem* Item = tvItems->Selected;
    _DELETE(m_Thm);
	if (Item&&Item->Data){
		AnsiString nm 		= Item->Text;
    	m_Thm 				= new EImageThumbnail(nm.c_str(),EImageThumbnail::EITObject);
        SDOData* dd			= (SDOData*)Item->Data;
		lbItemName->Caption = "\""+dd->m_RefName+"\"";
		AnsiString temp; 	temp.sprintf("Density: %1.2f\nScale: [%3.1f, %3.1f)\nNo waving: %s",dd->m_fDensityFactor,dd->m_fMinScale,dd->m_fMaxScale,(dd->m_dwFlags&DO_NO_WAVING)?"on":"off");
        lbInfo->Caption		= temp;
    }else{
		lbItemName->Caption = "-";
		lbInfo->Caption		= "-";
    }
    if (m_Thm&&m_Thm->Valid())	pbImagePaint(Sender);
    else                        pbImage->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::pbImagePaint(TObject *Sender)
{
	if (m_Thm) m_Thm->Draw(paImage,pbImage,true);
}
//---------------------------------------------------------------------------


bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails){
    char s1 = *(char*)SearchDetails;
    char s2 = *AnsiString(Item->Text).c_str();
	return (s1==tolower(s2));
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::tvItemsKeyPress(TObject *Sender, char &Key)
{
	TElTreeItem* node = tvItems->Items->LookForItemEx(tvItems->Selected,-1,false,false,false,&Key,LookupFunc);
    if (!node) node = tvItems->Items->LookForItemEx(0,-1,false,false,false,&Key,LookupFunc);
    FHelper.RestoreSelection(tvItems,node);
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
	LPCSTR S;
    if (TfrmChoseItem::SelectItem(TfrmChoseItem::smObject,S,8)){
	    AStringVec lst;
		SequenceToList(lst, S);
        for (AStringIt s_it=lst.begin(); s_it!=lst.end(); s_it++)
        	if (!FindItem(s_it->c_str())){
                if (tvItems->Items->Count>=dm_max_objects){
                    ELog.DlgMsg(mtInformation,"Maximum detail objects in scene '%d'",dm_max_objects);
                    return;
                }
                SDOData* dd 		= new SDOData;
                dd->m_RefName 		= s_it->c_str();
                dd->m_fMinScale 	= 0.5f;
                dd->m_fMaxScale 	= 2.f;
                dd->m_fDensityFactor= 1.f;
                dd->m_dwFlags		= 0;
             	AddItem(0,s_it->c_str(),(void*)dd);
            }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::ebDelObjectClick(TObject *Sender)
{
	if (tvItems->Selected){
		ModifColorInd();
		for (DWORD k=0; k<color_indices.size(); k++)
    		color_indices[k]->RemoveObject(AnsiString(tvItems->Selected->Text).c_str());
        tvItems->Selected->Delete();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::ebAppendIndexClick(TObject *Sender)
{
	ModifColorInd();
	color_indices.push_back(new TfrmOneColor(0));
	color_indices.back()->Parent = sbDO;
    color_indices.back()->ShowIndex(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::RemoveColorIndex(TfrmOneColor* p){
	form->ModifColorInd();
	form->color_indices.erase(find(form->color_indices.begin(),form->color_indices.end(),p));
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::ebMultiClearClick(TObject *Sender)
{
	ModifColorInd();
	for (DWORD k=0; k<color_indices.size(); k++)
    	_DELETE(color_indices[k]);
    color_indices.clear();
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::tvItemsDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	TfrmOneColor* OneColor = (TfrmOneColor*)((TElTree*)Source)->Parent;
    if (OneColor&&OneColor->FDragItem){
    	OneColor->FDragItem->Delete();
		ModifColorInd();
    }
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

void __fastcall TfrmDOShuffle::ebSaveListClick(TObject *Sender)
{
	AnsiString fname;
	if (Engine.FS.GetSaveName(Engine.FS.m_DetailObjects,fname)){
		Scene.m_DetailObjects->ExportColorIndices(fname.c_str());
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::ebLoadListClick(TObject *Sender)
{
	AnsiString fname;
	if (Engine.FS.GetOpenName(Engine.FS.m_DetailObjects,fname)){
        Scene.m_DetailObjects->InvalidateSlots();
		Scene.m_DetailObjects->ImportColorIndices(fname.c_str());
		ClearInfo();
        GetInfo();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmDOShuffle::ebClearListClick(TObject *Sender)
{
	ClearInfo();
}
//---------------------------------------------------------------------------

