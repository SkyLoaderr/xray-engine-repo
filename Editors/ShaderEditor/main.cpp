//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "main.h"
#include "ui_main.h"
#include "editorpref.h"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RenderWindow"
#pragma resource "*.dfm"

#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"

//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
        : TForm(Owner)
{
	Device.SetHandle		(Handle,D3DWindow->Handle);
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
	if (Key==VK_MENU) Key=0;
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
	psDeviceFlags.set(fsStorage->ReadInteger("Device Flags",rsStatistic|rsFilterLinear|rsFog|rsDrawGrid));
    fraLeftBar->fsStorage->RestoreFormPlacement();
    fraBottomBar->fsStorage->RestoreFormPlacement();
    fraTopBar->fsStorage->RestoreFormPlacement();
	// read recent list    
    for (int i=frmEditPrefs->seRecentFilesCount->Value; i>=0; i--){
		AnsiString recent_fn= frmMain->fsStorage->ReadString	(AnsiString("RecentFiles")+AnsiString(i),"");
        if (!recent_fn.IsEmpty()) UI.AppendRecentFile(recent_fn.c_str());
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::fsStorageSavePlacement(TObject *Sender)
{
	fsStorage->WriteInteger("Device Flags",psDeviceFlags.get());
	// save recent files
#ifdef _HAVE_RECENT_FILES
	for (int i = 0; i < fraLeftBar->miRecentFiles->Count; i++)
		fsStorage->WriteString(AnsiString("RecentFiles")+AnsiString(i),fraLeftBar->miRecentFiles->Items[i]->Caption);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paWindowResize(TObject *Sender)
{
	if (psDeviceFlags.is(rsDrawSafeRect)){
    	int w=paWindow->Width,h=paWindow->Height,w_2=w/2,h_2=h/2;
        Irect rect;
        if ((0.75f*float(w))>float(h)) 	rect.set(w_2-1.33f*float(h_2),0,1.33f*h,h);
        else                   			rect.set(0,h_2-0.75f*float(w_2),w,0.75f*w);
        D3DWindow->Left  	= rect.x1;
        D3DWindow->Top  	= rect.y1;
        D3DWindow->Width 	= rect.x2;
        D3DWindow->Height	= rect.y2;
    }else{
	    D3DWindow->Left  	= 0;
	    D3DWindow->Top  	= 0;
    	D3DWindow->Width 	= paWindow->Width;
    	D3DWindow->Height	= paWindow->Height;
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::D3DWindowChangeFocus(TObject *Sender)
{
	if (!UI.m_bReady) return;
	if (D3DWindow->Focused()){
//     	paWindow->Color=TColor(0x090FFFF);
		// ���� �������� �����, � �� ����� �������� ������ -> ������� ������� MouseUp
//        if (UI.IsMouseInUse())
//            UI.OnMouseRelease(0);
        UI.IR_Capture();
		UI.OnAppActivate();
    }else{
		UI.OnAppDeactivate();
        UI.IR_Release();
        paWindow->Color=paWindow->Color; // ����� �� ����  internal code gen error
//    	paWindow->Color=(TColor)0x00202020;
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

void __fastcall TfrmMain::D3DWindowMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    UI.MousePress(Shift,X,Y);
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    UI.MouseRelease(Shift,X,Y);
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    UI.MouseMove(Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ebAllMinClick(TObject *Sender)
{
    fraLeftBar->MinimizeAllFrames();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ebAllMaxClick(TObject *Sender)
{
    fraLeftBar->MaximizeAllFrames();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormResize(TObject *Sender)
{
    if (fraLeftBar) fraLeftBar->UpdateBar();
}
//---------------------------------------------------------------------------

