//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffect.h"
#include "TLSprite.h"

using namespace PAPI;
using namespace PS;
              
//------------------------------------------------------------------------------
// Render part
//------------------------------------------------------------------------------
/*
void CParticleEffect::Copy(IVisual*)
{
	THROW;
}
void CParticleEffect::Render(float)
{
	if (m_Def){
        // Get a pointer to the particles in gp memory
        ParticleEffect *pe = _GetEffectPtr(m_HandleEffect);

        if(pe == NULL)		return; // ERROR
        if(pe->p_count < 1)	return;

        Device.SetShader	(m_Def->m_CachedShader);
        CTLSprite 			m_Sprite;
        for(int i = 0; i < pe->p_count; i++){
            Particle &m 	= pe->list[i];

            Fvector p;
            Fcolor c;
            p.set(m.pos.x,m.pos.y,m.pos.z);
            c.set(m.color.x,m.color.y,m.color.z,m.alpha);
            if (m_Def->m_Flags.is(CPEDef::dfFramed)){
                Fvector2 lt,rb;
                m_Def->m_Frame.CalculateTC(m.frame,lt,rb);
                m_Sprite.Render(p,c.get(),m.size.x,m.rot.x,lt,rb);
            }else
                m_Sprite.Render(p,c.get(),m.size.x,m.rot.x);
        }
    }
}
*/
