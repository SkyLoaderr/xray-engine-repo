//----------------------------------------------------
// file: Light.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "Light.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "FileSystem.h"
#include "D3DUtils.h"
#include "MenuTemplate.rh"


//----------------------------------------------------

Light::Light( char *name )
	:SceneObject()
{
	Construct();
	strcpy( m_Name, name );
}

Light::Light()
	:SceneObject()
{
	Construct();
}

void Light::Construct(){

	m_ClassID = OBJCLASS_LIGHT;
	
	m_CastShadows = false;
	m_Directional = false;

	m_Ambient.set(0.2f,0.2f,0.2f,1.f);
	m_Diffuse.set(0.7f,0.7f,0.7f,1.f);
	m_Specular.set(0,0,0,1);

	m_Position.set(0,0,0);
	m_Direction.set(0,0,1);

	m_Attenuation0 = 1.f;
	m_Attenuation1 = 0;
	m_Attenuation2 = 0;
	m_Range = 8.f;

	m_D3DIndex = -1;

	FillD3DParams();
}

Light::~Light(){
}

//----------------------------------------------------

void Light::FillD3DParams(){

	memset(&m_D3D,0,sizeof(m_D3D));

	if( m_Directional ){
		
		m_D3D.dltType = D3DLIGHT_DIRECTIONAL;
		m_Diffuse.fillfloat4((float*)&m_D3D.dcvDiffuse);
		m_Ambient.fillfloat4((float*)&m_D3D.dcvAmbient);
		m_Specular.fillfloat4((float*)&m_D3D.dcvSpecular);
		m_D3D.dvDirection.x = m_Direction.x;
		m_D3D.dvDirection.y = m_Direction.y;
		m_D3D.dvDirection.z = m_Direction.z;

	} else {

		m_D3D.dltType = D3DLIGHT_POINT;
		m_Diffuse.fillfloat4((float*)&m_D3D.dcvDiffuse);
		m_Ambient.fillfloat4((float*)&m_D3D.dcvAmbient);
		m_Specular.fillfloat4((float*)&m_D3D.dcvSpecular);
		m_D3D.dvPosition.x = m_Position.x;
		m_D3D.dvPosition.y = m_Position.y;
		m_D3D.dvPosition.z = m_Position.z;
		m_D3D.dvAttenuation0 = m_Attenuation0;
		m_D3D.dvAttenuation1 = m_Attenuation1;
		m_D3D.dvAttenuation2 = m_Attenuation2;
		m_D3D.dvRange = m_Range;
	
	}
}

bool Light::GetBox( IAABox& box ){
	if( m_Directional )
		return false;
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

void Light::Render( IMatrix& parent ){

	if(!UI.candrawlights())
		return;

	DWORD sw0,sw1,sw2,sw3,sw4;
	DU_SphereDefinition params;
	
	params.center.set( m_Position );
	params.radius = m_Range;

	if( Selected() ){
		params.color.set(255,0,0);
	} else {
		params.color.set(200,200,200);
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

void Light::Set( int d3dindex ){
	_ASSERTE( d3dindex>0 );
	m_D3DIndex = d3dindex;
	CDX( UI.d3d()->SetLight(m_D3DIndex,&m_D3D) );
	CDX( UI.d3d()->LightEnable(m_D3DIndex,TRUE) );
}

void Light::UnSet(){
	_ASSERTE( m_D3DIndex>0 );
	CDX( UI.d3d()->LightEnable(m_D3DIndex,FALSE) );
}

bool Light::BoxPick(
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

bool Light::RTL_Pick(
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

void Light::Move( IVector& amount ){
	m_Position.add( amount );
	FillD3DParams();
}

void Light::Rotate( IVector& center, IVector& axis, float angle ){
	IMatrix m;
	m.r( angle, axis );
	m_Position.sub( center );
	m.shorttransform( m_Position );
	m_Position.add( center );
	m.shorttransform( m_Direction );
	FillD3DParams();
}

void Light::LocalRotate( IVector& axis, float angle ){
	IMatrix m;
	m.r( angle, axis );
	m.shorttransform( m_Direction );
	FillD3DParams();
}

//----------------------------------------------------

void Light::Load( FSChunkDef *chunk ){

	_ASSERTE( chunk );

	FS.readstring( chunk->filehandle, m_Name, MAX_OBJ_NAME );

	DWORD version = 0;
	FSChunkDef current;
	FS.initchunk( &current, chunk->filehandle );

	while( FS.rinchunk( chunk ) ){

		FS.ropenchunk( &current );
		switch( current.chunkid ){

		case LIGHT_CHUNK_VERSION:
			version = FS.readword( chunk->filehandle );
			_ASSERTE( version==10 );
			break;

		case LIGHT_CHUNK_DIRECTIONAL_FLAG:
			m_Directional = FS.readword( chunk->filehandle );
			break;

		case LIGHT_CHUNK_BUILD_OPTIONS:
			m_CastShadows = FS.readword( chunk->filehandle );
			break;

		case LIGHT_CHUNK_COLOR_PARAMS:
			FS.readcolor4(chunk->filehandle,m_Ambient.val);
			FS.readcolor4(chunk->filehandle,m_Diffuse.val);
			FS.readcolor4(chunk->filehandle,m_Specular.val);
			break;

		case LIGHT_CHUNK_OMNI_PARAMS:
			FS.readvector(chunk->filehandle,m_Position.a);
			m_Range = FS.readfloat(chunk->filehandle);
			m_Attenuation0 = FS.readfloat(chunk->filehandle);
			m_Attenuation1 = FS.readfloat(chunk->filehandle);
			m_Attenuation2 = FS.readfloat(chunk->filehandle);
			break;

		case LIGHT_CHUNK_DIR_PARAMS:
			FS.readvector(chunk->filehandle,m_Direction.a);
			break;

		default:
			NConsole.print( "Light: undefined chunk 0x%04X", current.chunkid );
			break; }

		FS.rclosechunk( &current );
	}

	FillD3DParams();
}

void Light::Save( int handle ){

	FSChunkDef chunk_level0;
	FS.initchunk(&chunk_level0,handle);

	FS.writestring( handle, m_Name );

	FS.wopenchunk(&chunk_level0,LIGHT_CHUNK_VERSION);
	FS.writeword( handle, 10 );
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,LIGHT_CHUNK_BUILD_OPTIONS);
	FS.writeword( handle, m_CastShadows );
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,LIGHT_CHUNK_DIRECTIONAL_FLAG);
	FS.writeword( handle, m_Directional );
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,LIGHT_CHUNK_COLOR_PARAMS);
	FS.writecolor4(handle,m_Ambient.val);
	FS.writecolor4(handle,m_Diffuse.val);
	FS.writecolor4(handle,m_Specular.val);
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,LIGHT_CHUNK_OMNI_PARAMS);
	FS.writevector(handle,m_Position.a);
	FS.writefloat(handle,m_Range);
	FS.writefloat(handle,m_Attenuation0);
	FS.writefloat(handle,m_Attenuation1);
	FS.writefloat(handle,m_Attenuation2);
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,LIGHT_CHUNK_DIR_PARAMS);
	FS.writevector(handle,m_Direction.a);
	FS.wclosechunk(&chunk_level0);
}

//----------------------------------------------------

