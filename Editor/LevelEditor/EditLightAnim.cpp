//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditLightAnim.h"
#include "Scene.h"
#include "UI_Main.h"
#include "FolderLib.h"
#include "LightAnimLibrary.h"
#include "ColorPicker.h"
//---------------------------------------------------------------------------
#pragma link "multi_edit"
#pragma link "Gradient"
#pragma link "ElTrackBar"
#pragma resource "*.dfm"

TfrmEditLightAnim* TfrmEditLightAnim::form=0;
AnsiString TfrmEditLightAnim::m_LastSelection;

//---------------------------------------------------------------------------
__fastcall TfrmEditLightAnim::TfrmEditLightAnim(TComponent* Owner)
    : TForm(Owner)
{
    DEFINE_INI(fsStorage);
    m_CurrentItem = 0;
	bFreezeUpdate = false;
}
//---------------------------------------------------------------------------
void TfrmEditLightAnim::OnModified()
{
	ebSave->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLightAnim::ShowEditor()
{
	if (!form){
    	form = new TfrmEditLightAnim(0);
		Scene.lock();
    }
    form->Show();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLightAnim::FormShow(TObject *Sender)
{
    ebSave->Enabled = false;
    UI.BeginEState(esEditLightAnim);

    if (!m_LastSelection.IsEmpty()){
    	TElTreeItem *node=FOLDER::FindObject(tvItems,m_LastSelection.c_str());
	    if (node){
    	    tvItems->Selected = node;
        	tvItems->EnsureVisible(node);
	    }
    }

    InitItems();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLightAnim::FormClose(TObject *Sender, TCloseAction &Action)
{
	form = 0;
	Action = caFree;
	Scene.unlock();
    UI.EndEState(esEditLightAnim);
}
//---------------------------------------------------------------------------
bool TfrmEditLightAnim::CloseEditLibrary(bool bReload){
    if (ebSave->Enabled){
    	int res = ELog.DlgMsg(mtConfirmation, "Library was change. Do you want save?");
		if (res==mrCancel) return false;
		if (res==mrYes) ebSaveClick(0);
    }
    if (bReload)
    	LALib.Reload();

    Close();
    return true;
}
//---------------------------------------------------------------------------
bool TfrmEditLightAnim::FinalClose(){
	if (!form) return true;
	return form->CloseEditLibrary(false);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::tvItemsItemFocused(TObject *Sender)
{
	if (bFreezeUpdate) return;
    
    TElTreeItem* node = tvItems->Selected;

    if (node&&FOLDER::IsObject(node)){
        // change thm
        AnsiString nm,obj_fn,thm_fn;
        FOLDER::MakeName		(node,0,nm,false);
		CLAItem* I  			= LALib.FindItem(nm.c_str());
        SetCurrentItem			(I);
    }else{
        SetCurrentItem			(0);
    }
}

//---------------------------------------------------------------------------
void TfrmEditLightAnim::InitItems()
{
	tvItems->IsUpdating		= true;
    tvItems->Items->Clear();
    for (ELightAnimLibrary::LAItemIt it=LALib.Items.begin(); it!=LALib.Items.end(); it++)
        FOLDER::AppendObject(tvItems,(*it)->cName);
	tvItems->IsUpdating		= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) 	Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebCancelClick(TObject *Sender)
{
    CloseEditLibrary(ebSave->Enabled);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::tvItemsDblClick(TObject *Sender)
{
//	ebPropertiesClick(Sender);
}
//---------------------------------------------------------------------------

extern bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails);
void __fastcall TfrmEditLightAnim::tvItemsKeyPress(TObject *Sender, char &Key){
	TElTreeItem* node = tvItems->Items->LookForItemEx(tvItems->Selected,-1,false,false,false,&Key,LookupFunc);
    if (!node) node = tvItems->Items->LookForItemEx(0,-1,false,false,false,&Key,LookupFunc);
    if (node){
    	tvItems->Selected = node;
		tvItems->EnsureVisible(node);
    }
}
//---------------------------------------------------------------------------
void TfrmEditLightAnim::UpdateView()
{
	if (m_CurrentItem){
        stStartFrame->Caption = 0;
        stEndFrame->Caption = m_CurrentItem->iFrameCount-1;
        sePointer->MaxValue	= m_CurrentItem->iFrameCount-1;
        sePointer->Enabled	= m_CurrentItem->iFrameCount>1;
        pbG->Repaint();

        stStartFrame->Caption = m_CurrentItem->Keys.size();
    }
}
//---------------------------------------------------------------------------

void TfrmEditLightAnim::GetItemData()
{
	if (m_CurrentItem){
		bFreezeUpdate = true;
		edName->Text 		= m_CurrentItem->cName;
        seFPS->Value 		= m_CurrentItem->fFPS;
        seFrameCount->Value = m_CurrentItem->iFrameCount;
        sePointer->Value	= 0;
		bFreezeUpdate = false;
        UpdateView			();
    }
}
//---------------------------------------------------------------------------

void TfrmEditLightAnim::SetCurrentItem(CLAItem* I)
{
	m_CurrentItem = I;
    if (m_CurrentItem) 	tvItems->Selected = FOLDER::FindObject(tvItems,m_CurrentItem->cName);
    else				tvItems->Selected = 0;
    paItemProps->Visible = tvItems->Selected;
    // fill data
    GetItemData();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebAddAnimClick(TObject *Sender)
{
    AnsiString folder;
	FOLDER::MakeName(tvItems->Selected,0,folder,true);
    CLAItem* I = LALib.AppendItem(folder.c_str(),0);
    FOLDER::AppendObject(tvItems,I->cName);
    SetCurrentItem(I);
    OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebDeleteAnimClick(TObject *Sender)
{
//
    OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebSaveClick(TObject *Sender)
{
	ebSave->Enabled = false;
	LALib.Save();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebReloadClick(TObject *Sender)
{
	ebSave->Enabled = false;
	LALib.Reload();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::pbGPaint(TObject *Sender)
{
	TRect R;
	pbG->Canvas->Brush->Style 	= bsSolid;
    pbG->Canvas->Brush->Color	= 0x00000000;
	R = pbG->ClientRect;
    pbG->Canvas->FrameRect(R);
    R.Right	-=	1;
    R.Left 	+= 	1;
    R.Top	+=	1;
    R.Bottom-= 	1;
    pbG->Canvas->Brush->Color	= 0x00707070;
    pbG->Canvas->FillRect(R);
    if (m_CurrentItem){
        // draw gradient
        CLAItem::KeyPairIt prev_key=m_CurrentItem->Keys.begin();
        CLAItem::KeyPairIt it=prev_key; it++;
        int x_prev=iFloor((float(prev_key->first)/float(m_CurrentItem->iFrameCount-1))*R.Width());
		TRect cb;
        cb.Top 		= R.Top;
        cb.Bottom	= R.Bottom;
	    for (; it!=m_CurrentItem->Keys.end(); it++){
		    int x		= iFloor((it->first/float(m_CurrentItem->iFrameCount-1))*R.Width());
        	float g_cnt	= it->first-prev_key->first;
            float w		= x-x_prev;
            for (int k=1; k<g_cnt; k++){
	            cb.Left		= x_prev+(k-1)*(w/g_cnt);
    	        cb.Right	= x+k*(w/g_cnt);
			    pbG->Canvas->Brush->Color	= m_CurrentItem->Interpolate(prev_key->first+k);
		    	pbG->Canvas->FillRect(cb);
            }
            prev_key 	= it;
            x_prev		= x;
        }
        // draw keys
/*	    pbG->Canvas->Pen->Color= 0x00BFFFFF;
	    for (it=m_CurrentItem->Keys.begin(); it!=m_CurrentItem->Keys.end(); it++){
		    int t=iFloor((it->first/float(m_CurrentItem->iFrameCount-1))*R.Width());
	    	pbG->Canvas->MoveTo(t,R.Bottom);
    		pbG->Canvas->LineTo(t,R.Top);
        }
*/	    // draw pointer
	    int t=iFloor((sePointer->Value/float(m_CurrentItem->iFrameCount-1))*R.Width());
	    pbG->Canvas->Pen->Color= 0x0000FF00;
    	pbG->Canvas->MoveTo(t,R.Bottom);
    	pbG->Canvas->LineTo(t,R.Top+R.Height()*0.75f);
        TRect rp=R;
        rp.Left 	= t-2;
        rp.Right 	= t+3;
        rp.Top		= R.Top;
        rp.Bottom	= R.Bottom-R.Height()*0.3f;
	    pbG->Canvas->Brush->Color	= 0x00000000;
	    pbG->Canvas->FrameRect(rp);
        //
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditLightAnim::pbGClick(TObject *Sender)
{
	DWORD color;
	if (SelectColorWin(&color)){
    	m_CurrentItem->InsertKey(sePointer->Value,color);
	    UpdateView();
    	OnModified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::seFrameCountExit(TObject *Sender)
{
	if (bFreezeUpdate) return;
	m_CurrentItem->Resize(seFrameCount->Value);
    UpdateView();
    OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::seFrameCountLWChange(TObject *Sender,
      int Val)
{
	if (bFreezeUpdate) return;
	m_CurrentItem->Resize(seFrameCount->Value);
    UpdateView();
    OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::seFrameCountKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (bFreezeUpdate) return;
	m_CurrentItem->Resize(seFrameCount->Value);
    UpdateView();
    OnModified();
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditLightAnim::sePointerChange(TObject *Sender)
{
	pbG->Repaint();
}
//---------------------------------------------------------------------------

