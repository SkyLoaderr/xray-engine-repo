//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "d3dutils.h"
#include "ParticleGroup.h"
#include "engine/particles/papi.h"
#include "engine/particles/general.h"

CParticleGroup::CParticleGroup()
{
	m_HandleGroup 		= pGenParticleGroups(1, 100);
    m_HandleActionList	= pGenActionLists();
}
CParticleGroup::~CParticleGroup()
{
	pDeleteParticleGroups	(m_HandleGroup);
	pDeleteActionLists		(m_HandleActionList);
}

void CParticleGroup::OnFrame()
{
	pCurrentGroup		(m_HandleGroup);
	pCallActionList		(m_HandleActionList);
}

void CParticleGroup::Render()
{
	pCallActionList(m_HandleActionList);

	// Get a pointer to the particles in gp memory
	ParticleGroup *pg = _GetGroupPtr(m_HandleGroup);

	if(pg == NULL)
		return; // ERROR
	
	if(pg->p_count < 1)
		return;
	

	Device.SetShader(Device.m_SelectionShader);
    Device.SetTransform(D3DTS_WORLD,Fidentity);
    for(int i = 0; i < pg->p_count; i++)
    {
        Particle &m = pg->list[i];

        Fvector p;
        Fcolor c;
        p.set(m.pos.x,m.pos.y,m.pos.z);
        c.set(m.color.x,m.color.y,m.color.z,m.alpha);
		DU::DrawCross(p,m.size.x,m.size.y,m.size.z,m.size.x,m.size.y,m.size.z,c.get(),false);
    }
}

