//----------------------------------------------------
// file: MSC_AddSndPlane.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_AddSndPlane.h"
#include "Scene.h"
#include "SndPlane.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_AddSndPlane::MouseCallback_AddSndPlane()
	:MouseCallback( TARGET_SNDPLANE, ACTION_ADD )
{
	m_PlaneSize = 1;
}

MouseCallback_AddSndPlane::~MouseCallback_AddSndPlane(){
}

//----------------------------------------------------

bool MouseCallback_AddSndPlane::Start(){
	Scene.UndoPrepare();
	AddHere();
	return false;
}

bool MouseCallback_AddSndPlane::End(){
	return true;
}

void MouseCallback_AddSndPlane::Move(){
}

//----------------------------------------------------

bool MouseCallback_AddSndPlane::AddHere(){

	IVector p;
	if( UI.PickGround(&p,&m_CurrentRStart,&m_CurrentRNorm) ){

		IVector vscale;
		vscale.set( m_PlaneSize, m_PlaneSize, m_PlaneSize );

		SndPlane *l = new SndPlane();
		l -> LocalScale( vscale );
		l -> Move( p );

		Scene.AddObject( l );

		return true;
	}

	return false;
}

//----------------------------------------------------

