//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ESound_Environment.h"
#include "EShape.h"
//----------------------------------------------------

#define SOUND_SEL0_COLOR 	0x00A0A0A0
#define SOUND_SEL1_COLOR 	0x00FFFFFF
#define SOUND_NORM_COLOR 	0x000000FF
#define SOUND_LOCK_COLOR 	0x00FF0000
//----------------------------------------------------

#define SOUND_CHUNK_ENV_SHAPE			0x1001
#define SOUND_CHUNK_ENV_REFS			0x1002
//----------------------------------------------------

ESoundEnvironment::ESoundEnvironment()
{
    m_Shape					= xr_new<CEditShape>((LPVOID)NULL,"internal");
	m_Shape->SetDrawColor	(0x205050FF, 0xFF202020);
    m_EnvRef				= "";
}

ESoundEnvironment::~ESoundEnvironment()
{
    xr_delete				(m_Shape);
}

//----------------------------------------------------

void ESoundEnvironment::InitDefault()
{
	m_Shape->add_box		(Fidentity);
}

bool ESoundEnvironment::GetBox( Fbox& box )
{
	return m_Shape->GetBox(box);
}

void ESoundEnvironment::Render(int priority, bool strictB2F, bool bLocked, bool bSelected)
{
	m_Shape->m_bSelected 	= bSelected;
    m_Shape->m_bLocked		= bLocked;
    m_Shape->Render	(priority,strictB2F);
}

bool ESoundEnvironment::FrustumPick(const CFrustum& frustum)
{
    return m_Shape->FrustumPick(frustum);
}

bool ESoundEnvironment::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
    return m_Shape->RayPick(distance, start, direction, pinf);
}
//----------------------------------------------------

bool ESoundEnvironment::Load(IReader& F)
{
    if (F.find_chunk(SOUND_CHUNK_ENV_SHAPE))
        m_Shape->Load(F);
    if (F.find_chunk(SOUND_CHUNK_ENV_REFS))
        F.r_stringZ	(m_EnvRef);

    return true;
}

void ESoundEnvironment::Save(IWriter& F)
{
    F.open_chunk	(SOUND_CHUNK_ENV_SHAPE);
    m_Shape->Save	(F);
    F.close_chunk	();
    F.open_chunk	(SOUND_CHUNK_ENV_REFS);
    F.w_stringZ		(m_EnvRef.c_str());
    F.close_chunk	();
}

//----------------------------------------------------

void ESoundEnvironment::FillProp(LPCSTR pref, PropItemVec& values)
{
    PHelper.CreateASoundEnv		(values, PHelper.PrepareKey(pref,"Evironment"),	&m_EnvRef);
}
//----------------------------------------------------

bool ESoundEnvironment::GetSummaryInfo(SSceneSummary* inf)
{
	return true;
}

void ESoundEnvironment::OnFrame()
{
	m_Shape->OnFrame();
}

void ESoundEnvironment::Scale(Fvector& amount)
{
	m_Shape->Scale(amount);
}

bool ESoundEnvironment::ExportGame(SExportStreams& F)
{
	SExportStreamItem& I	= F.sound_env_geom;
    
	I.stream.open_chunk		(I.chunk++);
    I.stream.w_stringZ		(m_WAVName.c_str());
    I.stream.w_fvector3		(m_Params.position);
    I.stream.w_float		(m_Params.volume);
    I.stream.w_float		(m_Params.freq);
    I.stream.w_float		(m_Params.min_distance);
    I.stream.w_float		(m_Params.max_distance);
    I.stream.close_chunk	();
    return true;
}
}

