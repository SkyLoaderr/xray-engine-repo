//----------------------------------------------------
// file: ObjectOptionPack.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "CustomControls.h"
#include "FileSystem.h"
#include "ObjectOptionPack.h"
#include "MenuTemplate.rh"
//----------------------------------------------------


bool ObjectOptionPack::ReadScript( XScrBlock *block ){

	_ASSERTE( block );

	XVarDef _Vars [] = {
		XVarDef( "staticlist",  CTE_INT, &m_StaticList  ),
		XVarDef( "collisionlist",  CTE_INT, &m_CollisionList  ),
		XVarDef( "dynamiclist",  CTE_INT, &m_DynamicList  ),
		XVarDef( "progressive",  CTE_INT, &m_Progressive  ),
		XVarDef( "shadow_rec",  CTE_INT, &m_RecShadows  ),
		XVarDef( "shadow_cast",  CTE_INT, &m_CastShadows  ),
		XVarDef( "ogf_specular",  CTE_INT, &m_OGF_specular  ),
		XVarDef( "ogf_light",  CTE_INT, &m_OGF_light  ),
		XVarDef( "ogf_alpha",  CTE_INT, &m_OGF_alpha  ),
		XVarDef( "ogf_fog",  CTE_INT, &m_OGF_fog  ),
		XVarDef( "ogf_ztest",  CTE_INT, &m_OGF_ztest  ),
		XVarDef( "ogf_zwrite",  CTE_INT, &m_OGF_zwrite  ),
		XVarDef( "vertexlighting",  CTE_INT, &m_VertexLighting  ),
		XVarDef( "lightmaps",  CTE_INT, &m_TextureLighting  ),
		XVarDef() };

	for(int i=0;
		i<block->getTokenCount();
		i=block->nextPth(i) )
			block->varSearch(i,_Vars);

	return true;
}



bool ObjectOptionPack::Load( FSChunkDef *chunk ){
	_ASSERTE( chunk );
	int version = FS.readword( chunk->filehandle );
	if( version == 3 ){
		m_StaticList = FS.readword( chunk->filehandle );
		m_CollisionList = FS.readword( chunk->filehandle );
		m_DynamicList = FS.readword( chunk->filehandle );
		m_Progressive = FS.readword( chunk->filehandle );
		m_RecShadows = FS.readword( chunk->filehandle );
		m_CastShadows = FS.readword( chunk->filehandle );
		m_OGF_specular = FS.readword( chunk->filehandle );
		m_OGF_light = FS.readword( chunk->filehandle );
		m_OGF_alpha = FS.readword( chunk->filehandle );
		m_OGF_fog = FS.readword( chunk->filehandle );
		m_OGF_ztest = FS.readword( chunk->filehandle );
		m_OGF_zwrite = FS.readword( chunk->filehandle );
		m_VertexLighting = FS.readword( chunk->filehandle );
		m_TextureLighting = FS.readword( chunk->filehandle );
	} else {
		Reset();
	}
	return true;
}



bool ObjectOptionPack::Save( int handle ){
	// version
	FS.writeword( handle, 3 );
	// data
	FS.writeword( handle, m_StaticList );
	FS.writeword( handle, m_CollisionList );
	FS.writeword( handle, m_DynamicList );
	FS.writeword( handle, m_Progressive );
	FS.writeword( handle, m_RecShadows );
	FS.writeword( handle, m_CastShadows );
	FS.writeword( handle, m_OGF_specular );
	FS.writeword( handle, m_OGF_light );
	FS.writeword( handle, m_OGF_alpha );
	FS.writeword( handle, m_OGF_fog );
	FS.writeword( handle, m_OGF_ztest );
	FS.writeword( handle, m_OGF_zwrite );
	FS.writeword( handle, m_VertexLighting );
	FS.writeword( handle, m_TextureLighting );
	return true;
}


//----------------------------------------------------

static HINSTANCE oop_Instance = 0;
static HWND oop_ParentWindow = 0;
static HANDLE oop_Thread = 0;
static DWORD oop_ThreadID = 0;
static ObjectOptionPack *oop_Pack = 0;

