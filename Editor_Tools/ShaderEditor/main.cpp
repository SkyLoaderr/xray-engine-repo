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
    fraBottomBar= new TfraBottomBar(0);
    fraLeftBar  = new TfraLeftBar(0);
    fraTopBar   = new TfraTopBar(0);
	fsMainForm->RestoreFormPlacement();
    if (!UI.Command(COMMAND_INITIALIZE)) exit(-1);
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
    fraLeftBar->fsStorage->SaveFormPlacement();
    fraBottomBar->fsStorage->SaveFormPlacement();
    fraTopBar->fsStorage->SaveFormPlacement();

    UI.Command(COMMAND_DESTROY);

    fraTopBar->Parent       = 0;
    fraLeftBar->Parent      = 0;
    fraBottomBar->Parent    = 0;
    _DELETE(fraLeftBar);
    _DELETE(fraTopBar);
    _DELETE(fraBottomBar);

    Application->OnIdle     = 0;
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
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
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
    if (g_bEditorValid) UI.Idle();
}
void __fastcall TfrmMain::D3DWindowResize(TObject *Sender)
{
    if (g_bEditorValid) UI.Resize();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    ShiftKey = Shift;
    if (g_bEditorValid) if (!UI.KeyDown(Key, Shift)){UI.ApplyShortCut(Key, Shift);}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (g_bEditorValid) if (!UI.KeyUp(Key, Shift)){;}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyPress(TObject *Sender, char &Key)
{
    if (g_bEditorValid) if (!UI.KeyPress(Key, ShiftKey)){;}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (g_bEditorValid&&!D3DWindow->Focused()) UI.ApplyGlobalShortCut(Key, Shift);
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
    if (g_bEditorValid) UI.RedrawScene();
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::fsMainFormRestorePlacement(TObject *Sender)
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
	if (!g_bEditorValid) return;
	if (D3DWindow->Focused()){
     	paWindow->Color=TColor(0x090FFFF);
		// ���� �������� �����, � �� ����� �������� ������ -> ������� ������� MouseUp
        if (UI.IsMouseInUse())
            UI.OnMouseRelease(0);
        UI.iCapture();
    }else{
        UI.iRelease();
    	paWindow->Color=clGray;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormActivate(TObject *Sender)
{
	UI.OnAppActivate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormDeactivate(TObject *Sender)
{
	UI.OnAppDeactivate();
}
//---------------------------------------------------------------------------


