#include "stdafx.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "EditObject.h"
#include "motion.h"
#include "FileSystem.h"
#include "Bone.h"
//---------------------------------------------------------------------
// Skeleton
//---------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsSAnimationShow(TObject *Sender)
{
// Set up surfaces&textures
    AnsiString name;
    tvSMotions->Items->Clear();

    // create root tree node (object name)
    name.sprintf("Motions '%s'",m_CurrentObject->GetName());
    TElTreeItem* root = tvSMotions->Items->AddObject(0,name,0);
    root->ParentStyle = false;
    root->Bold = true;
    root->Underlined = false;

    for (SMotionIt s_it=m_CurrentObject->m_SMotions.begin(); s_it!=m_CurrentObject->m_SMotions.end(); s_it++){
        TElTreeItem *Item = tvSMotions->Items->AddChildObject(root,(*s_it)->Name(),(TObject*)*s_it);
        Item->ShowCheckBox = true;
        Item->CheckBoxType = ectCheckBox;
        Item->CheckBoxEnabled = true;
    }
    root->Expand(false);

    lbSMotionCount->Caption = m_CurrentObject->m_SMotions.size();
    lbSBoneCount->Caption	= m_CurrentObject->m_Bones.size();

    lbActiveSMotion->Caption 	= "...";
    if (m_CurrentObject->m_ActiveSMotion){
        TElTreeItem* Item 		= FindSMotionItem(m_CurrentObject->m_ActiveSMotion->Name());
        Item->Checked 			= true;
        lbActiveSMotion->Caption= Item->Text;
    }

    // fill bone list
    cbSStartMotionBone->Items->Clear();
    for(BoneIt it=m_CurrentObject->FirstBone(); it!=m_CurrentObject->LastBone(); it++)
        cbSStartMotionBone->Items->Add(AnsiString((*it)->Name()));

    // temporary!!! add none bone part
    cbSBonePart->Items->Clear();
    cbSBonePart->Items->Add("--none--");

    gbSMotionCommand->Enabled = true;

    tvSMotions->Sort(true);

    FEditNode = 0;
    selected_smotion = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebSAppendMotionClick(TObject *Sender)
{
    AnsiString fn;
    if (FS.GetOpenName(&FS.m_SMotion,fn)){
    	CSMotion* M=m_CurrentObject->AppendSMotion(fn.c_str());
    	if (M){
        	char buf[1024];
            m_CurrentObject->GenerateSMotionName(buf,M->Name(),M);
            M->SetName(buf);

            // append to list
            TElTreeItem* Item = tvSMotions->Items->AddChildObject(tvSMotions->Items->GetFirstNode(),buf,(TObject*)M);
            Item->ShowCheckBox = true;
            Item->CheckBoxType = ectCheckBox;
            Item->CheckBoxEnabled = true;
            tvSMotions->Selected = Item;

        	ELog.DlgMsg(mtInformation,"Motion '%s' appended.",M->Name());
    		lbSMotionCount->Caption = m_CurrentObject->m_SMotions.size();
		    OnModified(Sender);
        }else
        	ELog.DlgMsg(mtError,"Append failed.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebSReloadMotionClick(TObject *Sender)
{
    if (selected_smotion){
    	AnsiString fn;
	    if (FS.GetOpenName(&FS.m_SMotion,fn)){
    		if (m_CurrentObject->ReloadSMotion(selected_smotion,fn.c_str())){
	        	ELog.DlgMsg(mtInformation,"Motion '%s' reloaded.",selected_smotion->Name());
		        seSMotionChange				(Sender);
			    OnModified(Sender);
            }
        }else
        	ELog.DlgMsg(mtError,"Reload failed.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebSDeleteMotionClick(TObject *Sender)
{
    if (tvSMotions->Selected){
        TElTreeItem* Item 	= tvSMotions->Selected;
        if (Item->Data){
	        m_CurrentObject->RemoveSMotion(Item->Text.c_str());
    	    Item->Delete();
    		lbSMotionCount->Caption = m_CurrentObject->m_SMotions.size();
	    	OnModified(Sender);
		    selected_smotion = 0;
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesObject::ebSRenameMotionClick(TObject *Sender)
{
    if (tvSMotions->Selected&&tvSMotions->Selected->Data) tvSMotions->Selected->EditText();
}
//---------------------------------------------------------------------------

TElTreeItem* TfrmPropertiesObject::FindSMotionItem(const char* name){
	AnsiString nm = name;
    for ( TElTreeItem* node = tvSMotions->Items->GetFirstNode(); node; node = node->GetNext())
        if (node->Text==nm) return node;
    return 0;
}
void __fastcall TfrmPropertiesObject::ebSResetActiveMotion(TElTreeItem* ignore_item){
	if (m_CurrentObject){
//		lbActiveSMotion->Caption 		= "...";
	    for ( TElTreeItem* node = tvSMotions->Items->GetFirstNode(); node; node = node->GetNext())
    	    if (node->Checked&&(ignore_item!=node)){ node->Checked = false; break; }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tvSMotionsItemChange(TObject *Sender,
      TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
	switch(ItemChangeMode){
    case icmText:
        if (FEditNode){
            if (!m_CurrentObject->RenameSMotion(last_name.c_str(),Item->Text.c_str())){
                ELog.DlgMsg(mtError,"Motion with name '%s' already present.",Item->Text.c_str());
                FEditNode = 0;
                Item->Text = last_name;
            }else{
                FEditNode = 0;
		        lbSMotionName->Caption 		= selected_smotion->Name();
                Item->Text = selected_smotion->Name();
                OnModified(Sender);
            }
            last_name = "";
        }
    break;
    case icmCheckState:
    	if (m_CurrentObject){
            if (Item->Checked){
		    	lbActiveSMotion->Caption 	 = Item->Text;
    		    m_CurrentObject->SetActiveSMotion((CSMotion*)Item->Data);
            }else{
		    	lbActiveSMotion->Caption 	 = "...";
    		    m_CurrentObject->SetActiveSMotion(0);
            }
        	ebSResetActiveMotion(Item);
			OnModified(Sender);
        }
    break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tvSMotionsItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
	// save previous motion data
    selected_smotion = 0;
    if (Item->Level==0){
    	gbSMotion->Hide();
    }else{
    	// select new motion

        CSMotion* M					= (CSMotion*)Item->Data;//m_EditObject->FindMotionByName(Item->Text.c_str());
        VERIFY(M);
        lbSMotionName->Caption 		= M->Name();
        lbSMotionFrames->Caption	= AnsiString(M->FrameStart())+AnsiString(" .. ")+AnsiString(M->FrameEnd());
        lbSMotionFPS->Caption		= (int)M->FPS();

        seSMotionSpeed->Value		= M->fSpeed;
        seSMotionAccrue->Value		= M->fAccrue;
        seSMotionFalloff->Value		= M->fFalloff;
        seSMotionPower->Value		= M->fPower;
        pcAnimType->ActivePage		= M->bFX?tsFX:tsCycle;
        cbSMotionStopAtEnd->Checked	= M->bStopAtEnd;
		cbSStartMotionBone->ItemIndex= 	cbSStartMotionBone->Items->IndexOf(AnsiString(M->cStartBone));
		cbSBonePart->ItemIndex		= cbSBonePart->Items->IndexOf(AnsiString(M->cBonePart));

        selected_smotion 			= M;

        seSMotionChange				(Sender);

        gbSMotion->Show();
   }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tvSMotionsTryEdit(TObject *Sender,
      TElTreeItem *Item, TElHeaderSection *Section, TFieldTypes &CellType,
      bool &CanEdit)
{
	FEditNode = tvSMotions->Selected;
    last_name = FEditNode->Text;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::seSMotionChange(TObject *Sender)
{
	if (selected_smotion){
        selected_smotion->fSpeed		= seSMotionSpeed->Value;
        selected_smotion->fAccrue		= seSMotionAccrue->Value;
        selected_smotion->fFalloff		= seSMotionFalloff->Value;
        selected_smotion->fPower		= seSMotionPower->Value;
        selected_smotion->bFX			= (pcAnimType->ActivePage==tsFX);
        selected_smotion->bStopAtEnd	= cbSMotionStopAtEnd->Checked;
        selected_smotion->SetStartBone	(cbSStartMotionBone->Text.c_str());
        selected_smotion->SetBonePart	(cbSBonePart->Text.c_str());

		OnModified(Sender);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::seSMotionSpeedKeyDown(
      TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (Key==VK_RETURN) seSMotionChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::seSMotionSpeedLWChange(
      TObject *Sender, int Val)
{
	seSMotionChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebSMotionClearClick(TObject *Sender)
{
    TElTreeItem* Item = tvSMotions->Items->GetFirstNode(); Item->Clear();
	m_CurrentObject->ClearSMotions();
	OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebSMotionSaveClick(TObject *Sender)
{
	AnsiString fn;
    if (m_CurrentObject->SMotionCount()){
    	fn = m_CurrentObject->GetName();
    	if(FS.GetSaveName(&FS.m_SMotions,fn)){
        	m_CurrentObject->SaveSMotions(fn.c_str());
	    	ELog.DlgMsg(mtInformation,"Motions saved.");
        }
    }else{
    	ELog.DlgMsg(mtInformation,"Motion list empty!");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebSMotionLoadClick(TObject *Sender)
{
	AnsiString fn;
    if (FS.GetOpenName(&FS.m_SMotions,fn)){
    	TElTreeItem* Item = tvSMotions->Items->GetFirstNode(); Item->Clear();
		m_CurrentObject->ClearSMotions();
		m_CurrentObject->LoadSMotions(fn.c_str());
        tsSAnimationShow(Sender);
		ELog.DlgMsg(mtInformation,"%d motion loaded.",m_CurrentObject->SMotionCount());
		OnModified(Sender);
    }
}
//---------------------------------------------------------------------------

