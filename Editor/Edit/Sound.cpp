// Sound.cpp: implementation of the CSound class.
//
//////////////////////////////////////////////////////////////////////

#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "FileSystem.h"
#include "D3DUtils.h"
#include "texture.h"
#include "Sound.h"

//----------------------------------------------------
CSound::CSound( char *name )
	:SceneObject()
{
	Construct();
	strcpy( m_Name, name );
}

CSound::CSound()
	:SceneObject()
{
	Construct();
}

void CSound::Construct(){

	m_ClassID = OBJCLASS_SOUND;
	
	m_Position.set(0,0,0);
	m_Range = 8.f;
	m_fName[0] = 0;
}

CSound::~CSound(){
}

//----------------------------------------------------

bool CSound::GetBox( IAABox& box ){
	box.vmin.set( m_Position );
	box.vmax.set( m_Position );
	box.vmin.x -= m_Range;
	box.vmin.y -= m_Range;
	box.vmin.z -= m_Range;
	box.vmax.x += m_Range;
	box.vmax.y += m_Range;
	box.vmax.z += m_Range;
	return true;
}

void CSound::Render( Fmatrix& parent ){

	if(!UI.bDrawSounds)
		return;

	DWORD sw0,sw1,sw2,sw3,sw4;
	DU_SphereDefinition params;
	
	params.center.set( m_Position );
	params.radius = m_Range;

	if( Selected() ){
		params.color.set(255,255,0,0);
	} else {
		params.color.set(0,0,255,0);
	}

	CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,parent.d3d()) );
	TM.BindNone();
    UI.D3D_ApplyRenderState(UI.SB_DrawWire);
	DU_DrawLineSphere( UI.d3d(), &params );
}

bool CSound::BoxPick(
	ICullPlane *planes,
	Fmatrix& parent )
{
    if (!UI.bDrawSounds) return false;

    UI.UpdateScene();
	Fvector transformed;
	parent.transform(transformed,m_Position);
	for( int i=0; i<4; i++)
		if( planes[i].dist_point(transformed) < (-m_Range) )
			return false;
	return true;
}

bool CSound::RTL_Pick(
	float *distance,
	Fvector& start,
	Fvector& direction,
	Fmatrix& parent, SPickInfo* pinf )
{
    if (!UI.bDrawSounds) return false;

    UI.UpdateScene();
	Fvector transformed;
	parent.transform(transformed,m_Position);

	Fvector ray2;
	ray2.sub( transformed, start );

	float d = ray2.dotproduct(direction);
	if( d > 0  ){
		float d2 = ray2.magnitude();
		if( ((d2*d2-d*d) < (m_Range*m_Range)) && (d>m_Range)){
			(*distance) = d;
			return true;
		}
	}

	return false;
}

void CSound::Move( Fvector& amount ){
    UI.UpdateScene();
	m_Position.add( amount );
}

void CSound::Rotate( Fvector& center, Fvector& axis, float angle ){
    UI.UpdateScene();
	Fmatrix m;
	m.rotation( axis, angle );
	m_Position.sub( center );
	m.shorttransform( m_Position );
	m_Position.add( center );
}

void CSound::LocalRotate( Fvector& axis, float angle ){
    UI.UpdateScene();
}

//----------------------------------------------------

void CSound::Load( FSChunkDef *chunk ){
	_ASSERTE( chunk );
	FS.readstring( chunk->filehandle, m_Name, sizeof(m_Name) );
	FS.readstring( chunk->filehandle, m_fName, sizeof(m_fName) );
	FS.readvector( chunk->filehandle, m_Position.m);
	m_Range = FS.readfloat(chunk->filehandle);
}

void CSound::Save( int handle ){
	FS.writestring	( handle, m_Name );
	FS.writestring	( handle, m_fName );
	FS.writevector	( handle, m_Position.m );
	FS.writefloat	( handle, m_Range );
}

//----------------------------------------------------

