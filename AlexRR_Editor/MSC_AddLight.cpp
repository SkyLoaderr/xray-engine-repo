//----------------------------------------------------
// file: MSC_AddLight.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_AddLight.h"
#include "Scene.h"
#include "Library.h"
#include "Light.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_AddLight::MouseCallback_AddLight()
	:MouseCallback( TARGET_LIGHT, ACTION_ADD )
{
	m_Directional = false;
	m_GridAlign = true;
}

MouseCallback_AddLight::~MouseCallback_AddLight(){
}

//----------------------------------------------------

bool MouseCallback_AddLight::Start(){
	IVector p;
	if( UI.PickGround(&p,&m_CurrentRStart,&m_CurrentRNorm) ){

		char namebuffer[MAX_OBJ_NAME];
		Scene.GenObjectName( namebuffer );

		Scene.UndoPrepare();
		Light *light = new Light( namebuffer );

		if( m_Directional ){
			light->m_Directional = 1;
		}

		if( m_GridAlign ){
			p.x = snapto( p.x, Scene.lx()/10.f );
			p.z = snapto( p.z, Scene.lz()/10.f );
			p.y = 0;
		}

		light->Move( p );
		Scene.AddObject( light );
	}

	return false;
}

bool MouseCallback_AddLight::End(){
	return true;
}

void MouseCallback_AddLight::Move(){
}

//----------------------------------------------------

