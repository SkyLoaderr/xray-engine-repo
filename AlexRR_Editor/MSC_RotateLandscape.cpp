//----------------------------------------------------
// file: MSC_RotateLandscape.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_RotateLandscape.h"
#include "Scene.h"
#include "Landscape.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_RotateLandscape::MouseCallback_RotateLandscape()
	:MouseCallback( TARGET_LANDSCAPE, ACTION_ROTATE )
{
	m_RCenter.set(0,0,0);
	m_RVector.set(0,1,0);
	
	m_ASnap = true;
	m_ARemainder = 0.f;
}

MouseCallback_RotateLandscape::~MouseCallback_RotateLandscape(){
}

//----------------------------------------------------

bool MouseCallback_RotateLandscape::Start(){
	if( Scene.SelectionCount( OBJCLASS_LANDSCAPE, true ) == 0 )
		return false;
	Scene.UndoPrepare();
	m_ARemainder = 0.f;
	m_RCenter.set( UI.pivot() );
	return true;
}

bool MouseCallback_RotateLandscape::End(){
	Scene.SmoothLandscape();
	return true;
}

void MouseCallback_RotateLandscape::Move(){
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
		if( (*_F)->ClassID() == OBJCLASS_LANDSCAPE && (*_F)->Visible() )
			if( (*_F)->Selected() )
				(*_F)->Rotate( m_RCenter, m_RVector, amount );
}

//----------------------------------------------------
