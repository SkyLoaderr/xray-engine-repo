//----------------------------------------------------
// file: UI_Main.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "UI_SceneOptions.h"
#include "Scene.h"
#include "Builder.h"
#include "CustomControls.h"
#include "SceneClassList.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

XRayEditorUI UI;
int g_RenderLightMaps = 0;
int g_LinearTFilter = 0;

//----------------------------------------------------

XRayEditorUI::XRayEditorUI(){
	
	m_LeftSubPlace = 0;
	m_LeftBar = 0;
	m_TopBar = 0;
	m_BottomBar = 0;
	m_Window = 0;
	m_Instance = 0;

	m_Target = TARGET_LANDSCAPE;
	m_Action = ACTION_SELECT;
	m_Viewport = VIEWPORT_MAP;

	m_WireMode = 0;
	m_TexturesEnabled = 1;
	m_GridEnabled = 1;
	
	m_FltPivots = 1;
	m_FltSounds = 0;
	m_FltLights = 1;

	m_LastPick.set(0,0,0);
	m_LastStart.set(0,0,0);
	m_LastDir.set(0,0,0);

	m_Aspect = 1.f;
	m_FOV = deg2rad( 60.f );
	m_Znear = 0.1f;
	m_Zfar = 300.f;

	m_Camera.identity();
	m_Projection.identity();
	m_Identity.identity();
	m_D3D_ok = false;
	m_RenderWidth = 0;
	m_RenderHeight = 0;

	m_Pivot.set( 0, 0, 0 );
	m_PivotSnap.set( 0.1f, 0.1f, 0.1f );
	m_AngleSnap = deg2rad( 5.f );

	m_MouseCaptured = false;
	m_SelectionRect = false;

	m_SelectionChanged = false;
	m_VisibilityChanged = false;

	m_LastFileName[0] = 0;

	ResetToDefaultColors();
	ResetToDefaultGrid();
}

XRayEditorUI::~XRayEditorUI(){
	m_GridPoints.clear();
	m_GridIndices.clear();
}

//----------------------------------------------------

bool XRayEditorUI::Init( HINSTANCE _Instance ){
	
	m_Instance = _Instance;

	InitColorSheme();
	InitCommonControls();
	MSC_Init();

	if( !CreateMainWindow() ) return false;
	if( !CreateLeftBar() ) return false;
	if( !CreateTopBar() ) return false;
	if( !CreateBottomBar() ) return false;
	if( !CreateVisWindow() ) return false;

	AlignWindows();
	UpdateToolOptions();

	if( !D3D_Create() )
		return false;

	m_FramePrevTime = timeGetTime();
	m_FrameDTime = 1;

	return true;
}

void XRayEditorUI::Clear( ){

	D3D_Clear();

	if( m_CurrentBox ){
		m_CurrentBox->Restore();
		m_CurrentBox = 0; }
	
	DestroyVisWindow();
	DestroyBottomBar();
	DestroyTopBar();
	DestroyLeftBar();
	DestroyMainWindow();

	MSC_Uninit();
	ClearColorSheme();
}

