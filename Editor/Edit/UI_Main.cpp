//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Main.h"
#include "D3DUtils.h"
#include "Builder.h"
#include "Scene.h"
#include "Library.h"
#include "Shader.h"
#include "Cursor3D.h"
#include "PSLibrary.h"

#include "leftbar.h"
#include "topbar.h"
#include "bottombar.h"
#include "EditorPref.h"
#include "EditLibrary.h"
#include "EditShaders.h"
#include "EditParticles.h"
#include "PropertiesShader.h"
#include "ImageEditor.h"
#include "main.h"
#include "xr_trims.h"

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

	dwRenderFillMode= D3DFILL_SOLID;
	dwRenderShadeMode=D3DSHADE_FLAT;
    dwRenderHWTransform=D3DX_DEFAULT;
    bRenderTextures = true;
    bRenderLights   = true;
    bRenderFilter   = true;
    bRenderEdgedFaces=false;
	bRenderRealTime	= false;
    bRenderFog		= true;
    bDrawGrid		= true;

// create base class
    ELog.Create		("ed.log");
    FS.Init			();
    Scene           = new EScene();
    Lib             = new ELibrary();
    SHLib			= new CShaderLibrary();
    PSLib			= new CPSLibrary();
    Builder         = new SceneBuilder();
}
//---------------------------------------------------------------------------
TUI::~TUI()
{
    VERIFY(m_EditorState.size()==0);
    _DELETE(m_Tools);
    _DELETE(m_Cursor);
// clear globals
    _DELETE(Scene);
    _DELETE(SHLib);
    _DELETE(PSLib);
    _DELETE(Lib);
    _DELETE(Builder);
}

bool TUI::Init(TD3DWindow* wnd){
    m_D3DWindow = wnd;
    VERIFY(m_D3DWindow);
	InitMath		();
    SHLib->Init		();
    PSLib->Init		();

    if (!Device.Create(m_D3DWindow->Handle)){
        ELog.DlgMsg(mtError,"Can't create DirectX device. Editor halted!");
        return false;
     }
    m_Tools         = new TUI_Tools(fraLeftBar->paFrames);
    m_Cursor        = new C3DCursor();

	g_bEditorValid  = true;

    Scene->Init     ();
    Lib->Init       ();

    XRC.RayMode		(RAY_CULL);

    BeginEState		(esEditScene);

    Command			(COMMAND_CLEAR);
	Command			(COMMAND_CHECK_TEXTURES);
	Command			(COMMAND_RENDER_FOCUS);

    return true;
}

void TUI::Clear()
{
    EndEState();
	DU::UninitUtilLibrary();

    m_Tools->Clear();

    Scene->Unload();
    Scene->UndoClear();
    Scene->Clear();
    Lib->Clear();
    SHLib->Clear();
    PSLib->Clear();
//    TM->Clear();
    Device.Destroy();
    g_bEditorValid = false;
}

