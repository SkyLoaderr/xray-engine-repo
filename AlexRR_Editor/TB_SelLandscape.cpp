//----------------------------------------------------
// file: TB_SelLandscape.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Landscape.h"
#include "SceneClassList.h"
#include "Texture.h"
#include "MSC_Main.h"
#include "MSC_SelLandscape.h"
#include "LandscapeEditor.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_SelLandscape::TB_SelLandscape()
	:XRayEditorUIToolBox()
{
}

TB_SelLandscape::~TB_SelLandscape(){
}

//----------------------------------------------------

void TB_SelLandscape::Command( WPARAM wp, LPARAM lp ){

	switch( LOWORD( wp ) ){

	case IDC_GROUND_PICK:
		MSC_SelLandscape.m_ByGround = !!IsChecked( m_ChildDialog, IDC_GROUND_PICK);
		break;

	case IDC_SELECTALL:
		Scene.SelectObjects( true, OBJCLASS_LANDSCAPE );
		break;
	case IDC_DESELECTALL:
		Scene.SelectObjects( false, OBJCLASS_LANDSCAPE );
		break;
	case IDC_INVERTSELECTION:
		Scene.InvertSelection( OBJCLASS_LANDSCAPE );
		break;
	case IDC_SELECTBYTYPE:
		TSelLandscape( true );
		break;
	case IDC_DESELECTBYTYPE:
		TSelLandscape( false );
		break;
	case IDC_LANDSCAPE_CONNECT:
		Scene.UndoPrepare();
		ConnectSelection();
		break;
	case IDC_LANDSCAPE_PROP:
		Scene.UndoPrepare();
		RunEditor();
		break;
	case IDC_REMOVE:
		Scene.UndoPrepare();
		Scene.RemoveSelection( OBJCLASS_LANDSCAPE );
		Scene.SmoothLandscape();
		break;
	}
}

void TB_SelLandscape::Init( HWND hw ){
	SetCheck( hw, IDC_GROUND_PICK, MSC_SelLandscape.m_ByGround );
	EnableWindow( GetDlgItem(hw,IDC_GROUND_PICK), !MSC_SelLandscape.m_Box );
}

//----------------------------------------------------

void TB_SelLandscape::TSelLandscape( bool flag ){
	ETexture *t = 0;
	if( t = UI.SelectLandscape() ){
		ObjectIt _F = Scene.FirstObj();
		for(;_F!=Scene.LastObj();_F++)
			if( (*_F)->ClassID() == OBJCLASS_LANDSCAPE ){
				SLandscape *_L = (SLandscape *)(*_F);
				if( _L->TexCompare( t->name() ) )
					_L->Select( flag );
			}
	}
}

//----------------------------------------------------


void TB_SelLandscape::RunEditor(){
	ObjectList objset;
	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_LANDSCAPE && (*_F)->Selected() )
			objset.push_back( (*_F) );
	if( !objset.empty() )
		E_Landscape.Create( UI.inst(), UI.wnd(), objset );
}

//----------------------------------------------------

void TB_SelLandscape::ConnectSelection(){

	ObjectIt f = Scene.FirstObj();
	for(;f!=Scene.LastObj();f++)
		if( (*f)->ClassID() == OBJCLASS_LANDSCAPE && (*f)->Selected() ){
			ObjectIt to = Scene.FirstObj();
			for(;to!=Scene.LastObj();to++)
				if( (*to)->ClassID() == OBJCLASS_LANDSCAPE && (!(*to)->Selected()) ){
					((SLandscape *)(*to))->TryConnectTo( (SLandscape *)(*f) );
				}
		}

	Scene.SmoothLandscape();
}

//----------------------------------------------------

