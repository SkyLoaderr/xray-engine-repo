//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "main.h"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXCtrls"
#pragma package(smart_init)
#pragma link "RxMenus"
#pragma link "Placemnt"
#pragma link "TopBar"
#pragma link "RenderWindow"
#pragma resource "*.dfm"
#include "ui_main.h"
#include "library.h"
#include "scene.h"
#include "SceneClassList.h"
#include "ui_tools.h"
#include "statistic.h"
#include "statisticform.h"
#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"
#include "EditorPref.h"

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
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormShow(TObject *Sender)
{
    fraBottomBar->Parent    = paBottomBar;
    fraTopBar->Parent       = paTopBar;
    fraLeftBar->Parent      = paLeftBar;
    UI->Command(COMMAND_CHANGE_TARGET, etObject);
    UI->Command(COMMAND_CHANGE_ACTION, eaSelect);
    if (paLeftBar->Tag > 0) paLeftBar->Parent = D3DWindow;
    else paLeftBar->Parent = frmMain;

    tmRefresh->Enabled = true; tmRefreshTimer(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	TfrmStatistic::HideStatistic();

    fraLeftBar->fsStorage->SaveFormPlacement();
    fraBottomBar->fsStorage->SaveFormPlacement();
    fraTopBar->fsStorage->SaveFormPlacement();

    _DELETE(UI);
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
    CanClose = UI->Command(COMMAND_EXIT);
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
        paLeftBar->Align  = alRight;
        paLeftBar->Parent = frmMain;
        paLeftBar->Height = paLeftBar->Tag;
        paLeftBar->Tag    = 0;
    }else{
        paLeftBar->Align  = alNone;
        paLeftBar->Parent = D3DWindow;
        paLeftBar->Tag    = paLeftBar->Height;
        paLeftBar->Height = MIN_PANEL_HEIGHT;
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
    if (g_bEditorValid) UI->Idle();
}
void __fastcall TfrmMain::D3DWindowResize(TObject *Sender)
{
    if (g_bEditorValid) UI->Resize();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (!g_bEditorValid) return;

    ShiftMouse = Shift;
    UI->MouseStart(Shift,X,Y);
    UI->RedrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::D3DWindowMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if (!g_bEditorValid) return;
    UI->MouseProcess(Shift,X,Y);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::D3DWindowMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (!g_bEditorValid) return;
    UI->MouseEnd(Shift,X,Y);
    UI->RedrawScene();
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
    if (Key==VK_ESCAPE)   		UI->Command(COMMAND_CHANGE_ACTION, eaSelect);
    if (Shift.Contains(ssCtrl)){
        if (Key=='V')    		UI->Command(COMMAND_PASTE);
        else if (Key=='C')    	UI->Command(COMMAND_COPY);
        else if (Key=='X')    	UI->Command(COMMAND_CUT);
        else if (Key=='Z')    	UI->Command(COMMAND_UNDO);
        else if (Key=='Y')    	UI->Command(COMMAND_REDO);
        else if (Key==VK_F5)    UI->Command(COMMAND_BUILD);
        else if (Key==VK_F7)    UI->Command(COMMAND_OPTIONS);
        else if (Key=='A')    	UI->Command(COMMAND_SELECT_ALL);
        else if (Key=='I')    	UI->Command(COMMAND_INVERT_SELECTION_ALL);
        else if (Key=='O')   	UI->Command(COMMAND_LOAD);
        else if (Key=='N')   	UI->Command(COMMAND_CLEAR);
        else if (Key=='S'){ 	if (Shift.Contains(ssAlt))  UI->Command(COMMAND_SAVEAS);
					            else                        UI->Command(COMMAND_SAVE);}
       	else if (Key=='1') 	 	UI->Command(COMMAND_CHANGE_TARGET, etEvent);
		else if (Key=='2')		UI->Command(COMMAND_CHANGE_TARGET, etPS);
        else if (Key=='3')		UI->Command(COMMAND_CHANGE_TARGET, etDPatch);
    }else{
        if (Shift.Contains(ssAlt)){
        }else{
            if (Key=='1')     	UI->Command(COMMAND_CHANGE_TARGET, etObject);
        	else if (Key=='2')  UI->Command(COMMAND_CHANGE_TARGET, etLight);
        	else if (Key=='3')  UI->Command(COMMAND_CHANGE_TARGET, etSound);
        	else if (Key=='4')  UI->Command(COMMAND_CHANGE_TARGET, etOccluder);
        	else if (Key=='5')  UI->Command(COMMAND_CHANGE_TARGET, etGlow);
        	else if (Key=='6')  UI->Command(COMMAND_CHANGE_TARGET, etDO);
        	else if (Key=='7')  UI->Command(COMMAND_CHANGE_TARGET, etRPoint);
        	else if (Key=='8')  UI->Command(COMMAND_CHANGE_TARGET, etAITPoint);
        	else if (Key=='9')  UI->Command(COMMAND_CHANGE_TARGET, etSector);
        	else if (Key=='0')  UI->Command(COMMAND_CHANGE_TARGET, etPortal);
            // simple press
        	else if (Key=='S')	UI->Command(COMMAND_CHANGE_ACTION, eaSelect);
        	else if (Key=='A')	UI->Command(COMMAND_CHANGE_ACTION, eaAdd);
        	else if (Key=='T')	UI->Command(COMMAND_CHANGE_ACTION, eaMove);
        	else if (Key=='Y')	UI->Command(COMMAND_CHANGE_ACTION, eaRotate);
        	else if (Key=='H')	UI->Command(COMMAND_CHANGE_ACTION, eaScale);
        	else if (Key=='Z')	UI->Command(COMMAND_CHANGE_AXIS,   eAxisX);
        	else if (Key=='X')	UI->Command(COMMAND_CHANGE_AXIS,   eAxisY);
        	else if (Key=='C')	UI->Command(COMMAND_CHANGE_AXIS,   eAxisZ);
        	else if (Key=='V')	UI->Command(COMMAND_CHANGE_AXIS,   eAxisZX);
        	else if (Key=='O')	UI->Command(COMMAND_CHANGE_SNAP,   (int)fraTopBar->ebOSnap);
        	else if (Key=='G')	UI->Command(COMMAND_CHANGE_SNAP,   (int)fraTopBar->ebGSnap);
        	else if (Key=='P')	UI->Command(COMMAND_EDITOR_PREF);
        	else if (Key=='W')	UI->Command(COMMAND_OBJECT_LIST);
        	else if (Key==VK_DELETE)	UI->Command(COMMAND_DELETE_SELECTION);
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
    name.sprintf("Level Editor - [%s%s]",UI->GetEditFileName()[0]?UI->GetEditFileName():"noname",Scene->IsModified()?"*":"");
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

void __fastcall TfrmMain::miHideSelectedClick(TObject *Sender)
{
	UI->Command(COMMAND_HIDE_SEL,FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::miHideAllClick(TObject *Sender)
{
	UI->Command(COMMAND_HIDE_ALL,FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::miUnhideAllClick(TObject *Sender)
{
	UI->Command(COMMAND_HIDE_ALL,TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::HideUnselected2Click(TObject *Sender)
{
	UI->Command(COMMAND_HIDE_UNSEL);
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

void __fastcall TfrmMain::ShowContextMenu(const EObjClass cls){
    if (g_bEditorValid){
        POINT pt;
        GetCursorPos(&pt);
		miProperties->Enabled = false;
        if (Scene->SelectionCount( true, cls )) miProperties->Enabled = true;
        UI->RedrawScene(true);
	    pmObjectContext->TrackButton = tbRightButton;
        pmObjectContext->Popup(pt.x,pt.y);
    }
}
//---------------------------------------------------------------------------

void ResetActionToSelect()
{
    UI->Command(COMMAND_CHANGE_ACTION, eaSelect);
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
        if (UI->IsMouseInUse()){
			// если потеряли фокус, а до этого кликнули мышкой -> вызовим событие MouseUp
        	POINT pt; GetCursorPos(&pt); pt=D3DWindow->ScreenToClient(pt);
        	D3DWindow->OnMouseUp(this,mbLeft,ShiftMouse,pt.x,pt.y); // mbLeft - не важно что передавать
        }
    }else{
    	paWindow->Color=clGray;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miCopyClick(TObject *Sender)
{
    UI->Command(COMMAND_COPY);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miPasteClick(TObject *Sender)
{
    UI->Command(COMMAND_PASTE);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miCutClick(TObject *Sender)
{
    UI->Command(COMMAND_CUT);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Position1Click(TObject *Sender)
{
    UI->Command(COMMAND_SET_NUMERIC_POSITION);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Rotation1Click(TObject *Sender)
{
    UI->Command(COMMAND_SET_NUMERIC_ROTATION);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Scale1Click(TObject *Sender)
{
    UI->Command(COMMAND_SET_NUMERIC_SCALE);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::LockAll1Click(TObject *Sender)
{
	UI->Command(COMMAND_LOCK_ALL,TRUE);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::LockUnselected1Click(TObject *Sender)
{
	UI->Command(COMMAND_LOCK_UNSEL,TRUE);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::UnlockAll1Click(TObject *Sender)
{
	UI->Command(COMMAND_LOCK_ALL,FALSE);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::UnlockUnselected1Click(TObject *Sender)
{
	UI->Command(COMMAND_LOCK_UNSEL,FALSE);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::UnlockSelected1Click(TObject *Sender)
{
	UI->Command(COMMAND_LOCK_SEL,FALSE);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::LockSelected1Click(TObject *Sender)
{
	UI->Command(COMMAND_LOCK_SEL,TRUE);
}
//---------------------------------------------------------------------------

