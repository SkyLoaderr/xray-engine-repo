//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Main.h"
#include "D3DUtils.h"
#include "cursor3d.h"
#include "leftbar.h"
#include "main.h"
#include "xr_input.h"
#include "cl_rapid.h"
#include "UI_Tools.h"

TUI UI;

bool g_bEditorValid = false;
TUI::TUI()
{
    m_D3DWindow = 0;
    bNeedAbort   = false;

	m_StartRStart.set(0,0,0);
	m_StartRNorm.set(0,0,0);
	m_CurrentRStart.set(0,0,0);
	m_CurrentRNorm.set(0,0,0);

    m_Cursor        = 0;

	DU::InitUtilLibrary();

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

    fHintPause      = 0.5f;
    fHintPauseTime  = fHintPause;
    fHintHide       = 15.f;
    fHintHideTime   = 0.f;
    bHintShowing    = false;

// create base class
    ELog.Create		("ed.log");
}
//---------------------------------------------------------------------------
TUI::~TUI()
{
    VERIFY(m_EditorState.size()==0);
#ifdef _LEVEL_EDITOR
    _DELETE(m_Cursor);
#endif
}

bool TUI::OnCreate(){
	Device.InitTimer();

    m_D3DWindow 	= frmMain->D3DWindow;
    VERIFY(m_D3DWindow);
    Device.Initialize();

#ifdef _LEVEL_EDITOR
    m_Cursor        = new C3DCursor();
#endif
	g_bEditorValid  = true;

    XRC.RayMode		(RAY_CULL);

    pInput			= new CInput(FALSE,mouse_device_key);
    UI.iCapture		();
    return true;
}

void TUI::OnDestroy()
{
	UI.iRelease			();
    _DELETE(pInput);
    EndEState();
	DU::UninitUtilLibrary();

    Device.ShutDown	();
    g_bEditorValid = false;
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
	m_ShiftState = Shift;
    if (m_ShiftState.Contains(ssLeft)) Log("LB press.");
	if (Device.m_Camera.KeyDown(Key,Shift)) return true;
    return Tools.KeyDown(Key, Shift);
}

bool __fastcall TUI::KeyUp   (WORD Key, TShiftState Shift)
{
	m_ShiftState = Shift;
	if (Device.m_Camera.KeyUp(Key,Shift)) return true;
    return Tools.KeyUp(Key, Shift);
}

bool __fastcall TUI::KeyPress(WORD Key, TShiftState Shift)
{
    return Tools.KeyPress(Key, Shift);
}

//----------------------------------------------------
void TUI::OnMousePress(int btn){
	if(!g_bEditorValid) return;
    if(m_MouseCaptured) return;

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
	if(!g_bEditorValid) return;

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
void TUI::OnMouseMove(int x, int y){
	if(!g_bEditorValid) return;
    bool bRayUpdated = false;

    if (iGetBtnState(0)) m_ShiftState << ssLeft; else m_ShiftState >> ssLeft;
    if (iGetBtnState(1)) m_ShiftState << ssRight;else m_ShiftState >> ssRight;

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
	pInput->OnAppActivate();
}
//---------------------------------------------------------------------------

void TUI::OnAppDeactivate()
{
	pInput->OnAppDeactivate();
}
//---------------------------------------------------------------------------