void XRayEditorUI::Idle( ){
	
	if( !Builder.InProgress() )
		Scene.Update( 0.030f );

	if( m_CurrentBox ){
		if( m_SelectionChanged )
			m_CurrentBox->SelectionChanged();
		if( m_VisibilityChanged )
			m_CurrentBox->VisibilityChanged();
	}

	m_SelectionChanged = false;
	m_VisibilityChanged = false;

	Redraw();

	if( GetForegroundWindow() == m_Window ){
		char map[256];
		if( GetKeyboardState((LPBYTE)map) ){

			m_FrameDTime = 0.001f * ( timeGetTime() - m_FramePrevTime );
			m_FramePrevTime = timeGetTime();

			IVector vr;
			IVector vmove;
			IMatrix rmatrix;

			vr.set( 0, 1, 0 );

			if( (map[VK_RIGHT]&0x80)  ){
				rmatrix.r( m_FrameDTime * M_PI, vr );
				rmatrix.transform( m_Camera.i );
				rmatrix.transform( m_Camera.j );
				rmatrix.transform( m_Camera.k );
			}

			if( (map[VK_LEFT]&0x80)  ){
				rmatrix.r( - m_FrameDTime * M_PI, vr );
				rmatrix.transform( m_Camera.i );
				rmatrix.transform( m_Camera.j );
				rmatrix.transform( m_Camera.k );
			}

			if( (map[VK_UP]&0x80)  ){
				rmatrix.r( -m_FrameDTime * M_PI, m_Camera.i );
				rmatrix.transform( m_Camera.i );
				rmatrix.transform( m_Camera.j );
				rmatrix.transform( m_Camera.k );
			}

			if( (map[VK_DOWN]&0x80)  ){
				rmatrix.r( m_FrameDTime * M_PI, m_Camera.i );
				rmatrix.transform( m_Camera.i );
				rmatrix.transform( m_Camera.j );
				rmatrix.transform( m_Camera.k );
			}

			if( (map[VK_CONTROL]&0x80)  ){

				if( (map['A']&0x80)  ){
					vmove.set( 0, 1, 0 );
					vmove.mul( m_FrameDTime * 5.f );
					m_Camera.c.add( vmove );
				}

				if( (map['Z']&0x80)  ){
					vmove.set( 0, 1, 0 );
					vmove.mul( -m_FrameDTime * 5.f );
					m_Camera.c.add( vmove );
				}

			} else {

				if( (map['A']&0x80)  ){
					vmove.set( m_Camera.k );
					vmove.y = 0;
					vmove.safe_normalize();
					vmove.mul( m_FrameDTime * 5.f );
					m_Camera.c.add( vmove );
				}

				if( (map['Z']&0x80)  ){
					vmove.set( m_Camera.k );
					vmove.y = 0;
					vmove.safe_normalize();
					vmove.mul( -m_FrameDTime * 5.f );
					m_Camera.c.add( vmove );
				}
			}
		}
	}
}

void XRayEditorUI::Redraw( ){

	RECT rc;
	if( !GetClientRect( m_VisWindow, &rc ) ) return;
	if( rc.bottom <= 0 || (rc.bottom < rc.top) ) return;
	D3D_StartDraw( &rc );
	D3D_DrawGrid();

	if( !Builder.InProgress() )
		Scene.Render( m_Aspect, &m_Camera );

	D3D_DrawPivot();
	D3D_DrawSelectionRect();
	D3D_EndDraw();
}

//----------------------------------------------------

void XRayEditorUI::ChangeTarget( int _NewTarget ){
	if( m_Target != _NewTarget ){
		m_Target = _NewTarget;
		InvalidateRect( GetDlgItem( m_LeftBar, IDC_TARGET_LAND), 0, 0 );
		InvalidateRect( GetDlgItem( m_LeftBar, IDC_TARGET_OBJ), 0, 0 );
		InvalidateRect( GetDlgItem( m_LeftBar, IDC_TARGET_LIGHT), 0, 0 );
		InvalidateRect( GetDlgItem( m_LeftBar, IDC_TARGET_PIVOT), 0, 0 );
		InvalidateRect( GetDlgItem( m_LeftBar, IDC_TARGET_SNDPLANE), 0, 0 );
		InvalidateRect( GetDlgItem( m_LeftBar, IDC_TARGET_SOUND), 0, 0 );
		UpdateToolOptions();
		NConsole.print( "Target changed to %d", m_Target );
	}
}

void XRayEditorUI::ChangeAction( int _NewAction ){
	if( m_Action != _NewAction ){
		m_Action = _NewAction;
		InvalidateRect( GetDlgItem( m_LeftBar, IDC_ACTION_SELECT), 0, 0 );
		InvalidateRect( GetDlgItem( m_LeftBar, IDC_ACTION_ADD), 0, 0 );
		InvalidateRect( GetDlgItem( m_LeftBar, IDC_ACTION_MOVE), 0, 0 );
		InvalidateRect( GetDlgItem( m_LeftBar, IDC_ACTION_ROTATE), 0, 0 );
		InvalidateRect( GetDlgItem( m_LeftBar, IDC_ACTION_SCALE), 0, 0 );
		UpdateToolOptions();
		NConsole.print( "Action changed to %d", m_Action );
	}
}

