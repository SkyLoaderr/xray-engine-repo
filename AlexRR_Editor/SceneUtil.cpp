//----------------------------------------------------
// file: SceneUtil.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "Scene.h"
#include "SceneClassList.h"
#include "Texture.h"
#include "Landscape.h"
#include "Light.h"
#include "SObject2.h"

//----------------------------------------------------

bool EScene::SearchName( char *name ){
	ObjectIt obj = m_Objects.begin();
	for(;obj!=m_Objects.end();obj++)
		if( 0==stricmp((*obj)->GetName(),name) )
			return true;
	return false;
}

void EScene::GenObjectName( char *buffer ){
	do{
		sprintf( buffer, "obj_%04d", m_LastAvailObject );
		m_LastAvailObject++;
	} while( SearchName( buffer ) );
}

//----------------------------------------------------

void EScene::SmoothLandscape(){

	ObjectIt f = m_Objects.begin();
	
	for(;f!=m_Objects.end();f++)
	
		if( (*f)->ClassID() == OBJCLASS_LANDSCAPE ){

			SLandscape *l = (SLandscape *)(*f);
			l->ResetPointNormals();

			ObjectIt f1 = m_Objects.begin();
			for(;f1!=m_Objects.end();f1++)
				if( (f != f1) && (*f1)->ClassID() == OBJCLASS_LANDSCAPE )
					l->TryAddPointNormals((SLandscape *)(*f1));

			l->NormalizePointNormals();
			l->FillD3DPoints();
		}
	
}

//----------------------------------------------------

void EScene::ResetDefaultLight(){
	m_GlobalAmbient.set( 1,1,1,0 );
	m_GlobalDiffuse.set( 1,1,1,0 );
	m_GlobalSpecular.set( 1,1,1,0);
	m_GlobalLightDirection.set(-1,-1,-1);
	m_GlobalLightDirection.normalize();
}

void EScene::FillDefaultLight( D3DLIGHT7 *light ){

	memset( light, 0, sizeof(D3DLIGHT7) );

	light->dltType = D3DLIGHT_DIRECTIONAL;
	light->dvRange = D3DLIGHT_RANGE_MAX;
	light->dvAttenuation0 = 1.0f;

	light->dcvAmbient.r = m_GlobalAmbient.r;
	light->dcvAmbient.g = m_GlobalAmbient.g;
	light->dcvAmbient.b = m_GlobalAmbient.b;
	light->dcvAmbient.a = m_GlobalAmbient.a;
	
	light->dcvDiffuse.r = m_GlobalDiffuse.r;
	light->dcvDiffuse.g = m_GlobalDiffuse.g;
	light->dcvDiffuse.b = m_GlobalDiffuse.b;
	light->dcvDiffuse.a = m_GlobalDiffuse.a;
	
	light->dcvSpecular.r = m_GlobalSpecular.r;
	light->dcvSpecular.g = m_GlobalSpecular.g;
	light->dcvSpecular.b = m_GlobalSpecular.b;
	light->dcvSpecular.a = m_GlobalSpecular.a;
	
	memcpy(
		&light->dvDirection, 
		&m_GlobalLightDirection,
		sizeof(m_GlobalLightDirection) );
}

void EScene::SetLights(){

	// default light

	D3DLIGHT7 light;
	FillDefaultLight( &light );

	CDX( UI.d3d()->SetLight(0,&light) );
	CDX( UI.d3d()->LightEnable(0,TRUE) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_LOCALVIEWER,TRUE) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_SPECULARENABLE,TRUE) );

	// scene lights
	int i=1;
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->ClassID()==OBJCLASS_LIGHT )
			((Light*)(*_F))->Set( i++ );
}

void EScene::ClearLights(){

	// scene lights
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->ClassID()==OBJCLASS_LIGHT )
			((Light*)(*_F))->UnSet();

	// default light
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_SPECULARENABLE,FALSE) );
	CDX( UI.d3d()->LightEnable(0,FALSE) );
}


void EScene::RebuildAllObjects(){
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->ClassID()==OBJCLASS_SOBJECT2 )
			((SObject2*)(*_F))->FillD3DPoints();
}
