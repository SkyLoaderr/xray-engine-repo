//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Main.h"
#include "D3DUtils.h"
#include "Shader.h"
#include "Cursor3D.h"

#include "leftbar.h"
#include "topbar.h"
#include "bottombar.h"
#include "EditorPref.h"
#include "main.h"
#include "xr_trims.h"

#include "xr_input.h"
//#include "EditLibrary.h"
//#include "EditParticles.h"
//#include "ImageEditor.h"
//#include "ImageManager.h"

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

    m_Tools         = 0;
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
    _DELETE(m_Tools);
    _DELETE(m_Cursor);
}

bool TUI::OnCreate(){
    pInput			= new CInput(FALSE,mouse_device_key);
    UI.iCapture		();
	Device.InitTimer();

    m_D3DWindow 	= frmMain->D3DWindow;
    VERIFY(m_D3DWindow);
    Device.Initialize();

    m_Tools         = new TUI_Tools(fraLeftBar->paFrames);
    m_Cursor        = new C3DCursor();

	g_bEditorValid  = true;

    XRC.RayMode		(RAY_CULL);

    return true;
}

void TUI::OnDestroy()
{
	UI.iRelease			();
    EndEState();
	DU::UninitUtilLibrary();

    m_Tools->Clear();

    Device.ShutDown	();
    g_bEditorValid = false;
}

bool TUI::PickGround(Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap){
    // pick object geometry
    if ((bSnap==-1)||(fraTopBar->ebOSnap->Down&&(bSnap==1))){
        bool bPickObject;
        SRayPickInfo pinf;
	    EEditorState est = GetEState();
        switch(est){
//S        case esEditLibrary:		bPickObject = !!TfrmEditLibrary::RayPick(start,direction,&pinf); break;
//S        case esEditScene:		bPickObject = !!Scene->RayPick(start,direction,OBJCLASS_SCENEOBJECT,&pinf,false,true); break;
        default: THROW;
        }
        if (bPickObject){
		    if (fraTopBar->ebVSnap->Down&&bSnap){
                Fvector pn;
                float u = pinf.rp_inf.u;
                float v = pinf.rp_inf.v;
                float w = 1-(u+v);
                if ((w>u) && (w>v)) pn.set(pinf.rp_inf.p[0]);
                else if ((u>w) && (u>v)) pn.set(pinf.rp_inf.p[1]);
                else pn.set(pinf.rp_inf.p[2]);
                if (pn.distance_to(pinf.pt) < movesnap()) hitpoint.set(pn);
                else hitpoint.set(pinf.pt);
            }else{
            	hitpoint.set(pinf.pt);
            }
            return true;
        }
    }

    // pick grid
	Fvector normal;
	normal.set( 0, 1, 0 );
	float clcheck = direction.dotproduct( normal );
	if( fis_zero( clcheck ) ) return false;
	float alpha = - start.dotproduct(normal) / clcheck;
	if( alpha <= 0 ) return false;

	hitpoint.x = start.x + direction.x * alpha;
	hitpoint.y = start.y + direction.y * alpha;
	hitpoint.z = start.z + direction.z * alpha;

    if (fraTopBar->ebGSnap->Down && bSnap){
        hitpoint.x = snapto( hitpoint.x, movesnap() );
        hitpoint.z = snapto( hitpoint.z, movesnap() );
        hitpoint.y = 0.f;
    }
	return true;
}

bool TUI::IsModified()
{
	return Command(COMMAND_CHECK_MODIFIED);
}
//----------------------------------------------------

bool TUI::SelectionFrustum(CFrustum& frustum){
    Fvector st,d,p[4];
    Ipoint pt[4];

    float depth = 0;

    float x1=m_StartCp.x, x2=m_CurrentCp.x;
    float y1=m_StartCp.y, y2=m_CurrentCp.y;

	if(!(x1!=x2&&y1!=y2)) return false;

	pt[0].set(min(x1,x2),min(y1,y2));
	pt[1].set(max(x1,x2),min(y1,y2));
	pt[2].set(max(x1,x2),max(y1,y2));
	pt[3].set(min(x1,x2),max(y1,y2));

    SRayPickInfo pinf;
    for (int i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        if (frmEditorPreferences->cbBoxPickLimitedDepth->Checked){
			pinf.rp_inf.range = Device.m_Camera.m_Zfar; // max pick range
//S            if (Scene->RayPick(st, d, OBJCLASS_SCENEOBJECT, &pinf, false, false))
	            if (pinf.rp_inf.range > depth) depth = pinf.rp_inf.range;
        }
    }
    if (depth<Device.m_Camera.m_Znear) depth = Device.m_Camera.m_Zfar;
    else depth += frmEditorPreferences->seBoxPickDepthTolerance->Value;

    for (i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        p[i].direct(st,d,depth);
    }

    frustum.CreateFromPoints(p,4);

    Fplane P; P.build(p[0],p[1],p[2]);
    if (P.classify(st)>0) P.build(p[2],p[1],p[0]);
	frustum._add(P);

	return true;
}
//----------------------------------------------------