void XRayEditorUI::ChangeViewport( int _NewViewport ){
	if( m_Viewport != _NewViewport ){
		m_Viewport = _NewViewport;
		InvalidateRect( GetDlgItem( m_TopBar, IDC_VIEW_TYPE_MAP), 0, 0 );
		InvalidateRect( GetDlgItem( m_TopBar, IDC_VIEW_TYPE_USER), 0, 0 );
		InvalidateRect( GetDlgItem( m_TopBar, IDC_VIEW_TYPE_OBJECT), 0, 0 );
		NConsole.print( "Viewport changed to %d", m_Viewport );
	}
}

void XRayEditorUI::AccelCommand( int _Command ){
	switch( _Command ){

	case ID_ACCEL_LOAD:
		Command( COMMAND_LOAD );
		break;
	case ID_ACCEL_SAVE:
		Command( COMMAND_SAVE );
		break;

	case ID_ACCEL_TARGET_LAND:
		ChangeTarget( TARGET_LANDSCAPE );
		break;
	case ID_ACCEL_TARGET_OBJ:
		ChangeTarget( TARGET_OBJECT );
		break;
	case ID_ACCEL_TARGET_LIGHT:
		ChangeTarget( TARGET_LIGHT );
		break;
	case ID_ACCEL_TARGET_PIVOT:
		ChangeTarget( TARGET_PIVOT );
		break;
	case ID_ACCEL_TARGET_SNDPLANE:
		ChangeTarget( TARGET_SNDPLANE );
		break;

	case ID_ACCEL_ACTION_MOVE:
		ChangeAction( ACTION_MOVE );
		break;
	case ID_ACCEL_ACTION_ROTATE:
		ChangeAction( ACTION_ROTATE );
		break;
	case ID_ACCEL_ACTION_SCALE:
		ChangeAction( ACTION_SCALE );
		break;
	case ID_ACCEL_ACTION_SELECT:
		ChangeAction( ACTION_SELECT );
		break;
	case ID_ACCEL_ACTION_ADD:
		ChangeAction( ACTION_ADD );
		break;

	case ID_ACCEL_DESELECT_ALL:
		Command( COMMAND_DESELECT_ALL );
		break;
	case ID_ACCEL_SEL_ALL:
		Command( COMMAND_SELECT_ALL );
		break;
	case ID_ACCEL_INVSEL_ALL:
		Command( COMMAND_INVERT_SELECTION_ALL );
		break;

	case ID_ACCEL_RLM:
		g_RenderLightMaps = !g_RenderLightMaps;
		Scene.RebuildAllObjects();
		break;
	case ID_ACCEL_FILTER:
		g_LinearTFilter = !g_LinearTFilter;
		Scene.RebuildAllObjects();
		break;

	case ID_ACCEL_DELETE_SELECTION:
		Command( COMMAND_DELETE_SELECTION );
		break;

	}
}


