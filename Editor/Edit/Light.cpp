//----------------------------------------------------
// file: Light.cpp
//----------------------------------------------------
#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "Light.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "FileSystem.h"
#include "D3DUtils.h"
#include "Texture.h"

//----------------------------------------------------
#define LIGHT_CHUNK_VERSION				0xB411
#define LIGHT_CHUNK_BUILD_OPTIONS		0xB412
#define LIGHT_CHUNK_DIRECTIONAL_FLAG	0xB413
#define LIGHT_CHUNK_COLOR_PARAMS		0xB414
#define LIGHT_CHUNK_OMNI_PARAMS			0xB415
#define LIGHT_CHUNK_DIR_PARAMS			0xB416
#define LIGHT_CHUNK_SUN_PARAMS			0xB417
#define LIGHT_CHUNK_OMNI_PARAMS2		0xB418
//----------------------------------------------------

Light::Light( char *name ):SceneObject(){
	Construct();
	strcpy( m_Name, name );
}

Light::Light():SceneObject(){
	Construct();
}

void Light::Construct(){
	m_ClassID = OBJCLASS_LIGHT;

    fSunSize    = 0.15f;
    fPower      = 0.6f;
    bGradient   = true;
    bFlares     = true;

	m_CastShadows = false;
	m_Type = ltPoint; //Point

	m_Ambient.set(0,0,0,0);
	m_Diffuse.set(0.7f,0.7f,0.7f,0);
	m_Specular.set(0,0,0,0);

	m_Position.set(0,0,0);
	m_Direction.set(0,-1,0);

	m_Attenuation0 = 1.f;
	m_Attenuation1 = 0;
	m_Attenuation2 = 0;
	m_Range = 8.f;

    m_Brightness = 1;

	m_D3DIndex = -1;

    m_Enabled = TRUE;

	FillD3DParams();
}

Light::~Light(){
}

//----------------------------------------------------

void Light::FillD3DParams(){
    UI.UpdateScene();

	memset(&m_D3D,0,sizeof(m_D3D));

    switch (m_Type){
    case ltSun:
		m_D3D.type = D3DLIGHT_DIRECTIONAL;
		m_D3D.diffuse.set(m_Diffuse);
		m_D3D.ambient.set(m_Ambient);
		m_D3D.specular.set(m_Specular);
		m_D3D.direction.set(m_Direction);
        break;
    case ltPoint:
		m_D3D.type = D3DLIGHT_POINT;
		m_D3D.diffuse.mul(m_Diffuse,m_Brightness);
        m_D3D.ambient.mul(m_Ambient,m_Brightness);
        m_D3D.specular.mul(m_Specular,m_Brightness);
		m_D3D.position.set(m_Position);
		m_D3D.attenuation0 = m_Attenuation0;
		m_D3D.attenuation1 = m_Attenuation1;
		m_D3D.attenuation2 = m_Attenuation2;
		m_D3D.range = m_Range;
        break;
    }
}

bool Light::GetBox( IAABox& box ){
	if( m_Type == ltSun )
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

void Light::Render( Fmatrix& parent ){

	if(!UI.bDrawLights)
		return;

	DU_SphereDefinition point_params;
    DU_DirectionDefinition sun_params;
    FPcolor c;
	if( Selected() ) c.set(255,0,0,0);
	else    		 c.set(255,255,255,0);
    if (m_Type==ltPoint){
    	point_params.center.set( m_Position );
	    point_params.radius = m_Range;
        point_params.color.set(c);
    }else{
        sun_params.center.set( m_Position );
        sun_params.direction.set( m_Direction );
        sun_params.color.set(c);
    }

	CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,parent.d3d()) );
	TM.BindNone();
    UI.D3D_ApplyRenderState(UI.SB_DrawWire);
	if (m_Type==ltPoint) DU_DrawLineSphere( UI.d3d(), &point_params );
    if (m_Type==ltSun)   DU_DrawDirectionalLight( UI.d3d(), &sun_params );
}

void Light::Enable( BOOL flag )
{
	CDX( UI.d3d()->LightEnable(m_D3DIndex,flag) );
    m_Enabled = flag;
}

void Light::Set( int d3dindex ){
	_ASSERTE( d3dindex>=0 );
	m_D3DIndex = d3dindex;
	CDX( UI.d3d()->SetLight(m_D3DIndex,m_D3D.d3d()) );
}

void Light::UnSet(){
	_ASSERTE( m_D3DIndex>=0 );
	CDX( UI.d3d()->LightEnable(m_D3DIndex,FALSE) );
}

