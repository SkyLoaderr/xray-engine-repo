#include "stdafx.h"
#pragma hdrstop

#include "PropertiesSceneObject.h"
#include "SceneClassList.h"
#include "SceneObject.h"
#include "Scene.h"
#include "ChoseForm.h"
#include "FolderLib.h"
#include "motion.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MXCtrls"
#pragma link "ElPgCtl"
#pragma link "ElXPThemedControl"
#pragma link "ElTree"
#pragma link "mxPlacemnt"
#pragma link "ElTreeAdvEdit"
#pragma link "MxMenus"
#pragma resource "*.dfm"

TfrmPropertiesSceneObject* TfrmPropertiesSceneObject::form = 0;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesSceneObject::TfrmPropertiesSceneObject(TComponent* Owner)
    : TForm(Owner)
{
	bLoadMode = false;
    m_NewReference="";
    InplaceMotionEdit->Editor->Color		= TColor(0x00A0A0A0);
    InplaceMotionEdit->Editor->BorderStyle	= bsNone;
}
//---------------------------------------------------------------------------


void TfrmPropertiesSceneObject::GetObjectsInfo()
{
	bLoadMode = true;

	VERIFY(!m_Objects->empty());
	ObjectIt _F = m_Objects->begin();
    VERIFY( (*_F)->ClassID==OBJCLASS_SCENEOBJECT );

    CSceneObject *_S = (CSceneObject *)(*_F);
	edName->Text= _S->Name;
    ebReference->Caption=_S->GetReference()->GetName();
	m_EditObject	= _S;
    cbDummy->ObjFirstInit((TCheckBoxState)_S->IsFlag(CSceneObject::eDummy));

	_F++;
	for(;_F!=m_Objects->end();_F++){
	    cbDummy->ObjNextInit((TCheckBoxState)((CSceneObject*)(*_F))->IsFlag(CSceneObject::eDummy));
    }

    if (m_Objects->size()>1){
		m_EditObject		= 0;
    	edName->Enabled 	= false;
        edName->Text		= "<Multiple selection>";
        ebReference->Caption= "<Multiple selection>";
		paMotions->Visible	= false;
    }else{
    	edName->Enabled 	= true;
		paMotions->Visible	= true;
        tsMotions->TabVisible=m_EditObject->IsDynamic();
        SaveObjectsInfo		();
    }
	bLoadMode = false;
}

bool TfrmPropertiesSceneObject::ApplyObjectsInfo()
{
    VERIFY( !m_Objects->empty() );
	ObjectIt _F = m_Objects->begin();
    bool bMultiSel = (m_Objects->size()>1);
	for(;_F!=m_Objects->end();_F++){
    	CSceneObject *_O = (CSceneObject*)(*_F);
		// apply flags
        int f=_O->IsFlag(CSceneObject::eDummy); cbDummy->ObjApply(f); _O->SetFlag(CSceneObject::eDummy);
        if (!bMultiSel){
        	if (Scene.FindObjectByName(edName->Text.c_str(),_O)){
            	ELog.DlgMsg(mtError,"Name already exist in scene: '%s'",edName->Text.c_str());
            	return false;
            }
        	_O->Name = edName->Text.c_str();
        }
        // change reference
        if (!m_NewReference.IsEmpty())
        	_O->SetReference(m_NewReference.c_str());
	}
    return true;
}

void TfrmPropertiesSceneObject::SaveObjectsInfo()
{
	if (m_EditObject&&m_EditObject->IsDynamic()){
		ClearObjectsInfo();
    	for (OMotionIt it=m_EditObject->m_OMotions.begin(); it!=m_EditObject->m_OMotions.end(); it++)
        	m_OMotions.push_back(new COMotion(*it));
    	for (AStringIt s_it=m_EditObject->m_Sounds.begin(); s_it!=m_EditObject->m_Sounds.end(); s_it++)
        	m_Sounds.push_back(*s_it);
    }
}

void TfrmPropertiesSceneObject::ClearObjectsInfo()
{
	if (m_EditObject&&m_EditObject->IsDynamic()){
    	for (OMotionIt it=m_OMotions.begin(); it!=m_OMotions.end(); it++)
        	_DELETE(*it);
     	m_OMotions.clear();
     	m_Sounds.clear();
    }
}

