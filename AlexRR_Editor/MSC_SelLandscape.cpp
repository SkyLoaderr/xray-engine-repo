//----------------------------------------------------
// file: MSC_SelLandscape.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_SelLandscape.h"
#include "Scene.h"
#include "Landscape.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_SelLandscape::MouseCallback_SelLandscape()
	:MouseCallback( TARGET_LANDSCAPE, ACTION_SELECT )
{
	m_Box = false;
	m_ByGround = false;
	m_Select = true;
}

MouseCallback_SelLandscape::~MouseCallback_SelLandscape(){
}

//----------------------------------------------------

bool MouseCallback_SelLandscape::Start(){

	m_Select = !(GetKeyState(VK_CONTROL)&0x80);
	m_Box = !!(GetKeyState(VK_SHIFT)&0x80);

	if( m_Box ){
		UI.EnableSelectionRect( true );
		UI.UpdateSelectionRect(&m_StartCp,&m_CurrentCp);

	} else {
		if( m_ByGround ){
			return SelectHere();
		} else{
			SelectHereRTL();
			return true;
		}
	}

	return true;
}

bool MouseCallback_SelLandscape::End(){
	if( m_Box ){
		UI.EnableSelectionRect( false );
		ICullPlane planes[4];
		if( UI.MouseBox( planes,&m_StartCp,&m_CurrentCp ) )
			Scene.BoxPickSelect( planes,
				OBJCLASS_LANDSCAPE,
				m_Select );
	}
	return true;
}

void MouseCallback_SelLandscape::Move(){
	if( m_Box ){
		UI.UpdateSelectionRect(&m_StartCp,&m_CurrentCp);
	} else {
		if( m_ByGround ){
			SelectHere();
		} else{
			SelectHereRTL();
		}
	}
}

//----------------------------------------------------

bool MouseCallback_SelLandscape::SelectHere(){
	IVector p;
	if( UI.PickGround(&p,&m_CurrentRStart,&m_CurrentRNorm) ){
		
		int x = floorf( p.x / Scene.lx() );
		int z = floorf( p.z / Scene.lz() );
		
		SceneObject *obj = Scene.QuadPick(x,z,OBJCLASS_LANDSCAPE);
		
		if( obj )
			obj->Select( m_Select );
		
		return true;
	}

	return false;
}

bool MouseCallback_SelLandscape::SelectHereRTL(){
	
	SceneObject *obj = Scene.RTL_Pick(
		m_CurrentRStart,m_CurrentRNorm,
		OBJCLASS_LANDSCAPE);

	if( obj ){
		obj->Select( m_Select );
		return true;
	}
	return false;
}

//----------------------------------------------------

