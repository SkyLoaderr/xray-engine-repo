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
#pragma link "ElTree"
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

void __fastcall TfraLeftBar::ebSceneCommandsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmPreviewObject,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebImageCommandsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmImages,dynamic_cast<TExtBtn*>(Sender));
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

void __fastcall TfraLeftBar::ebEngineShaderRemoveClick(TObject *Sender)
{
//.	FHelper.RemoveItem(tvParticles,tvParticles->Selected,RemoveItem);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebParticleCloneClick(TObject *Sender)
{
/*
    TElTreeItem* pNode = tvParticles->Selected;
    if (pNode&&FHelper.IsObject(pNode)){
		AnsiString full_name;
		FHelper.MakeName(pNode,0,full_name,false);
        PS::SDef* P = Tools.FindPS(full_name.c_str());
        if (P){
	        PS::SDef* C = Tools.ClonePS(full_name.c_str());
			Tools.SetCurrentPS(C);        
			Tools.Modified();
        }
    }else{                                    
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
*/
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebPSCreateClick(TObject *Sender)
{
	Tools.AppendPS(0);
	Tools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebPECreateClick(TObject *Sender)
{
    Tools.AppendPE(0);
	Tools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebPGCreateClick(TObject *Sender)
{
    Tools.AppendPG(0);
	Tools.Modified();
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

void __fastcall TfraLeftBar::ExtBtn3Click(TObject *Sender)
{
	Tools.ResetState();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebFileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmEngineShadersFile,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCreateMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmCreateMenu,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::fsStorageRestorePlacement(TObject *Sender)
{
    Tools.m_ItemProps->RestoreParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::fsStorageSavePlacement(TObject *Sender)
{
    Tools.m_ItemProps->SaveParams(fsStorage);
}
//---------------------------------------------------------------------------


