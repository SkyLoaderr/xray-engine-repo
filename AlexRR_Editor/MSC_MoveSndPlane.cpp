//----------------------------------------------------
// file: MSC_MoveSndPlane.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_MoveSndPlane.h"
#include "Scene.h"
#include "SndPlane.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_MoveSndPlane::MouseCallback_MoveSndPlane()
	:MouseCallback( TARGET_SNDPLANE, ACTION_MOVE )
{
	m_XVector.set(1,0,0);
	m_YVector.set(0,1,0);

	m_AxisEnable[0] = true;
	m_AxisEnable[1] = true;
	m_AxisEnable[2] = true;

	m_Snap = true;
	m_MoveRemainder.set(0,0,0);
}

MouseCallback_MoveSndPlane::~MouseCallback_MoveSndPlane(){
}

//----------------------------------------------------

bool MouseCallback_MoveSndPlane::Start(){
	if( Scene.SelectionCount( OBJCLASS_SNDPLANE, true ) == 0 )
		return false;

	Scene.UndoPrepare();

	if( m_Snap )
		m_MoveRemainder.set(0,0,0);

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

bool MouseCallback_MoveSndPlane::End(){
	return true;
}

void MouseCallback_MoveSndPlane::Move(){

	float dx = m_DeltaCpH.x * g_MouseMoveSpeed;
	float dy = - m_DeltaCpH.y * g_MouseMoveSpeed;

	IVector amount;
	amount.mul( m_XVector, dx );
	amount.translate( m_YVector, dy );

	if( m_Snap ){
		
		amount.add( m_MoveRemainder );
		m_MoveRemainder.set( amount );

		amount.x = snapto( amount.x, UI.pivotsnap().x );
		amount.y = snapto( amount.y, UI.pivotsnap().y );
		amount.z = snapto( amount.z, UI.pivotsnap().z );

		m_MoveRemainder.sub( amount );
	}

	for(int i=0;i<3;i++)
		if(!m_AxisEnable[i])
			amount.a[i] = 0.f;

	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_SNDPLANE && (*_F)->Visible() )
			if( (*_F)->Selected() )
				(*_F)->Move( amount );
}

//----------------------------------------------------
