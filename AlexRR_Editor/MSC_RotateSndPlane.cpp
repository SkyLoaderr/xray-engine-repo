//----------------------------------------------------
// file: MSC_RotateSndPlane.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "UI_Main.h"
#include "NetDeviceLog.h"
#include "MSC_RotateSndPlane.h"
#include "SceneClassList.h"
#include "Scene.h"

//----------------------------------------------------

MouseCallback_RotateSndPlane::MouseCallback_RotateSndPlane()
	:MouseCallback( TARGET_SNDPLANE, ACTION_ROTATE )
{
	m_Local = false;
	m_RCenter.set(0,0,0);
	m_RVector.set(0,1,0);

	m_ASnap = true;
	m_ARemainder = 0.f;
}

MouseCallback_RotateSndPlane::~MouseCallback_RotateSndPlane(){
}

//----------------------------------------------------

bool MouseCallback_RotateSndPlane::Start(){
	if( Scene.SelectionCount( OBJCLASS_SNDPLANE, true ) == 0 )
		return false;
	Scene.UndoPrepare();
	m_ARemainder = 0.f;
	m_RCenter.set( UI.pivot() );
	return true;
}

bool MouseCallback_RotateSndPlane::End(){
	return true;
}

void MouseCallback_RotateSndPlane::Move(){
	
	float dy = - m_DeltaCpH.y * g_MouseRotateSpeed;
	float amount = dy * 0.1f;

	if( m_ASnap ){
		amount += m_ARemainder;
		m_ARemainder = amount;
		amount = snapto( amount, UI.anglesnap() );
		m_ARemainder -= amount;
	}

	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_SNDPLANE && (*_F)->Visible() )
			if( (*_F)->Selected() ){
				if( m_Local ){
					(*_F)->LocalRotate( m_RVector, amount );
				} else {
					(*_F)->Rotate( m_RCenter, m_RVector, amount );
				}
			}
}

//----------------------------------------------------
