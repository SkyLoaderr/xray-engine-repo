//---------------------------------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "UI_Main.h"
#include "D3DUtils.h"
#include "builder.h"
#include "SceneClassList.h"
#include "sceneproperties.h"

#include "SelectObject.h"
#include "SelectLight.h"
#include "SelectSound.h"
#include "SelectPClipper.h"
#include "VisibilityTester.h"
#include "Statistic.h"
#include "UI_Tools.h"
#include "Scene.h"
#include "Library.h"
#include "Texturizer.h"

TUI UI;

TUI::TUI()
{
    paRender = 0;
    ppRender = 0;

    m_RenderWidth = m_RenderHeight = 0;
    m_RealWidth = m_RealHeight = 0;

    m_DX = 0;

    SB_DrawMeshLayer = 0;
    SB_DrawMeshLayerAdd = 0;
    SB_DrawMeshLayerMul = 0;
    SB_DrawMeshLayerAlpha = 0;
    SB_DrawWire = 0;
    SB_DrawSelectionRect = 0;
    SB_DrawClipPlane = 0;

	m_StartRStart.set(0,0,0);
	m_StartRNorm.set(0,0,0);
	m_CurrentRStart.set(0,0,0);
	m_CurrentRNorm.set(0,0,0);

    frustum     = new CFrustumClipper();
    stat        = new CStatistic();
    m_Tools     = new TUI_Tools();

	DU_InitUtilLibrary();

    m_D3D_ok = false;
    bRedraw = false;
	bResize = true;

	m_LastPick.set(0,0,0);
	m_LastStart.set(0,0,0);
	m_LastDir.set(0,0,0);

	m_Aspect = 1.f;
	m_FOV = deg2rad( 60.f );
	m_Znear = 0.1f;
	m_Zfar = 500.f;
    m_ScreenQuality = 1.f;

	m_Camera.identity();
	m_Projection.identity();
	m_Identity.identity();

	m_Pivot.set( 0, 0, 0 );
	m_PivotSnap.set( 0.1f, 0.1f, 0.1f );
    m_MoveSnap = 0.1f;
	m_AngleSnap = deg2rad( 5.f );

    m_CameraMoveMode = false;
	m_MouseCaptured = false;
    m_MouseMultiClickCaptured = false;
 	m_SelectionRect = false;

	m_SelectionChanged = false;
	m_VisibilityChanged = false;

	m_LastFileName[0] = 0;
	m_FramePrevTime  = 0;
	m_FrameDTime     = 0;

// grid
	m_GridCenter.set( 0, 0 );
	m_GridStep.set( 2.f, 2.f );
	m_GridSubDiv[0] = 8;
	m_GridSubDiv[1] = 8;
	m_GridCounts[0] = 127;
	m_GridCounts[1] = 127;

	m_ColorFore_B.set( 191, 255, 191, 0 );
	m_ColorBack_S.set( 140, 116, 175, 0 );
	m_ColorFore_S.set( 191, 255, 191, 0 );
	m_BackgroundColor.set( 127, 127, 127, 0 );
	m_ColorGrid.set( 150, 150, 150, 0 );
	m_ColorGridTh.set( 180, 180, 180, 0 );

	D3D_UpdateGrid();

    bRenderTextures = true;
    bRenderWire     = false;
    bRenderLights   = true;
    bRenderFilter   = true;
    bRenderMultiTex = true;
    bDrawLights     = true;
    bDrawSounds     = true;
    bDrawGrid       = true;
    bDrawPivot      = true;
    bDrawStatistic  = false;
}
//---------------------------------------------------------------------------
TUI::~TUI()
{
	DU_UninitUtilLibrary();
    _DELETE(frustum);
    _DELETE(stat);
    _DELETE(m_Tools);
}

bool TUI::Init(TPanel* pa, TPaintBox* pb)
{
    paRender = pa;
    ppRender = pb;
    VERIFY(paRender);
    VERIFY(ppRender);
    if (!D3D_Create()) return false;
    m_Tools->Init();
    CameraPan(0,600);
    CameraScale(-100);
    return true;
}

