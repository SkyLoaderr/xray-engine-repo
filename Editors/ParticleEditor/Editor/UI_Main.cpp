//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "leftbar.h"
#include "main.h"
#include "xr_input.h"
#include "UI_Tools.h"
#include "xr_trims.h"
#include "bottombar.h"
#include "xr_ini.h"
#ifdef _LEVEL_EDITOR
 	#include "cursor3d.h"
#endif

#include "ui_main.h"
#include "d3dutils.h"
#include "editorpref.h"

TUI UI;

TUI::TUI()
{
    m_bAppActive = false;
	m_bReady = false;
    m_D3DWindow = 0;
    bNeedAbort   = false;

	m_StartRStart.set(0,0,0);
	m_StartRNorm.set(0,0,0);
	m_CurrentRStart.set(0,0,0);
	m_CurrentRNorm.set(0,0,0);

#ifdef _LEVEL_EDITOR
    m_Cursor        = 0;
#endif

    bRedraw = false;
	bResize = true;
    bUpdateScene = false;

	m_Pivot.set( 0, 0, 0 );
    m_MoveSnap = 0.1f;
	m_AngleSnap = deg2rad( 5.f );

	m_MouseCaptured = false;
    m_MouseMultiClickCaptured = false;
 	m_SelectionRect = false;
    bMouseInUse		= false;

	m_LastFileName[0]= 0;

    m_bHintShowing	= false;
	m_pHintWindow	= 0;
	m_LastHint		= "";
}
//---------------------------------------------------------------------------
TUI::~TUI()
{
    VERIFY(m_EditorState.size()==0);
#ifdef _LEVEL_EDITOR
    _DELETE(m_Cursor);
#endif
}

void TUI::OnDeviceCreate()
{
	DU::OnDeviceCreate();
}

void TUI::OnDeviceDestroy()
{
	DU::OnDeviceDestroy();
}

bool TUI::OnCreate(){
// create base class
	Device.InitTimer();

    m_D3DWindow 	= frmMain->D3DWindow;
    VERIFY(m_D3DWindow);
    Device.Initialize();

#ifdef _LEVEL_EDITOR
    m_Cursor        = new C3DCursor();
#endif
	// Creation
	XRC.ray_options	(CDB::OPT_ONLYNEAREST | CDB::OPT_CULL);

    pInput			= new CInput(FALSE,mouse_device_key);
    UI.iCapture		();

    // game configure
    AnsiString si_name = "system.ltx";
    Engine.FS.m_GameRoot.Update(si_name);
	pSettings		= new CInifile(si_name.c_str(),TRUE);// FALSE,TRUE,TRUE);

    m_bReady		= true;

    Engine.FS.WriteAccessLog(_EDITOR_FILE_NAME_,"Started");
    
    return true;
}

void TUI::OnDestroy()
{
    Engine.FS.WriteAccessLog(_EDITOR_FILE_NAME_,"Finished");

	VERIFY(m_bReady);
	m_bReady		= false;
    _DELETE			(pSettings);
	UI.iRelease		();
    _DELETE			(pInput);
    EndEState		();

    Device.ShutDown	();
}

bool TUI::IsModified()
{
	return Command(COMMAND_CHECK_MODIFIED);
}
//---------------------------------------------------------------------------

void TUI::EnableSelectionRect( bool flag ){
	m_SelectionRect = flag;
	m_SelEnd.x = m_SelStart.x = 0;
	m_SelEnd.y = m_SelStart.y = 0;
}

void TUI::UpdateSelectionRect( const Ipoint& from, const Ipoint& to ){
	m_SelStart.set(from);
	m_SelEnd.set(to);
}

bool __fastcall TUI::KeyDown (WORD Key, TShiftState Shift)
{
	if (!m_bReady) return false;
//	m_ShiftState = Shift;
//	Log("Dn  ",Shift.Contains(ssShift)?"1":"0");
	if (Device.m_Camera.KeyDown(Key,Shift)) return true;
    return Tools.KeyDown(Key, Shift);
}

