//----------------------------------------------------
// file: MSC_AddObject.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_AddObject.h"
#include "Scene.h"
#include "Library.h"
#include "SObject2.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_AddObject::MouseCallback_AddObject()
	:MouseCallback( TARGET_OBJECT, ACTION_ADD )
{
	m_AddReference = true;
	m_GridAlign = true;
	m_QCenterAlign = true;
}

MouseCallback_AddObject::~MouseCallback_AddObject(){
}

//----------------------------------------------------

bool MouseCallback_AddObject::Start(){
	if( Lib.o()!=0 ){
		IVector p;
		if( UI.PickGround(&p,&m_CurrentRStart,&m_CurrentRNorm) ){

			char namebuffer[MAX_OBJ_NAME];
			Scene.GenObjectName( namebuffer );

			Scene.UndoPrepare();
			SObject2 *obj = new SObject2( namebuffer );

			if( m_AddReference ){
				obj->LibReference( Lib.o() );
			} else {
				obj->LibCopy( Lib.o() );
			}

			if( m_QCenterAlign ){
				p.x = snapto( p.x, Scene.lx()/2.f );
				p.z = snapto( p.z, Scene.lz()/2.f );
				p.y = 0;
			} else if( m_GridAlign ){
				p.x = snapto( p.x, Scene.lx()/10.f );
				p.z = snapto( p.z, Scene.lz()/10.f );
				p.y = 0;
			}

			obj->Move( p );
			Scene.AddObject( obj );
		}
	}
	return false;
}

bool MouseCallback_AddObject::End(){
	return true;
}

void MouseCallback_AddObject::Move(){
}

//----------------------------------------------------

