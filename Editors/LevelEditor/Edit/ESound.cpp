// Sound.cpp: implementation of the ESound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "ESound.h"
#include "PropertiesListHelper.h"
#include "ESound_Static.h"
#include "ESound_Environment.h"


#define SOUND_VERSION   				0x0012
//----------------------------------------------------
#define SOUND_CHUNK_VERSION				0x0001
#define SOUND_CHUNK_TYPE				0x0002
//----------------------------------------------------

//----------------------------------------------------
ESound::ESound(LPVOID data, LPCSTR name)
	:CCustomObject(data,name)
{
	Construct(data);
}

void ESound::Construct(LPVOID data)
{
	ClassID						= OBJCLASS_SOUND;
    
    m_Type						= (ESoundType)data;
    m_Instance					= 0;
    
    switch(m_Type){
    case stStaticSource:    	m_Instance = xr_new<EStaticSound>(); 		m_Instance->InitDefault(); break;
    case stEnvironment:	    	m_Instance = xr_new<ESoundEnvironment>();   m_Instance->InitDefault(); break;
    }
}

ESound::~ESound()
{
    xr_delete				(m_Instance);
}
//----------------------------------------------------

bool ESound::Load(IReader& F)
{
	u32 version 	= 0;

    R_ASSERT(F.r_chunk(SOUND_CHUNK_VERSION,&version));
    if(version!=SOUND_VERSION){
        ELog.DlgMsg( mtError, "ESound: Unsupported version.");
        return false;
    }

	inherited::Load			(F);

    R_ASSERT(F.find_chunk(SOUND_CHUNK_TYPE));
	m_Type					= ESoundType(F.r_u32());

    R_ASSERT				(0==m_Instance);
    switch(m_Type){
    case stStaticSource:    m_Instance = xr_new<EStaticSound>(); 		break;
    case stEnvironment:	    m_Instance = xr_new<ESoundEnvironment>();   break;
    }

    m_Instance->Load		(F);

    UpdateTransform	();

    return true;
}

void ESound::Save(IWriter& F)
{
	inherited::Save			(F);

	F.open_chunk			(SOUND_CHUNK_VERSION);
	F.w_u16					(SOUND_VERSION);
	F.close_chunk			();

    F.w_chunk				(SOUND_CHUNK_TYPE,&m_Type,sizeof(m_Type));

    m_Instance->Save		(F);
}
//----------------------------------------------------

void ESound::FillProp(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp(pref,values);
	m_Instance->FillProp(pref, values);
}
//----------------------------------------------------


