// Sound.cpp: implementation of the CSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "D3DUtils.h"
#include "texture.h"
#include "Sound.h"
#include "Frustum.h"

#define VIS_RADIUS 0.25f
#define SOUND_SEL_COLOR 	0x00FFFFFF
#define SOUND_NORM_COLOR 	0x000000FF
#define SOUND_LOCK_COLOR 	0x00FF0000

#define SOUND_VERSION   				0x0010
//----------------------------------------------------
#define SOUND_CHUNK_VERSION				0x0211
#define SOUND_CHUNK_PARAMS				0x0212
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

bool CSound::GetBox( Fbox& box ){
	box.set( m_Position, m_Position );
	box.min.sub(m_Range);
	box.max.add(m_Range);
	return true;
}

void CSound::Render( Fmatrix& parent, ERenderPriority flag ){
    if(flag==rpNormal){
        DWORD clr=Locked()?SOUND_LOCK_COLOR:(Selected()?SOUND_SEL_COLOR:SOUND_NORM_COLOR);
        if (Selected()) DU::DrawLineSphere( m_Position, m_Range, clr, true );
        DU::DrawSound(m_Position,VIS_RADIUS, clr);
    }
}

bool CSound::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
    return (frustum.testSphere(m_Position,VIS_RADIUS))?true:false;
}

bool CSound::RayPick(float& distance, Fvector& start, Fvector& direction, Fmatrix& parent, SRayPickInfo* pinf){
	Fvector transformed;
	parent.transform_tiny(transformed,m_Position);

	Fvector ray2;
	ray2.sub( transformed, start );

	float d = ray2.dotproduct(direction);
	if( d > 0  ){
		float d2 = ray2.magnitude();
		if( ((d2*d2-d*d) < (VIS_RADIUS*VIS_RADIUS)) && (d>VIS_RADIUS)){
        	if (d<distance){
	            distance = d;
    	        return true;
            }
		}
	}

	return false;
}

void CSound::Move( Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
	m_Position.add( amount );
}

void CSound::Rotate( Fvector& center, Fvector& axis, float angle ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	Fmatrix m;
	m.rotation			(axis, -angle);
	m_Position.sub		(center);
	m.transform_tiny	(m_Position);
	m_Position.add		(center);
    UI->UpdateScene		();
}

void CSound::LocalRotate( Fvector& axis, float angle ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
}

//----------------------------------------------------

bool CSound::Load(CStream& F){
	DWORD version = 0;

    R_ASSERT(F.ReadChunk(SOUND_CHUNK_VERSION,&version));
    if( version!=SOUND_VERSION ){
        Log->DlgMsg( mtError, "CSound: Unsupported version.");
        return false;
    }
    
	SceneObject::Load(F);

    R_ASSERT(F.FindChunk(SOUND_CHUNK_PARAMS));
	F.RstringZ		(m_fName);
	F.Rvector		(m_Position);
	m_Range 		= F.Rfloat();

    return true;
}

void CSound::Save(CFS_Base& F){
	SceneObject::Save(F);

	F.open_chunk	(SOUND_CHUNK_VERSION);
	F.Wword			(SOUND_VERSION);
	F.close_chunk	();

	F.open_chunk	(SOUND_CHUNK_PARAMS);
	F.WstringZ		(m_fName);
	F.Wvector		(m_Position);
	F.Wfloat		(m_Range);
	F.close_chunk	();
}

//----------------------------------------------------

