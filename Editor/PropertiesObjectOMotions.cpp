#include "stdafx.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "EditObject.h"
#include "motion.h"
#include "FileSystem.h"
//---------------------------------------------------------------------
// motions
//---------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsOAnimationShow(TObject *Sender){
// Set up surfaces&textures
    AnsiString name;
    tvOMotions->Items->Clear();

    // create root tree node (object name)
    name.sprintf("Motions %s (%s)",m_CurrentObject->GetName(),m_CurrentObject->GetName());
    TElTreeItem* root = tvOMotions->Items->AddObject(0,name,0);
    root->ParentStyle = false;
    root->Bold = true;
    root->Underlined = false;

    for (OMotionIt m_it=m_CurrentObject->m_OMotions.begin(); m_it!=m_CurrentObject->m_OMotions.end(); m_it++){
        TElTreeItem *Item = tvOMotions->Items->AddChildObject(root,(*m_it)->Name(),(TObject*)*m_it);
        Item->ShowCheckBox = true;
        Item->CheckBoxType = ectCheckBox;
        Item->CheckBoxEnabled = true;
    }
    root->Expand(false);

    lbOMotionCount->Caption = m_CurrentObject->m_OMotions.size();

    lbActiveOMotion->Caption 	= "...";
    if (m_CurrentObject->m_ActiveOMotion){
        TElTreeItem* Item 		= FindOMotionItem(m_CurrentObject->m_ActiveOMotion->Name());
        Item->Checked 			= true;
        lbActiveOMotion->Caption	= Item->Text;
    }

    gbOMotionCommand->Enabled = true;

    tvOMotions->Sort(true);

    FEditNode = 0;
    selected_omotion = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tvOMotionsItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
	selected_omotion = 0;
    if (Item->Level==0){
    	gbOMotion->Hide();
    }else{
        COMotion* M					= (COMotion*)Item->Data;//m_EditObject->FindMotionByName(Item->Text.c_str());
        VERIFY2(M,"Can't find motion in 'm_OMotions'.");
        lbOMotionName->Caption 		= M->Name();
        lbOMotionFrames->Caption	= AnsiString(M->FrameStart())+AnsiString(" .. ")+AnsiString(M->FrameEnd());
        lbOMotionFPS->Caption		= (int)M->FPS();
        gbOMotion->Show();
        selected_omotion	  		= M;
   }
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesObject::ebOAppendMotionClick(TObject *Sender)
{
    AnsiString fn;
    if (FS.GetOpenName(&FS.m_OMotion,fn)){
    	COMotion* M=m_CurrentObject->AppendOMotion(fn.c_str());
    	if (M){
        	char buf[1024];
            m_CurrentObject->GenerateOMotionName(buf,M->Name(),M);
            M->SetName(buf);

            // append to list
            TElTreeItem* Item = tvOMotions->Items->AddChildObject(tvOMotions->Items->GetFirstNode(),buf,(TObject*)M);
            Item->ShowCheckBox = true;
            Item->CheckBoxType = ectCheckBox;
            Item->CheckBoxEnabled = true;
            tvOMotions->Selected = Item;

        	ELog.DlgMsg(mtInformation,"Motion '%s' appended.",M->Name());
    		lbOMotionCount->Caption = m_CurrentObject->m_OMotions.size();
		    OnModified(Sender);
        }else
        	ELog.DlgMsg(mtError,"Append failed.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebODeleteMotionClick(TObject *Sender)
{
    if (tvOMotions->Selected){
        TElTreeItem* Item 	= tvOMotions->Selected;
        if (Item->Data){
	        m_CurrentObject->RemoveOMotion(Item->Text.c_str());
    	    Item->Delete();
    		lbOMotionCount->Caption = m_CurrentObject->m_OMotions.size();
	    	OnModified(Sender);
		    selected_omotion = 0;
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesObject::ebORenameMotionClick(TObject *Sender)
{
    if (tvOMotions->Selected&&tvOMotions->Selected->Data) tvOMotions->Selected->EditText();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tvOMotionsTryEdit(TObject *Sender,
      TElTreeItem *Item, TElHeaderSection *Section, TFieldTypes &CellType,
      bool &CanEdit)
{
	FEditNode = tvOMotions->Selected;
    last_name = FEditNode->Text;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tvOMotionsItemChange(TObject *Sender,
      TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
	switch(ItemChangeMode){
    case icmText:
        if (FEditNode){
            if (!m_CurrentObject->RenameOMotion(last_name.c_str(),Item->Text.c_str())){
                ELog.DlgMsg(mtError,"Motion with name '%s' already present.",Item->Text.c_str());
                FEditNode = 0;
                Item->Text = last_name;
            }else{
                FEditNode = 0;
		        lbOMotionName->Caption 		= selected_omotion->Name();
                Item->Text = selected_omotion->Name();
                OnModified(Sender);
            }
            last_name = "";
        }
    break;
    case icmCheckState:
    	if (m_CurrentObject){
            if (Item->Checked){
		    	lbActiveOMotion->Caption 	 = Item->Text;
    		    m_CurrentObject->SetActiveOMotion((COMotion*)Item->Data);
            }else{
		    	lbActiveOMotion->Caption 	 = "...";
    		    m_CurrentObject->SetActiveOMotion(0);
            }
        	ebOResetActiveMotion(Item);
			OnModified(Sender);
        }
    break;
    }
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmPropertiesObject::FindOMotionItem(const char* name){
	AnsiString nm = name;
    for ( TElTreeItem* node = tvOMotions->Items->GetFirstNode(); node; node = node->GetNext())
        if (node->Text==nm) return node;
    return 0;
}


void __fastcall TfrmPropertiesObject::ebOResetActiveMotion(TElTreeItem* ignore_item){
	if (m_CurrentObject){
//		lbActiveOMotion->Caption 		= "...";
	    for ( TElTreeItem* node = tvOMotions->Items->GetFirstNode(); node; node = node->GetNext())
    	    if (node->Checked&&(ignore_item!=node)){ node->Checked = false; break; }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebOMotionClearClick(TObject *Sender)
{
    TElTreeItem* Item = tvOMotions->Items->GetFirstNode(); Item->Clear();
	m_CurrentObject->ClearOMotions();
	OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebOMotionSaveClick(TObject *Sender)
{
	AnsiString fn;
    if (m_CurrentObject->OMotionCount()){
    	fn = m_CurrentObject->GetName();
	    if (FS.GetSaveName(&FS.m_OMotions,fn)){
        	m_CurrentObject->SaveOMotions(fn.c_str());
	    	ELog.DlgMsg(mtInformation,"Motions saved.");
        }
    }else{
    	ELog.DlgMsg(mtInformation,"Motion list empty!");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebOMotionLoadClick(TObject *Sender)
{
	AnsiString fn;
    if (FS.GetOpenName(&FS.m_OMotions,fn)){
	    TElTreeItem* Item = tvOMotions->Items->GetFirstNode(); Item->Clear();
		m_CurrentObject->ClearOMotions();
		m_CurrentObject->LoadOMotions(fn.c_str());
		ELog.DlgMsg(mtInformation,"%d motion loaded.",m_CurrentObject->OMotionCount());
        tsOAnimationShow(Sender);
		OnModified(Sender);
    }
}
//---------------------------------------------------------------------------

