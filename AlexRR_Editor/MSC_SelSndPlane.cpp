//----------------------------------------------------
// file: MSC_SelSndPlane.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_SelSndPlane.h"
#include "Scene.h"
#include "SndPlane.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_SelSndPlane::MouseCallback_SelSndPlane()
	:MouseCallback( TARGET_SNDPLANE, ACTION_SELECT )
{
	m_Box = false;
	m_Select = true;
}

MouseCallback_SelSndPlane::~MouseCallback_SelSndPlane(){
}

//----------------------------------------------------

bool MouseCallback_SelSndPlane::Start(){

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

bool MouseCallback_SelSndPlane::End(){
	_ASSERTE( m_Box );
	UI.EnableSelectionRect( false );
	ICullPlane planes[4];
	if( UI.MouseBox( planes,&m_StartCp,&m_CurrentCp ) )
		Scene.BoxPickSelect( planes,
			OBJCLASS_SNDPLANE,
			m_Select );
	return true;
}

void MouseCallback_SelSndPlane::Move(){
	_ASSERTE( m_Box );
	UI.UpdateSelectionRect(&m_StartCp,&m_CurrentCp);
}

//----------------------------------------------------

bool MouseCallback_SelSndPlane::SelectHereRTL(){
	
	SceneObject *obj = Scene.RTL_Pick(
		m_CurrentRStart,m_CurrentRNorm,
		OBJCLASS_SNDPLANE);

	if( obj ){
		obj->Select( m_Select );
		return true;
	}

	return false;
}

//----------------------------------------------------

