#include "stdafx.h"
#pragma hdrstop

#include "LeftBar.h"
#include "BottomBar.h"
#include "UI_Main.h"
#include "main.h"
#include "xr_trims.h"
#include "UI_Tools.h"
#include "FolderLib.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma link "ElTree"
#pragma link "ElXPThemedControl"
#pragma link "ElTreeAdvEdit"
#pragma link "ElPgCtl"
#pragma link "MXCtrls"
#pragma resource "*.dfm"
TfraLeftBar *fraLeftBar;

#define MIN_PANEL_HEIGHT 15
//---------------------------------------------------------------------------
void __fastcall PanelMinimizeClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }else{
        pa->Tag    = pa->Height;
        pa->Height = MIN_PANEL_HEIGHT;
    }
    UI.Command(COMMAND_UPDATE_TOOLBAR);
}
void __fastcall PanelMaximizeOnlyClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }
    UI.Command(COMMAND_UPDATE_TOOLBAR);
}

//---------------------------------------------------------------------------
__fastcall TfraLeftBar::TfraLeftBar(TComponent* Owner)
        : TFrame(Owner)
{
	DEFINE_INI(fsStorage);

    InplaceParticleEdit->Editor->Color		= TColor(0x00A0A0A0);
    InplaceParticleEdit->Editor->BorderStyle= bsNone;
    DragItem = 0;
    frmMain->paLeftBar->Width = paLeftBar->Width+2;
    frmMain->sbToolsMin->Left = paLeftBar->Width-frmMain->sbToolsMin->Width-3;
}
//---------------------------------------------------------------------------

void UpdatePanel(TPanel* p){
    if (p){
        for (int j=0; j<p->ControlCount; j++){
            TExtBtn* btn = dynamic_cast<TExtBtn *>(p->Controls[j]);
            if (btn) btn->UpdateMouseInControl();
        }
    }
}
//---------------------------------------------------------------------------

