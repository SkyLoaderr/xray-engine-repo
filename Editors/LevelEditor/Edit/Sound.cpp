// Sound.cpp: implementation of the CSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "texture.h"
#include "Sound.h"
#include "Frustum.h"
#include "d3dutils.h"
#include "PropertiesListHelper.h"

#define VIS_RADIUS 0.25f
#define SOUND_SEL_COLOR 	0x00FFFFFF
#define SOUND_NORM_COLOR 	0x000000FF
#define SOUND_LOCK_COLOR 	0x00FF0000

#define SOUND_VERSION   				0x0011
//----------------------------------------------------
#define SOUND_CHUNK_VERSION				0x0211
#define SOUND_CHUNK_PARAMS				0x0212
#define SOUND_CHUNK_PARAMS2				0x0213
//----------------------------------------------------
CSound::CSound(LPVOID data, LPCSTR name)
	:CCustomObject(data,name)
{
	Construct(data);
}

void CSound::Construct(LPVOID data){
	ClassID		= OBJCLASS_SOUND;

	m_Range 	= 8.f;
	m_WAVName	= "";
}

CSound::~CSound(){
}

//----------------------------------------------------

bool CSound::GetBox( Fbox& box ){
	box.set( PPosition, PPosition );
	box.min.sub(m_Range);
	box.max.add(m_Range);
	return true;
}

void CSound::Render(int priority, bool strictB2F){
    if((1==priority)&&(false==strictB2F)){
        DWORD clr=Locked()?SOUND_LOCK_COLOR:(Selected()?SOUND_SEL_COLOR:SOUND_NORM_COLOR);
        if (Selected()) DU::DrawLineSphere( PPosition, m_Range, clr, true );
        DU::DrawSound(PPosition,VIS_RADIUS, clr);
    }
}

bool CSound::FrustumPick(const CFrustum& frustum){
    return (frustum.testSphere_dirty(PPosition,VIS_RADIUS))?true:false;
}

bool CSound::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf){
	Fvector ray2;
	ray2.sub( PPosition, start );

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
//----------------------------------------------------

bool CSound::Load(CStream& F){
	DWORD version = 0;

    R_ASSERT(F.ReadChunk(SOUND_CHUNK_VERSION,&version));
    if(version!=SOUND_VERSION){
        ELog.DlgMsg( mtError, "CSound: Unsupported version.");
        return false;
    }

	inherited::Load(F);

    string1024		buf;
    R_ASSERT(F.FindChunk(SOUND_CHUNK_PARAMS2));
    F.RstringZ		(buf); m_WAVName=buf;
    m_Range 		= F.Rfloat();

    UpdateTransform	();

    return true;
}

void CSound::Save(CFS_Base& F){
	inherited::Save(F);

	F.open_chunk	(SOUND_CHUNK_VERSION);
	F.Wword			(SOUND_VERSION);
	F.close_chunk	();

	F.open_chunk	(SOUND_CHUNK_PARAMS2);
	F.WstringZ		(m_WAVName.c_str());
	F.Wfloat		(m_Range);
	F.close_chunk	();
}

//----------------------------------------------------

void CSound::FillProp(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp(pref,values);
    PHelper.CreateALibSound	(values,PHelper.PrepareKey(pref,"WAVE name"),	&m_WAVName);
    PHelper.CreateFloat		(values,PHelper.PrepareKey(pref,"Range"),		&m_Range,0.1f,1000.f,0.1f,1);
}
//----------------------------------------------------