void TfrmPropertiesSceneObject::RestoreObjectsInfo()
{
	if (m_EditObject&&m_EditObject->IsDynamic()){
		m_EditObject->ClearOMotions();
    	for (OMotionIt it=m_OMotions.begin(); it!=m_OMotions.end(); it++)
        	m_EditObject->m_OMotions.push_back(new COMotion(*it));
    	for (AStringIt s_it=m_Sounds.begin(); s_it!=m_Sounds.end(); s_it++)
        	m_EditObject->m_Sounds.push_back(*s_it);
    }
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesSceneObject::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Key==VK_RETURN) ebOk->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesSceneObject::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::OnModified(TObject *Sender)
{
	if (bLoadMode) return;
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebCancelClick(TObject *Sender)
{
	if (ebOk->Enabled) RestoreObjectsInfo();
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

//----------------------------------------------------
int __fastcall TfrmPropertiesSceneObject::Run(ObjectList* pObjects, bool& bChange)
{
	VERIFY(!TfrmPropertiesSceneObject::form);
	form = new TfrmPropertiesSceneObject(0);
    form->m_Objects = pObjects;
    VERIFY(form->m_Objects);
	form->GetObjectsInfo();
    int res = form->ShowModal();
    bChange = (res==mrOk);
    return res;
}

void __fastcall TfrmPropertiesSceneObject::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	ClearObjectsInfo();
	Action = caFree;
    form = 0;
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesSceneObject::ebReferenceClick(TObject *Sender)
{
	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Change object reference?")==mrYes){
		LPCSTR N = TfrmChoseItem::SelectObject(false,0,0);
        if (N){
        	m_NewReference=N;
            ebReference->Caption=N;
            OnModified(Sender);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Motion routines
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesSceneObject::tsMotionsShow(TObject *Sender)
{
// Set up surfaces&textures
    AnsiString name;
    tvOMotions->Items->Clear();
    if (!IsMultiSelection()){
        // create root tree node (object name)
        for (OMotionIt m_it=m_EditObject->m_OMotions.begin(); m_it!=m_EditObject->m_OMotions.end(); m_it++){
        	TElTreeItem *Item = FOLDER::AppendObject(tvOMotions,(*m_it)->Name());
            Item->ShowCheckBox = true;
            Item->CheckBoxType = ectCheckBox;
            Item->CheckBoxEnabled = true;
        }
        tvOMotions->FullExpand();

        lbOMotionCount->Caption 	= m_EditObject->m_OMotions.size();
		lbActiveOMotion->Caption 	= "...";
        if (m_EditObject->m_ActiveOMotion){
	        TElTreeItem* Item 		= FOLDER::FindObject(tvOMotions,m_EditObject->m_ActiveOMotion->Name());
            Item->Checked 			= true;
	    	lbActiveOMotion->Caption= Item->Text;
        }
    }

    tvOMotions->Sort(true);

//    FEditNode = 0;
//    selected_omotion = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::tvOMotionsItemFocused(
      TObject *Sender)
{
//	selected_omotion = 0;
	TElTreeItem* item = tvOMotions->Selected;
    if (item&&FOLDER::IsObject(item)){
	    R_ASSERT(!IsMultiSelection());
        AnsiString fn;
        FOLDER::MakeFullName		(item,0,fn);
        COMotion* M					= m_EditObject->FindOMotionByName(fn.c_str());
        VERIFY2(M,"Can't find motion in 'm_OMotions'.");
        lbOMotionName->Caption 		= item->Text;
        lbOMotionFrames->Caption	= AnsiString(M->FrameStart())+AnsiString(" .. ")+AnsiString(M->FrameEnd());
        lbOMotionFPS->Caption		= (int)M->FPS();
        gbOMotion->Show();
//        selected_omotion	  		= M;
   }else{
    	gbOMotion->Hide();
   }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebOMotionAppendClick(
      TObject *Sender)
{
    AnsiString fn;
    if (Engine.FS.GetOpenName(Engine.FS.m_OMotion,fn)){
    	COMotion* M=m_EditObject->AppendOMotion(fn.c_str());
    	if (M){
		    AnsiString nm; FOLDER::MakeName(tvOMotions->Selected,0,nm,true); nm += ChangeFileExt(ExtractFileName(fn),"");
        	string256 buf;
            m_EditObject->GenerateOMotionName(buf,nm.c_str(),M);
            M->SetName(buf);
            // append to list
            TElTreeItem* Item = FOLDER::AppendObject(tvOMotions,M->Name());
            Item->ShowCheckBox = true;
            Item->CheckBoxType = ectCheckBox;
            Item->CheckBoxEnabled = true;
            tvOMotions->Selected = Item;
            tvOMotions->EnsureVisible(Item);

        	ELog.DlgMsg(mtInformation,"Motion '%s' appended.",M->Name());
    		lbOMotionCount->Caption = m_EditObject->m_OMotions.size();
		    OnModified(Sender);
        }else
        	ELog.DlgMsg(mtError,"Append failed.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::OnRemoveItem(LPCSTR name)
{
	m_EditObject->RemoveOMotion(name);
	lbOMotionCount->Caption = m_EditObject->m_OMotions.size();
    OnModified(0);
}
void __fastcall TfrmPropertiesSceneObject::ebOMotionDeleteClick(
      TObject *Sender)
{
    if (tvOMotions->Selected)
		FOLDER::RemoveItem(tvOMotions,tvOMotions->Selected,OnRemoveItem);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebResetActive(TElTree* tv, TElTreeItem* ignore_item){
	if (m_EditObject){
	    for ( TElTreeItem* node = tv->Items->GetFirstNode(); node; node = node->GetNext())
    	    if (node->Checked&&(ignore_item!=node)){ node->Checked = false; break; }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::tvOMotionsItemChange(
      TObject *Sender, TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
	switch(ItemChangeMode){
	case icmCheckState:
    	if (m_EditObject){
            if (Item->Checked){
		        AnsiString fn; FOLDER::MakeFullName(Item,0,fn);
    		    m_EditObject->SetActiveOMotion(m_EditObject->FindOMotionByName(fn.c_str()));
		    	lbActiveOMotion->Caption	= Item->Text;
            }else{
		    	lbActiveOMotion->Caption	= "...";
    		    m_EditObject->SetActiveOMotion(0);
            }
			ebResetActive(tvOMotions,Item);
			OnModified(Sender);
        }
    break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebOMotionClearClick(
      TObject *Sender)
{
    tvOMotions->Items->Clear();
	m_EditObject->ClearOMotions();
	OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebOMotionSaveClick(
      TObject *Sender)
{
	AnsiString fn;
    if (m_EditObject->OMotionCount()){
    	fn = m_EditObject->Name;
	    if (Engine.FS.GetSaveName(Engine.FS.m_OMotions,fn)){
        	m_EditObject->SaveOMotions(fn.c_str());
	    	ELog.DlgMsg(mtInformation,"Motions saved.");
        }
    }else{
    	ELog.DlgMsg(mtInformation,"Motion list empty!");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebOMotionLoadClick(
      TObject *Sender)
{
	AnsiString fn;
    if (Engine.FS.GetOpenName(Engine.FS.m_OMotions,fn)){
	    TElTreeItem* Item = tvOMotions->Items->GetFirstNode(); Item->Clear();
		m_EditObject->ClearOMotions();
		m_EditObject->LoadOMotions(fn.c_str());
		ELog.DlgMsg(mtInformation,"%d motion loaded.",m_EditObject->OMotionCount());
        tsMotionsShow(Sender);
		OnModified(Sender);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::tvOMotionsDragDrop(
      TObject *Sender, TObject *Source, int X, int Y)
{
	FOLDER::DragDrop(Sender,Source,X,Y,OnRenameItem);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::tvOMotionsDragOver(
      TObject *Sender, TObject *Source, int X, int Y, TDragState State,
      bool &Accept)
{
	FOLDER::DragOver(Sender,Source,X,Y,State,Accept);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::tvOMotionsStartDrag(
      TObject *Sender, TDragObject *&DragObject)
{
	FOLDER::StartDrag(Sender,DragObject);
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesSceneObject::OnRenameItem(LPCSTR p0, LPCSTR p1)
{
    m_EditObject->RenameOMotion((LPCSTR)p0,(LPCSTR)p1);
	OnModified(0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::InplaceMotionEditValidateResult(
      TObject *Sender, bool &InputValid)
{
	InputValid = FOLDER::RenameItem(tvOMotions,InplaceMotionEdit->Item,InplaceMotionEdit->Editor->Text,OnRenameItem);
	if (InputValid) OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebOMotionRenameClick(
      TObject *Sender)
{
	if (tvOMotions->Selected) tvOMotions->EditItem(tvOMotions->Selected,-1);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::Rename1Click(TObject *Sender)
{
	ebOMotionRenameClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::CreateFolder1Click(
      TObject *Sender)
{
	FOLDER::CreateNewFolder(tvOMotions,true);
	OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ExpandAll1Click(TObject *Sender)
{
	tvOMotions->FullExpand();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::CollapseAll1Click(
      TObject *Sender)
{
	tvOMotions->FullCollapse();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::tvOMotionsMouseDown(
      TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
	if (Button==mbRight)	FOLDER::ShowPPMenu(pmMotions,0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::fsStorageRestorePlacement(
      TObject *Sender)
{
	if (!tsMotions->TabVisible) pcSceneProps->ActivePage = tsBasic;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::cbDummyClick(TObject *Sender)
{
	OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::OnRemoveSoundItem(LPCSTR name)
{
	m_EditObject->RemoveSound(name);
	lbSoundCount->Caption = m_EditObject->m_Sounds.size();
    OnModified(0);
}

void __fastcall TfrmPropertiesSceneObject::ebSoundAppendClick(
      TObject *Sender)
{
    LPCSTR N=TfrmChoseItem::SelectSound(false,0,true);
    if (N){
    	if (m_EditObject->AppendSound(N)){
            // append to list
            TElTreeItem* Item = FOLDER::AppendObject(tvSounds,N);
            Item->ShowCheckBox = true;
            Item->CheckBoxType = ectCheckBox;
            Item->CheckBoxEnabled = true;
            tvSounds->Selected = Item;
            tvSounds->EnsureVisible(Item);

        	ELog.DlgMsg(mtInformation,"Sound '%s' appended.",N);
    		lbSoundCount->Caption = m_EditObject->m_Sounds.size();
		    OnModified(Sender);
        }else
        	ELog.DlgMsg(mtError,"Append failed.");
    }
}                                                
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebSoundDeleteClick(
      TObject *Sender)
{
    if (tvSounds->Selected)
		FOLDER::RemoveItem(tvSounds,tvSounds->Selected,OnRemoveSoundItem);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebSoundClearClick(
      TObject *Sender)
{
    tvSounds->Items->Clear();
	m_EditObject->ClearSounds();
	OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::tvSoundsItemChange(
      TObject *Sender, TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
	switch(ItemChangeMode){
	case icmCheckState:
    	if (m_EditObject){
            if (Item->Checked){
		        AnsiString fn; FOLDER::MakeFullName(Item,0,fn);
    		    m_EditObject->SetActiveSound(fn.c_str());
		    	lbSoundActive->Caption	= Item->Text;
            }else{
		    	lbSoundActive->Caption	= "...";
    		    m_EditObject->SetActiveSound(0);
            }
			ebResetActive(tvSounds,Item);
			OnModified(Sender);
        }
    break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::tsSoundsShow(TObject *Sender)
{
// Set up surfaces&textures
    AnsiString name;
    tvSounds->Items->Clear();
    if (!IsMultiSelection()){
        // create root tree node (object name)
        for (AStringIt m_it=m_EditObject->m_Sounds.begin(); m_it!=m_EditObject->m_Sounds.end(); m_it++){
        	TElTreeItem *Item = FOLDER::AppendObject(tvSounds,m_it->c_str());
            Item->ShowCheckBox = true;
            Item->CheckBoxType = ectCheckBox;
            Item->CheckBoxEnabled = true;
        }
        tvSounds->FullExpand();

        lbSoundCount->Caption 	= m_EditObject->m_Sounds.size();
		lbSoundActive->Caption 	= "...";
        if (!m_EditObject->m_ActiveSound.IsEmpty()){
	        TElTreeItem* Item 		= FOLDER::FindObject(tvSounds,m_EditObject->m_ActiveSound.c_str());
            Item->Checked 			= true;
	    	lbSoundActive->Caption	= Item->Text;
        }
    }

    tvSounds->Sort(true);

//    FEditNode = 0;
//    selected_omotion = 0;
}
//---------------------------------------------------------------------------

