#include "stdafx.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "EditObject.h"
#include "motion.h"
//---------------------------------------------------------------------
// motions
//---------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsOAnimationShow(TObject *Sender){
	if (!m_LibObject) return;
// Set up surfaces&textures
    AnsiString name;
    tvOMotions->IsUpdating = true;
    tvOMotions->Items->Clear();

    // create root tree node (object name)
    TElTreeItem* root = tvOMotions->Items->Add(0,"Motions");
    root->ParentStyle = false;
    root->Bold = true;
    root->Underlined = false;

    for (OMotionIt m_it=m_LibObject->m_OMotions.begin(); m_it!=m_LibObject->m_OMotions.end(); m_it++){
        TElTreeItem *Item = tvOMotions->Items->AddChildObject(root,(*m_it)->Name(),(TObject*)*m_it);
        Item->ShowCheckBox = true;
        Item->CheckBoxType = ectCheckBox;
        Item->CheckBoxEnabled = true;
    }
    root->Expand(false);

    lbOMotionCount->Caption = m_LibObject->m_OMotions.size();

    lbActiveOMotion->Caption 	= "...";
    if (m_LibObject->m_ActiveOMotion){
        TElTreeItem* Item 		= FindOMotionItem(m_LibObject->m_ActiveOMotion->Name());
        Item->Checked 			= true;
        lbActiveOMotion->Caption	= Item->Text;
    }

    gbOMotionCommand->Enabled = true;

    tvOMotions->Sort(true);

    FEditNode = 0;
    selected_omotion = 0;
    tvOMotions->IsUpdating = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tvOMotionsItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
	if (Item==tvOMotions->Selected) return;
	selected_omotion = 0;
    if (Item->Level==0){
    	gbOMotion->Hide();
    }else{
        COMotion* M					= (COMotion*)Item->Data;//m_LibObject->FindMotionByName(Item->Text.c_str());
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
	VERIFY(m_LibObject);
    AnsiString fn;
    if (FS.GetOpenName(FS.m_OMotion,fn)){
    	COMotion* M=m_LibObject->AppendOMotion(fn.c_str());
    	if (M){
        	char buf[1024];
            m_LibObject->GenerateOMotionName(buf,M->Name(),M);
            M->SetName(buf);

            // append to list
            TElTreeItem* Item = tvOMotions->Items->AddChildObject(tvOMotions->Items->GetFirstNode(),buf,(TObject*)M);
            Item->ShowCheckBox = true;
            Item->CheckBoxType = ectCheckBox;
            Item->CheckBoxEnabled = true;
            tvOMotions->Selected = Item;

        	ELog.DlgMsg(mtInformation,"Motion '%s' appended.",M->Name());
    		lbOMotionCount->Caption = m_LibObject->m_OMotions.size();
		    OnModified(Sender);
        }else
        	ELog.DlgMsg(mtError,"Append failed.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebODeleteMotionClick(TObject *Sender)
{
	VERIFY(m_LibObject);
    if (tvOMotions->Selected){
        TElTreeItem* Item 	= tvOMotions->Selected;
        if (Item->Data){
	        m_LibObject->RemoveOMotion(AnsiString(Item->Text).c_str());
    	    Item->Delete();
    		lbOMotionCount->Caption = m_LibObject->m_OMotions.size();
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
//S
/*
void __fastcall TfrmPropertiesObject::tvOMotionsTryEdit(TObject *Sender,
      TElTreeItem *Item, TElHeaderSection *Section, TFieldTypes &CellType,
      bool &CanEdit)
{
	FEditNode = tvOMotions->Selected;
    last_name = FEditNode->Text;
}
*/
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tvOMotionsItemChange(TObject *Sender,
      TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
	VERIFY(m_LibObject);
	switch(ItemChangeMode){
    case icmText:
        if (FEditNode){
            if (!m_LibObject->RenameOMotion(last_name.c_str(),AnsiString(Item->Text).c_str())){
                ELog.DlgMsg(mtError,"Motion with name '%s' already present.",AnsiString(Item->Text).c_str());
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
    	if (m_LibObject){
            if (Item->Checked){
		    	lbActiveOMotion->Caption 	 = Item->Text;
    		    m_LibObject->SetActiveOMotion((COMotion*)Item->Data);
            }else{
		    	lbActiveOMotion->Caption 	 = "...";
    		    m_LibObject->SetActiveOMotion(0);
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
	if (m_LibObject){
//		lbActiveOMotion->Caption 		= "...";
	    for ( TElTreeItem* node = tvOMotions->Items->GetFirstNode(); node; node = node->GetNext())
    	    if (node->Checked&&(ignore_item!=node)){ node->Checked = false; break; }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebOMotionClearClick(TObject *Sender)
{
	VERIFY(m_LibObject);
    TElTreeItem* Item = tvOMotions->Items->GetFirstNode(); Item->Clear();
	m_LibObject->ClearOMotions();
	OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebOMotionSaveClick(TObject *Sender)
{
	VERIFY(m_LibObject);
	AnsiString fn;
    if (m_LibObject->OMotionCount()){
    	fn = m_LibObject->GetName();
	    if (FS.GetSaveName(FS.m_OMotions,fn)){
        	m_LibObject->SaveOMotions(fn.c_str());
	    	ELog.DlgMsg(mtInformation,"Motions saved.");
        }
    }else{
    	ELog.DlgMsg(mtInformation,"Motion list empty!");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebOMotionLoadClick(TObject *Sender)
{
	VERIFY(m_LibObject);
	AnsiString fn;
    if (FS.GetOpenName(FS.m_OMotions,fn)){
	    TElTreeItem* Item = tvOMotions->Items->GetFirstNode(); Item->Clear();
		m_LibObject->ClearOMotions();
		m_LibObject->LoadOMotions(fn.c_str());
		ELog.DlgMsg(mtInformation,"%d motion loaded.",m_LibObject->OMotionCount());
        tsOAnimationShow(Sender);
		OnModified(Sender);
    }
}
//---------------------------------------------------------------------------

