//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditLightAnim.h"
#include "Scene.h"
#include "FolderLib.h"
#include "LightAnimLibrary.h"
#include "ColorPicker.h"
//---------------------------------------------------------------------------
#pragma link "multi_edit"
#pragma link "Gradient"
#pragma link "ElTrackBar"
#pragma link "ElTreeAdvEdit"
#pragma link "MxMenus"
#pragma link "MXCtrls"
#pragma resource "*.dfm"

TfrmEditLightAnim* TfrmEditLightAnim::form=0;
AnsiString TfrmEditLightAnim::m_LastSelection;

//---------------------------------------------------------------------------
__fastcall TfrmEditLightAnim::TfrmEditLightAnim(TComponent* Owner)
    : TForm(Owner)
{
    DEFINE_INI(fsStorage);
    bFinalClose		= false;
    m_CurrentItem 	= 0;
	bFreezeUpdate 	= false;
    iMoveKey        = -1;
    InplaceTextEdit->Editor->Color			= TColor(0x00A0A0A0);
    InplaceTextEdit->Editor->BorderStyle	= bsNone;
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

   	if (ebSave->Enabled&&!bFinalClose)
    	LALib.Reload();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLightAnim::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
	if (!bFinalClose) 	CanClose	= IsClose();
    else                CanClose	= true;
}
//---------------------------------------------------------------------------
bool TfrmEditLightAnim::IsClose(){
    if (ebSave->Enabled){
    	int res = ELog.DlgMsg(mtConfirmation, "Library was change. Do you want save?");
		if (res==mrCancel) return false;
		if (res==mrYes) ebSaveClick(0);
    }
    return true;
}
//---------------------------------------------------------------------------
bool TfrmEditLightAnim::FinalClose(){
	if (!form) return true;
	if (form->IsClose()){
	    form->bFinalClose=true;
		form->Close();
        return true;
    }
    return false;
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
        if (sePointer->Value>sePointer->MaxValue) sePointer->Value = sePointer->MaxValue;
        sePointer->Enabled	= m_CurrentItem->iFrameCount>1;
        pbG->Repaint();

//        stStartFrame->Caption = m_CurrentItem->Keys.size();
		sePointer->Color = TColor(m_CurrentItem->IsKey(sePointer->Value)?0x00BFFFFF:0x00A0A0A0);
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
	TElTreeItem* node = tvItems->Selected;
    if (node){
	    AnsiString name;
		FOLDER::MakeName(node,0,name,false);
        if (node->GetPrev()) 		tvItems->Selected = node->GetPrev();
        else if (node->GetNext())	tvItems->Selected = node->GetNext();
        else						tvItems->Selected = 0;
		LALib.DeleteItem(name.c_str());
        node->Delete();
	    OnModified();
    }else{
    	ELog.DlgMsg(mtError,"Select item at first");
    }
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
	m_CurrentItem = 0;
	LALib.Reload();
    InitItems();
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditLightAnim::pbGMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Shift.Contains(ssLeft)){
		TRect R 	= pbG->ClientRect;
		TPoint pt(X,Y);
		pbG->ScreenToClient(pt);
        sePointer->Value = iFloor(float(m_CurrentItem->iFrameCount)*float(X)/float(R.Width()));
        pbG->Repaint();
	    if (Shift.Contains(ssDouble))
        	ebCreateKeyClick(Sender);
        else{
        	if (m_CurrentItem->IsKey(sePointer->Value)&&(sePointer->Value!=0)){
            	iMoveKey 	= sePointer->Value;
                iTgtMoveKey = iMoveKey;
            }
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::pbGMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    if (Shift.Contains(ssLeft))
        if (iMoveKey>=0){
            TRect R 	= pbG->ClientRect;
            TPoint pt(X,Y);
            pbG->ScreenToClient(pt);
            int new_key = iFloor(float(m_CurrentItem->iFrameCount)*float(X)/float(R.Width()));
            if ((new_key!=iTgtMoveKey)&&(new_key<m_CurrentItem->iFrameCount)&&(!m_CurrentItem->IsKey(new_key)||(new_key==iMoveKey))){
				iTgtMoveKey = new_key;
            	pbG->Repaint();
            }
        }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::pbGMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (iMoveKey>=0){
    	if (iTgtMoveKey!=iMoveKey){
        	m_CurrentItem->MoveKey(iMoveKey,iTgtMoveKey);
	        sePointer->Value = iTgtMoveKey;
            pbG->Repaint();
            OnModified();
        }
		iMoveKey = -1;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebDeleteKeyClick(TObject *Sender)
{
	m_CurrentItem->DeleteKey(sePointer->Value);
    UpdateView();
    OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebCreateKeyClick(TObject *Sender)
{
    DWORD color=m_CurrentItem->Interpolate(sePointer->Value);
    if (SelectColorWin(&color)){
        m_CurrentItem->InsertKey(sePointer->Value,color);
        UpdateView();
        OnModified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::pbGPaint(TObject *Sender)
{
	TRect R 	= pbG->ClientRect;
    Graphics::TBitmap* B 	= new Graphics::TBitmap();
    B->Width 	= R.Width()-2;
    B->Height	= R.Height()-2;
	pbG->Canvas->Brush->Style 	= bsSolid;
    pbG->Canvas->Brush->Color	= clBlack;
    pbG->Canvas->FrameRect		(R);
    R.right		-= 2;
    R.bottom	-= 2;
    B->Canvas->Brush->Color		= TColor(0x00707070);
    B->Canvas->FillRect(R);
    if (m_CurrentItem){
    	float segment 	= float(R.Width())/(float(m_CurrentItem->iFrameCount));
        int half_segment= iFloor(segment/2);
        // draw gradient
        CLAItem::KeyMap& Keys=m_CurrentItem->Keys;
        int last=m_CurrentItem->iFrameCount;
        Keys[last]=Keys.rbegin()->second;
        CLAItem::KeyPairIt prev_key=Keys.begin();
        CLAItem::KeyPairIt it=prev_key; it++;
        float x_prev=(float(prev_key->first)/float(m_CurrentItem->iFrameCount))*R.Width();
		TRect cb;
        cb.Top 		= R.Top;
        cb.Bottom	= R.Bottom-R.Height()*0.3f;
	    for (; it!=Keys.end(); it++){
		    float x		= (it->first/float(m_CurrentItem->iFrameCount))*R.Width();
        	float g_cnt	= it->first-prev_key->first;
            for (int k=0; k<g_cnt; k++){
	            cb.Left		= iFloor(x_prev+k*segment);
    	        cb.Right	= iFloor(x_prev+k*segment+segment+1);
			    B->Canvas->Brush->Color	= TColor(m_CurrentItem->Interpolate(prev_key->first+k));
		    	B->Canvas->FillRect(cb);
            }
            prev_key 	= it;
            x_prev		= x;
        }
        Keys.erase(Keys.find(last));
        // draw keys
	    B->Canvas->Brush->Color= TColor(0x00BFFFFF);
        cb.Top 		= R.Bottom-R.Height()*0.1f;
        cb.Bottom	= R.Bottom;
	    for (it=m_CurrentItem->Keys.begin(); it!=m_CurrentItem->Keys.end(); it++){
		    int t		= iFloor((it->first/float(m_CurrentItem->iFrameCount))*R.Width());
            cb.Left		= t-1+half_segment;
   	        cb.Right	= t+2+half_segment;
			B->Canvas->FillRect(cb);
        }
	    // draw pointer
	    int t=iFloor((sePointer->Value/float(m_CurrentItem->iFrameCount))*R.Width())+half_segment;
	    B->Canvas->Pen->Color= TColor(0x0000FF00);
    	B->Canvas->MoveTo(t,R.Bottom);
    	B->Canvas->LineTo(t,R.Top+R.Height()*0.75f);
	    if ((iMoveKey>=0)&&(iTgtMoveKey!=iMoveKey))
        	t=iFloor((iTgtMoveKey/float(m_CurrentItem->iFrameCount))*R.Width())+half_segment;
        TRect rp=R;
        rp.Left 	= t-2;
        rp.Right 	= t+3;
        rp.Top		= R.Top+1;
        rp.Bottom	= R.Bottom-R.Height()*0.3f-1;
	    B->Canvas->Brush->Color	= TColor(0x00AAAAAA);
	    B->Canvas->FrameRect(rp);
        rp.Left 	-=1;
        rp.Right 	+=1;
        rp.Top		-=1;
        rp.Bottom	+=1;
	    B->Canvas->Brush->Color	= TColor(0x00000000);
	    B->Canvas->FrameRect(rp);
        //
    }
	pbG->Canvas->Draw(1,1,B);
    _DELETE(B);
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
    if (Key==VK_RETURN){
		m_CurrentItem->Resize(seFrameCount->Value);
    	UpdateView();
	    OnModified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::sePointerKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (Key==VK_RETURN) pbG->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::sePointerLWChange(TObject *Sender,
      int Val)
{
	pbG->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::sePointerExit(TObject *Sender)
{
	pbG->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::tvItemsDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	FOLDER::DragDrop(Sender,Source,X,Y,OnRenameItem);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::tvItemsDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	FOLDER::DragOver(Sender,Source,X,Y,State,Accept);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::tvItemsStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
	FOLDER::StartDrag(Sender,DragObject);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::OnRenameItem(LPCSTR p0, LPCSTR p1)
{
	LALib.RenameItem(p0,p1);
	OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::tvItemsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button==mbRight)	FOLDER::ShowPPMenu(pmActionList);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::Rename1Click(TObject *Sender)
{
	TElTreeItem* node = tvItems->Selected;
    if (node) tvItems->EditItem(node,-1);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::InplaceTextEditValidateResult(
      TObject *Sender, bool &InputValid)
{
	TElTreeInplaceAdvancedEdit* IE=0;
    IE=InplaceTextEdit;

    AnsiString new_text = AnsiString(IE->Editor->Text).LowerCase();
    IE->Editor->Text = new_text;

    TElTreeItem* node = IE->Item;
    for (TElTreeItem* item=node->GetFirstSibling(); item; item=item->GetNextSibling()){
        if ((item->Text==new_text)&&(item!=IE->Item)){
            InputValid = false;
            return;
        }
    }
    AnsiString full_name;
    if (FOLDER::IsFolder(node)){
        for (item=node->GetFirstChild(); item&&(item->Level>node->Level); item=item->GetNext()){
            if (FOLDER::IsObject(item)){
                FOLDER::MakeName(item,0,full_name,false);
                string256 new_nm;
                FOLDER::ReplacePart(full_name.c_str(),new_text.c_str(),node->Level,new_nm);
                LALib.RenameItem(full_name.c_str(),new_nm);
            }
        }
    }else if (FOLDER::IsObject(node)){
        FOLDER::MakeName(node,0,full_name,false);
        string256 new_nm;
        FOLDER::ReplacePart(full_name.c_str(),new_text.c_str(),node->Level,new_nm);
		LALib.RenameItem(full_name.c_str(),new_nm);
    }
    tvItems->Selected=node;
	OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::CreateFolder1Click(TObject *Sender)
{
	AnsiString folder;
    AnsiString start_folder;
    FOLDER::MakeName(tvItems->Selected,0,start_folder,true);
    FOLDER::GenerateFolderName(tvItems,tvItems->Selected,folder);
    folder = start_folder+folder;
	TElTreeItem* node = FOLDER::AppendFolder(tvItems,folder.c_str());
    if (tvItems->Selected) tvItems->Selected->Expand(false);
    tvItems->EditItem(node,-1);
	OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ExpandAll1Click(TObject *Sender)
{
	tvItems->FullExpand();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::CollapseAll1Click(TObject *Sender)
{
	tvItems->FullCollapse();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::seFPSLWChange(TObject *Sender, int Val)
{
	if (bFreezeUpdate) return;
	m_CurrentItem->fFPS = seFPS->Value;
    UpdateView();
    OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::seFPSKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (bFreezeUpdate) return;
    if (Key==VK_RETURN){
		m_CurrentItem->fFPS = seFPS->Value;
    	UpdateView();
	    OnModified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::seFPSExit(TObject *Sender)
{
	if (bFreezeUpdate) return;
	m_CurrentItem->fFPS = seFPS->Value;
    UpdateView();
    OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::OnIdle(){
	if (form){
		if (form->m_CurrentItem){
        	int frame;
			form->paColor->Color=TColor(form->m_CurrentItem->Calculate(Device.fTimeGlobal,frame));
            form->lbCurFrame->Caption=frame;
        }
    }
}

void __fastcall TfrmEditLightAnim::sePointerChange(TObject *Sender)
{
	sePointer->Color = TColor(m_CurrentItem->IsKey(sePointer->Value)?0x00BFFFFF:0x00A0A0A0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebFirstKeyClick(TObject *Sender)
{
	sePointer->Value	= m_CurrentItem->FirstKeyFrame();
	pbG->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebPrevKeyClick(TObject *Sender)
{
	sePointer->Value	= m_CurrentItem->PrevKeyFrame(sePointer->Value);
	pbG->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebNextKeyClick(TObject *Sender)
{
	sePointer->Value	= m_CurrentItem->NextKeyFrame(sePointer->Value);
	pbG->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebLastKeyClick(TObject *Sender)
{
	sePointer->Value	= m_CurrentItem->LastKeyFrame();
	pbG->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebMoveKeyLeftClick(TObject *Sender)
{
	if ((sePointer->Value!=0)&&(m_CurrentItem->IsKey(sePointer->Value))){
    	int f0, f1;
    	f1 = f0 = sePointer->Value;
        f1--;
        if (f1>=0){
	        m_CurrentItem->MoveKey(f0,f1);
    	    sePointer->Value++;
        	pbG->Repaint();
	        OnModified();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebMoveKeyRightClick(TObject *Sender)
{
	if ((sePointer->Value!=0)&&(m_CurrentItem->IsKey(sePointer->Value))){
    	int f0, f1;
    	f1 = f0 = sePointer->Value;
        f1++;
        if (f1<m_CurrentItem->iFrameCount){
	        m_CurrentItem->MoveKey(f0,f1);
    	    sePointer->Value++;
        	pbG->Repaint();
	        OnModified();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebLastFrameClick(TObject *Sender)
{
	sePointer->Value	= m_CurrentItem->iFrameCount-1;
	pbG->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLightAnim::ebFirstFrameClick(TObject *Sender)
{
	sePointer->Value	= 0;
	pbG->Repaint();
}
//---------------------------------------------------------------------------


