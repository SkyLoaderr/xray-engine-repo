//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "leftbar.h"
#include "main.h"
#include "xr_input.h"
#include "UI_Tools.h"
#include "bottombar.h"
#ifdef _LEVEL_EDITOR
 	#include "cursor3d.h"
#endif

#include "ui_main.h"
#include "d3dutils.h"
#include "SoundManager.h"
#include "PSLibrary.h"

#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"
#include "editorpref.h"

#include "itemlist.h"
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
	m_Flags.set(flResize);

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
    xr_delete(m_Cursor);
#endif
}

void TUI::OnDeviceCreate()
{
	DU.OnDeviceCreate();
    ::Render->PSLibrary.OnDeviceCreate();
}

void TUI::OnDeviceDestroy()
{
	DU.OnDeviceDestroy();
    ::Render->PSLibrary.OnDeviceDestroy();
}

bool TUI::OnCreate()
{
// create base class
	Device.InitTimer();

    fraBottomBar	= xr_new<TfraBottomBar>((TComponent*)0);
    fraLeftBar  	= xr_new<TfraLeftBar>((TComponent*)0);
    fraTopBar   	= xr_new<TfraTopBar>((TComponent*)0);
    
    m_D3DWindow 	= frmMain->D3DWindow;
    VERIFY(m_D3DWindow);
    Device.Initialize();

#ifdef _LEVEL_EDITOR
    m_Cursor        = xr_new<C3DCursor>();
#endif
	frmEditPrefs	= xr_new<TfrmEditPrefs>((TComponent*)0);
	// Creation
	XRC.ray_options	(CDB::OPT_ONLYNEAREST | CDB::OPT_CULL);

    pInput			= xr_new<CInput>(FALSE,mouse_device_key);
    UI.IR_Capture	();

    m_bReady		= true;

    EFS.WriteAccessLog(_EDITOR_FILE_NAME_,"Started");

    if (!CreateMailslot()){
    	ELog.DlgMsg	(mtError,"Can't create mail slot.\nIt's possible two Editors started.");
        return 		false;
    }

    if (!FS.path_exist(_local_root_)){
    	ELog.DlgMsg	(mtError,"Undefined Editor local directory.");
        return 		false;
    }

	BeginEState		(esEditScene);

    return true;
}

