//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditParticles.h"
#include "UI_Main.h"
#include "scene.h"
#include "main.h"
#include "PSLibrary.h"
#include "PSObject.h"
#include "ParticleSystem.h"
#include "PropertiesPSDef.h"
#include "PropertiesPS.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Placemnt"
#pragma link "multi_edit"
#pragma resource "*.dfm"
TfrmEditParticles* TfrmEditParticles::form = 0;
//---------------------------------------------------------------------------
__fastcall TfrmEditParticles::TfrmEditParticles(TComponent* Owner)
    : TForm(Owner)
{
    FEditNode = 0;
    m_SelectedPS = 0;
    m_TestObject = 0;
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::ShowEditor(){
	if (!form){
        form = new TfrmEditParticles(0);

        form->init_cam_hpb.set(UI->Device.m_Camera.GetHPB());
        form->init_cam_pos.set(UI->Device.m_Camera.GetPosition());
        UI->Device.m_Camera.Set(0,0,0, 0,6,-20);
        // scene locking
        Scene->lock();
    }

    form->Show();
    UI->RedrawScene();
//    UI->Command(COMMAND_RENDER_FOCUS);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::HideEditor(bool bNeedReload){
	if (form){ 
	    if (form->ebSave->Enabled){
            int res = Log->DlgMsg(mtConfirmation, "Library was change. Do you want save?");
            if (res==mrCancel) return;
            if (res==mrYes){
    	        form->ebSaveClick(0);
        	}else{
	            if (bNeedReload){
			        UI->SetStatus("Library reloading...");
					PSLib->Reload();// RestoreBackup();
		    	    UI->SetStatus("");
	            }
            }
        }
    	form->Close();
    }
}
//---------------------------------------------------------------------------
void TfrmEditParticles::FinalClose(){
	if (form){ 
	    if (form->ebSave->Enabled){
		    if (Log->DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Library was change. Do you want save?")==mrYes)
				form->ebSaveClick(0);
        }
    	form->Close();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::SetCurrent(PS::SDef* ps){
	m_SelectedPS = ps;
    if (!ps) return;
    m_TestObject = new CPSObject("Test");
    m_TestObject->Compile(m_SelectedPS);
    gbCurrentPS->Show();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::ResetCurrent(){
	_DELETE(m_TestObject);
    m_SelectedPS = 0;
    gbCurrentPS->Hide();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::UpdateCurrent(){
	if (!Visible()) return;
    VERIFY(form->m_SelectedPS);
	_DELETE(form->m_TestObject);
    form->m_TestObject = new CPSObject("Test");
    form->m_TestObject->Compile(form->m_SelectedPS);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::UpdateEmitter(){
	if (!Visible()) return;
    VERIFY(form->m_SelectedPS);
    VERIFY(form->m_TestObject);
    form->m_TestObject->UpdateEmitter(&form->m_SelectedPS->m_DefaultEmitter);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::OnNameUpdate(){
	if (!Visible()) return;
    if (form->tvItems->Selected){
    	form->tvItems->Selected->Text = form->m_SelectedPS->m_Name;
	    form->SetCurrent(PSLib->FindPS(form->tvItems->Selected->Text.c_str()));
    }
    PSLib->Sort();
}
//---------------------------------------------------------------------------

PS::SDef* __fastcall TfrmEditParticles::FindPS(LPCSTR name){
	return PSLib->FindPS(name);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::ZoomObject(){
	if (!Visible()) return;
    if (form->m_SelectedPS){
    	Fbox bb;
//        form->m_SelectedPS->GetBox(bb);
        UI->Device.m_Camera.ZoomExtents(bb);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::OnRender(){
	if (!Visible()) return;
	if (form->m_TestObject) form->m_TestObject->RenderSingle(precalc_identity);
}

//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::OnIdle(){
	if (!Visible()) return;
	if (form->m_TestObject){
    	form->m_TestObject->RTL_Update(UI->Device.m_FrameDTime);
        form->lbCurState->Caption = (form->m_TestObject->ParticleCount()||form->m_TestObject->IsPlaying())?"playing":"stopping";
        form->lbParticleCount->Caption = form->m_TestObject->ParticleCount();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::FormShow(TObject *Sender)
{
    InitItemsList();
    ebSave->Enabled = false;
    UI->BeginEState(esEditParticles);

    // add directional light
    Flight L;
    ZeroMemory(&L,sizeof(Flight));
    L.type = D3DLIGHT_DIRECTIONAL;
    L.diffuse.set(1,1,1,1);
    L.direction.set(1,-1,1); L.direction.normalize();
	UI->Device.SetLight(0,L);
	UI->Device.LightEnable(0,true);
    L.diffuse.set(0.5,0.5,0.5,1);
    L.direction.set(1,-1,-1); L.direction.normalize();
	UI->Device.SetLight(1,L);
	UI->Device.LightEnable(1,true);
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::FormClose(TObject *Sender, TCloseAction &Action)
{
    _DELETE(m_TestObject);
	Action = caFree;
	form = 0;
	UI->Device.m_Camera.Set(init_cam_hpb,init_cam_pos);
	Scene->unlock();
    UI->EndEState(esEditParticles);
    UI->Command(COMMAND_CLEAR);
    // remove directional light
	UI->Device.LightEnable(0,false);
	UI->Device.LightEnable(1,false);
    // close properties window
    TfrmPropertiesPSDef::HideProperties();
}
//---------------------------------------------------------------------------
void TfrmEditParticles::OnModified(){
    ebSave->Enabled = true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Folder routines
//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::tvItemsItemSelectedChange(TObject *Sender, TElTreeItem *Item)
{
	if (Item==tvItems->Selected) return;
	ResetCurrent();
	if (Item->Data) SetCurrent(PSLib->FindPS(Item->Text.c_str()));
    UI->RedrawScene();
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditParticles::FindFolder(const char* s)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Data && (node->Text == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditParticles::FindItem(const char* s)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (node->Data && (node->Text == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditParticles::AddFolder(const char* s)
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
TElTreeItem* TfrmEditParticles::AddItem(TElTreeItem* node, const char* name)
{
    TElTreeItem* obj_node = tvItems->Items->AddChildObject(node, name, (void*)1);
    obj_node->ParentStyle = false;
    obj_node->Bold = false;
    return obj_node;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditParticles::AddItemToFolder(const char* folder, const char* name){
	TElTreeItem* node = FindFolder(folder);
    if (!node) node = AddFolder(folder);
	return AddItem(node,name);
}
//---------------------------------------------------------------------------
void TfrmEditParticles::InitItemsList(const char* nm)
{
	SendMessage(tvItems->Handle,WM_SETREDRAW,0,0);
    tvItems->Selected = 0;
    tvItems->Items->Clear();

    for(PSIt it=PSLib->FirstPS(); it!=PSLib->LastPS(); it++)
        AddItemToFolder(it->m_Folder,it->m_Name);
        
	SendMessage(tvItems->Handle,WM_SETREDRAW,1,0);
	tvItems->Repaint();
    if (nm){
		tvItems->Selected = tvItems->Items->LookForItem(0,nm,0,0,false,true,false,false,true);
        ResetCurrent();
        SetCurrent(PSLib->FindPS(nm));
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::tvItemsTryEdit(TObject *Sender,
      TElTreeItem *Item, TElHeaderSection *Section, TFieldTypes &CellType,
      bool &CanEdit){
    FEditNode = Item;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::tvItemsItemChange(TObject *Sender,
      TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
    if (FEditNode){
        for ( TElTreeItem* pNode = FEditNode->GetFirstChild(); pNode; pNode = FEditNode->GetNextChild(pNode))
            if (pNode->Data){
                PS::SDef* ps = PSLib->FindPS(pNode->Text.c_str());
                ps->SetFolder(FEditNode->Text.c_str());
                OnModified();
            }
        FEditNode = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) 		ebCancel->Click();
    else if (Key==VK_DELETE) 	ebRemovePS->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::ebPropertiesPSClick(TObject *Sender)
{
    TElTreeItem* pNode = tvItems->Selected;
    if (pNode && pNode->Data){
	    AnsiString nm = pNode->Text;
        if (PSLib->FindPS(nm.c_str())){
		    TfrmPropertiesPSDef::ShowProperties();
        }else{
			Log->DlgMsg(mtInformation, "Select PS to edit.");
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::ebSaveClick(TObject *Sender)
{
    if (!PSLib->Validate())
        MessageDlg("Validation failed! Incorrect Library!", mtError, TMsgDlgButtons() << mbOK, 0);
    else{
        ebSave->Enabled = false;
        // save
        PSLib->Backup();
        PSLib->Save();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::ebNewPSClick(TObject *Sender)
{
	sh_name name;
    PSLib->GenerateName(name);
    ResetCurrent();
    SetCurrent(PSLib->AddPS(name));
//	PSLib->Save();
    tvItems->Selected = AddItem(0,name);
	ebPropertiesPSClick(Sender);
    OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::ebClonePSClick(TObject *Sender)
{
    TElTreeItem* pNode = tvItems->Selected;
    if (pNode){
        PS::SDef* ps_src = PSLib->FindPS(pNode->Text.c_str()); VERIFY(ps_src);
        if (ps_src){
	        AnsiString folder=ps_src->m_Folder;
            AnsiString pref;
            sh_name name;
            pref = ps_src->m_Name;
            PSLib->GenerateName(name,pref.c_str());
		    ResetCurrent();
		    SetCurrent(PSLib->AddPS(name,ps_src));
            tvItems->Selected = AddItemToFolder(folder.c_str(), name);
		    TfrmPropertiesPSDef::ShowProperties();
            OnModified();
//            PSLib->Save();
        }
    }else{
		Log->DlgMsg(mtInformation, "At first selected item.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::ebRemovePSClick(TObject *Sender)
{
    TElTreeItem* pNode = tvItems->Selected;
    if (pNode){
    	if(pNode->Data){
	        if (Log->DlgMsg(mtConfirmation, "Delete selected item?") == mrYes){
				ResetCurrent();
    	        PSLib->DeletePS(pNode->Text.c_str());
//	    	    PSLib->Save();
	            pNode->Delete();
    	        OnModified();
        	}
        }else{
            if (Log->DlgMsg(mtConfirmation, "Delete selected folder and inside objects?") == mrYes){
                TElTreeItem* fld=tvItems->Selected;
                for ( TElTreeItem* pNode = fld->GetFirstChild(); pNode; pNode = fld->GetNextChild(pNode))
                    if (pNode->Data){
						ResetCurrent();
                        PSLib->DeletePS(pNode->Text.c_str());
                        OnModified();
                    }
                fld->Delete();
            }
        }
		SetFocus();
    }else{
		Log->DlgMsg(mtInformation, "At first selected item.");
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::ebCancelClick(TObject *Sender)
{
    HideEditor(true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::tvItemsDblClick(TObject *Sender)
{
	ebPropertiesPSClick(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditParticles::miNewFolderClick(TObject *Sender)
{
    TElTreeItem* pNode = AddFolder("New folder");
    pNode->EditText();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::miEditFolderClick(TObject *Sender)
{
    if (tvItems->Selected&&!tvItems->Selected->Data) tvItems->Selected->EditText();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::tvItemsDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
    TElTreeItem* node;
    node = tvItems->GetItemAtY(Y);
    if (node&&(!node->Data)&&(!node->IsUnder(FDragItem)))FDragItem->MoveTo(node);
    else if (node&&(!node->IsUnder(FDragItem)))          FDragItem->MoveTo(node->Parent);
    if (FDragItem&&FDragItem->Data){
        OnModified();
        node=(node->Parent)?node->Parent:node;
        PS::SDef* P = PSLib->FindPS(FDragItem->Text.c_str());
        VERIFY(P);
        P->SetFolder(node->Text.c_str());
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::tvItemsDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
    TElTreeItem* node;
    Accept = false;
    if (!FDragItem->Data) return;
    if (Source != tvItems) return;
    node = tvItems->GetItemAtY(Y);
    if (node&&(!node->IsUnder(FDragItem))) Accept = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::tvItemsStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
    FDragItem = tvItems->ItemFocused;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditParticles::tvItemsKeyPress(TObject *Sender, char &Key)
{
	if (Key==VK_DELETE)		ebRemovePSClick(Sender);
	else if (Key==VK_INSERT)ebNewPSClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::ebReloadPSClick(TObject *Sender)
{
	AnsiString ps_nm;
	if (tvItems->Selected) ps_nm = tvItems->Selected->Text;
	PSLib->Reload();
    ResetCurrent();
    InitItemsList(ps_nm.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::ebMergeClick(TObject *Sender)
{
	AnsiString fn;
    if( FS.GetOpenName( &FS.m_GameRoot, fn ) ){
    	int cnt=PSLib->Merge(fn.c_str());
        Log->DlgMsg(mtInformation,"Append %d PS(s)",cnt);
        AnsiString ps_nm;
        if (tvItems->Selected) ps_nm = tvItems->Selected->Text;
        ResetCurrent();
        InitItemsList(ps_nm.c_str());
//		PSLib->Save();
        OnModified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::ExtBtn2Click(TObject *Sender)
{
	VERIFY(form->m_TestObject);
	m_TestObject->Play();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditParticles::ExtBtn5Click(TObject *Sender)
{
	VERIFY(form->m_TestObject);
	m_TestObject->Stop();
}
//---------------------------------------------------------------------------