void TfraLeftBar::UpdateBar(){
    int i, j;
    for (i=0; i<fraLeftBar->ComponentCount; i++){
        TComponent* temp = fraLeftBar->Components[i];
        if (dynamic_cast<TExtBtn *>(temp) != NULL)
            ((TExtBtn*)temp)->UpdateMouseInControl();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSaveClick(TObject *Sender)
{
	UI.Command( COMMAND_SAVE );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebReloadClick(TObject *Sender)
{
	UI.Command( COMMAND_RELOAD );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebRefreshTexturesClick(TObject *Sender)
{
	UI.Command( COMMAND_REFRESH_TEXTURES );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PanelMimimizeClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
    UpdateBar();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PanelMaximizeClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
    UpdateBar();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEditorPreferencesClick(TObject *Sender)
{
	UI.Command(COMMAND_EDITOR_PREF);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebResetAnimationClick(TObject *Sender)
{
	UI.Command( COMMAND_RESET_ANIMATION );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ShowPPMenu(TMxPopupMenu* M, TObject* B){
    POINT pt;
    GetCursorPos(&pt);
	M->Popup(pt.x,pt.y-10);
    TExtBtn* btn = dynamic_cast<TExtBtn*>(B); if(btn) btn->MouseManualUp();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEngineShaderFileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmEngineShadersFile,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneCommandsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmPreviewObject,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebImageCommandsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmImages,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvParticlesMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button==mbRight)	ShowPPMenu(pmShaderList,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PreviewClick(TObject *Sender)
{
	UI.Command( COMMAND_SELECT_PREVIEW_OBJ, dynamic_cast<TMenuItem*>(Sender)->Tag );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEngineApplyChangesClick(TObject *Sender)
{
	UI.Command( COMMAND_APPLY_CHANGES );
}
//---------------------------------------------------------------------------

void TfraLeftBar::ClearParticleList(){
	tvParticles->Items->Clear();
}
//---------------------------------------------------------------------------

void TfraLeftBar::AddPS(LPCSTR full_name, bool bLoadMode){
	TElTreeItem* node = FOLDER::AppendObject(tvParticles,full_name);
    if (!bLoadMode){
	    if (node&&node->Parent) node->Parent->Expand(false);
    	node->Selected = true;
		tvParticles->Selected = node;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CreateFolder1Click(TObject *Sender)
{
	AnsiString folder;
    AnsiString start_folder;
    FOLDER::MakeName(tvParticles->Selected,0,start_folder,true);
    FOLDER::GenerateFolderName(tvParticles,tvParticles->Selected,folder);
    folder = start_folder+folder;
	TElTreeItem* node = FOLDER::AppendFolder(tvParticles,folder.c_str());
    if (tvParticles->Selected) tvParticles->Selected->Expand(false);
    tvParticles->EditItem(node,-1);
	Tools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ExpandAll1Click(TObject *Sender)
{
	tvParticles->FullExpand();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CollapseAll1Click(TObject *Sender)
{
	tvParticles->FullCollapse();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebParticleShaderRemoveClick(TObject *Sender)
{
    TElTreeItem* pNode = tvParticles->Selected;
    if (pNode){
		AnsiString full_name;
    	if (FOLDER::IsFolder(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected folder?") == mrYes){
		        for (TElTreeItem* item=pNode->GetFirstChild(); item&&(item->Level>pNode->Level); item=item->GetNext()){
                    FOLDER::MakeName(item,0,full_name,false);
                	if (FOLDER::IsObject(item)) Tools.RemovePS(full_name.c_str());
                }
				Tools.ResetCurrentPS();
	            pNode->Delete();
                Tools.Modified();
        	}
        }
    	if (FOLDER::IsObject(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected item?") == mrYes){
				FOLDER::MakeName(pNode,0,full_name,false);
	            Tools.RemovePS(full_name.c_str());
				Tools.ResetCurrentPS();
	            pNode->Delete();
                Tools.Modified();
        	}
        }
    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvParticlesItemFocused(TObject *Sender)
{
	AnsiString name;
    FOLDER::MakeName(tvParticles->Selected, 0, name, false);
	Tools.SetCurrentPS(name.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebParticleCloneClick(TObject *Sender)
{
    TElTreeItem* pNode = tvParticles->Selected;
    if (pNode&&FOLDER::IsObject(pNode)){
		AnsiString full_name;
		FOLDER::MakeName(pNode,0,full_name,false);
        PS::SDef* P = Tools.ClonePS(full_name.c_str());
		Tools.SetCurrentPS(P);
		Tools.Modified();
    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvParticlesKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (Key==VK_DELETE) ebParticleShaderRemoveClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Rename1Click(TObject *Sender)
{
	TElTreeItem* node = tvParticles->Selected;
    if (node) tvParticles->EditItem(node,-1);
}
//---------------------------------------------------------------------------


void __fastcall TfraLeftBar::InplaceParticleEditValidateResult(
      TObject *Sender, bool &InputValid)
{
	TElTreeInplaceAdvancedEdit* IE=InplaceParticleEdit;

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
                Tools.RenamePS(full_name.c_str(),new_text.c_str(),node->Level);
            }
        }
    }else if (FOLDER::IsObject(node)){
        FOLDER::MakeName(node,0,full_name,false);
        Tools.RenamePS(full_name.c_str(),new_text.c_str(),node->Level);
    }
	Tools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebPSCreateClick(TObject *Sender)
{
    AnsiString folder;
	FOLDER::MakeName(tvParticles->Selected,0,folder,true);
    PS::SDef* S = Tools.AppendPS(folder.IsEmpty()?0:folder.c_str(),0);
	Tools.SetCurrentPS(S);
	Tools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvParticlesStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
	TElTree* tv = dynamic_cast<TElTree*>(Sender); VERIFY(Sender);
	if (tv->ItemFocused) 	DragItem = tv->ItemFocused;
  	else					DragItem = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvParticlesDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	TElTree* tv = dynamic_cast<TElTree*>(Sender); VERIFY(Sender);
	TElTreeItem* tgt;
    TElTreeItem* src=DragItem;
    TSTItemPart IP;
    int HCol;
    if (!DragItem) Accept = false;
  	else{
		tgt = tv->GetItemAt(X, Y, IP, HCol);
        if (tgt){
        	if (FOLDER::IsFolder(src)){
            	if (FOLDER::IsFolder(tgt)){
		        	Accept = (tgt!=src)&&(src->Parent!=tgt);
                }else if (FOLDER::IsObject(tgt)){
		        	Accept = (tgt!=src)&&(tgt->Parent!=src->Parent)&&(src!=tgt->Parent);
                }
            }else if (FOLDER::IsObject(src)){
            	if (FOLDER::IsFolder(tgt)){
		        	Accept = (tgt!=src)&&(src->Parent!=tgt);
                }else if (FOLDER::IsObject(tgt)){
		        	Accept = (tgt!=src)&&(src->Parent!=tgt->Parent);
                }
            }
        }else Accept = !!DragItem->Parent;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfraLeftBar::tvParticlesDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	TElTree* tv = dynamic_cast<TElTree*>(Sender); VERIFY(Sender);
	TElTreeItem* tgt_folder = tv->GetItemAt(X, Y, 0, 0);
    if (tgt_folder&&(FOLDER::IsObject(tgt_folder))) tgt_folder=tgt_folder->Parent;

    AnsiString base_name;
    FOLDER::MakeName(tgt_folder,0,base_name,true);
    AnsiString cur_fld_name=base_name;
    TElTreeItem* cur_folder=tgt_folder;

    int drg_level=DragItem->Level;

    TElTreeItem* item = DragItem;
    do{
    	DWORD type = DWORD(item->Data);
		TElTreeItem* pNode = FOLDER::FindItemInFolder(type,tvParticles,cur_folder,item->Text);
		if (pNode&&FOLDER::IsObject(item)){
            item=item->GetNext();
        	continue;
        }

        if (!pNode) pNode = tvParticles->Items->AddChildObject(cur_folder,item->Text,(TObject*)type);
		if (FOLDER::IsFolder(item)){
        	cur_folder = pNode;
		    FOLDER::MakeName(cur_folder,0,cur_fld_name,true);
            item=item->GetNext();
        }else{
        	// rename
		    AnsiString old_name, new_name;
		    FOLDER::MakeName(item,0,old_name,false);
		    FOLDER::MakeName(pNode,0,new_name,false);
            Tools.RenamePS(old_name.c_str(),new_name.c_str());
            Tools.Modified();

            TElTreeItem* parent=item->Parent;
            // get next item && delete existance
            TElTreeItem* next=item->GetNext();
			item->Delete();

            if (parent&&((parent->GetLastChild()==item)||(0==parent->ChildrenCount))){
	            if (0==parent->ChildrenCount) parent->Delete();
	        	cur_folder = cur_folder->Parent;
            }

            item=next;
        }
    }while(item&&(item->Level>drg_level));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCurrentPSPlayClick(TObject *Sender)
{
	Tools.PlayCurrentPS();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCurrentPSStopClick(TObject *Sender)
{
	Tools.StopCurrentPS();
}
//---------------------------------------------------------------------------


void __fastcall TfraLeftBar::ImageEditor1Click(TObject *Sender)
{
	UI.Command( COMMAND_IMAGE_EDITOR );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Refresh1Click(TObject *Sender)
{
	UI.Command( COMMAND_REFRESH_TEXTURES );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Checknewtextures1Click(TObject *Sender)
{
	UI.Command( COMMAND_CHECK_TEXTURES );
}
//---------------------------------------------------------------------------