bool __fastcall TUI::KeyUp   (WORD Key, TShiftState Shift)
{
	if (!m_bReady) return false;
//	m_ShiftState = Shift;
	if (Device.m_Camera.KeyUp(Key,Shift)) return true;
    return Tools.KeyUp(Key, Shift);
}

bool __fastcall TUI::KeyPress(WORD Key, TShiftState Shift)
{
	if (!m_bReady) return false;
    return Tools.KeyPress(Key, Shift);
}
//----------------------------------------------------

void TUI::MousePress(TShiftState Shift, int X, int Y)
{
	if (!m_bReady) return;
    if (m_MouseCaptured) return;

    bMouseInUse = true;

    m_ShiftState = Shift;

    // camera activate
    if(!Device.m_Camera.MoveStart(m_ShiftState)){
    	if (Tools.Pick()) return;
        if( !m_MouseCaptured ){
            if( Tools.HiddenMode() ){
				iGetMousePosScreen(m_StartCpH);
                m_DeltaCpH.set(0,0);
            }else{
                iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
                m_StartCp = m_CurrentCp;

                Device.m_Camera.MouseRayFromPoint(m_StartRStart, m_StartRNorm, m_StartCp );
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart, m_CurrentRNorm, m_CurrentCp );
            }

            if(Tools.MouseStart(m_ShiftState)){
                if(Tools.HiddenMode()) ShowCursor( FALSE );
                m_MouseCaptured = true;
            }
        }
    }
    RedrawScene();
}

void TUI::MouseRelease(TShiftState Shift, int X, int Y)
{
	if (!m_bReady) return;

    m_ShiftState = Shift;

    if( Device.m_Camera.IsMoving() ){
        if (Device.m_Camera.MoveEnd(m_ShiftState)) bMouseInUse = false;
    }else{
	    bMouseInUse = false;
        if( m_MouseCaptured ){
            if( !Tools.HiddenMode() ){
                iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp );
            }
            if( Tools.MouseEnd(m_ShiftState) ){
                if( Tools.HiddenMode() ){
                    SetCursorPos(m_StartCpH.x,m_StartCpH.y);
                    ShowCursor( TRUE );
                }
                m_MouseCaptured = false;
            }
        }
    }
    // update tools (change action)
    Tools.Update();
    RedrawScene();
}
//----------------------------------------------------
void TUI::MouseMove(TShiftState Shift, int X, int Y)
{
	if (!m_bReady) return;
    m_ShiftState = Shift;
}
//----------------------------------------------------
void TUI::OnMouseMove(int x, int y){
	if (!m_bReady) return;
    bool bRayUpdated = false;

	if (!Device.m_Camera.Process(m_ShiftState,x,y)){
        if( m_MouseCaptured || m_MouseMultiClickCaptured ){
            if( Tools.HiddenMode() ){
				m_DeltaCpH.set(x,y);
                if( m_DeltaCpH.x || m_DeltaCpH.y ){
                	Tools.MouseMove(m_ShiftState);
                }
            }else{
                iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp);
                Tools.MouseMove(m_ShiftState);
            }
		    RedrawScene();
            bRayUpdated = true;
        }
    }
    if (!bRayUpdated){
		iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
        Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp);
    }
#ifdef _LEVEL_EDITOR
    if (m_Cursor->GetVisible()) RedrawScene();
#endif
    // Out cursor pos
    OutUICursorPos();
}
//---------------------------------------------------------------------------

void TUI::OnAppActivate()
{
	VERIFY(m_bReady);
	if (pInput){
        m_ShiftState.Clear();
     	pInput->OnAppActivate();
    }
    m_bAppActive = true;
}
//---------------------------------------------------------------------------

void TUI::OnAppDeactivate()
{
	VERIFY(m_bReady);
	if (pInput){
		pInput->OnAppDeactivate();
        m_ShiftState.Clear();
    }
    HideHint();
    m_bAppActive = false;
}
//---------------------------------------------------------------------------


