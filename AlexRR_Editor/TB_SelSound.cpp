//----------------------------------------------------
// file: TB_SelSound.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Sound.h"
#include "SceneClassList.h"
#include "MSC_Main.h"
#include "MSC_SelSound.h"
#include "SoundEditor.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_SelSound::TB_SelSound()
	:XRayEditorUIToolBox()
{
}

TB_SelSound::~TB_SelSound(){
}

//----------------------------------------------------

void TB_SelSound::Command( WPARAM wp, LPARAM lp ){

	switch( LOWORD( wp ) ){

	case IDC_SELECTALL:
		Scene.SelectObjects( true, OBJCLASS_SOUND );
		break;
	case IDC_DESELECTALL:
		Scene.SelectObjects( false, OBJCLASS_SOUND );
		break;
	case IDC_INVERTSELECTION:
		Scene.InvertSelection( OBJCLASS_SOUND );
		break;
	case IDC_SOUND_PROP:
		Scene.UndoPrepare();
		RunEditor();
		break;
	case IDC_REMOVE:
		Scene.UndoPrepare();
		Scene.RemoveSelection( OBJCLASS_SOUND );
		break;
	}
}

void TB_SelSound::Init( HWND hw ){
}

//----------------------------------------------------

void TB_SelSound::RunEditor(){
	ObjectList objset;
	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_SOUND && (*_F)->Selected() )
			objset.push_back( (*_F) );
	if( !objset.empty() )
		E_Sound.Create( UI.inst(), UI.wnd(), objset );
}

//----------------------------------------------------

