//----------------------------------------------------
// file: TB_SelLight.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Light.h"
#include "SceneClassList.h"
#include "MSC_Main.h"
#include "MSC_SelLight.h"
#include "LightEditor.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_SelLight::TB_SelLight()
	:XRayEditorUIToolBox()
{
}

TB_SelLight::~TB_SelLight(){
}

//----------------------------------------------------

void TB_SelLight::Command( WPARAM wp, LPARAM lp ){

	switch( LOWORD( wp ) ){

	case IDC_SELECTALL:
		Scene.SelectObjects( true, OBJCLASS_LIGHT );
		break;
	case IDC_DESELECTALL:
		Scene.SelectObjects( false, OBJCLASS_LIGHT );
		break;
	case IDC_INVERTSELECTION:
		Scene.InvertSelection( OBJCLASS_LIGHT );
		break;
	case IDC_LIGHT_PROP:
		Scene.UndoPrepare();
		RunEditor();
		break;
	case IDC_REMOVE:
		Scene.UndoPrepare();
		Scene.RemoveSelection( OBJCLASS_LIGHT );
		break;
	}
}

void TB_SelLight::Init( HWND hw ){
}

//----------------------------------------------------

void TB_SelLight::RunEditor(){
	ObjectList objset;
	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_LIGHT && (*_F)->Selected() )
			objset.push_back( (*_F) );
	if( !objset.empty() )
		E_Light.Create( UI.inst(), UI.wnd(), objset );
}

//----------------------------------------------------

