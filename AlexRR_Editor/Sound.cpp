// Sound.cpp: implementation of the CSound class.
//
//////////////////////////////////////////////////////////////////////

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "Sound.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "FileSystem.h"
#include "D3DUtils.h"
#include "MenuTemplate.rh"


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

void CSound::Render( IMatrix& parent ){

	if(!UI.candrawsounds())
		return;

	DWORD sw0,sw1,sw2,sw3,sw4;
	DU_SphereDefinition params;
	
	params.center.set( m_Position );
	params.radius = m_Range;

	if( Selected() ){
		params.color.set(255,255,0);
	} else {
		params.color.set(0,0,255);
	}

	CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,(LPD3DMATRIX)parent.a) );

	CDX( UI.d3d()->GetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,&sw0) );
	CDX( UI.d3d()->GetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,&sw1) );
	CDX( UI.d3d()->GetRenderState(D3DRENDERSTATE_ZENABLE,&sw2) );
	CDX( UI.d3d()->GetRenderState(D3DRENDERSTATE_COLORVERTEX,&sw3) );
	CDX( UI.d3d()->GetRenderState(D3DRENDERSTATE_LIGHTING,&sw4) );

	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE ) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_COLORVERTEX,TRUE) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE) );

	DU_DrawLineSphere( UI.d3d(), &params );

	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,sw0) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,sw1) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_ZENABLE,sw2) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_COLORVERTEX,sw3) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_LIGHTING,sw4) );
}

bool CSound::BoxPick(
	ICullPlane *planes,
	IMatrix& parent )
{
	IVector transformed;
	parent.transform(transformed,m_Position);
	for( int i=0; i<4; i++)
		if( planes[i].dist_point(transformed) < (-m_Range) )
			return false;
	return true;
}

bool CSound::RTL_Pick(
	float *distance,
	IVector& start,
	IVector& direction,
	IMatrix& parent )
{
	IVector transformed;
	parent.transform(transformed,m_Position);

	IVector ray2;
	ray2.sub( transformed, start );
	
	float d = ray2.dot(direction);
	if( d > 0  ){
		float d2 = ray2.lenght();
		if( (d2*d2-d*d) < (m_Range*m_Range) ){
			(*distance) = d;
			return true;
		}
	}

	return false;
}

void CSound::Move( IVector& amount ){
	m_Position.add( amount );
}

void CSound::Rotate( IVector& center, IVector& axis, float angle ){
	IMatrix m;
	m.r( angle, axis );
	m_Position.sub( center );
	m.shorttransform( m_Position );
	m_Position.add( center );
}

void CSound::LocalRotate( IVector& axis, float angle ){
}

//----------------------------------------------------

void CSound::Load( FSChunkDef *chunk ){
	_ASSERTE( chunk );
	FS.readstring( chunk->filehandle, m_Name, sizeof(m_Name) );
	FS.readstring( chunk->filehandle, m_fName, sizeof(m_fName) );
	FS.readvector( chunk->filehandle, m_Position.a);
	m_Range = FS.readfloat(chunk->filehandle);
}

void CSound::Save( int handle ){
	FS.writestring	( handle, m_Name );
	FS.writestring	( handle, m_fName );
	FS.writevector	( handle, m_Position.a );
	FS.writefloat	( handle, m_Range );
}

//----------------------------------------------------