void TUI::Clear()
{
    D3D_Clear();
}

LPDIRECT3DDEVICE7 TUI::d3d()    {   return m_DX->pD3DDev;}
LPDIRECTDRAW7 TUI::dd()         {   return m_DX->pDD;}
LPDDPIXELFORMAT TUI::backdesc() {   return &(m_DX->BackDesc.ddpfPixelFormat);}

void TUI::CameraPan(float dx, float dz)
{
    Fvector vmove;
    vmove.set( m_Camera.k );  vmove.y = 0;
    vmove.normalize_safe();
    vmove.mul( dz*-0.02f );
    m_Camera.c.add( vmove );

    vmove.set( m_Camera.i );  vmove.y = 0;
    vmove.normalize_safe();
    vmove.mul( dx*0.02f );
    m_Camera.c.add( vmove );

    UpdateScene();
}

void TUI::CameraScale(float dy)
{
    Fvector vmove;
    vmove.set( 0, dy, 0 );
    vmove.y *= -0.02f;
    m_Camera.c.add( vmove );
    UpdateScene();
}

void TUI::CameraRotate(float dx, float dy)
{
    Fvector vr;
    Fvector vmove;
    Fmatrix rmatrix;

    vr.set( 0, 1, 0 );

    rmatrix.rotation( vr, 0.0015f * dx );
    rmatrix.transform( m_Camera.i );
    rmatrix.transform( m_Camera.j );
    rmatrix.transform( m_Camera.k );

    rmatrix.rotation( m_Camera.i, 0.0015f * dy );
    rmatrix.transform( m_Camera.i );
    rmatrix.transform( m_Camera.j );
    rmatrix.transform( m_Camera.k );

    UpdateScene();
}

bool TUI::PickGround(
	Fvector *hitpoint,
	Fvector *start,
	Fvector *direction )
{
	_ASSERTE( start );
	_ASSERTE( direction );
	_ASSERTE( hitpoint );

	Fvector normal;
	normal.set( 0, 1, 0 );

	float clcheck = direction->dotproduct( normal );
	if( is_zero( clcheck ) )
		return false;

	float alpha = - start->dotproduct(normal) / clcheck;
	if( alpha <= 0 )
		return false;

	hitpoint->x = start->x + direction->x * alpha;
	hitpoint->y = start->y + direction->y * alpha;
	hitpoint->z = start->z + direction->z * alpha;

	m_LastPick.set( (*hitpoint) );

	return true;
}

void TUI::MouseRayFromPoint(
	Fvector *start,
	Fvector *direction,
	POINT *point )
{
	_ASSERTE( start );
	_ASSERTE( direction );
	_ASSERTE( point );

	int halfwidth = paRender->Width / 2;
	int halfheight = paRender->Height / 2;

	POINT point2;
	point2.x = point->x - halfwidth;
	point2.y = halfheight - point->y;

	start->set( m_Camera.c );

	float size_y = m_Znear * tan( m_FOV * 0.5f );
	float size_x = size_y / m_Aspect;

	float r_pt = point2.x * size_x / (float) halfwidth;
	float u_pt = point2.y * size_y / (float) halfheight;

	direction->mul( m_Camera.k, m_Znear );
	direction->translate( m_Camera.j, u_pt );
	direction->translate( m_Camera.i, r_pt );
	direction->normalize();

	m_LastStart.set( (*start) );
	m_LastDir.set( (*direction) );
}

//----------------------------------------------------

bool TUI::MouseBox(
	ICullPlane *planes, // four planes !
	POINT *p1, POINT *p2 )
{
    POINT point1, point2;
    point1 = *p1; point2 = *p2;//-p2->x; point2.y = -p2->y;
	if( point1.x != point2.x &&
		point1.y != point2.y )
	{
		POINT p[4];
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
			MouseRayFromPoint( &st1, &n1, p+i );
			MouseRayFromPoint( &st2, &n2, p+i1 );
			planenormal.crossproduct( n2, n1 );
			planenormal.normalize_safe();
			planes[i].set( planenormal, st1 );
		}

		return true;
	}

	return false;
}

