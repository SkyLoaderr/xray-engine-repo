//----------------------------------------------------
// file: MSC_ScaleLandscape.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_ScaleLandscape.h"
#include "Scene.h"
#include "Landscape.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_ScaleLandscape::MouseCallback_ScaleLandscape()
	:MouseCallback( TARGET_LANDSCAPE, ACTION_SCALE )
{
	m_Center.set(0,0,0);
	m_AxisEnable[0] = true;
	m_AxisEnable[1] = true;
	m_AxisEnable[2] = true;
}

MouseCallback_ScaleLandscape::~MouseCallback_ScaleLandscape(){
}

//----------------------------------------------------

bool MouseCallback_ScaleLandscape::Start(){
	if( Scene.SelectionCount( OBJCLASS_LANDSCAPE, true ) == 0 )
		return false;
	Scene.UndoPrepare();
	m_Center.set( UI.pivot() );
	return true;
}

bool MouseCallback_ScaleLandscape::End(){
	Scene.SmoothLandscape();
	return true;
}

void MouseCallback_ScaleLandscape::Move(){

	float dy = - m_DeltaCpH.y * g_MouseScaleSpeed;

	IVector amount;
	amount.set( 1.f+dy, 1.f+dy, 1.f+dy );

	for(int i=0;i<3;i++)
		if(!m_AxisEnable[i])
			amount.a[i] = 1.f;

	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_LANDSCAPE && (*_F)->Visible() )
			if( (*_F)->Selected() )
				(*_F)->Scale( m_Center, amount );
}

//----------------------------------------------------
