//----------------------------------------------------
// file: TB_SelSndPlane.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "Scene.h"
#include "SndPlane.h"
#include "SceneClassList.h"
#include "SndPlaneEditor.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_SelSndPlane::TB_SelSndPlane()
	:XRayEditorUIToolBox()
{
}

TB_SelSndPlane::~TB_SelSndPlane(){
}

//----------------------------------------------------

void TB_SelSndPlane::Command( WPARAM wp, LPARAM lp ){

	switch( LOWORD( wp ) ){

	case IDC_SELECTALL:
		Scene.SelectObjects( true, OBJCLASS_SNDPLANE );
		break;
	case IDC_DESELECTALL:
		Scene.SelectObjects( false, OBJCLASS_SNDPLANE );
		break;
	case IDC_INVERTSELECTION:
		Scene.InvertSelection( OBJCLASS_SNDPLANE );
		break;
	case IDC_PROPERTIES:
		Scene.UndoPrepare();
		RunEditor();
		break;
	case IDC_REMOVE:
		Scene.UndoPrepare();
		Scene.RemoveSelection( OBJCLASS_SNDPLANE );
		break;
	}
}

void TB_SelSndPlane::Init( HWND hw ){
}

//----------------------------------------------------

void TB_SelSndPlane::RunEditor(){
	ObjectList objset;
	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_SNDPLANE && (*_F)->Selected() )
			objset.push_back( (*_F) );
	if( !objset.empty() )
		E_SndPlane.Create( UI.inst(), UI.wnd(), objset );
}

//----------------------------------------------------

