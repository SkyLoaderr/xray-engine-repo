//----------------------------------------------------
// file: MSC_ScaleSndPlane.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "UI_Main.h"
#include "NetDeviceLog.h"
#include "MSC_List.h"
#include "MSC_ScaleSndPlane.h"
#include "SceneClassList.h"
#include "Scene.h"

//----------------------------------------------------

MouseCallback_ScaleSndPlane::MouseCallback_ScaleSndPlane()
	:MouseCallback( TARGET_SNDPLANE, ACTION_SCALE )
{
	m_Center.set(0,0,0);
	m_AxisEnable[0] = true;
	m_AxisEnable[1] = true;
	m_AxisEnable[2] = true;
}

MouseCallback_ScaleSndPlane::~MouseCallback_ScaleSndPlane(){
}

//----------------------------------------------------

bool MouseCallback_ScaleSndPlane::Start(){
	if( Scene.SelectionCount( OBJCLASS_SNDPLANE, true ) == 0 )
		return false;
	Scene.UndoPrepare();
	m_Center.set( UI.pivot() );
	return true;
}

bool MouseCallback_ScaleSndPlane::End(){
	return true;
}

void MouseCallback_ScaleSndPlane::Move(){

	float dy = - m_DeltaCpH.y * g_MouseScaleSpeed;

	IVector amount;
	amount.set( 1.f+dy, 1.f+dy, 1.f+dy );

	for(int i=0;i<3;i++)
		if(!m_AxisEnable[i])
			amount.a[i] = 1.f;

	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_SNDPLANE && (*_F)->Visible() )
			if( (*_F)->Selected() ){
				if( m_Local ){
					(*_F)->LocalScale( amount );
				} else {
					(*_F)->Scale( m_Center, amount );
				}
			}
}

//----------------------------------------------------