void XRayEditorUI::Command( int _Command ){
	
	char filebuffer[MAX_PATH]="";

	switch( _Command ){

	case COMMAND_EXIT:
		PostQuitMessage(0);
		break;

	case COMMAND_HIDE:
		ShowWindow( m_Window, SW_MINIMIZE );
		break;

	case COMMAND_CONSOLE:
		NConsole.Init( m_Instance, 0 );
		break;

	case COMMAND_LOAD:
		if( !Scene.locked() ){
			strcpy( filebuffer, m_LastFileName );
			if( FS.getopenname( &FS.m_Maps, filebuffer ) ){
				Scene.UndoPrepare();
				Scene.Load( filebuffer );
				strcpy(m_LastFileName,filebuffer);
				UpdateCaption();
			}
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_SAVE:
		if( !Scene.locked() ){
			if( m_LastFileName[0] ){
				Scene.Save( m_LastFileName );
				UpdateCaption();
			}
			else{
				Command( COMMAND_SAVEAS ); }
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_SAVEAS:
		if( !Scene.locked() ){
			filebuffer[0] = 0;
			if( FS.getsavename( &FS.m_Maps, filebuffer ) ){
				Scene.Save( filebuffer );
				strcpy(m_LastFileName,filebuffer);
				UpdateCaption();
			}
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_CLEAR:
		if( !Scene.locked() ){
			Scene.UndoPrepare();
			Scene.Unload();
			m_LastFileName[0] = 0;
			UpdateCaption();
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_ENTER_PIVOT:
		EnterPivotDialog();
		break;

	case COMMAND_UPDATE_ALL:
		if( !Scene.locked() ){
			Scene.SmoothLandscape();
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_CUT:
		if( !Scene.locked() ){
			Scene.UndoPrepare();
			Scene.CutSelection();
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_COPY:
		if( !Scene.locked() ){
			Scene.CopySelection();
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_PASTE:
		if( !Scene.locked() ){
			Scene.UndoPrepare();
			Scene.PasteSelection();
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_UNDO:
		if( !Scene.locked() ){
			if( !Scene.Undo() ){
				NConsole.print( "Undo buffer empty" ); }
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_REDO:
		if( !Scene.locked() ){
			if( !Scene.Redo() ){
				NConsole.print( "Redo buffer empty" ); }
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_OPTIONS:
		if( !Scene.locked() ){
			OpDialog.Open( inst(), wnd() );
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_BUILD:
		if( !Builder.InProgress() ){
			Builder.Execute( &Scene );
		}
		break;

	case COMMAND_INVERT_SELECTION_ALL:
		if( !Scene.locked() ){
			InvertSelection();
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_SELECT_ALL:
		if( !Scene.locked() ){
			SetSelection( true );
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_DESELECT_ALL:
		if( !Scene.locked() ){
			SetSelection( false );
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	case COMMAND_DELETE_SELECTION:
		if( !Scene.locked() ){
			Scene.RemoveSelection( ClassIDFromTargetID(m_Target) );
		} else {
			NConsole.print( "Scene sharing violation" ); }
		break;

	default:
		NConsole.print( "Warning: Undefined command: %04d", _Command );
		break; }
}

//----------------------------------------------------

void XRayEditorUI::AlignWindows(){

	if( m_Window == 0 )
		return;

	RECT _Main;
	GetClientRect( m_Window, &_Main  );

	RECT _Left;

	if( m_LeftBar ){
		GetWindowRect( m_LeftBar, &_Left );
		SetWindowPos( m_LeftBar, 0,  0, 0, 
			_Left.right-_Left.left,
			_Main.bottom - _Main.top,
			SWP_NOZORDER );

		if( m_LeftSubPlace ){

			RECT parentrect,wrect;
			GetClientRect( m_LeftBar, &parentrect );
			GetWindowRect( m_LeftSubPlace, &wrect );
			MapWindowPoints( 0, m_LeftBar, (LPPOINT)&wrect, 2 );

			SetWindowPos( m_LeftSubPlace, 0, 0, 0,
				wrect.right - wrect.left,
				parentrect.bottom - wrect.top - 4,
				SWP_NOMOVE | SWP_NOZORDER );
		}

	} else {
		memset( &_Left, 0, sizeof(_Left) );
	}

	RECT _Top;

	if( m_TopBar ){
		GetWindowRect( m_TopBar, &_Top );
		SetWindowPos( m_TopBar, 0, 
			_Left.right - _Left.left, 0, 
			(_Main.right-_Main.left)-(_Left.right-_Left.left), 
			_Top.bottom - _Top.top, 
			SWP_NOZORDER );
	} else {
		memset( &_Top, 0, sizeof(_Top) );
	}

	RECT _Bottom;

	if( m_BottomBar ){
		GetWindowRect( m_BottomBar, &_Bottom );
		SetWindowPos( m_BottomBar, 0,
			_Left.right - _Left.left,
			_Main.bottom - (_Bottom.bottom - _Bottom.top),
			(_Main.right-_Main.left)-(_Left.right-_Left.left), 
			_Bottom.bottom - _Bottom.top, 
			SWP_NOZORDER );
	} else {
		memset( &_Bottom, 0, sizeof(_Bottom) );
	}

	if( m_VisWindow ){
		
		SetWindowPos( m_VisWindow, 0,
			_Left.right - _Left.left,
			_Top.bottom - _Top.top,
			_Main.right - (_Left.right - _Left.left),
			_Main.bottom - (_Top.bottom - _Top.top) - (_Bottom.bottom - _Bottom.top),
			SWP_NOZORDER );

		RECT renderrect;
		GetClientRect(m_VisWindow,&renderrect);
		m_RenderWidth = renderrect.right;
		m_RenderHeight = renderrect.bottom;

	} else{

		m_RenderWidth = 0;
		m_RenderHeight = 0;
	}

}

void XRayEditorUI::UpdateCaption(){
	if( m_TopBar != 0 ){
		char buffer[MAX_PATH] = "";
		if( m_LastFileName[0] )
			strcpy( buffer, m_LastFileName );
		SetDlgItemText( m_TopBar, IDC_FILENAME_TEXT, buffer );
	}
}

void XRayEditorUI::UpdateToolOptions(){
	
	if( m_CurrentBox ){
		m_CurrentBox->Restore();
		m_CurrentBox = 0; }

	if( m_Target==TARGET_LANDSCAPE ) {
		if( m_Action == ACTION_ADD ){
			m_CurrentBox = &m_CtlAddLandscape;
			m_CtlAddLandscape.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_ADD_LANDSCAPE );
		} else if( m_Action == ACTION_SELECT ){
			m_CurrentBox = &m_CtlSelectLandscape;
			m_CtlSelectLandscape.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_SELECT_LANDSCAPE );
		} else if( m_Action == ACTION_MOVE ){
			m_CurrentBox = &m_CtlMoveLandscape;
			m_CtlMoveLandscape.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_MOVE_LANDSCAPE );
		} else if( m_Action == ACTION_ROTATE ){
			m_CurrentBox = &m_CtlRotateLandscape;
			m_CtlRotateLandscape.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_ROTATE_LANDSCAPE );
		} else if( m_Action == ACTION_SCALE ){
			m_CurrentBox = &m_CtlScaleLandscape;
			m_CtlScaleLandscape.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_SCALE_LANDSCAPE );
		}

	} else if( m_Target==TARGET_OBJECT ) {
		if( m_Action == ACTION_ADD ){
			m_CurrentBox = &m_CtlAddObject;
			m_CtlAddObject.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_ADD_OBJECT );
		} else if( m_Action == ACTION_SELECT ){
			m_CurrentBox = &m_CtlSelectObject;
			m_CtlSelectObject.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_SELECT_OBJECT );
		} else if( m_Action == ACTION_MOVE ){
			m_CurrentBox = &m_CtlMoveObject;
			m_CtlMoveObject.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_MOVE_OBJECT );
		} else if( m_Action == ACTION_ROTATE ){
			m_CurrentBox = &m_CtlRotateObject;
			m_CtlRotateObject.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_ROTATE_OBJECT );
		} else if( m_Action == ACTION_SCALE ){
			m_CurrentBox = &m_CtlScaleObject;
			m_CtlScaleObject.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_SCALE_OBJECT );
		}

	} else if( m_Target==TARGET_LIGHT ) {
		if( m_Action == ACTION_ADD ){
			m_CurrentBox = &m_CtlAddLight;
			m_CtlAddLight.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_ADD_LIGHT );
		} else if( m_Action == ACTION_SELECT ){
			m_CurrentBox = &m_CtlSelectLight;
			m_CtlSelectLight.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_SELECT_LIGHT );
		} else if( m_Action == ACTION_MOVE ){
			m_CurrentBox = &m_CtlMoveLight;
			m_CtlMoveLight.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_MOVE_LIGHT );
		} else if( m_Action == ACTION_ROTATE ){
			m_CurrentBox = &m_CtlRotateLight;
			m_CtlRotateLight.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_ROTATE_LIGHT );
		}

	} else if( m_Target==TARGET_SNDPLANE ) {
		if( m_Action == ACTION_ADD ){
			m_CurrentBox = &m_CtlAddSndPlane;
			m_CtlAddSndPlane.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_ADD_SNDPLANE );
		} else if( m_Action == ACTION_SELECT ){
			m_CurrentBox = &m_CtlSelectSndPlane;
			m_CtlSelectSndPlane.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_SELECT_SNDPLANE );
		} else if( m_Action == ACTION_MOVE ){
			m_CurrentBox = &m_CtlMoveSndPlane;
			m_CtlMoveSndPlane.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_MOVE_SNDPLANE );
		} else if( m_Action == ACTION_ROTATE ){
			m_CurrentBox = &m_CtlRotateSndPlane;
			m_CtlRotateSndPlane.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_ROTATE_SNDPLANE );
		} else if( m_Action == ACTION_SCALE ){
			m_CurrentBox = &m_CtlScaleSndPlane;
			m_CtlScaleSndPlane.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_ROTATE_SNDPLANE );
		}
	} else if( m_Target==TARGET_SOUND ) {
		if( m_Action == ACTION_ADD ){
			m_CurrentBox = &m_CtlAddSound;
			m_CtlAddSound.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_ADD_SOUND );
		} else if( m_Action == ACTION_SELECT ){
			m_CurrentBox = &m_CtlSelectSound;
			m_CtlSelectSound.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_SELECT_SOUND );
		} /*else if( m_Action == ACTION_MOVE ){
			m_CurrentBox = &m_CtlMoveSound;
			m_CtlMoveSndPlane.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_MOVE_SOUND );
		}*/
	}

	if( m_CurrentBox == 0 ){
		m_CurrentBox = &m_CtlNothing;
		m_CtlNothing.Create( m_Instance, m_LeftSubPlace, IDD_LTOOL_NOTHING );
	}
}