//----------------------------------------------------
void TUI::Redraw(){
    stat->Reset();

	if( bRenderFilter ){
        CDX( m_DX->pD3DDev->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFG_LINEAR));
        CDX( m_DX->pD3DDev->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFG_LINEAR));
	} else {
		CDX( m_DX->pD3DDev->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFG_POINT));
		CDX( m_DX->pD3DDev->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFG_POINT));
	}

    D3D_StartDraw();

    stat->lTotalFrame++;
// frustum culling sets
    frustum->BuildFrustum(m_Camera.c,m_Camera.k,m_Camera.j);

// set camera matrix
    Fmatrix viewmatrix;
    viewmatrix.invert( m_Camera );
    CDX( m_DX->pD3DDev->SetTransform(D3DTRANSFORMSTATE_VIEW,viewmatrix.d3d()) );

// draw grid
    D3D_DrawGrid();
	D3D_DrawPivot();

// draw all objects
	if( !Builder.InProgress() )
		Scene.Render( m_Aspect, &m_Camera );

// draw pivot
	D3D_DrawSelectionRect();

// end draw
    D3D_EndDraw();

    bRedraw = false;
}
//---------------------------------------------------------------------------
void TUI::Idle()
{
    float t = timeGetTime();
    m_FrameDTime = 0.001f * ( t - m_FramePrevTime );
    m_FramePrevTime = t;
    Sleep(1);
//    AnsiString n;
//    n.sprintf("%.3f : %.3f", m_FrameDTime, m_FramePrevTime);
//    frmMain->Panel4->Caption = n;
    if (bRedraw)  Redraw();
}
//---------------------------------------------------------------------------
void TUI::EnableSelectionRect( bool flag ){
	m_SelectionRect = flag;
	m_SelEnd.x = m_SelStart.x = 0;
	m_SelEnd.y = m_SelStart.y = 0;
}

void TUI::UpdateSelectionRect( POINT *from, POINT *to ){
	_ASSERTE( from && to );
	m_SelStart = (*from);
	m_SelEnd = (*to);
}

void __fastcall TUI::MouseStart(TShiftState Shift, int X, int Y){
	if( Scene.locked() ){
		Log.Msg( "Scene sharing violation..." );
		return;
	}

    if(m_MouseCaptured||m_CameraMoveMode) return;

    if(Shift.Contains(ssShift)){
// camera move
        m_CenterCpH.x = GetSystemMetrics(SM_CXSCREEN)/2;
        m_CenterCpH.y = GetSystemMetrics(SM_CYSCREEN)/2;
        GetCursorPos( &m_StartCpH );
        m_DeltaCpH.x = 0;
        m_DeltaCpH.y = 0;

        ShowCursor( FALSE );
        SetCursorPos( m_CenterCpH.x, m_CenterCpH.y );

        SetCapture( ppRender );
        m_CameraMoveMode = true;
    }else{
        if( !m_MouseCaptured ){
            if( m_Tools->HiddenMode() ){
                m_CenterCpH.x = GetSystemMetrics(SM_CXSCREEN)/2;
                m_CenterCpH.y = GetSystemMetrics(SM_CYSCREEN)/2;
                GetCursorPos( &m_StartCpH );
                m_DeltaCpH.x = 0;
                m_DeltaCpH.y = 0;
            }else{
                m_CurrentCp.x = X; m_CurrentCp.y = Y;
                m_StartCp = m_CurrentCp;

                MouseRayFromPoint(&m_StartRStart, &m_StartRNorm, &m_StartCp );
                MouseRayFromPoint(&m_CurrentRStart, &m_CurrentRNorm, &m_CurrentCp );
            }

            if( m_Tools->MouseStart(Shift) ){
                if( m_Tools->HiddenMode() ){
                    ShowCursor( FALSE );
                    SetCursorPos(
                        m_CenterCpH.x,
                        m_CenterCpH.y );
                }

                SetCapture( ppRender );
                m_MouseCaptured = true;
            }
        }
    }
}