static BOOL CALLBACK OGFOptionPackDialogProc(
		HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	LPPSHNOTIFY psn;
	switch( msg ){
	case WM_INITDIALOG:
		SetCheck(hw,IDC_OGF_SPECULAR,oop_Pack->m_OGF_specular );
		SetCheck(hw,IDC_OGF_LIGHTING,oop_Pack->m_OGF_light );
		SetCheck(hw,IDC_OGF_ALPHA,oop_Pack->m_OGF_alpha );
		SetCheck(hw,IDC_OGF_FOG,oop_Pack->m_OGF_fog );
		SetCheck(hw,IDC_OGF_ZTEST,oop_Pack->m_OGF_ztest );
		SetCheck(hw,IDC_OGF_ZWRITE,oop_Pack->m_OGF_zwrite );
		SetCheck(hw,IDC_OGF_BUILD_VCOLORS,oop_Pack->m_VertexLighting );
		SetCheck(hw,IDC_OGF_BUILD_TCOLORS,oop_Pack->m_TextureLighting );
		break;
	case WM_NOTIFY:
		psn = (LPPSHNOTIFY)lp;
		if( psn->hdr.code == PSN_APPLY ){
			oop_Pack->m_OGF_specular = IsChecked(hw,IDC_OGF_SPECULAR);
			oop_Pack->m_OGF_light = IsChecked(hw,IDC_OGF_LIGHTING);
			oop_Pack->m_OGF_alpha = IsChecked(hw,IDC_OGF_ALPHA);
			oop_Pack->m_OGF_fog = IsChecked(hw,IDC_OGF_FOG);
			oop_Pack->m_OGF_ztest = IsChecked(hw,IDC_OGF_ZTEST);
			oop_Pack->m_OGF_zwrite = IsChecked(hw,IDC_OGF_ZWRITE);
			oop_Pack->m_VertexLighting = IsChecked(hw,IDC_OGF_BUILD_VCOLORS);
			oop_Pack->m_TextureLighting = IsChecked(hw,IDC_OGF_BUILD_TCOLORS);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static BOOL CALLBACK ListsOptionPackDialogProc(
		HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	LPPSHNOTIFY psn;
	switch( msg ){
	case WM_INITDIALOG:
		SetCheck(hw,IDC_COLL_LIST,oop_Pack->m_CollisionList );
		SetCheck(hw,IDC_RENDER_LIST,oop_Pack->m_StaticList );
		break;
	case WM_NOTIFY:
		psn = (LPPSHNOTIFY)lp;
		if( psn->hdr.code == PSN_APPLY ){
			oop_Pack->m_CollisionList = IsChecked(hw,IDC_COLL_LIST);
			oop_Pack->m_StaticList = IsChecked(hw,IDC_RENDER_LIST);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static BOOL CALLBACK MiscOptionPackDialogProc(
		HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	LPPSHNOTIFY psn;
	switch( msg ){
	case WM_INITDIALOG:
		SetCheck(hw,IDC_RCV_SHADOWS,oop_Pack->m_RecShadows );
		SetCheck(hw,IDC_CAST_SHADOWS,oop_Pack->m_CastShadows );
		SetCheck(hw,IDC_PROGRESSIVE,oop_Pack->m_Progressive );
		break;
	case WM_NOTIFY:
		psn = (LPPSHNOTIFY)lp;
		if( psn->hdr.code == PSN_APPLY ){
			oop_Pack->m_RecShadows = IsChecked(hw,IDC_RCV_SHADOWS);
			oop_Pack->m_CastShadows = IsChecked(hw,IDC_CAST_SHADOWS);
			oop_Pack->m_Progressive = IsChecked(hw,IDC_PROGRESSIVE);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static DWORD WINAPI OptionPackThread( LPVOID param){

	oop_Pack = (ObjectOptionPack*)param;

	PROPSHEETHEADER m_PSHeader;
	PROPSHEETPAGE m_PSPages[3];

	m_PSPages[0].dwSize = sizeof(PROPSHEETPAGE);
	m_PSPages[0].dwFlags = PSP_USETITLE;
	m_PSPages[0].hInstance = oop_Instance;
	m_PSPages[0].pszTemplate = MAKEINTRESOURCE(IDD_OPPACK_OGF);
	m_PSPages[0].lParam = (LPARAM)param;
	m_PSPages[0].pfnDlgProc = OGFOptionPackDialogProc;
	m_PSPages[0].pszTitle = "OGF Options";

	m_PSPages[1].dwSize = sizeof(PROPSHEETPAGE);
	m_PSPages[1].dwFlags = PSP_USETITLE;
	m_PSPages[1].hInstance = oop_Instance;
	m_PSPages[1].pszTemplate = MAKEINTRESOURCE(IDD_OPPACK_LISTS);
	m_PSPages[1].lParam = (LPARAM)param;
	m_PSPages[1].pfnDlgProc = ListsOptionPackDialogProc;
	m_PSPages[1].pszTitle = "Lists";

	m_PSPages[2].dwSize = sizeof(PROPSHEETPAGE);
	m_PSPages[2].dwFlags = PSP_USETITLE;
	m_PSPages[2].hInstance = oop_Instance;
	m_PSPages[2].pszTemplate = MAKEINTRESOURCE(IDD_OPPACK_MISC);
	m_PSPages[2].lParam = (LPARAM)param;
	m_PSPages[2].pfnDlgProc = MiscOptionPackDialogProc;
	m_PSPages[2].pszTitle = "Misc";

	memset( &m_PSHeader, 0, sizeof(m_PSHeader) );
	m_PSHeader.dwSize = sizeof(m_PSHeader);
	m_PSHeader.dwFlags = PSH_DEFAULT|PSH_PROPSHEETPAGE;
	m_PSHeader.hwndParent = oop_ParentWindow;
	m_PSHeader.hInstance = oop_Instance;
	m_PSHeader.nPages = 3;
	m_PSHeader.nStartPage = 0;
	m_PSHeader.pszCaption = "Sub-Object Properties";
	m_PSHeader.ppsp = m_PSPages;

	PropertySheet( &m_PSHeader );

	oop_Pack = 0;

	return 0;
}

bool ObjectOptionPack::RunEditor( HINSTANCE instance, HWND parent ){

	if( oop_Thread ){
		DWORD exit_code = STILL_ACTIVE;
		GetExitCodeThread(oop_Thread,&exit_code);
		if( exit_code == STILL_ACTIVE )
			TerminateThread(oop_Thread, 0 );
		CloseHandle( oop_Thread );
		oop_Thread = 0;
	}

	oop_Instance = instance;
	oop_ParentWindow = parent;

	oop_Thread = CreateThread( 0, 0, 
		OptionPackThread, this, 0, &oop_ThreadID );

	return true;
}

//----------------------------------------------------
