//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "d3dutils.h"
#include "ParticleGroup.h"
#include "engine/particles/papi.h"
#include "engine/particles/general.h"
#include "TLSprite.h"

CParticleGroup::CParticleGroup()
{
	m_HandleGroup 		= pGenParticleGroups(1, 100);
    m_HandleActionList	= pGenActionLists();
    m_Shader			= 0;
    m_ShaderName[0]		= 0;
    m_TextureName[0]	= 0;
}
CParticleGroup::~CParticleGroup()
{
	Device.Shader.Delete(m_Shader);
	pDeleteParticleGroups(m_HandleGroup);
	pDeleteActionLists	(m_HandleActionList);
}

void CParticleGroup::pSprite(LPCSTR sh_name, LPCSTR tex_name)
{
	Device.Shader.Delete(m_Shader);
    strcpy				(m_ShaderName,sh_name);
    strcpy				(m_TextureName,tex_name);
    m_Shader 			= Device.Shader.Create(sh_name,tex_name);
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
	

	Device.SetShader	(m_Shader);
    Device.SetTransform	(D3DTS_WORLD,Fidentity);
    CTLSprite m_Sprite;
    for(int i = 0; i < pg->p_count; i++)
    {
        Particle &m = pg->list[i];

        Fvector p;
        Fcolor c;
        p.set(m.pos.x,m.pos.y,m.pos.z);
        c.set(m.color.x,m.color.y,m.color.z,m.alpha);
//		m_Sprite.Render(p,m.size.x,false,c.get());
		m_Sprite.Render(p,c.get(),m.size.x,m.rot.x);
//		DU::DrawCross(p,m.size.x,m.size.y,m.size.z,m.size.x,m.size.y,m.size.z,c.get(),false);
    }
}