bool Light::BoxPick(
	ICullPlane *planes,
	Fmatrix& parent )
{
    if (!UI.bDrawLights) return false;

    UI.UpdateScene();
	Fvector transformed;
	parent.transform(transformed,m_Position);
	for( int i=0; i<4; i++)
		if( planes[i].dist_point(transformed) < (-m_Range) )
			return false;
	return true;
}

bool Light::RTL_Pick(
	float *distance,
	Fvector& start,
	Fvector& direction,
	Fmatrix& parent, SPickInfo* pinf )
{
    if (!UI.bDrawLights) return false;

    UI.UpdateScene();
	Fvector transformed;
	parent.transform(transformed,m_Position);

	Fvector ray2;
	ray2.sub( transformed, start );

    if (m_Type==ltPoint){
        float d = ray2.dotproduct(direction);
        if( d > 0  ){
            float d2 = ray2.magnitude();
            if( ((d2*d2-d*d) < (m_Range*m_Range)) && (d>m_Range) ){
                (*distance) = d;
                return true;
            }
        }
    }else{
        float d = ray2.dotproduct(direction);
        if( d > 0  ){
            float d2 = ray2.magnitude();
            if( (d2*d2-d*d) < (1) ){
                (*distance) = d;
                return true;
            }
        }
    }

	return false;
}

void Light::Move( Fvector& amount ){
    UI.UpdateScene();
	m_Position.add( amount );
	FillD3DParams();
}

void Light::Rotate( Fvector& center, Fvector& axis, float angle ){
    UI.UpdateScene();
	Fmatrix m;
	m.rotation( axis, angle );
	m_Position.sub( center );
	m.shorttransform( m_Position );
	m_Position.add( center );
	m.shorttransform( m_Direction );
	FillD3DParams();
}

void Light::LocalRotate( Fvector& axis, float angle ){
    UI.UpdateScene();
	Fmatrix m;
	m.rotation( axis, angle );
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
        case LIGHT_CHUNK_SUN_PARAMS:
            fSunSize = FS.readfloat(chunk->filehandle);
            fPower   = FS.readfloat(chunk->filehandle);
            bGradient= FS.readdword(chunk->filehandle);
            bFlares  = FS.readdword(chunk->filehandle);
            break;

		case LIGHT_CHUNK_VERSION:
			version = FS.readword( chunk->filehandle );
			_ASSERTE( version==10 );
			break;

		case LIGHT_CHUNK_DIRECTIONAL_FLAG:
			m_Type = (ELightType)FS.readword( chunk->filehandle );
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
			FS.readvector(chunk->filehandle,m_Position.m);
			m_Range = FS.readfloat(chunk->filehandle);
			m_Attenuation0 = FS.readfloat(chunk->filehandle);
			m_Attenuation1 = FS.readfloat(chunk->filehandle);
			m_Attenuation2 = FS.readfloat(chunk->filehandle);
			break;

		case LIGHT_CHUNK_OMNI_PARAMS2:
			m_Brightness = FS.readfloat(chunk->filehandle);
            break;

		case LIGHT_CHUNK_DIR_PARAMS:
			FS.readvector(chunk->filehandle,m_Direction.m);
			break;

		default:
			Log.Msg( "Light: undefined chunk 0x%04X", current.chunkid );
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
	FS.writeword( handle, m_Type );
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,LIGHT_CHUNK_COLOR_PARAMS);
	FS.writecolor4(handle,m_Ambient.val);
	FS.writecolor4(handle,m_Diffuse.val);
	FS.writecolor4(handle,m_Specular.val);
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,LIGHT_CHUNK_OMNI_PARAMS);
	FS.writevector(handle,m_Position.m);
	FS.writefloat(handle,m_Range);
	FS.writefloat(handle,m_Attenuation0);
	FS.writefloat(handle,m_Attenuation1);
	FS.writefloat(handle,m_Attenuation2);
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,LIGHT_CHUNK_DIR_PARAMS);
	FS.writevector(handle,m_Direction.m);
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,LIGHT_CHUNK_SUN_PARAMS);
	FS.writefloat(handle,fSunSize);
	FS.writefloat(handle,fPower);
	FS.writedword( handle, bGradient );
	FS.writedword( handle, bFlares );
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,LIGHT_CHUNK_OMNI_PARAMS2);
	FS.writefloat(handle,m_Brightness);
	FS.wclosechunk(&chunk_level0);
}

//----------------------------------------------------

