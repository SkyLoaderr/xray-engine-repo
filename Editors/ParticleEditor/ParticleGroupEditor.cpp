//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleGroup.h"
#include "TLSprite.h"
#include "particles/general.h"

using namespace PAPI;
using namespace PS;
              
//------------------------------------------------------------------------------
// Render part
//------------------------------------------------------------------------------
void CParticleGroup::RenderEditor()
{
	if (m_Def){
        // Get a pointer to the particles in gp memory
        ParticleGroup *pg = _GetGroupPtr(m_HandleGroup);

        if(pg == NULL)		return; // ERROR
        if(pg->p_count < 1)	return;

        Device.SetShader	(m_Shader);
        RCache.set_xform_world(Fidentity);
        CTLSprite 			m_Sprite;
        for(int i = 0; i < pg->p_count; i++){
            Particle &m = pg->list[i];

            Fvector p;
            Fcolor c;
            p.set(m.pos.x,m.pos.y,m.pos.z);
            c.set(m.color.x,m.color.y,m.color.z,m.alpha);
            if (m_Def->m_Flags.is(CPGDef::flFramed)){
    //        	||m_Flags.test(flAnimated)){
                Fvector2 lt,rb;
                m_Def->m_Frame.CalculateTC(m.frame,lt,rb);
                m_Sprite.Render(p,c.get(),m.size.x,m.rot.x,lt,rb);
            }else
                m_Sprite.Render(p,c.get(),m.size.x,m.rot.x);
    //		DU::DrawCross(p,m.size.x,m.size.y,m.size.z,m.size.x,m.size.y,m.size.z,c.get(),false);
        }
    }
}
/*
//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
void CParticleGroup::Save(CFS_Base& F)
{
	F.open_chunk	(PG_CHUNK_VERSION);
	F.Wword			(PG_VERSION);
	F.close_chunk	();

	F.open_chunk	(PG_CHUNK_NAME);
	F.WstringZ		(m_Name);
	F.close_chunk	();
    
	ParticleGroup *pg = _GetGroupPtr(m_HandleGroup);
	F.open_chunk	(PG_CHUNK_GROUPDATA);
    F.Wdword		(pg->max_particles); // max particles
	F.close_chunk	();
    
	F.open_chunk	(PG_CHUNK_ACTIONLIST);
	BOOL bRes		= SaveActionList(F); R_ASSERT2(bRes,"Can't save action list.");
	F.close_chunk	();

	F.write_chunk	(PG_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));

    if (m_Flags.is(flSprite)){
        F.open_chunk	(PG_CHUNK_SPRITE);
        F.WstringZ		(m_ShaderName);
        F.WstringZ		(m_TextureName);
        F.close_chunk	();
    }

    if (m_Flags.is(flFramed)){
        F.open_chunk	(PG_CHUNK_FRAME);
        F.write			(&m_Animation,sizeof(SAnimation));
        F.close_chunk	();
    }

	F.open_chunk	(PG_CHUNK_SOURCETEXT);
    F.WstringZ		(m_SourceText.c_str());
	F.close_chunk	();
}

BOOL CParticleGroup::SaveActionList(CFS_Base& F)
{
	// get pointer to specified action list.
	PAPI::PAHeader *pa	= _GetListPtr(m_HandleActionList);

	if(pa == NULL)
		return FALSE; // ERROR

	int num_actions = pa->count-1;
	PAPI::PAHeader *action= pa+1;

	// save actions
	F.open_chunk	(AL_CHUNK_VERSION);
	F.Wword			(ACTION_LIST_VERSION);
	F.close_chunk	();

	F.open_chunk	(AL_CHUNK_ACTIONS);
	F.write			(action,num_actions*sizeof(PAPI::PAHeader));
	F.close_chunk	();

	return TRUE;
}

*/
