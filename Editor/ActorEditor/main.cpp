//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "main.h"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include "ui_main.h"
#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"

//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
        : TForm(Owner)
{
    if (!UI.Command(COMMAND_INITIALIZE)) TerminateProcess(GetCurrentProcess(),-1);
	fsStorage->RestoreFormPlacement();
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
    UI.Command				(COMMAND_UPDATE_GRID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    Application->OnIdle     = 0;

    fraLeftBar->fsStorage->SaveFormPlacement();
    fraBottomBar->fsStorage->SaveFormPlacement();
    fraTopBar->fsStorage->SaveFormPlacement();

    fraTopBar->Parent       = 0;
    fraLeftBar->Parent      = 0;
    fraBottomBar->Parent    = 0;

    UI.Command(COMMAND_DESTROY);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    tmRefresh->Enabled = false;
    CanClose = UI.Command(COMMAND_EXIT);
    if (!CanClose) tmRefresh->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	DEFINE_INI(fsStorage);
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
    UI.Idle();
}
void __fastcall TfrmMain::D3DWindowResize(TObject *Sender)
{
    UI.Resize();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    ShiftKey = Shift;
    if (!UI.KeyDown(Key, Shift)){UI.ApplyShortCut(Key, Shift);}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (!UI.KeyUp(Key, Shift)){;}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyPress(TObject *Sender, char &Key)
{
    if (!UI.KeyPress(Key, ShiftKey)){;}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (!D3DWindow->Focused()) UI.ApplyGlobalShortCut(Key, Shift);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::UpdateCaption()
{
    AnsiString name;
    name.sprintf("%s - [%s%s]",UI.GetTitle(),UI.GetCaption(),UI.IsModified()?"*":"");
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
    UI.RedrawScene();
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::fsStorageRestorePlacement(TObject *Sender)
{
    fraLeftBar->fsStorage->RestoreFormPlacement();
    fraBottomBar->fsStorage->RestoreFormPlacement();
    fraTopBar->fsStorage->RestoreFormPlacement();
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
	if (!UI.m_bReady) return;
	if (D3DWindow->Focused()){
     	paWindow->Color=TColor(0x090FFFF);
		// ���� �������� �����, � �� ����� �������� ������ -> ������� ������� MouseUp
//        if (UI.IsMouseInUse())
            UI.OnMouseRelease(0);
        UI.iCapture();
		UI.OnAppActivate();
    }else{
		UI.OnAppDeactivate();
        UI.iRelease();
    	paWindow->Color=0x00A0A6A9;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormActivate(TObject *Sender)
{
//	UI.OnAppActivate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormDeactivate(TObject *Sender)
{
//	UI.OnAppDeactivate();
}
//---------------------------------------------------------------------------