bool TUI::ShowHint(const AStringVec& SS){
	VERIFY(m_bReady);
    if (SS.size()){
        AnsiString S=ListToSequence2(SS);
        if (m_bHintShowing&&(S==m_LastHint)) return true;
        m_LastHint = S;
        m_bHintShowing = true;
        if (!m_pHintWindow){
            m_pHintWindow = new THintWindow(frmMain);
            m_pHintWindow->Brush->Color = (TColor)0x0d9F2FF;
        }
        TRect rect = m_pHintWindow->CalcHintRect(320,S,0);
        rect.Left+=m_HintPoint.x;    rect.Top+=m_HintPoint.y;
        rect.Right+=m_HintPoint.x;   rect.Bottom+=m_HintPoint.y;
        m_pHintWindow->ActivateHint(rect,S);
    }else{
    	m_bHintShowing = false;
        m_LastHint = "";
    }
    return m_bHintShowing;
}
//---------------------------------------------------------------------------

void TUI::HideHint(){
	VERIFY(m_bReady);
    m_bHintShowing = false;
    _DELETE(m_pHintWindow);
}
//---------------------------------------------------------------------------

void TUI::ShowObjectHint(){
	VERIFY(m_bReady);
    if (!fraBottomBar->miShowHint->Checked){
    	if (m_bHintShowing) HideHint();
    	return;
    }
    if (Device.m_Camera.IsMoving()||m_MouseCaptured) return;
    if (!m_bAppActive) return;

    GetCursorPos(&m_HintPoint);
    TWinControl* ctr = FindVCLWindow(m_HintPoint);
    if (ctr!=frmMain->D3DWindow) return;

	AStringVec SS;
	Tools.OnShowHint(SS);
    if (!ShowHint(SS)) HideHint();
}
//---------------------------------------------------------------------------
void TUI::SetStatus(LPSTR s){
	VERIFY(m_bReady);
    fraBottomBar->paStatus->Caption=s; fraBottomBar->paStatus->Repaint();
}
void TUI::ProgressInfo(LPCSTR text)
{
	if (text){
		fraBottomBar->paStatus->Caption=fraBottomBar->sProgressTitle+" ("+text+")";
    	fraBottomBar->paStatus->Repaint();
    }
}                                                           
void TUI::ProgressStart(float max_val, const char* text)
{
	VERIFY(m_bReady);
    fraBottomBar->sProgressTitle = text;
	fraBottomBar->paStatus->Caption=text;
    fraBottomBar->paStatus->Repaint();
	fraBottomBar->fMaxVal=max_val;
	fraBottomBar->fStatusProgress=0;
	fraBottomBar->cgProgress->Progress=0;
	fraBottomBar->cgProgress->Visible=true;
}
void TUI::ProgressEnd(){
	VERIFY(m_bReady);
    fraBottomBar->sProgressTitle = "";
	fraBottomBar->paStatus->Caption="";
    fraBottomBar->paStatus->Repaint();
	fraBottomBar->cgProgress->Visible=false;
}
void TUI::ProgressUpdate(float val){
	VERIFY(m_bReady);
	fraBottomBar->fStatusProgress=val;
    if (fraBottomBar->fMaxVal>=0){
    	int new_val = (int)((fraBottomBar->fStatusProgress/fraBottomBar->fMaxVal)*100);
        if (new_val!=fraBottomBar->cgProgress->Progress){
			fraBottomBar->cgProgress->Progress=(int)((fraBottomBar->fStatusProgress/fraBottomBar->fMaxVal)*100);
    	    fraBottomBar->cgProgress->Repaint();
        }
    }
}
void TUI::ProgressInc(const char* info){
	VERIFY(m_bReady);
    ProgressInfo(info);
	fraBottomBar->fStatusProgress++;
    if (fraBottomBar->fMaxVal>=0){
    	int val = (int)((fraBottomBar->fStatusProgress/fraBottomBar->fMaxVal)*100);
        if (val!=fraBottomBar->cgProgress->Progress){
			fraBottomBar->cgProgress->Progress=val;
	        fraBottomBar->cgProgress->Repaint();
        }
    }
}
//---------------------------------------------------------------------------
void TUI::OutCameraPos(){
	VERIFY(m_bReady);
    AnsiString s;
	const Fvector& c 	= Device.m_Camera.GetPosition();
	s.sprintf("Cam: %3.1f, %3.1f, %3.1f",c.x,c.y,c.z);
//	const Fvector& hpb 	= Device.m_Camera.GetHPB();
//	s.sprintf(" Cam: %3.1f°, %3.1f°, %3.1f°",rad2deg(hpb.y),rad2deg(hpb.x),rad2deg(hpb.z));
    fraBottomBar->paCamera->Caption=s; fraBottomBar->paCamera->Repaint();
}
//---------------------------------------------------------------------------
void TUI::OutUICursorPos(){
	VERIFY(m_bReady);
    AnsiString s; POINT pt;
    GetCursorPos(&pt);
    s.sprintf("Cur: %d, %d",pt.x,pt.y);
    fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
}
//---------------------------------------------------------------------------
void TUI::OutGridSize(){
	VERIFY(m_bReady);
    AnsiString s;
    s.sprintf("Grid: %1.1f",float(frmEditorPreferences->seGridSquareSize->Value));
    fraBottomBar->paGridSquareSize->Caption=s; fraBottomBar->paGridSquareSize->Repaint();
}
//---------------------------------------------------------------------------
/*
void TUI::OnMousePress(int btn){
	if (!m_bReady) return;
    if (!frmMain->IsFocused()) return;
    if (m_MouseCaptured) return;

    // test owner
    Ipoint pt;
	iGetMousePosScreen(pt);
    TWinControl* ctr 	= FindVCLWindow(*pt.d3d());
    if (ctr!=m_D3DWindow) return;

    bMouseInUse = true;

    if (iGetBtnState(0)) m_ShiftState << ssLeft; else m_ShiftState >> ssLeft;
    if (iGetBtnState(1)) m_ShiftState << ssRight;else m_ShiftState >> ssRight;

    // camera activate
    if(!Device.m_Camera.MoveStart(m_ShiftState)){
    	if (Tools.Pick()) return;
        if( !m_MouseCaptured ){
            if( Tools.HiddenMode() ){
				iGetMousePosScreen(m_StartCpH);
                m_DeltaCpH.set(0,0);
            }else{
                iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
                m_StartCp = m_CurrentCp;

                Device.m_Camera.MouseRayFromPoint(m_StartRStart, m_StartRNorm, m_StartCp );
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart, m_CurrentRNorm, m_CurrentCp );
            }

            if(Tools.MouseStart(m_ShiftState)){
                if(Tools.HiddenMode()) ShowCursor( FALSE );
                m_MouseCaptured = true;
            }
        }
    }
    RedrawScene();
}

void TUI::OnMouseRelease(int btn){
	if (!m_bReady) return;

    if (iGetBtnState(0)) m_ShiftState << ssLeft; else m_ShiftState >> ssLeft;
    if (iGetBtnState(1)) m_ShiftState << ssRight;else m_ShiftState >> ssRight;

    if( Device.m_Camera.IsMoving() ){
        if (Device.m_Camera.MoveEnd(m_ShiftState)) bMouseInUse = false;
    }else{
	    bMouseInUse = false;
        if( m_MouseCaptured ){
            if( !Tools.HiddenMode() ){
                iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp );
            }
            if( Tools.MouseEnd(m_ShiftState) ){
                if( Tools.HiddenMode() ){
                    SetCursorPos(m_StartCpH.x,m_StartCpH.y);
                    ShowCursor( TRUE );
                }
                m_MouseCaptured = false;
            }
        }
    }
    // update tools (change action)
    Tools.Update();
    RedrawScene();
}
*/

