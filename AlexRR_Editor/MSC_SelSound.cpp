//----------------------------------------------------
// file: MSC_SelLight.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_SelSound.h"
#include "Scene.h"
#include "Sound.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_SelSound::MouseCallback_SelSound()
	:MouseCallback( TARGET_SOUND, ACTION_SELECT )
{
	m_Box = false;
	m_Select = true;
}

MouseCallback_SelSound::~MouseCallback_SelSound(){
}

//----------------------------------------------------

bool MouseCallback_SelSound::Start(){

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

bool MouseCallback_SelSound::End(){
	if( m_Box ){
		UI.EnableSelectionRect( false );
		ICullPlane planes[4];
		if( UI.MouseBox( planes,&m_StartCp,&m_CurrentCp ) )
			Scene.BoxPickSelect( planes,
				OBJCLASS_SOUND,
				m_Select );
	}
	return true;
}

void MouseCallback_SelSound::Move(){
	if( m_Box ){
		UI.UpdateSelectionRect(&m_StartCp,&m_CurrentCp);
	} else {
		SelectHereRTL();
	}
}

//----------------------------------------------------

bool MouseCallback_SelSound::SelectHereRTL(){
	
	SceneObject *obj = Scene.RTL_Pick(
		m_CurrentRStart,m_CurrentRNorm,
		OBJCLASS_SOUND);

	if( obj ){
		obj->Select( m_Select );
		return true;
	}
	return false;
}

//----------------------------------------------------

