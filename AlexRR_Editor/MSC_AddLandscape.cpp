//----------------------------------------------------
// file: MSC_AddLandscape.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_AddLandscape.h"
#include "Scene.h"
#include "Library.h"
#include "Landscape.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_AddLandscape::MouseCallback_AddLandscape()
	:MouseCallback( TARGET_LANDSCAPE, ACTION_ADD )
{
	m_BrushSize = 1;
}

MouseCallback_AddLandscape::~MouseCallback_AddLandscape(){
}

//----------------------------------------------------

bool MouseCallback_AddLandscape::Start(){
	if( Lib.l()==0 )
		return false;
	Scene.UndoPrepare();
	return AddHere();
}

bool MouseCallback_AddLandscape::End(){
	Scene.SmoothLandscape();
	return true;
}

void MouseCallback_AddLandscape::Move(){
	AddHere();
}

//----------------------------------------------------

bool MouseCallback_AddLandscape::AddHere(){

	IVector p;
	if( UI.PickGround(&p,&m_CurrentRStart,&m_CurrentRNorm) ){

		int x = floorf( p.x / Scene.lx() );
		int z = floorf( p.z / Scene.lz() );

		int add_xmin = x-m_BrushSize/2;
		int add_zmin = z-m_BrushSize/2;
		int add_xmax = add_xmin + m_BrushSize;
		int add_zmax = add_zmin + m_BrushSize;
		
		for( int i=add_xmin; i<add_xmax; i++)
			for( int j=add_zmin; j<add_zmax; j++)
				AddTo( i, j );

		return true;
	}

	return false;
}

void MouseCallback_AddLandscape::AddTo( int x, int z ){

	SLandscape *l = 0;
	//char namebuffer[MAX_OBJ_NAME];
	
	SceneObject *obj = Scene.QuadPick(x,z,OBJCLASS_LANDSCAPE);
	if( obj == 0 ){
		//Scene.GenObjectName( namebuffer );
		//l = new SLandscape( namebuffer );
		l = new SLandscape();
		l->SetQuad(x,z);
		Scene.AddObject( l );
	} else {
		l = (SLandscape*)obj;
	}

	_ASSERTE( l );
	l->SetTexture(Lib.l());
}

//----------------------------------------------------

