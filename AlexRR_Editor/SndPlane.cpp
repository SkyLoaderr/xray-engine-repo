//----------------------------------------------------
// file: SndPlane.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "SndPlane.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "FileSystem.h"
#include "Primitives.h"


//----------------------------------------------------

SndPlane::SndPlane( char *name )
	:SceneObject()
{
	Construct();
	strcpy( m_Name, name );
}

SndPlane::SndPlane()
	:SceneObject()
{
	Construct();
}

void SndPlane::Construct(){

	m_ClassID = OBJCLASS_SNDPLANE;

	m_Dencity = 0.3f;

	m_Points[0].set( 0, 0, 0 );
	m_Points[1].set( 0, 1, 0 );
	m_Points[2].set( 0, 1, 1 );
	m_Points[3].set( 0, 0, 1 );
}

SndPlane::~SndPlane(){
}

//----------------------------------------------------

bool SndPlane::GetBox( IAABox& box ){
	box.vmin.set( m_Points[0] );
	box.vmax.set( m_Points[0] );
	for( int i=1; i<4; i++)
		box.growrange(
			m_Points[i].x,
			m_Points[i].y,
			m_Points[i].z );
	return true;
}

void SndPlane::Render( IMatrix& parent ){
	DrawQuadPlane( m_Points, Selected() );
}

bool SndPlane::BoxPick(
	ICullPlane *planes,
	IMatrix& parent )
{
	IVector p[4];
	for( int i=0;i<4;i++)
		parent.transform( p[i], m_Points[i] );

	if( FrustumPickFace(planes, p[0], p[1], p[2] ) ||
		FrustumPickFace(planes, p[2], p[3], p[0] )    )
			return true;

	return false;
}

bool SndPlane::RTL_Pick(
	float *distance,
	IVector& start,
	IVector& direction,
	IMatrix& parent )
{

	IVector p[4];
	for( int i=0;i<4;i++)
		parent.transform( p[i], m_Points[i] );

	IVector facenormal;
	
	facenormal.mknormal(p[0], p[1], p[2]);
	if( PickFace(distance, start, direction, facenormal, p[0], p[1], p[2] ))
		return true;

	facenormal.mknormal(p[2], p[3], p[0]);
	if( PickFace(distance, start, direction, facenormal,p[2], p[3], p[0] ))
		return true;

	return false;
}

void SndPlane::Move( IVector& amount ){
	for( int i=0; i<4; i++)
		m_Points[i].add( amount );
}

void SndPlane::Rotate( IVector& center, IVector& axis, float angle ){
	
	IMatrix m;
	m.r( angle, axis );

	for( int i=0; i<4; i++){
		m_Points[i].sub( center );
		m.shorttransform( m_Points[i] );
		m_Points[i].add( center );
	}
}

void SndPlane::Scale( IVector& center, IVector& amount ){
	
	IMatrix m;
	m.s( amount );

	for( int i=0; i<4; i++){
		m_Points[i].sub( center );
		m.shorttransform( m_Points[i] );
		m_Points[i].add( center );
	}
}

void SndPlane::LocalRotate( IVector& axis, float angle ){

	IVector center;
	center.set( m_Points[0] );
	for( int i=1; i<4; i++)
		center.add( m_Points[i] );

	center.mul( 0.25f );

	IMatrix m;
	m.r( angle, axis );
	
	for( i=0; i<4; i++){
		m_Points[i].sub( center );
		m.shorttransform( m_Points[i] );
		m_Points[i].add( center );
	}
}

void SndPlane::LocalScale( IVector& amount ){
	
	IVector center;
	center.set( m_Points[0] );
	for( int i=1; i<4; i++)
		center.add( m_Points[i] );

	center.mul( 0.25f );

	IMatrix m;
	m.s( amount );
	
	for( i=0; i<4; i++){
		m_Points[i].sub( center );
		m.shorttransform( m_Points[i] );
		m_Points[i].add( center );
	}
}

//----------------------------------------------------

void SndPlane::Load( FSChunkDef *chunk ){
	_ASSERTE( chunk );
	FS.readstring( chunk->filehandle, m_Name, sizeof(m_Name) );
	m_Dencity = FS.readfloat( chunk->filehandle );
	for( int i=0; i<4; i++)
		FS.readvector( chunk->filehandle, m_Points[i].a );
}

void SndPlane::Save( int handle ){
	FS.writestring( handle, m_Name );
	FS.writefloat( handle, m_Dencity );
	for( int i=0; i<4; i++)
		FS.writevector( handle, m_Points[i].a );
}

//----------------------------------------------------

