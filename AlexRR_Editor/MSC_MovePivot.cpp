//----------------------------------------------------
// file: MSC_MovePivot.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_MovePivot.h"
#include "Scene.h"
#include "Landscape.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_MovePivot::MouseCallback_MovePivot()
	:MouseCallback( TARGET_PIVOT, 0 )
{
	m_XVector.set(1,0,0);
	m_YVector.set(0,1,0);
}

MouseCallback_MovePivot::~MouseCallback_MovePivot(){
}

bool MouseCallback_MovePivot::Compare( int _Target, int _Action ){
	return (_Target==TARGET_PIVOT);
}

//----------------------------------------------------

bool MouseCallback_MovePivot::Start(){

	Scene.UndoPrepare();

	if( m_Alternate ){
		m_XVector.set(0,0,0);
		m_YVector.set(0,1,0);
	}
	else{
		m_XVector.set( UI.camera().i );
		m_XVector.y = 0;
		m_YVector.set( UI.camera().k );
		m_YVector.y = 0;
		m_XVector.safe_normalize();
		m_YVector.safe_normalize();
	}

	return true;
}

bool MouseCallback_MovePivot::End(){
	UI.pivot().x = snapto( UI.pivot().x, UI.pivotsnap().x );
	UI.pivot().y = snapto( UI.pivot().y, UI.pivotsnap().y );
	UI.pivot().z = snapto( UI.pivot().z, UI.pivotsnap().z );
	return true;
}

void MouseCallback_MovePivot::Move(){

	float dx = m_DeltaCpH.x * g_MouseMoveSpeed;
	float dy = - m_DeltaCpH.y * g_MouseMoveSpeed;

	UI.pivot().translate( m_YVector, dy );
	UI.pivot().translate( m_XVector, dx );
}

//----------------------------------------------------