//----------------------------------------------------

void XRayEditorUI::EnableSelectionRect( bool flag ){
	m_SelectionRect = flag;
	m_SelEnd.x = m_SelStart.x = 0;
	m_SelEnd.y = m_SelStart.y = 0;
}

void XRayEditorUI::UpdateSelectionRect( POINT *from, POINT *to ){
	_ASSERTE( from && to );
	m_SelStart = (*from);
	m_SelEnd = (*to);
}


//----------------------------------------------------

void XRayEditorUI::SetDlgItemFloat( HWND hw, int ctlid, float value, int prec ){
	char s[32];
	sprintf(s,"%.*f", prec, value );
	SetDlgItemText( hw, ctlid, s );
}

float XRayEditorUI::GetDlgItemFloat( HWND hw, int ctlid ){
	char s[32];
	GetDlgItemText( hw, ctlid, s, 32 );
	return (float)atof( s );
}

//----------------------------------------------------

void XRayEditorUI::PrintCamera(){
	NConsole.print( "UI: --------- Camera -----------" );
	NConsole.print( "UI: %4.4f %4.4f %4.4f %4.4f     ", m_Camera._11, m_Camera._12, m_Camera._13, m_Camera._14 );
	NConsole.print( "UI: %4.4f %4.4f %4.4f %4.4f     ", m_Camera._21, m_Camera._22, m_Camera._23, m_Camera._24 );
	NConsole.print( "UI: %4.4f %4.4f %4.4f %4.4f     ", m_Camera._31, m_Camera._32, m_Camera._33, m_Camera._34 );
	NConsole.print( "UI: %4.4f %4.4f %4.4f %4.4f     ", m_Camera._41, m_Camera._42, m_Camera._43, m_Camera._44 );
	NConsole.print( "UI: ----------------------------" );
}


//----------------------------------------------------


void XRayEditorUI::SetSelection( bool flag ){
	int clsid = ClassIDFromTargetID( m_Target );
	if( clsid >= 0 )
		Scene.SelectObjects(flag,clsid);
}

void XRayEditorUI::InvertSelection(){
	int clsid = ClassIDFromTargetID( m_Target );
	if( clsid >= 0 )
		Scene.InvertSelection( clsid );
}

int XRayEditorUI::ClassIDFromTargetID( int target ){
	switch( target ){
	case TARGET_LANDSCAPE: return OBJCLASS_LANDSCAPE;
	case TARGET_OBJECT: return OBJCLASS_SOBJECT2;
	case TARGET_SNDPLANE: return OBJCLASS_SNDPLANE;
	case TARGET_SOUND: return OBJCLASS_SOUND;
	case TARGET_LIGHT: return OBJCLASS_LIGHT;
	}
	return -1;
}

