// Sound.cpp: implementation of the ESound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "texture.h"
#include "ESound.h"
#include "Frustum.h"
#include "d3dutils.h"
#include "PropertiesListHelper.h"

#define VIS_RADIUS 0.25f
#define SOUND_SEL0_COLOR 	0x00A0A0A0
#define SOUND_SEL1_COLOR 	0x00FFFFFF
#define SOUND_NORM_COLOR 	0x000000FF
#define SOUND_LOCK_COLOR 	0x00FF0000

#define SOUND_VERSION   				0x0011
//----------------------------------------------------
#define SOUND_CHUNK_VERSION				0x0211
#define SOUND_CHUNK_PARAMS				0x0212
#define SOUND_CHUNK_PARAMS2				0x0213
#define SOUND_CHUNK_SOURCE_PARAMS		0x0214
//----------------------------------------------------
ESound::ESound(LPVOID data, LPCSTR name)
	:CCustomObject(data,name)
{
	Construct(data);
}

void ESound::Construct(LPVOID data)
{
	ClassID					= OBJCLASS_SOUND;

	m_WAVName				= "";
    m_Params.volume 		= 1.f;
    m_Params.freq			= 1.f;
    m_Params.min_distance   = 1.f;
    m_Params.max_distance   = 300.f;
}

ESound::~ESound()
{
	m_Source.destroy		();
}

//----------------------------------------------------

bool ESound::GetBox( Fbox& box )
{
	box.set( PPosition, PPosition );
	box.min.sub(m_Params.max_distance);
	box.max.add(m_Params.max_distance);
	return true;
}

void ESound::Render(int priority, bool strictB2F)
{
    if((1==priority)&&(false==strictB2F)){
        u32 clr0=Locked()?SOUND_LOCK_COLOR:(Selected()?SOUND_SEL0_COLOR:SOUND_NORM_COLOR);
        u32 clr1=Locked()?SOUND_LOCK_COLOR:(Selected()?SOUND_SEL1_COLOR:SOUND_NORM_COLOR);
        if (Selected()){ 
        	DU::DrawLineSphere( PPosition, m_Params.max_distance, clr1, true );
        	DU::DrawLineSphere( PPosition, m_Params.min_distance, clr0, false );
        }else{
			DU::DrawSound(PPosition,VIS_RADIUS, clr1);
        }
    }
}

bool ESound::FrustumPick(const CFrustum& frustum)
{
    return (frustum.testSphere_dirty(PPosition,VIS_RADIUS))?true:false;
}

bool ESound::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
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

bool ESound::Load(IReader& F)
{
	u32 version = 0;

    R_ASSERT(F.r_chunk(SOUND_CHUNK_VERSION,&version));
    if(version!=SOUND_VERSION){
        ELog.DlgMsg( mtError, "ESound: Unsupported version.");
        return false;
    }

	inherited::Load(F);

    string1024		buf;
    R_ASSERT(F.find_chunk(SOUND_CHUNK_PARAMS2));
    F.r_stringZ		(buf); m_WAVName=buf;

    if (F.find_chunk(SOUND_CHUNK_SOURCE_PARAMS))
    	F.r			(&m_Params,sizeof(m_Params));
    
    ResetSource		();

    UpdateTransform	();

    return true;
}

void ESound::Save(IWriter& F)
{
	inherited::Save(F);

	F.open_chunk	(SOUND_CHUNK_VERSION);
	F.w_u16			(SOUND_VERSION);
	F.close_chunk	();

	F.open_chunk	(SOUND_CHUNK_PARAMS2);
	F.w_stringZ		(m_WAVName.c_str());
	F.close_chunk	();

	F.w_chunk		(SOUND_CHUNK_SOURCE_PARAMS,&m_Params,sizeof(m_Params));
}

//----------------------------------------------------

void __fastcall	ESound::OnChangeWAV	(PropValue* prop)
{
//.	BOOL bPlay 		= !!m_Source.feedback;
	ResetSource		();
//.	if (bPlay) 		Play(TRUE);
}

void __fastcall	ESound::OnChangeSource	(PropValue* prop)
{
	m_Source.set_params			(&m_Params);
}

void ESound::FillProp(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp(pref,values);
    PropValue* V;
    V=PHelper.CreateALibSound	(values,PHelper.PrepareKey(pref,"WAVE name"),	&m_WAVName);
    V->SetEvents				(0,0,OnChangeWAV);
	V=PHelper.CreateFloat		(values,PHelper.PrepareKey(pref,"Min dist"),	&m_Params.min_distance,	0.1f,1000.f,0.1f,1);
    V->SetEvents				(0,0,OnChangeSource);
	V=PHelper.CreateFloat		(values,PHelper.PrepareKey(pref,"Max dist"),	&m_Params.max_distance,	0.1f,1000.f,0.1f,1);
    V->SetEvents				(0,0,OnChangeSource);
	V=PHelper.CreateFloat		(values,PHelper.PrepareKey(pref,"Frequency"),	&m_Params.freq,			0.0f,2.f);
    V->SetEvents				(0,0,OnChangeSource);
	V=PHelper.CreateFloat		(values,PHelper.PrepareKey(pref,"Volume"),		&m_Params.volume,		0.0f,2.f);
    V->SetEvents				(0,0,OnChangeSource);
}
//----------------------------------------------------

bool ESound::GetSummaryInfo(SSceneSummary* inf)
{
	if (!m_WAVName.IsEmpty()) inf->waves.insert(m_WAVName);
    inf->sound_source_cnt++;
	return true;
}

void ESound::OnFrame()
{
	inherited::OnFrame();
    m_Source.set_position(PPosition);
}

void ESound::ResetSource()
{
	m_Source.destroy();
	m_Source.create	(1,m_WAVName.c_str());
	m_Source.set_params(&m_Params);
}

void ESound::SetSourceWAV(LPCSTR fname)
{
    m_WAVName		= fname;
    ResetSource		();
}

void ESound::Play(BOOL bLoop)
{
	m_Source.play		(0,bLoop);
	m_Source.set_params	(&m_Params);
}

void ESound::Stop()
{
	m_Source.stop();
}

