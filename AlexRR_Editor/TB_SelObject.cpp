//----------------------------------------------------
// file: TB_SelObject.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "Scene.h"
#include "SObject2.h"
#include "SceneClassList.h"
#include "Texture.h"
#include "SObject2Editor.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_SelObject::TB_SelObject()
	:XRayEditorUIToolBox()
{
}

TB_SelObject::~TB_SelObject(){
}

//----------------------------------------------------

void TB_SelObject::Command( WPARAM wp, LPARAM lp ){

	switch( LOWORD( wp ) ){

	case IDC_SELECTALL:
		Scene.SelectObjects( true, OBJCLASS_SOBJECT2 );
		break;
	case IDC_DESELECTALL:
		Scene.SelectObjects( false, OBJCLASS_SOBJECT2 );
		break;
	case IDC_INVERTSELECTION:
		Scene.InvertSelection( OBJCLASS_SOBJECT2 );
		break;

	case IDC_SELECTBYREF:
		TSelRefObject( true );
		break;
	case IDC_SELECTBYCLASS:
		TSelClassObject( true );
		break;

	case IDC_DESELECTBYREF:
		TSelRefObject( false );
		break;
	case IDC_DESELECTBYCLASS:
		TSelClassObject( false );
		break;

	case IDC_OBJECT_PROP:
		Scene.UndoPrepare();
		RunEditor();
		break;
	case IDC_OBJECT_RESOLVE:
		Scene.UndoPrepare();
		ResolveObjects();
		break;
	case IDC_REMOVE:
		Scene.UndoPrepare();
		Scene.RemoveSelection( OBJCLASS_SOBJECT2 );
		break;
	}
}

void TB_SelObject::Init( HWND hw ){
}

//----------------------------------------------------


void TB_SelObject::TSelRefObject( bool flag ){
	SceneObject *o = 0;
	if( o = UI.SelectSObject() ){
		_ASSERTE( o->ClassID() == OBJCLASS_SOBJECT2 );
		ObjectIt _F = Scene.FirstObj();
		for(;_F!=Scene.LastObj();_F++){
			if( (*_F)->ClassID() == OBJCLASS_SOBJECT2 ){
				SObject2 *_O = (SObject2 *)(*_F);
				if( _O->RefCompare( (SObject2*)o ) ){
					_O->Select( flag );
				}
			}
		}
	}
}

void TB_SelObject::TSelClassObject( bool flag ){
	SceneObject *o = 0;
	if( o = UI.SelectSObject() ){
		_ASSERTE( o->ClassID() == OBJCLASS_SOBJECT2 );
		ObjectIt _F = Scene.FirstObj();
		for(;_F!=Scene.LastObj();_F++){
			if( (*_F)->ClassID() == OBJCLASS_SOBJECT2 ){
				SObject2 *_O = (SObject2 *)(*_F);
				if( _O->ClassCompare( (SObject2*)o ) ){
					_O->Select( flag );
				}
			}
		}
	}
}

//----------------------------------------------------

void TB_SelObject::RunEditor(){
	ObjectList objset;
	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_SOBJECT2 && (*_F)->Selected() )
			objset.push_back( (*_F) );
	if( !objset.empty() )
		E_Object2.Create( UI.inst(), UI.wnd(), objset );
}

void TB_SelObject::ResolveObjects(){
	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_SOBJECT2 && (*_F)->Selected() ){
			SObject2 *sobj = (SObject2 *)(*_F);
			sobj->ResolveReference(); }
}

//----------------------------------------------------
