//----------------------------------------------------
// file: PClipper.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "PClipper.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "FileSystem.h"
#include "Primitives.h"
#include "cl_intersect.h"
#include "texture.h"

//----------------------------------------------------

PClipper::PClipper( char *name )
	:SceneObject()
{
	Construct();
	strcpy( m_Name, name );
}

PClipper::PClipper()
	:SceneObject()
{
	Construct();
}

void PClipper::Construct(){

	m_ClassID = OBJCLASS_PCLIPPER;

	m_Density = 0.3f;

	m_Points[0].set( 0, 0, 0 );
	m_Points[1].set( 0, 1, 0 );
	m_Points[2].set( 1, 1, 0 );
	m_Points[3].set( 1, 0, 0 );
}

void PClipper::SetPlane(int p_num, Fvector& p, float d)
{
    switch (p_num){
        case 0:
            m_Points[0].set( p );
            m_Points[1].set( p ); m_Points[1].y += d*0.005f;
            m_Points[2].set( p ); m_Points[2].y += d*0.005f; m_Points[2].x += d*0.005f;
            m_Points[3].set( p ); m_Points[3].x += d*0.005f;
            break;
        case 1:
            m_Points[3].set( m_Points[0] );
            m_Points[1].set( p );
            m_Points[2].set( p );
            break;
        case 2:{
    		_vector c;			// calc point 3
            m_Points[2].set( p );
	    	c.add(m_Points[0],m_Points[2]);
    		c.div(2);
    		c.sub(m_Points[1]);
    		m_Points[3].direct(m_Points[1], c, 2.f);
            }break;
        default: throw -1;
    }
}

PClipper::~PClipper(){
}

//----------------------------------------------------

bool PClipper::GetBox( IAABox& box ){
	box.vmin.set( m_Points[0] );
	box.vmax.set( m_Points[0] );
	for( int i=1; i<4; i++)
		box.growrange(
			m_Points[i].x,
			m_Points[i].y,
			m_Points[i].z );
	return true;
}

void PClipper::Render( Fmatrix& parent ){
	Fmatrix matrix;
	matrix.identity();

	CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,matrix.d3d()) );
	TM.BindNone();
    UI.D3D_ApplyRenderState(UI.SB_DrawClipPlane);

	DrawQuadPlane( m_Points, Selected(), m_Density );
}

bool PClipper::BoxPick(
	ICullPlane *planes,
	Fmatrix& parent )
{
	Fvector p[4];
	for( int i=0;i<4;i++)
		parent.transform( p[i], m_Points[i] );

	if( FrustumPickFace(planes, p[0], p[1], p[2] ) ||
		FrustumPickFace(planes, p[2], p[3], p[0] )    )
			return true;

	return false;
}

bool PClipper::RTL_Pick(
	float *distance,
	Fvector& start,
	Fvector& direction,
	Fmatrix& parent, SPickInfo* pinf )
{

	Fvector p[4];
	for( int i=0;i<4;i++)
		parent.transform( p[i], m_Points[i] );

	if( PickFace2(start, direction, p[0], p[1], p[2], distance, false ))
		return true;

	if( PickFace2(start, direction, p[2], p[3], p[0], distance, false ))
		return true;

	return false;
}

void PClipper::Move( Fvector& amount ){
	for( int i=0; i<4; i++)
		m_Points[i].add( amount );
}

void PClipper::Rotate( Fvector& center, Fvector& axis, float angle ){

	Fmatrix m;
	m.rotation( axis, angle );

	for( int i=0; i<4; i++){
		m_Points[i].sub( center );
		m.shorttransform( m_Points[i] );
		m_Points[i].add( center );
	}
}

void PClipper::Scale( Fvector& center, Fvector& amount ){

	Fmatrix m;
	m.scale( amount );

	for( int i=0; i<4; i++){
		m_Points[i].sub( center );
		m.shorttransform( m_Points[i] );
		m_Points[i].add( center );
	}
}

void PClipper::LocalRotate( Fvector& axis, float angle ){

	Fvector center;
	center.set( m_Points[0] );
	for( int i=1; i<4; i++)
		center.add( m_Points[i] );

	center.mul( 0.25f );

	Fmatrix m;
	m.rotation( axis, angle );

	for( i=0; i<4; i++){
		m_Points[i].sub( center );
		m.shorttransform( m_Points[i] );
		m_Points[i].add( center );
	}
}

void PClipper::LocalScale( Fvector& amount ){

	Fvector center;
	center.set( m_Points[0] );
	for( int i=1; i<4; i++)
		center.add( m_Points[i] );

	center.mul( 0.25f );

	Fmatrix m;
	m.scale( amount );

	for( i=0; i<4; i++){
		m_Points[i].sub( center );
		m.shorttransform( m_Points[i] );
		m_Points[i].add( center );
	}
}

//----------------------------------------------------

void PClipper::Load( FSChunkDef *chunk ){
	_ASSERTE( chunk );
	FS.readstring( chunk->filehandle, m_Name, sizeof(m_Name) );
	m_Density = FS.readfloat( chunk->filehandle );
	for( int i=0; i<4; i++)
		FS.readvector( chunk->filehandle, m_Points[i].m );
}

void PClipper::Save( int handle ){
	FS.writestring( handle, m_Name );
	FS.writefloat( handle, m_Density );
	for( int i=0; i<4; i++)
		FS.writevector( handle, m_Points[i].m );
}

//----------------------------------------------------

