#include "stdafx.h"
#pragma hdrstop

#include "LeftBar.h"
#include "BottomBar.h"
#include "UI_Main.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma link "FrameProperties"
#pragma link "ElTree"
#pragma link "ElXPThemedControl"
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
    UI->Command(COMMAND_UPDATE_TOOLBAR);
}
void __fastcall PanelMaximizeOnlyClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }
    UI->Command(COMMAND_UPDATE_TOOLBAR);
}

//---------------------------------------------------------------------------
__fastcall TfraLeftBar::TfraLeftBar(TComponent* Owner)
        : TFrame(Owner)
{
    char buf[MAX_PATH] = {"shader_ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
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
    int i, j, h=0;
    for (i=0; i<fraLeftBar->ComponentCount; i++){
        TComponent* temp = fraLeftBar->Components[i];
        if (dynamic_cast<TExtBtn *>(temp) != NULL)
            ((TExtBtn*)temp)->UpdateMouseInControl();
    }
/*
    for (j=0; j<paLeftBar->ControlCount; j++){
        TPanel* pa = dynamic_cast<TPanel*>(paLeftBar->Controls[j]);
        if (pa&&pa->Visible) h+=pa->Height;
    }
    paLeftBar->Height = h+2;
*/
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebClearClick(TObject *Sender)
{
	UI->Command( COMMAND_CLEAR );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebLoadClick(TObject *Sender)
{
	UI->Command( COMMAND_LOAD );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSaveClick(TObject *Sender)
{
	UI->Command( COMMAND_SAVE );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebReloadClick(TObject *Sender)
{
	UI->Command( COMMAND_SAVEAS );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebRefreshTexturesClick(TObject *Sender)
{
	UI->Command( COMMAND_REFRESH_TEXTURES );
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
	UI->Command(COMMAND_EDITOR_PREF);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::sbPropertiesClick(TObject *Sender)
{
	UI->Command(COMMAND_SHOWPROPERTIES);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebResetAnimationClick(TObject *Sender)
{
	UI->Command( COMMAND_RESET_ANIMATION );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ShowPPMenu(TMxPopupMenu* M, TObject* B){
    POINT pt;
    GetCursorPos(&pt);
	M->Popup(pt.x,pt.y-10);
    TExtBtn* btn = dynamic_cast<TExtBtn*>(B); if(btn) btn->MouseManualUp();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneFileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmSceneFile,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneCommandsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmPreviewObject,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ElTree1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button==mbRight)	ShowPPMenu(pmShaderList,Sender);
}
//---------------------------------------------------------------------------


