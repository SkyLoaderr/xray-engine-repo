//----------------------------------------------------
// file: UI_SceneOptions.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "CustomControls.h"
#include "UI_Main.h"
#include "Scene.h"
#include "UI_SceneOptions.h"
#include "MenuTemplate.rh"
//----------------------------------------------------

UI_SceneOptions OpDialog;

//----------------------------------------------------

bool BrowseFolder( HWND parent, char *path ){
	_ASSERTE( path );

	char displayname[MAX_PATH]="";

	BROWSEINFO info;
	memset(&info,0,sizeof(info));
	info.hwndOwner = parent;
	info.pszDisplayName = displayname;
	info.lpszTitle = "Select folder";
	info.ulFlags = 0; /*BIF_RETURNFSANCESTORS*/

	LPITEMIDLIST item = SHBrowseForFolder( &info );
	if( item ){
		if( SHGetPathFromIDList( item, path ) )
			return true;
	}

	return false;
}

//----------------------------------------------------

static BOOL CALLBACK MainSceneOptionsDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	LPPSHNOTIFY psn;
	switch( msg ){
	case WM_INITDIALOG:
		SetDlgItemText( hw, IDC_LEVEL_NAME, Scene.m_LevelName );
		break;
	case WM_NOTIFY:
		psn = (LPPSHNOTIFY)lp;
		if( psn->hdr.code == PSN_APPLY ){
			GetDlgItemText( hw, IDC_LEVEL_NAME,
				Scene.m_LevelName, sizeof(Scene.m_LevelName) );
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static BOOL CALLBACK BuildSceneOptionsDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	LPPSHNOTIFY psn;
	switch( msg ){
	case WM_INITDIALOG:
		UI.SetDlgItemFloat( hw, IDC_VIS_DX, Scene.m_BOPVisCellSize.x, 3 );
		UI.SetDlgItemFloat( hw, IDC_VIS_DZ, Scene.m_BOPVisCellSize.z, 3 );
		UI.SetDlgItemFloat( hw, IDC_IND_X_SUB, Scene.m_BOPIndentLow.x, 3 );
		UI.SetDlgItemFloat( hw, IDC_IND_Z_SUB, Scene.m_BOPIndentLow.z, 3 );
		UI.SetDlgItemFloat( hw, IDC_IND_X_ADD, Scene.m_BOPIndentHigh.x, 3 );
		UI.SetDlgItemFloat( hw, IDC_IND_Z_ADD, Scene.m_BOPIndentHigh.z, 3 );
		UI.SetDlgItemFloat( hw, IDC_COLL_CELL, Scene.m_BOPVisQuad, 3 );
		UI.SetDlgItemFloat( hw, IDC_OPT_MINDIST, Scene.m_BOPConnectDist, 3 );
		UI.SetDlgItemFloat( hw, IDC_OPT_MAXSIZE, Scene.m_BOPAvObjSize, 3 );
		SetCheck( hw, IDC_ENABLE_OPT, Scene.m_BOPOptimize );
		SetCheck( hw, IDC_ENABLE_LMAPS, Scene.m_BOPLightMaps );
		break;
	case WM_NOTIFY:
		psn = (LPPSHNOTIFY)lp;
		if( psn->hdr.code == PSN_APPLY ){
			Scene.m_BOPVisCellSize.x = UI.GetDlgItemFloat( hw, IDC_VIS_DX );
			Scene.m_BOPVisCellSize.z = UI.GetDlgItemFloat( hw, IDC_VIS_DZ );
			Scene.m_BOPIndentLow.x = UI.GetDlgItemFloat( hw, IDC_IND_X_SUB );
			Scene.m_BOPIndentLow.z = UI.GetDlgItemFloat( hw, IDC_IND_Z_SUB );
			Scene.m_BOPIndentHigh.x = UI.GetDlgItemFloat( hw, IDC_IND_X_ADD );
			Scene.m_BOPIndentHigh.z = UI.GetDlgItemFloat( hw, IDC_IND_Z_ADD );
			Scene.m_BOPVisQuad = UI.GetDlgItemFloat( hw, IDC_COLL_CELL );
			Scene.m_BOPConnectDist = UI.GetDlgItemFloat( hw, IDC_OPT_MINDIST );
			Scene.m_BOPAvObjSize = UI.GetDlgItemFloat( hw, IDC_OPT_MAXSIZE );
			Scene.m_BOPOptimize = IsChecked( hw, IDC_ENABLE_OPT );
			Scene.m_BOPLightMaps = IsChecked( hw, IDC_ENABLE_LMAPS );
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static BOOL CALLBACK PathsSceneOptionsDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	LPPSHNOTIFY psn;
	char pathbuffer[MAX_PATH];
	switch( msg ){
	case WM_INITDIALOG:
		SetDlgItemText( hw, IDC_LEVEL_PATH_T, Scene.m_FNLevelPath );
		SetDlgItemText( hw, IDC_TEX_PATH_T, Scene.m_FNTexPath );
		break;
	case WM_COMMAND:
		switch( LOWORD(wp)) {
		case IDC_LEVEL_PATH_B:
			GetDlgItemText( hw, IDC_LEVEL_PATH_T, pathbuffer, MAX_PATH );
			if( BrowseFolder( hw, pathbuffer ) )
				SetDlgItemText( hw, IDC_LEVEL_PATH_T, pathbuffer );
			break;
		case IDC_TEX_PATH_B:
			GetDlgItemText( hw, IDC_TEX_PATH_T, pathbuffer, MAX_PATH );
			if( BrowseFolder( hw, pathbuffer ) )
				SetDlgItemText( hw, IDC_TEX_PATH_T, pathbuffer );
			break;
		}
		break;
	case WM_NOTIFY:
		psn = (LPPSHNOTIFY)lp;
		if( psn->hdr.code == PSN_APPLY ){
			GetDlgItemText( hw, IDC_LEVEL_PATH_T,
				Scene.m_FNLevelPath, sizeof(Scene.m_FNLevelPath) );
			GetDlgItemText( hw, IDC_TEX_PATH_T,
				Scene.m_FNTexPath, sizeof(Scene.m_FNTexPath) );
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static BOOL CALLBACK FilesSceneOptionsDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	LPPSHNOTIFY psn;
	switch( msg ){
	case WM_INITDIALOG:
		SetDlgItemText( hw, IDC_OGFS, Scene.m_BOPAddFiles );
		SetDlgItemText( hw, IDC_FN_LTX, Scene.m_FNLtx );
		SetDlgItemText( hw, IDC_FN_FORMS, Scene.m_FNForms );
		SetDlgItemText( hw, IDC_FN_LIGHTS, Scene.m_FNLights );
		SetDlgItemText( hw, IDC_FN_MESHLIST, Scene.m_FNMeshList );
		SetDlgItemText( hw, IDC_FN_MAP, Scene.m_FNMap );
		SetDlgItemText( hw, IDC_FN_VIS, Scene.m_FNVisibility );
		break;
	case WM_NOTIFY:
		psn = (LPPSHNOTIFY)lp;
		if( psn->hdr.code == PSN_APPLY ){
			GetDlgItemText( hw, IDC_OGFS, Scene.m_BOPAddFiles, sizeof(Scene.m_BOPAddFiles) );
			GetDlgItemText( hw, IDC_FN_LTX, Scene.m_FNLtx, sizeof(Scene.m_FNLtx) );
			GetDlgItemText( hw, IDC_FN_FORMS, Scene.m_FNForms, sizeof(Scene.m_FNForms) );
			GetDlgItemText( hw, IDC_FN_LIGHTS, Scene.m_FNLights, sizeof(Scene.m_FNLights) );
			GetDlgItemText( hw, IDC_FN_MESHLIST, Scene.m_FNMeshList, sizeof(Scene.m_FNMeshList) );
			GetDlgItemText( hw, IDC_FN_MAP, Scene.m_FNMap, sizeof(Scene.m_FNMap) );
			GetDlgItemText( hw, IDC_FN_VIS, Scene.m_FNVisibility, sizeof(Scene.m_FNVisibility) );
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static BOOL CALLBACK TextSceneOptionsDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	LPPSHNOTIFY psn;
	switch( msg ){
	case WM_INITDIALOG:
		SetDlgItemText( hw, IDC_LEVEL_TEXT, Scene.m_BOPText );
		break;
	case WM_NOTIFY:
		psn = (LPPSHNOTIFY)lp;
		if( psn->hdr.code == PSN_APPLY ){
			GetDlgItemText( hw, IDC_LEVEL_TEXT,
				Scene.m_BOPText, sizeof(Scene.m_BOPText) );
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static bool SelColor( HWND hw, IFColor& color ){

	static COLORREF custcolors[16];
	static bool custcolorsinitialized = false;
	
	if( !custcolorsinitialized ){
		for( int i=0; i<16; i++)
			custcolors[i] = RGB( i*16, i*16, i*16 );
		custcolorsinitialized = true;
	}

	CHOOSECOLOR ccr;
	memset( &ccr, 0, sizeof(ccr) );

	ccr.lStructSize = sizeof(ccr);
	ccr.hwndOwner = hw;
	ccr.rgbResult = color.colorref();
	ccr.lpCustColors = custcolors;
	ccr.Flags = CC_ANYCOLOR|CC_FULLOPEN|CC_RGBINIT;

	if( ChooseColor( &ccr ) ){
		color.set(
			GetRValue(ccr.rgbResult) / 255.f,
			GetGValue(ccr.rgbResult) / 255.f,
			GetBValue(ccr.rgbResult) / 255.f );
		return true;
	}

	return false;
}

static BOOL CALLBACK LightSceneOptionsDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	IFColor color;
	LPPSHNOTIFY psn;
	HBRUSH hbr;
	LPDRAWITEMSTRUCT ds;
	switch( msg ){
	case WM_INITDIALOG:
		UI.SetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_R, Scene.m_GlobalAmbient.r, 3 );
		UI.SetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_G, Scene.m_GlobalAmbient.g, 3 );
		UI.SetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_B, Scene.m_GlobalAmbient.b, 3 );
		UI.SetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_R, Scene.m_GlobalDiffuse.r, 3 );
		UI.SetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_G, Scene.m_GlobalDiffuse.g, 3 );
		UI.SetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_B, Scene.m_GlobalDiffuse.b, 3 );
		UI.SetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_R, Scene.m_GlobalSpecular.r, 3 );
		UI.SetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_G, Scene.m_GlobalSpecular.g, 3 );
		UI.SetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_B, Scene.m_GlobalSpecular.b, 3 );
		InvalidateRect( GetDlgItem(hw,IDC_LIGHT_AMBIENT),0,0);
		InvalidateRect( GetDlgItem(hw,IDC_LIGHT_DIFFUSE),0,0);
		InvalidateRect( GetDlgItem(hw,IDC_LIGHT_SPECULAR),0,0);
		break;
	case WM_NOTIFY:
		psn = (LPPSHNOTIFY)lp;
		if( psn->hdr.code == PSN_APPLY ){
			Scene.m_GlobalAmbient.r = UI.GetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_R );
			Scene.m_GlobalAmbient.g = UI.GetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_G );
			Scene.m_GlobalAmbient.b = UI.GetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_B );
			Scene.m_GlobalDiffuse.r = UI.GetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_R );
			Scene.m_GlobalDiffuse.g = UI.GetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_G );
			Scene.m_GlobalDiffuse.b = UI.GetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_B );
			Scene.m_GlobalSpecular.r = UI.GetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_R );
			Scene.m_GlobalSpecular.g = UI.GetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_G );
			Scene.m_GlobalSpecular.b = UI.GetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_B );
		}
		break;
	case WM_COMMAND:
		switch( LOWORD(wp) ){
		case IDC_LIGHT_AMBIENT:
			color.r = UI.GetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_R );
			color.g = UI.GetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_G );
			color.b = UI.GetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_B );
			if( SelColor( hw, color ) ){
				UI.SetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_R, color.r, 4 );
				UI.SetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_G, color.g, 4 );
				UI.SetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_B, color.b, 4 );
				InvalidateRect( GetDlgItem(hw,LOWORD(wp)),0,0);
			}
			break;
		case IDC_LIGHT_DIFFUSE:
			color.r = UI.GetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_R );
			color.g = UI.GetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_G );
			color.b = UI.GetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_B );
			if( SelColor( hw, color ) ){
				UI.SetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_R, color.r, 4 );
				UI.SetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_G, color.g, 4 );
				UI.SetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_B, color.b, 4 );
				InvalidateRect( GetDlgItem(hw,LOWORD(wp)),0,0);
			}
			break;
		case IDC_LIGHT_SPECULAR:
			color.r = UI.GetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_R );
			color.g = UI.GetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_G );
			color.b = UI.GetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_B );
			if( SelColor( hw, color ) ){
				UI.SetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_R, color.r, 4 );
				UI.SetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_G, color.g, 4 );
				UI.SetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_B, color.b, 4 );
				InvalidateRect( GetDlgItem(hw,LOWORD(wp)),0,0);
			}
			break;
		}
		break;
	case WM_DRAWITEM:
		ds = (LPDRAWITEMSTRUCT)lp;
		switch( (UINT)wp ){
		case IDC_LIGHT_AMBIENT:
			color.r = UI.GetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_R );
			color.g = UI.GetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_G );
			color.b = UI.GetDlgItemFloat( hw, IDC_LIGHT_AMBIENT_B );
			hbr = CreateSolidBrush( color.colorref() );
			FillRect( ds->hDC, &ds->rcItem, hbr );
			DeleteObject( hbr );
			break;
		case IDC_LIGHT_DIFFUSE:
			color.r = UI.GetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_R );
			color.g = UI.GetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_G );
			color.b = UI.GetDlgItemFloat( hw, IDC_LIGHT_DIFFUSE_B );
			hbr = CreateSolidBrush( color.colorref() );
			FillRect( ds->hDC, &ds->rcItem, hbr );
			DeleteObject( hbr );
			break;
		case IDC_LIGHT_SPECULAR:
			color.r = UI.GetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_R );
			color.g = UI.GetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_G );
			color.b = UI.GetDlgItemFloat( hw, IDC_LIGHT_SPECULAR_B );
			hbr = CreateSolidBrush( color.colorref() );
			FillRect( ds->hDC, &ds->rcItem, hbr );
			DeleteObject( hbr );
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static DWORD WINAPI SceneOptionsThread( LPVOID ){

	PROPSHEETHEADER m_PSHeader;
	PROPSHEETPAGE m_PSPages[6];

	m_PSPages[0].dwSize = sizeof(PROPSHEETPAGE);
	m_PSPages[0].dwFlags = PSP_USETITLE;
	m_PSPages[0].hInstance = OpDialog.m_Instance;
	m_PSPages[0].pszTemplate = MAKEINTRESOURCE(IDD_SCENEPROP_MAIN);
	m_PSPages[0].pfnDlgProc = MainSceneOptionsDialogProc;
	m_PSPages[0].pszTitle = "Main Options";

	m_PSPages[1].dwSize = sizeof(PROPSHEETPAGE);
	m_PSPages[1].dwFlags = PSP_USETITLE;
	m_PSPages[1].hInstance = OpDialog.m_Instance;
	m_PSPages[1].pszTemplate = MAKEINTRESOURCE(IDD_SCENEPROP_BUILD);
	m_PSPages[1].pfnDlgProc = BuildSceneOptionsDialogProc;
	m_PSPages[1].pszTitle = "Build Options";

	m_PSPages[2].dwSize = sizeof(PROPSHEETPAGE);
	m_PSPages[2].dwFlags = PSP_USETITLE;
	m_PSPages[2].hInstance = OpDialog.m_Instance;
	m_PSPages[2].pszTemplate = MAKEINTRESOURCE(IDD_SCENEPROP_PATHS);
	m_PSPages[2].pfnDlgProc = PathsSceneOptionsDialogProc;
	m_PSPages[2].pszTitle = "Directories";

	m_PSPages[3].dwSize = sizeof(PROPSHEETPAGE);
	m_PSPages[3].dwFlags = PSP_USETITLE;
	m_PSPages[3].hInstance = OpDialog.m_Instance;
	m_PSPages[3].pszTemplate = MAKEINTRESOURCE(IDD_SCENEPROP_FILES);
	m_PSPages[3].pfnDlgProc = FilesSceneOptionsDialogProc;
	m_PSPages[3].pszTitle = "Files";

	m_PSPages[4].dwSize = sizeof(PROPSHEETPAGE);
	m_PSPages[4].dwFlags = PSP_USETITLE;
	m_PSPages[4].hInstance = OpDialog.m_Instance;
	m_PSPages[4].pszTemplate = MAKEINTRESOURCE(IDD_SCENEPROP_TEXT);
	m_PSPages[4].pfnDlgProc = TextSceneOptionsDialogProc;
	m_PSPages[4].pszTitle = "Text";

	m_PSPages[5].dwSize = sizeof(PROPSHEETPAGE);
	m_PSPages[5].dwFlags = PSP_USETITLE;
	m_PSPages[5].hInstance = OpDialog.m_Instance;
	m_PSPages[5].pszTemplate = MAKEINTRESOURCE(IDD_SCENEPROP_LIGHT);
	m_PSPages[5].pfnDlgProc = LightSceneOptionsDialogProc;
	m_PSPages[5].pszTitle = "Global lighting";

	memset( &m_PSHeader, 0, sizeof(m_PSHeader) );
	m_PSHeader.dwSize = sizeof(m_PSHeader);
	m_PSHeader.dwFlags = PSH_DEFAULT|PSH_PROPSHEETPAGE;
	m_PSHeader.hwndParent = OpDialog.m_ParentWindow;
	m_PSHeader.hInstance = OpDialog.m_Instance;
	m_PSHeader.nPages = 6;
	m_PSHeader.nStartPage = 0;
	m_PSHeader.pszCaption = "Scene Properties";
	m_PSHeader.ppsp = m_PSPages;

	PropertySheet( &m_PSHeader );
	return 0;
}

//----------------------------------------------------

UI_SceneOptions::UI_SceneOptions(){
	_ASSERTE( &OpDialog == this );
	m_Instance = 0;
	m_ParentWindow = 0;
	m_ThreadID = 0;
	m_Thread = 0;
}

UI_SceneOptions::~UI_SceneOptions(){
}



bool UI_SceneOptions::Open( HINSTANCE instance, HWND parent ){

	if( m_Thread ){
		DWORD exit_code = STILL_ACTIVE;
		GetExitCodeThread(m_Thread,&exit_code);
		if( exit_code == STILL_ACTIVE )
			TerminateThread(m_Thread, 0 );
		CloseHandle( m_Thread );
		m_Thread = 0;
	}

	m_Instance = instance;
	m_ParentWindow = parent;

	m_Thread = CreateThread( 0, 0, 
		SceneOptionsThread, 0, 0, &m_ThreadID );

	return true;
}

//----------------------------------------------------