void TUI::Redraw(){
	if (bResize){ Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height); bResize=false; }
// set render state
    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    // filter
    for (DWORD k=0; k<HW.Caps.pixel.dwStages; k++){
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
    	    DU::DrawPivot(m_Pivot);
        }

	    EEditorState est = GetEState();
        switch(est){
//S        case esEditLibrary: 	TfrmEditLibrary::OnRender(); break;
//S        case esEditParticles: 	TfrmEditParticles::OnRender(); break;
//S        case esEditImages:	 	TfrmImageLib::OnRender(); break;
//S        case esEditScene:		Scene->Render(&precalc_identity); break;
        }

    // draw selection rect
		if(m_SelectionRect) DU::DrawSelectionRect(m_SelStart,m_SelEnd);

    // draw cursor
        m_Cursor->Render();

    // draw axis
        DU::DrawAxis();

    // end draw
        Device.End();
    }catch(...){
        Device.End();
		Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height);
    }
    if (!(psDeviceFlags&rsRenderRealTime)) bRedraw = false;
	Command(COMMAND_OUT_INFO);
}
//---------------------------------------------------------------------------
void TUI::Idle()
{
    if (g_ErrorMode) return;
//    ELog.Msg(mtInformation,"%f",Device.m_FrameDTime);
    Sleep(5);
	Device.UpdateTimer();
    EEditorState est = GetEState();
    if ((est==esEditScene)||(est==esEditLibrary)||(est==esEditParticles)||(est==esEditImages)){
//S    	switch(est){
//S    	case esEditParticles: 	TfrmEditParticles::OnIdle(); break;
//S    	case esEditImages: 		TfrmImageLib::OnIdle(); break;
//S		}
	    if (bUpdateScene) RealUpdateScene();
    	if (bRedraw){
//S            Scene->Update(Device.fTimeDelta);
        	Redraw();
        }
		m_Tools->Update();
    }
        // show hint
    ShowObjectHint();

	ResetBreak();
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

bool TUI::ShowHint(const AStringVec& SS){
    if (SS.size()){
        AnsiString S=ListToSequence2(SS);
        if (S==LastHint) return false;
        LastHint = S;
        bHintShowing = true;
        if (!pHintWindow){
            pHintWindow = new THintWindow(frmMain);
            pHintWindow->Brush->Color = 0x0d9F2FF;
        }
        TRect rect = pHintWindow->CalcHintRect(320,S,0);
        rect.Left+=pt.x;    rect.Top+=pt.y;
        rect.Right+=pt.x;   rect.Bottom+=pt.y;
        pHintWindow->ActivateHint(rect,S);
    }else bHintShowing = false;
    return bHintShowing;
}
void TUI::HideHint(){
    bHintShowing = false;
    _DELETE(pHintWindow);
}
void TUI::ShowObjectHint(){
    if (!fraBottomBar->miShowHint->Checked) return;

    GetCursorPos(&pt);
    TWinControl* ctr = FindVCLWindow(pt);
    if (ctr!=frmMain->D3DWindow) return;

    iDeltaMouseX=pt.x-iLastMouseX;
    iDeltaMouseY=pt.y-iLastMouseY;
    iLastMouseX=pt.x; iLastMouseY=pt.y;

    if (iDeltaMouseX||iDeltaMouseY){
        if(bHintShowing){
            fHintHideTime = 0.f;
            HideHint();
        }
        fHintPauseTime = fHintPause+1.f;
        return;
    }
    if (!iDeltaMouseX&&!iDeltaMouseY&&(fHintPauseTime>fHintPause)){
        fHintPauseTime = fHintPause;
    }
    if (bHintShowing){
        fHintHideTime -= Device.fTimeDelta;
        if (fHintHideTime<0) HideHint();
        return;
    }
    if (fHintPauseTime>fHintPause) return;
    if (fHintPauseTime<0){
        fHintPauseTime = fHintPause+1;
        AStringVec SS;
//S        Scene->OnShowHint(SS);
        if (ShowHint(SS)) fHintHideTime = fHintHide;
    }else{
        fHintPauseTime -= Device.fTimeDelta;
    }
}

bool __fastcall TUI::KeyDown (WORD Key, TShiftState Shift)
{
	m_ShiftState = Shift;
    if (m_ShiftState.Contains(ssLeft)) Log("LB press.");
	if (Device.m_Camera.KeyDown(Key,Shift)) return true;
    return m_Tools->KeyDown(Key, Shift);
}

bool __fastcall TUI::KeyUp   (WORD Key, TShiftState Shift)
{
	m_ShiftState = Shift;
	if (Device.m_Camera.KeyUp(Key,Shift)) return true;
    return m_Tools->KeyUp(Key, Shift);
}

bool __fastcall TUI::KeyPress(WORD Key, TShiftState Shift)
{
    return m_Tools->KeyPress(Key, Shift);
}

//----------------------------------------------------
void TUI::RealUpdateScene(){
	if (GetEState()==esEditScene){
//S	    Scene->OnObjectsUpdate();
    	m_Tools->OnObjectsUpdate(); // обновить все что как-то связано с объектами
	    RedrawScene();
    }
    bUpdateScene = false;
}

EObjClass TUI::CurrentClassID(){
	return (fraLeftBar->ebIgnoreTarget->Down)?OBJCLASS_DUMMY:m_Tools->GetTargetClassID();
}

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
//S        if( Scene->locked() )
        {
		    EEditorState est = GetEState();
            switch(est){
            case esEditLibrary:{
                iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
                m_StartCp = m_CurrentCp;
                Device.m_Camera.MouseRayFromPoint(m_StartRStart, m_StartRNorm, m_StartCp );
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart, m_CurrentRNorm, m_CurrentCp );
				SRayPickInfo pinf;
//S                TfrmEditLibrary::RayPick(m_CurrentRStart,m_CurrentRNorm,&pinf);
            }break;
            case esEditParticles: 	break;
            case esEditImages: 		break;
            }
            return;
        }
        if( !m_MouseCaptured ){
            if( m_Tools->HiddenMode() ){
				iGetMousePosScreen(m_StartCpH);
                m_DeltaCpH.set(0,0);
            }else{
                iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
                m_StartCp = m_CurrentCp;

                Device.m_Camera.MouseRayFromPoint(m_StartRStart, m_StartRNorm, m_StartCp );
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart, m_CurrentRNorm, m_CurrentCp );
            }

            if(m_Tools->MouseStart(m_ShiftState)){
                if(m_Tools->HiddenMode()) ShowCursor( FALSE );
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
            if( !m_Tools->HiddenMode() ){
                iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp );
            }
            if( m_Tools->MouseEnd(m_ShiftState) ){
                if( m_Tools->HiddenMode() ){
                    SetCursorPos(m_StartCpH.x,m_StartCpH.y);
                    ShowCursor( TRUE );
                }
                m_MouseCaptured = false;
            }
        }
    }
    // update tools (change action)
    m_Tools->Update();
    RedrawScene();
}
void TUI::OnMouseMove(int x, int y){
	if(!g_bEditorValid) return;
    bool bRayUpdated = false;

    if (iGetBtnState(0)) m_ShiftState << ssLeft; else m_ShiftState >> ssLeft;
    if (iGetBtnState(1)) m_ShiftState << ssRight;else m_ShiftState >> ssRight;

	if (!Device.m_Camera.Process(m_ShiftState,x,y)){
        if( m_MouseCaptured || m_MouseMultiClickCaptured ){
            if( m_Tools->HiddenMode() ){
				m_DeltaCpH.set(x,y);
                if( m_DeltaCpH.x || m_DeltaCpH.y ){
                	m_Tools->MouseMove(m_ShiftState);
                }
            }else{
                iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp);
                m_Tools->MouseMove(m_ShiftState);
            }
		    RedrawScene();
            bRayUpdated = true;
        }
    }
    if (!bRayUpdated){
		iGetMousePosReal(Device.m_hRenderWnd, m_CurrentCp);
        Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp);
    }
    if (m_Cursor->GetVisible()) RedrawScene();
    // Out cursor pos
    OutUICursorPos();
}

void TUI::ShowContextMenu(int cls)
{
    if (g_bEditorValid){
        POINT pt;
        GetCursorPos(&pt);
		fraLeftBar->miProperties->Enabled = false;
//S        if (Scene->SelectionCount( true, cls )) fraLeftBar->miProperties->Enabled = true;
        RedrawScene(true);
	    fraLeftBar->pmObjectContext->TrackButton = tbRightButton;
        fraLeftBar->pmObjectContext->Popup(pt.x,pt.y);
    }
}

void ResetActionToSelect()
{
    UI.Command(COMMAND_CHANGE_ACTION, eaSelect);
}
//---------------------------------------------------------------------------