void __fastcall TUI::MouseEnd(TShiftState Shift, int X, int Y){
    if( m_CameraMoveMode ){
        GetCursorPos( &m_DeltaCpH );
        m_DeltaCpH.x -= m_CenterCpH.x;
        m_DeltaCpH.y -= m_CenterCpH.y;
        SetCursorPos(m_StartCpH.x,m_StartCpH.y);
        ShowCursor( TRUE );
        ReleaseCapture();
        m_CameraMoveMode = false;
    }else{
        if( m_MouseCaptured ){
            if( m_Tools->HiddenMode() ){
                GetCursorPos( &m_DeltaCpH );
                m_DeltaCpH.x -= m_CenterCpH.x;
                m_DeltaCpH.y -= m_CenterCpH.y;
            }
            else {
                m_CurrentCp.x = X; m_CurrentCp.y = Y;

                MouseRayFromPoint(
                    &m_CurrentRStart,
                    &m_CurrentRNorm,
                    &m_CurrentCp );
            }
            if( m_Tools->MouseEnd(Shift) ){
                if( m_Tools->HiddenMode() ){
                    SetCursorPos(
                        m_StartCpH.x,
                        m_StartCpH.y );
                    ShowCursor( TRUE );
                }
                ReleaseCapture();
                m_MouseCaptured = false;
            }
        }
    }
}

void __fastcall TUI::MouseProcess(TShiftState Shift, int X, int Y){
    if (m_CameraMoveMode){
// camera move
        GetCursorPos( &m_DeltaCpH );
        m_DeltaCpH.x -= m_CenterCpH.x;
        m_DeltaCpH.y -= m_CenterCpH.y;
        if( m_DeltaCpH.x || m_DeltaCpH.y ){
            SetCursorPos(m_CenterCpH.x,m_CenterCpH.y);
            if (Shift.Contains(ssLeft) && Shift.Contains(ssRight)){
                UI.CameraRotate (UI.m_DeltaCpH.x,UI.m_DeltaCpH.y);
            }else{
                if (Shift.Contains(ssLeft))        UI.CameraPan    (UI.m_DeltaCpH.x,UI.m_DeltaCpH.y);
                else if(Shift.Contains(ssRight))   UI.CameraScale  (UI.m_DeltaCpH.y);
            }
        }
    }else{
        if( m_MouseCaptured || m_MouseMultiClickCaptured ){
            if( m_Tools->HiddenMode() ){
                GetCursorPos( &m_DeltaCpH );
                m_DeltaCpH.x -= m_CenterCpH.x;
                m_DeltaCpH.y -= m_CenterCpH.y;
                if( m_DeltaCpH.x || m_DeltaCpH.y ){
                        SetCursorPos(
                            m_CenterCpH.x,
                            m_CenterCpH.y );
                        m_Tools->MouseMove(Shift);
                }
            }
            else{
                m_CurrentCp.x = X; m_CurrentCp.y = Y;

                MouseRayFromPoint(
                    &m_CurrentRStart,
                    &m_CurrentRNorm,
                    &m_CurrentCp );

                m_Tools->MouseMove(Shift);
            }
        }
    }
}

bool __fastcall TUI::KeyDown (WORD Key, TShiftState Shift)
{
    return m_Tools->KeyDown(Key, Shift);
}

bool __fastcall TUI::KeyUp   (WORD Key, TShiftState Shift)
{
    return m_Tools->KeyUp(Key, Shift);
}

bool __fastcall TUI::KeyPress(WORD Key, TShiftState Shift)
{
    return m_Tools->KeyPress(Key, Shift);
}

