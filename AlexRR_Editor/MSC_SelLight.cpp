//----------------------------------------------------
// file: MSC_SelLight.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_SelLight.h"
#include "Scene.h"
#include "Light.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_SelLight::MouseCallback_SelLight()
	:MouseCallback( TARGET_LIGHT, ACTION_SELECT )
{
	m_Box = false;
	m_Select = true;
}

MouseCallback_SelLight::~MouseCallback_SelLight(){
}

//----------------------------------------------------

bool MouseCallback_SelLight::Start(){

	m_Select = !(GetKeyState(VK_CONTROL)&0x80);
	m_Box = !!(GetKeyState(VK_SHIFT)&0x80);

	if( m_Box ){
		UI.EnableSelectionRect( true );
		UI.UpdateSelectionRect(&m_StartCp,&m_CurrentCp);

	} else {
		SelectHereRTL();
		return true;
	}

	return true;
}

bool MouseCallback_SelLight::End(){
	if( m_Box ){
		UI.EnableSelectionRect( false );
		ICullPlane planes[4];
		if( UI.MouseBox( planes,&m_StartCp,&m_CurrentCp ) )
			Scene.BoxPickSelect( planes,
				OBJCLASS_LIGHT,
				m_Select );
	}
	return true;
}

void MouseCallback_SelLight::Move(){
	if( m_Box ){
		UI.UpdateSelectionRect(&m_StartCp,&m_CurrentCp);
	} else {
		SelectHereRTL();
	}
}

//----------------------------------------------------

bool MouseCallback_SelLight::SelectHereRTL(){
	
	SceneObject *obj = Scene.RTL_Pick(
		m_CurrentRStart,m_CurrentRNorm,
		OBJCLASS_LIGHT);

	if( obj ){
		obj->Select( m_Select );
		return true;
	}
	return false;
}

//----------------------------------------------------