bool TUI::PickGround(Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap){
    // pick object geometry
    if ((bSnap==-1)||(fraTopBar->ebOSnap->Down&&(bSnap==1))){
        bool bPickObject;
        SRayPickInfo pinf;
	    EEditorState est = GetEState();
        switch(est){
        case esEditLibrary:		bPickObject = !!frmEditLibrary->RayPick(start,direction,&pinf); break;
        case esEditScene:		bPickObject = !!Scene->RayPick(start,direction,OBJCLASS_EDITOBJECT,&pinf,false,true); break;
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

//----------------------------------------------------
/*
bool TUI::MouseBox(
	ICullPlane *planes, // four planes !
	const Fvector2& p1, const Fvector2& p2 )
{
    Fvector2 point1, point2;
    point1.set(p1); point2.set(p2);//-p2->x; point2.y = -p2->y;
	if( point1.x != point2.x &&
		point1.y != point2.y )
	{
		Fvector2 p[4];
		Fvector st1,st2,n1,n2;
		Fvector planenormal;

		p[0].x = min( point1.x, point2.x );
		p[0].y = min( point1.y, point2.y );
		p[1].x = max( point1.x, point2.x );
		p[1].y = min( point1.y, point2.y );
		p[2].x = max( point1.x, point2.x );
		p[2].y = max( point1.y, point2.y );
		p[3].x = min( point1.x, point2.x );
		p[3].y = max( point1.y, point2.y );

		for( int i=0; i<4; i++){
			int i1 = (i+1)%4;
			MouseRayFromPoint( st1, n1, p[i] );
			MouseRayFromPoint( st2, n2, p[i1] );
			planenormal.crossproduct( n2, n1 );
			planenormal.normalize_safe();
			planes[i].set( planenormal, st1 );
		}

		return true;
	}

	return false;
}
*/
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
            if (Scene->RayPick(st, d, OBJCLASS_EDITOBJECT, &pinf, false, false))
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
// set render state
    // filter
    for (DWORD k=0; k<HW.Caps.dwNumBlendStages; k++){
        if( bRenderFilter ){
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
    if (bRenderLights)	Device.SetRS(D3DRS_AMBIENT,0x00000000);
    else                Device.SetRS(D3DRS_AMBIENT,0xFFFFFFFF);

    try{
	    if (bResize){ Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height); bResize=false; }

        Device.Begin();
        Device.UpdateView();
		Device.ResetMaterial();

        Device.SetRS(D3DRS_FILLMODE, dwRenderFillMode);
		Device.SetRS(D3DRS_SHADEMODE,dwRenderShadeMode);

    // draw grid
    	if (bDrawGrid){
	        DU::DrawGrid();
    	    DU::DrawPivot(m_Pivot);
        }

	    EEditorState est = GetEState();
        switch(est){
        case esEditLibrary: 	if (frmEditLibrary) frmEditLibrary->OnRender(); break;
        case esEditShaders: 	if (frmEditShaders) frmEditShaders->OnRender(); break;
        case esEditParticles: 	TfrmEditParticles::OnRender(); break;
        case esEditImages:	 	TfrmImageLib::OnRender(); break;
        case esEditScene:		Scene->Render(&precalc_identity); break;
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

    if (!bRenderRealTime) bRedraw = false;

    fraBottomBar->paSel->Caption = AnsiString(AnsiString(" Sel: ")+AnsiString(Scene->SelectionCount(true,UI->CurrentClassID())));
}
//---------------------------------------------------------------------------
void TUI::Idle()
{
    if (g_ErrorMode) return;
//    ELog.Msg(mtInformation,"%f",Device.m_FrameDTime);
    Sleep(2);
    EEditorState est = GetEState();
    if ((est==esEditScene)||(est==esEditLibrary)||(est==esEditShaders)||(est==esEditParticles)||(est==esEditImages)){
    	switch(est){
        case esEditShaders: 	if (frmEditShaders) frmEditShaders->OnIdle(); break;
    	case esEditLibrary: 	if (frmEditLibrary) frmEditLibrary->OnIdle(); break;
    	case esEditParticles: 	TfrmEditParticles::OnIdle(); break;
    	case esEditImages: 		TfrmImageLib::OnIdle(); break;
        }
	    if (bUpdateScene) RealUpdateScene();
    	if (bRedraw){
			Device.UpdateTimer();
            Scene->Update(Device.fTimeDelta);
        	Redraw();
        }
		m_Tools->Update();
    }
        // show hint
    UI->ShowObjectHint();
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
        Scene->OnShowHint(SS);
        if (ShowHint(SS)) fHintHideTime = fHintHide;
    }else{
        fHintPauseTime -= Device.fTimeDelta;
    }
}

bool __fastcall TUI::KeyDown (WORD Key, TShiftState Shift)
{
	if (Device.m_Camera.KeyDown(Key,Shift)) return true;
    return m_Tools->KeyDown(Key, Shift);
}

bool __fastcall TUI::KeyUp   (WORD Key, TShiftState Shift)
{
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
	    Scene->OnObjectsUpdate();
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
//    if(m_MouseCaptured||Device.m_Camera.IsMoving()) return;
    if(m_MouseCaptured) return;

    bMouseInUse = true;

    TShiftState Shift;
    if (iGetKeyState(DIK_LSHIFT)) 	Shift << ssShift;
    if (iGetKeyState(DIK_RSHIFT)) 	Shift << ssShift;
    if (iGetKeyState(DIK_LALT)) 	Shift << ssAlt;
    if (iGetKeyState(DIK_RALT)) 	Shift << ssAlt;
    if (iGetKeyState(DIK_LCONTROL))	Shift << ssCtrl;
    if (iGetKeyState(DIK_RCONTROL)) Shift << ssCtrl;
    if (iGetBtnState(0))			Shift << ssLeft;
    if (iGetBtnState(1))			Shift << ssRight;

    // camera activate
    if(!Device.m_Camera.MoveStart(Shift)){
        if( Scene->locked() ){
		// ELog.DlgMsg( mtError, "Scene sharing violation..." );
            return;
        }
        if( !m_MouseCaptured ){
            if( m_Tools->HiddenMode() ){
				iGetMousePosReal(m_StartCpH);
                m_DeltaCpH.set(0,0);
            }else{
                iGetMousePosReal(m_CurrentCp);
                ClientToScreen(Device.m_hWnd,(LPPOINT)&m_CurrentCp);
                ScreenToClient(Device.m_hRenderWnd,(LPPOINT)&m_CurrentCp);
                m_StartCp = m_CurrentCp;

                Device.m_Camera.MouseRayFromPoint(m_StartRStart, m_StartRNorm, m_StartCp );
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart, m_CurrentRNorm, m_CurrentCp );
            }

            if(m_Tools->MouseStart(Shift)){
                if(m_Tools->HiddenMode()) ShowCursor( FALSE );

                SetCapture( m_D3DWindow );
                m_MouseCaptured = true;
            }
        }
    }
    RedrawScene();
}
void TUI::OnMouseRelease(int btn){
	if(!g_bEditorValid) return;

    TShiftState Shift;
    if (iGetKeyState(DIK_LSHIFT)) 	Shift << ssShift;
    if (iGetKeyState(DIK_RSHIFT)) 	Shift << ssShift;
    if (iGetKeyState(DIK_LALT)) 	Shift << ssAlt;
    if (iGetKeyState(DIK_RALT)) 	Shift << ssAlt;
    if (iGetKeyState(DIK_LCONTROL))	Shift << ssCtrl;
    if (iGetKeyState(DIK_RCONTROL)) Shift << ssCtrl;
    if (iGetBtnState(0))			Shift << ssLeft;
    if (iGetBtnState(1))			Shift << ssRight;

    if( Device.m_Camera.IsMoving() ){
        if (Device.m_Camera.MoveEnd(Shift)) bMouseInUse = false;
    }else{
	    bMouseInUse = false;
        if( m_MouseCaptured ){
            if( m_Tools->HiddenMode() ){
//                GetCursorPos( &m_DeltaCpH );
//                m_DeltaCpH.x -= m_CenterCpH.x;
//                m_DeltaCpH.y -= m_CenterCpH.y;
            }else{
                iGetMousePosReal(m_CurrentCp);
                ClientToScreen(Device.m_hWnd,(LPPOINT)&m_CurrentCp);
                ScreenToClient(Device.m_hRenderWnd,(LPPOINT)&m_CurrentCp);
                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp );
            }
            if( m_Tools->MouseEnd(Shift) ){
                if( m_Tools->HiddenMode() ){
                    SetCursorPos(m_StartCpH.x,m_StartCpH.y);
                    ShowCursor( TRUE );
                }
                ReleaseCapture();
                m_MouseCaptured = false;
            }
        }
    }
    // update tools (change action)
    m_Tools->Update();
    RedrawScene();
}
void TUI::OnMouseHold(int btn){
}
void TUI::OnMouseMove(int x, int y){
	if(!g_bEditorValid) return;
    bool bRayUpdated = false;
    TShiftState Shift;
    if (iGetKeyState(DIK_LSHIFT)) 	Shift << ssShift;
    if (iGetKeyState(DIK_RSHIFT)) 	Shift << ssShift;
    if (iGetKeyState(DIK_LALT)) 	Shift << ssAlt;
    if (iGetKeyState(DIK_RALT)) 	Shift << ssAlt;
    if (iGetKeyState(DIK_LCONTROL))	Shift << ssCtrl;
    if (iGetKeyState(DIK_RCONTROL)) Shift << ssCtrl;
    if (iGetBtnState(0))			Shift << ssLeft;
    if (iGetBtnState(1))			Shift << ssRight;
	if (!Device.m_Camera.Process(Shift)){
        if( m_MouseCaptured || m_MouseMultiClickCaptured ){
            if( m_Tools->HiddenMode() ){
//                GetCursorPos( &m_DeltaCpH );
//                m_DeltaCpH.x -= m_CenterCpH.x;
//                m_DeltaCpH.y -= m_CenterCpH.y;
				m_DeltaCpH.set(x,y);
                if( m_DeltaCpH.x || m_DeltaCpH.y ){
//                	SetCursorPos(m_CenterCpH.x,m_CenterCpH.y);
                	m_Tools->MouseMove(Shift);
                }
            }else{
                iGetMousePosReal(m_CurrentCp);
                ClientToScreen(Device.m_hWnd,(LPPOINT)&m_CurrentCp);
                ScreenToClient(Device.m_hRenderWnd,(LPPOINT)&m_CurrentCp);

                Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp);
                m_Tools->MouseMove(Shift);
            }
		    UI->RedrawScene();
            bRayUpdated = true;
        }
    }
    if (!bRayUpdated){
		iGetMousePosReal(m_CurrentCp);
        Device.m_Camera.MouseRayFromPoint(m_CurrentRStart,m_CurrentRNorm,m_CurrentCp);
    }
    if (m_Cursor->GetVisible()) UI->RedrawScene();
    // Out cursor pos
    OutUICursorPos();
}
void TUI::OnMouseStop(int x, int y){
}

void TUI::OnKeyboardPress(int dik){
}
void TUI::OnKeyboardRelease(int dik){
}
void TUI::OnKeyboardHold(int dik){
}

