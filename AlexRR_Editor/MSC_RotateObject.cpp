//----------------------------------------------------
// file: MSC_RotateObject.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_RotateObject.h"
#include "Scene.h"
#include "SObject2.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_RotateObject::MouseCallback_RotateObject()
	:MouseCallback( TARGET_OBJECT, ACTION_ROTATE )
{
	m_Local = false;
	m_RCenter.set(0,0,0);
	m_RVector.set(0,1,0);

	m_ASnap = true;
	m_ARemainder = 0.f;
}

MouseCallback_RotateObject::~MouseCallback_RotateObject(){
}

//----------------------------------------------------

bool MouseCallback_RotateObject::Start(){
	if( Scene.SelectionCount( OBJCLASS_SOBJECT2, true ) == 0 )
		return false;
	Scene.UndoPrepare();
	m_ARemainder = 0.f;
	m_RCenter.set( UI.pivot() );
	return true;
}

bool MouseCallback_RotateObject::End(){
	return true;
}

void MouseCallback_RotateObject::Move(){
	
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
		if( (*_F)->ClassID() == OBJCLASS_SOBJECT2 && (*_F)->Visible() )
			if( (*_F)->Selected() ){
				if( m_Local ){
					(*_F)->LocalRotate( m_RVector, amount );
				} else {
					(*_F)->Rotate( m_RCenter, m_RVector, amount );
				}
			}
}

//----------------------------------------------------
