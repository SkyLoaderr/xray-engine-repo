//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Main.h"
#include "D3DUtils.h"
#include "Shader.h"

#include "leftbar.h"
#include "topbar.h"
#include "bottombar.h"
#include "EditorPref.h"
#include "main.h"
#include "xr_trims.h"
#include "ShaderTools.h"

TUI* UI=0;

bool g_bEditorValid = false;
TUI::TUI()
{
    UI = this;
    m_D3DWindow = 0;

	m_StartRStart.set(0,0,0);
	m_StartRNorm.set(0,0,0);
	m_CurrentRStart.set(0,0,0);
	m_CurrentRNorm.set(0,0,0);

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

// create base class
    ELog.Create		("ed.log");
    FS.Init			();

    SHLib			= new CShaderLibrary();
	m_ShaderTools	= new CShaderTools();
}
//---------------------------------------------------------------------------
TUI::~TUI()
{
    _DELETE(SHLib);
    _DELETE(m_ShaderTools);
}

bool TUI::Init(TD3DWindow* wnd){
	DU::UpdateGrid(25,1,5);

    m_D3DWindow = wnd;
    VERIFY(m_D3DWindow);
	InitMath		();
    SHLib->Init		();

    if (!Device.Create(m_D3DWindow->Handle)){
        ELog.DlgMsg(mtError,"Can't create DirectX device. Editor halted!");
        return false;
     }
	g_bEditorValid  = true;

    XRC.RayMode		(RAY_CULL);

    Command			(COMMAND_CLEAR);
	Command			(COMMAND_RENDER_FOCUS);

    return true;
}

void TUI::Clear()
{
	DU::UninitUtilLibrary();
	m_ShaderTools->Clear();
    SHLib->Clear();

    Device.Destroy();
    g_bEditorValid = false;
}
//------------------------------------------------------------------------------

void TUI::Redraw(){
	if (bResize){ Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height); bResize=false; }

// set render state
    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    // filter
    for (DWORD k=0; k<HW.Caps.dwNumBlendStages; k++){
        if( psDeviceFlags&rsFilterLinear){
            Device.SetTSS(k,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
            Device.SetTSS(k,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
            Device.SetTSS(k,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);
        } else {
            Device.SetTSS(k,D3DTSS_MAGFILTER,D3DTEXF_POINT);
            Device.SetTSS(k,D3DTSS_MINFILTER,D3DTEXF_POINT);
            Device.SetTSS(k,D3DTSS_MIPFILTER,D3DTEXF_POINT);
        }
    }
	// ligthing
    if (psDeviceFlags&rsLighting) 	Device.SetRS(D3DRS_AMBIENT,0x00000000);
    else                			Device.SetRS(D3DRS_AMBIENT,0xFFFFFFFF);

    try{
        Device.Begin();
        Device.UpdateView();
		Device.ResetMaterial();

        Device.SetRS(D3DRS_FILLMODE, Device.dwFillMode);
		Device.SetRS(D3DRS_SHADEMODE,Device.dwShadeMode);

    // draw grid
    	if (psDeviceFlags&rsDrawGrid){
	        DU::DrawGrid();
    	    DU::DrawPivot(m_Pivot,2.f);
        }
        
	// render
	m_ShaderTools->Render();

    // draw selection rect
		if(m_SelectionRect) DU::DrawSelectionRect(m_SelStart,m_SelEnd);

    // draw axis
        DU::DrawAxis();

    // end draw
        Device.End();
    }catch(...){
        Device.End();
		Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height);
    }

    if (!(psDeviceFlags&rsRenderRealTime)) bRedraw = false;
}
//---------------------------------------------------------------------------
void TUI::Idle()
{
    if (g_ErrorMode) return;
    Sleep(5);
	Device.UpdateTimer();
    if (bRedraw){
		m_ShaderTools->Update();
        Redraw();
    }
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

static int iLastMouseX=0;
static int iLastMouseY=0;
static int iDeltaMouseX=0;
static int iDeltaMouseY=0;
static POINT pt;
static THintWindow* pHintWindow=0;
static AnsiString LastHint="";

bool __fastcall TUI::KeyDown (WORD Key, TShiftState Shift)
{
	m_ShiftState = Shift;
    if (m_ShiftState.Contains(ssLeft)) Log("LB press.");
	if (Device.m_Camera.KeyDown(Key,Shift)) return true;
    return false;
}

bool __fastcall TUI::KeyUp   (WORD Key, TShiftState Shift)
{
	m_ShiftState = Shift;
	if (Device.m_Camera.KeyUp(Key,Shift)) return true;
    return false;
}

bool __fastcall TUI::KeyPress(WORD Key, TShiftState Shift)
{
    return false;
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
    	/// other
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
    	/// other
    }
    RedrawScene();
}
void TUI::OnMouseMove(int x, int y){
	if(!g_bEditorValid) return;
    bool bRayUpdated = false;

    if (iGetBtnState(0)) m_ShiftState << ssLeft; else m_ShiftState >> ssLeft;
    if (iGetBtnState(1)) m_ShiftState << ssRight;else m_ShiftState >> ssRight;

	if (!Device.m_Camera.Process(m_ShiftState,x,y)){
    	/// other
    }
    if (!bRayUpdated){
		iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
        Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp);
    }
    // Out cursor pos
    OutUICursorPos();
}


