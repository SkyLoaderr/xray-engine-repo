//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "main.h"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TopBar"
#pragma link "RenderWindow"
#pragma link "ExtBtn"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"
#include "ui_main.h"
#include "SceneClassList.h"
#include "ui_tools.h"
#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"
#include "EditorPref.h"
#include "xr_input.h"

//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
        : TForm(Owner)
{
    UI = new TUI();
    fraBottomBar= new TfraBottomBar(0);
    fraLeftBar  = new TfraLeftBar(0);
    fraTopBar   = new TfraTopBar(0);
	fsMainForm->RestoreFormPlacement();
	if (!UI->Init(D3DWindow)) exit(-1);
    pInput		= new CInput(FALSE,mouse_device_key);
    UI->iCapture();
	Device.InitTimer();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormShow(TObject *Sender)
{
    fraBottomBar->Parent    = paBottomBar;
    fraTopBar->Parent       = paTopBar;
    fraLeftBar->Parent      = paLeftBar;
    if (paLeftBar->Tag > 0) paLeftBar->Parent = paTopBar;
    else paLeftBar->Parent = frmMain;

    tmRefresh->Enabled = true; tmRefreshTimer(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    fraLeftBar->fsStorage->SaveFormPlacement();
    fraBottomBar->fsStorage->SaveFormPlacement();
    fraTopBar->fsStorage->SaveFormPlacement();

	UI->iRelease			();
    _DELETE(UI);
    fraTopBar->Parent       = 0;
    fraLeftBar->Parent      = 0;
    fraBottomBar->Parent    = 0;
    _DELETE(fraLeftBar);
    _DELETE(fraTopBar);
    _DELETE(fraBottomBar);
    _DELETE(pInput);

    Application->OnIdle     = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    tmRefresh->Enabled = false;
    CanClose = UI->Command(COMMAND_EXIT);
    if (!CanClose) tmRefresh->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
    char buf[MAX_PATH] = {"shader_ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsMainForm->IniFileName = buf;
    Application->OnIdle = IdleHandler;
}

//---------------------------------------------------------------------------

#define MIN_PANEL_HEIGHT 17
void __fastcall TfrmMain::sbToolsMinClick(TObject *Sender)
{
    if (paLeftBar->Tag > 0){
        paLeftBar->Parent = frmMain;
        paLeftBar->Tag    = 0;
    }else{
        paLeftBar->Parent = paTopBar;//D3DWindow;
        paLeftBar->Tag    = 1;//paLeftBar->Height;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::TopClick(TObject *Sender)
{
    if (paLeftBar->Tag > 0){
        paLeftBar->Align  = alRight;
        paLeftBar->Parent = frmMain;
        paLeftBar->Height = paLeftBar->Tag;
        paLeftBar->Tag    = 0;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::IdleHandler(TObject *Sender, bool &Done)
{
    Done = false;
    pInput->OnFrame();
    if (g_bEditorValid) UI->Idle();
}
void __fastcall TfrmMain::D3DWindowResize(TObject *Sender)
{
    if (g_bEditorValid) UI->Resize();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    ShiftKey = Shift;
    if (g_bEditorValid) if (!UI->KeyDown(Key, Shift)){ApplyShortCut(Key, Shift);}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (g_bEditorValid) if (!UI->KeyUp(Key, Shift)){;}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyPress(TObject *Sender, char &Key)
{
    if (g_bEditorValid) if (!UI->KeyPress(Key, ShiftKey)){;}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (g_bEditorValid&&!D3DWindow->Focused()) ApplyGlobalShortCut(Key, Shift);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ApplyShortCut(WORD Key, TShiftState Shift)
{
//    if (Key==VK_ESCAPE)   		UI->Command(COMMAND_CHANGE_ACTION, eaSelect);
    if (Shift.Contains(ssCtrl)){
        if (Key=='O')   		UI->Command(COMMAND_LOAD);
        else if (Key=='N')   	UI->Command(COMMAND_CLEAR);
        else if (Key=='S'){ 	if (Shift.Contains(ssAlt))  UI->Command(COMMAND_SAVEAS);
					            else                        UI->Command(COMMAND_SAVE);}
    }else{
        if (Shift.Contains(ssAlt)){
        }else{
        	if (Key=='P')				UI->Command(COMMAND_EDITOR_PREF);
        	else if (Key==VK_RETURN)    UI->Command(COMMAND_SHOWPROPERTIES);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
    if (Shift.Contains(ssCtrl)){
        if (Key=='S'){
            if (Shift.Contains(ssAlt))  UI->Command(COMMAND_SAVEAS);
            else                        UI->Command(COMMAND_SAVE);
        }
        else if (Key=='O')   			UI->Command(COMMAND_LOAD);
        else if (Key=='N')   			UI->Command(COMMAND_CLEAR);
    }
    if (Key==VK_OEM_3)		  			UI->Command(COMMAND_RENDER_FOCUS);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::UpdateCaption()
{
    AnsiString name;
    name.sprintf("Shader Editor - [%s%s]",UI->GetEditFileName()[0]?UI->GetEditFileName():"noname","");//Scene->IsModified()?"*":"");
    Caption = name;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmRefreshTimer(TObject *Sender)
{
    int i;
    for (i=0; i<frmMain->ComponentCount; i++){
        TComponent* temp = frmMain->Components[i];
        if (dynamic_cast<TExtBtn *>(temp) != NULL)
            ((TExtBtn*)temp)->UpdateMouseInControl();
    }
    fraLeftBar->UpdateBar();
    fraTopBar->OnTimer();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowPaint(TObject *Sender)
{
    if (g_bEditorValid) UI->RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miPropertiesClick(TObject *Sender)
{
    UI->Command(COMMAND_SHOWPROPERTIES);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::fsMainFormRestorePlacement(TObject *Sender)
{
    fraLeftBar->fsStorage->RestoreFormPlacement();
    fraBottomBar->fsStorage->RestoreFormPlacement();
    fraTopBar->fsStorage->RestoreFormPlacement();
}
//---------------------------------------------------------------------------

void ResetActionToSelect()
{
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paWindowResize(TObject *Sender)
{
	D3DWindow->Width = paWindow->Width-2;
	D3DWindow->Height = paWindow->Height-2;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowChangeFocus(TObject *Sender)
{
	if (D3DWindow->Focused()){
     	paWindow->Color=TColor(0x090FFFF);
		// если потеряли фокус, а до этого кликнули мышкой -> вызовим событие MouseUp
        if (UI->IsMouseInUse())
            UI->OnMouseRelease(0);
        if (UI) UI->iCapture();
    }else{
        if (UI) UI->iRelease();
    	paWindow->Color=clGray;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormActivate(TObject *Sender)
{
	pInput->OnAppActivate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormDeactivate(TObject *Sender)
{
	pInput->OnAppDeactivate();
}
//---------------------------------------------------------------------------