void TUI::OnDestroy()
{
    EFS.WriteAccessLog(_EDITOR_FILE_NAME_,"Finished");

	VERIFY(m_bReady);
	m_bReady		= false;
	UI.IR_Release	();
    xr_delete		(pInput);
    EndEState		();

    Device.ShutDown	();
    
    //----------------
    xr_delete(frmEditPrefs);
    xr_delete(fraLeftBar);
    xr_delete(fraTopBar);
    xr_delete(fraBottomBar);
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

void TUI::UpdateSelectionRect( const Ivector2& from, const Ivector2& to ){
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
    	if (Tools.Pick(Shift)) return;
        if( !m_MouseCaptured ){
            if( Tools.HiddenMode() ){
				IR_GetMousePosScreen(m_StartCpH);
                m_DeltaCpH.set(0,0);
            }else{
                IR_GetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
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
                IR_GetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
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
    Tools.OnFrame();
    RedrawScene();
}
//----------------------------------------------------
void TUI::MouseMove(TShiftState Shift, int X, int Y)
{
	if (!m_bReady) return;
    m_ShiftState = Shift;
}
//----------------------------------------------------
void TUI::IR_OnMouseMove(int x, int y){
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
                IR_GetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp);
                Tools.MouseMove(m_ShiftState);
            }
		    RedrawScene();
            bRayUpdated = true;
        }
    }
    if (!bRayUpdated){
		IR_GetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
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
        AnsiString S=_ListToSequence2(SS);
        if (m_bHintShowing&&(S==m_LastHint)) return true;
        m_LastHint = S;
        m_bHintShowing = true;
        if (!m_pHintWindow){
            m_pHintWindow = xr_new<THintWindow>(frmMain);
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

void TUI::HideHint()
{
	VERIFY(m_bReady);
    m_bHintShowing = false;
    xr_delete(m_pHintWindow);
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
    if (!ShowHint(SS)&&m_pHintWindow) HideHint();
}
//---------------------------------------------------------------------------
void TUI::ResetStatus()
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=""){
	    fraBottomBar->paStatus->Caption=""; fraBottomBar->paStatus->Repaint();
    }
}
void TUI::SetStatus(LPSTR s, bool bOutLog)
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=s){
	    fraBottomBar->paStatus->Caption=s; fraBottomBar->paStatus->Repaint();
    	if (bOutLog) ELog.Msg(mtInformation,s);
    }
}
void TUI::ProgressInfo(LPCSTR text)
{
	if (text){
		fraBottomBar->paStatus->Caption=fraBottomBar->sProgressTitle+" ("+text+")";
    	fraBottomBar->paStatus->Repaint();
	    ELog.Msg(mtInformation,fraBottomBar->paStatus->Caption.c_str());
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
    ELog.Msg(mtInformation,text);
}
void TUI::ProgressEnd()
{
	VERIFY(m_bReady);
    fraBottomBar->sProgressTitle = "";
	fraBottomBar->paStatus->Caption="";
    fraBottomBar->paStatus->Repaint();
	fraBottomBar->cgProgress->Visible=false;
}
void TUI::ProgressUpdate(float val)
{
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
void TUI::ProgressInc(const char* info)
{
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
void TUI::OutCameraPos()
{
	VERIFY(m_bReady);
    AnsiString s;
	const Fvector& c 	= Device.m_Camera.GetPosition();
	s.sprintf("C: %3.1f, %3.1f, %3.1f",c.x,c.y,c.z);
//	const Fvector& hpb 	= Device.m_Camera.GetHPB();
//	s.sprintf(" Cam: %3.1f°, %3.1f°, %3.1f°",rad2deg(hpb.y),rad2deg(hpb.x),rad2deg(hpb.z));
    fraBottomBar->paCamera->Caption=s; fraBottomBar->paCamera->Repaint();
}
//---------------------------------------------------------------------------
void TUI::OutUICursorPos()
{
	VERIFY(m_bReady);
    AnsiString s; POINT pt;
    GetCursorPos(&pt);
    s.sprintf("Cur: %d, %d",pt.x,pt.y);
    fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
}
//---------------------------------------------------------------------------
void TUI::OutGridSize()
{
	VERIFY(m_bReady);
    AnsiString s;
    s.sprintf("Grid: %1.1f",float(frmEditPrefs->seGridSquareSize->Value));
    fraBottomBar->paGridSquareSize->Caption=s; fraBottomBar->paGridSquareSize->Repaint();
}
//---------------------------------------------------------------------------
void TUI::CheckWindowPos(TForm* form)
{
	if (form->Left+form->Width>Screen->Width) 	form->Left	= Screen->Width-form->Width;
	if (form->Top+form->Height>Screen->Height)	form->Top 	= Screen->Height-form->Height;
	if (form->Left<0) 							form->Left	= 0;
	if (form->Top<0) 							form->Top 	= 0;
}
//---------------------------------------------------------------------------

void TUI::Redraw(){
	VERIFY(m_bReady);
	if (m_Flags.is(flResize)) Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height); m_Flags.set(flResize,FALSE);
// set render state
    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    // fog
    u32 fog_color;
	float fog_start, fog_end;
    Tools.GetCurrentFog	(fog_color, fog_start, fog_end);
	Device.SetRS( D3DRS_FOGCOLOR,		fog_color			);         
	Device.SetRS( D3DRS_RANGEFOGENABLE,	FALSE				);
	if (HW.Caps.bTableFog)	{
		Device.SetRS( D3DRS_FOGTABLEMODE,	D3DFOG_LINEAR 	);
		Device.SetRS( D3DRS_FOGVERTEXMODE,	D3DFOG_NONE	 	);
	} else {
		Device.SetRS( D3DRS_FOGTABLEMODE,	D3DFOG_NONE	 	);
		Device.SetRS( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR	);
	}
	Device.SetRS( D3DRS_FOGSTART,	*(DWORD *)(&fog_start)	);
	Device.SetRS( D3DRS_FOGEND,		*(DWORD *)(&fog_end)	);
    // filter
    for (u32 k=0; k<HW.Caps.pixel.dwStages; k++){
        if( psDeviceFlags.is(rsFilterLinear)){
            Device.SetTSS(k,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
            Device.SetTSS(k,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
            Device.SetTSS(k,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
        } else {
            Device.SetTSS(k,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
            Device.SetTSS(k,D3DSAMP_MINFILTER,D3DTEXF_POINT);
            Device.SetTSS(k,D3DSAMP_MIPFILTER,D3DTEXF_POINT);
        }
    }
	// ligthing
    if (psDeviceFlags.is(rsLighting)) 	Device.SetRS(D3DRS_AMBIENT,0x00000000);
    else                				Device.SetRS(D3DRS_AMBIENT,0xFFFFFFFF);

//    try
    {
    	Device.Statistic.RenderDUMP_RT.Begin();
        Device.Begin			();
        Device.UpdateView		();
		Device.ResetMaterial	();

        Device.SetRS			(D3DRS_FILLMODE, Device.dwFillMode);
		Device.SetRS			(D3DRS_SHADEMODE,Device.dwShadeMode);

        Tools.RenderEnvironment	();

    	// draw grid
    	if (psDeviceFlags.is(rsDrawGrid)){
	        DU.DrawGrid		();
    	    DU.DrawPivot		(m_Pivot);
        }

		Tools.Render			();

    	// draw selection rect
		if(m_SelectionRect) 	DU.DrawSelectionRect(m_SelStart,m_SelEnd);

    	// draw axis
        DU.DrawAxis(Device.m_Camera.GetTransform());
    	// end draw
        Device.End();
    	Device.Statistic.RenderDUMP_RT.End();
    }
/*    catch(...)
    {
		_clear87();
		FPU::m24r();
    	ELog.DlgMsg(mtError, "Critical error has occured in render routine.\nEditor may work incorrectly.");
        Device.End();
		Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height);
    }
*/
	fraBottomBar->paSel->Caption = Tools.GetInfo();
}
//---------------------------------------------------------------------------
void TUI::Idle()
{
	VERIFY(m_bReady);
	// input
    pInput->OnFrame();
    Sleep(1);
    if (ELog.in_use) return;
	Device.UpdateTimer	();
    SndLib.OnFrame();
    // tools on frame
    if (m_Flags.is(flUpdateScene)){
        Tools.UpdateProperties	(false);
        RealUpdateScene			();
    }
    if (m_Flags.is(flRedraw)){
	    if (!psDeviceFlags.is(rsRenderRealTime)) m_Flags.set(flRedraw,FALSE);
	    Tools.OnFrame		();
        Redraw				();
    }
	// show hint
    ShowObjectHint	();
	ResetBreak		();
	// check mail
    CheckMailslot	();
    // update item list
    TItemList::OnFrame();
    // test quit
    if (m_Flags.is(flNeedQuit))			frmMain->Close();
}
//---------------------------------------------------------------------------
void ResetActionToSelect()
{
    UI.Command(COMMAND_CHANGE_ACTION, eaSelect);
}
//---------------------------------------------------------------------------

void __fastcall TUI::miRecentFilesClick(TObject *Sender)
{
	TMenuItem* MI = dynamic_cast<TMenuItem*>(Sender); R_ASSERT(MI&&(MI->Tag==0x1001));
    AnsiString fn = MI->Caption;
    if (FS.exist(fn.c_str()))	Command(COMMAND_LOAD,(u32)fn.c_str());
    else						ELog.DlgMsg(mtError, "Error reading file '%s'",fn.c_str());
}
//---------------------------------------------------------------------------

void TUI::AppendRecentFile(LPCSTR name)
{
#ifdef _HAVE_RECENT_FILES
	R_ASSERT(fraLeftBar->miRecentFiles->Count<=frmEditPrefs->seRecentFilesCount->Value);

	for (int i = 0; i < fraLeftBar->miRecentFiles->Count; i++)
    	if (fraLeftBar->miRecentFiles->Items[i]->Caption==name){
        	fraLeftBar->miRecentFiles->Items[i]->MenuIndex = 0;
            return;
		}

	if (fraLeftBar->miRecentFiles->Count==frmEditPrefs->seRecentFilesCount->Value) 
    	fraLeftBar->miRecentFiles->Remove(fraLeftBar->miRecentFiles->Items[frmEditPrefs->seRecentFilesCount->Value-1]);

    TMenuItem *MI 	= xr_new<TMenuItem>((TComponent*)0);
    MI->Caption 	= name;
    MI->OnClick 	= miRecentFilesClick;
    MI->Tag			= 0x1001;
    fraLeftBar->miRecentFiles->Insert(0,MI);

    fraLeftBar->miRecentFiles->Enabled = true;
#endif
}
//---------------------------------------------------------------------------

LPCSTR TUI::FirstRecentFile()
{
#ifdef _HAVE_RECENT_FILES
	if (fraLeftBar->miRecentFiles->Count>0)
    	return fraLeftBar->miRecentFiles->Items[0]->Caption.c_str();
#endif
    return 0;
}


