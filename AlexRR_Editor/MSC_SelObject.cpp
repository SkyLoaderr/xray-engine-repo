//----------------------------------------------------
// file: MSC_SelObject.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_SelObject.h"
#include "Scene.h"
#include "SObject2.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_SelObject::MouseCallback_SelObject()
	:MouseCallback( TARGET_OBJECT, ACTION_SELECT )
{
	m_Box = false;
	m_Select = true;
}

MouseCallback_SelObject::~MouseCallback_SelObject(){
}

//----------------------------------------------------

bool MouseCallback_SelObject::Start(){

	m_Select = !(GetKeyState(VK_CONTROL)&0x80);
	m_Box = !!(GetKeyState(VK_SHIFT)&0x80);

	if( m_Box ){
		UI.EnableSelectionRect( true );
		UI.UpdateSelectionRect(&m_StartCp,&m_CurrentCp);
		return true;

	} else {
		SelectHereRTL();
	}

	return false;
}

bool MouseCallback_SelObject::End(){
	_ASSERTE( m_Box );
	UI.EnableSelectionRect( false );
	ICullPlane planes[4];
	if( UI.MouseBox( planes,&m_StartCp,&m_CurrentCp ) )
		Scene.BoxPickSelect( planes,
			OBJCLASS_SOBJECT2,
			m_Select );
	return true;
}

void MouseCallback_SelObject::Move(){
	_ASSERTE( m_Box );
	UI.UpdateSelectionRect(&m_StartCp,&m_CurrentCp);
}

//----------------------------------------------------

bool MouseCallback_SelObject::SelectHereRTL(){
	
	SceneObject *obj = Scene.RTL_Pick(
		m_CurrentRStart,m_CurrentRNorm,
		OBJCLASS_SOBJECT2);

	if( obj ){
		obj->Select( m_Select );
		return true;
	}
	return false;
}

//----------------------------------------------------