bool TUI::Command( int _Command, int p1 ){

	char filebuffer[MAX_PATH]="";

	switch( _Command ){

	case COMMAND_EXIT:
        if (!Scene.IfModified()) return false;
        Scene.Unload();
        Scene.UndoClear();
        Scene.Clear();
        Lib.Clear();
        UI.Clear();
		break;
	case COMMAND_SHOWPROPERTIES:{
        switch(p1){
            case OBJCLASS_SOBJECT2: fraSelectObject->RunEditor(); break;
            case OBJCLASS_LIGHT:    fraSelectLight->RunEditor(); break;
            case OBJCLASS_SOUND:    fraSelectSound->RunEditor(); break;
            case OBJCLASS_PCLIPPER: fraSelectPClipper->RunEditor(); break;
            default: throw -1;
            }
        }break;

	case COMMAND_LOAD:
		if( !Scene.locked() ){
			strcpy( filebuffer, m_LastFileName );
			if( FS.getopenname( &FS.m_Maps, filebuffer ) ){
                if (!Scene.IfModified()) return false;
                paRender->Caption = "Level loading...";  paRender->Repaint();
            	UI.Command( COMMAND_CLEAR );
				Scene.Load( filebuffer );
				strcpy(m_LastFileName,filebuffer);
                paRender->Caption = "";
              	Scene.UndoClear();
			}
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_SAVE:
		if( !Scene.locked() ){
			if( m_LastFileName[0] ){
				Scene.Save( m_LastFileName );
			}
			else{
				Command( COMMAND_SAVEAS ); }
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_SAVEAS:
		if( !Scene.locked() ){
			filebuffer[0] = 0;
			if( FS.getsavename( &FS.m_Maps, filebuffer ) ){
				Scene.Save( filebuffer );
				strcpy(m_LastFileName,filebuffer);
			}
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_CLEAR:
		if( !Scene.locked() ){
            if (!Scene.IfModified()) return false;
            Texturizer.Reset();
			Scene.Unload();
            Scene.ResetGlobals();
			m_LastFileName[0] = 0;
           	Scene.UndoClear();
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_UPDATE_ALL:
		if( !Scene.locked() ){
//			Scene.SmoothLandscape();
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_CUT:
		if( !Scene.locked() ){
			Scene.UndoPrepare();
			Scene.CutSelection();
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_COPY:
		if( !Scene.locked() ){
			Scene.UndoPrepare();
			Scene.CopySelection();
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_PASTE:
		if( !Scene.locked() ){
			Scene.UndoPrepare();
			Scene.PasteSelection();
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_UNDO:
		if( !Scene.locked() ){
			if( !Scene.Undo() ){
				Log.Msg( "Undo buffer empty" ); }
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_REDO:
		if( !Scene.locked() ){
			if( !Scene.Redo() ){
				Log.Msg( "Redo buffer empty" ); }
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_OPTIONS:
		if( !Scene.locked() ){
            frmSceneProperties->ShowModal();
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_BUILD:
		if( !Builder.InProgress() ){
            if (frmSceneProperties->Run()==mrOk)
			    Builder.Execute( &Scene );
		}
		break;

	case COMMAND_INVERT_SELECTION_ALL:
		if( !Scene.locked() ){
			Scene.UndoPrepare();
			InvertSelection();
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_SELECT_ALL:
		if( !Scene.locked() ){
			SetSelection( true );
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_DESELECT_ALL:
		if( !Scene.locked() ){
			SetSelection( false );
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_DELETE_SELECTION:
		if( !Scene.locked() ){
			Scene.UndoPrepare();
			Scene.RemoveSelection( ClassIDFromTargetID(UI.m_Tools->GetTarget()) );
		} else {
			Log.Msg( "Scene sharing violation" ); }
		break;

	case COMMAND_HIDE_SELECTION:
		if( !Scene.locked() ){
			Scene.UndoPrepare();
            //(Hide,  class filter, only selected)
			Scene.ShowObjects( false, ClassIDFromTargetID(UI.m_Tools->GetTarget()), true );
		} else {
			Log.Msg( "Scene sharing violation" ); }
        break;
	default:
		Log.Msg( "Warning: Undefined command: %04d", _Command );
        return false;
		}

    UI.UpdateScene();
    return true;
}

//----------------------------------------------------
void TUI::SetSelection( bool flag ){
	int clsid = ClassIDFromTargetID( UI.m_Tools->GetTarget() );
	Scene.SelectObjects(flag,clsid);
}

void TUI::InvertSelection(){
	int clsid = ClassIDFromTargetID( UI.m_Tools->GetTarget() );
	Scene.InvertSelection( clsid );
}

