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
    frmMain->paLeftBar->Width = paLeftBar->Width+2;
    frmMain->sbToolsMin->Left = paLeftBar->Width-frmMain->sbToolsMin->Width-3;
    tvParticles->OnStartDrag 	= FHelper.StartDrag;
    tvParticles->OnDragOver 	= FHelper.DragOver;
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
	TElTreeItem* node = FHelper.AppendObject(tvParticles,full_name);
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
    FHelper.MakeName(tvParticles->Selected,0,start_folder,true);
    FHelper.GenerateFolderName(tvParticles,tvParticles->Selected,folder);
    folder = start_folder+folder;
	TElTreeItem* node = FHelper.AppendFolder(tvParticles,folder.c_str());
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

BOOL __fastcall TfraLeftBar::RemoveItem(LPCSTR p0)
{
	Tools.RemovePS(p0);
    return TRUE;
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::AfterRemoveItem()
{
	Tools.ResetCurrentPS();
	Tools.Modified();
}

void __fastcall TfraLeftBar::ebParticleRemoveClick(TObject *Sender)
{
	FHelper.RemoveItem(tvParticles,tvParticles->Selected,RemoveItem);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvParticlesItemFocused(TObject *Sender)
{
	AnsiString name;
    FHelper.MakeName(tvParticles->Selected, 0, name, false);
	Tools.SetCurrentPS(name.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebParticleCloneClick(TObject *Sender)
{
    TElTreeItem* pNode = tvParticles->Selected;
    if (pNode&&FHelper.IsObject(pNode)){
		AnsiString full_name;
		FHelper.MakeName(pNode,0,full_name,false);
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
	if (Key==VK_DELETE) ebParticleRemoveClick(Sender);
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
    if (FHelper.IsFolder(node)){
        for (item=node->GetFirstChild(); item&&(item->Level>node->Level); item=item->GetNext()){
            if (FHelper.IsObject(item)){
                FHelper.MakeName(item,0,full_name,false);
                Tools.RenamePS(full_name.c_str(),new_text.c_str(),node->Level);
            }
        }
    }else if (FHelper.IsObject(node)){
        FHelper.MakeName(node,0,full_name,false);
        Tools.RenamePS(full_name.c_str(),new_text.c_str(),node->Level);
    }
	Tools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebPSCreateClick(TObject *Sender)
{
    AnsiString folder;
	FHelper.MakeName(tvParticles->Selected,0,folder,true);
    PS::SDef* S = Tools.AppendPS(folder.IsEmpty()?0:folder.c_str(),0);
	Tools.SetCurrentPS(S);
	Tools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::RenameItem(LPCSTR p0, LPCSTR p1)
{
	Tools.RenamePS(p0,p1);
	Tools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::OnDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	FHelper.DragDrop(Sender,Source,X,Y,RenameItem);
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


