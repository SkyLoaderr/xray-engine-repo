#ifndef _INCDEF_PSVisual_H_
#define _INCDEF_PSVisual_H_

#include "FBasicVisual.h"
#include "PSRuntime.h"

#define MAX_PARTICLES	1024

class ENGINE_API CPSVisual: public CVisual
{
protected:
	PS::Particles	  	m_Particles;
	PS::SParams*		m_Definition;
public:
    PS::SEmitter*		m_Emitter;
public:
						CPSVisual		();
	virtual				~CPSVisual		();

	// Visual
	u32				RenderTO		(FVF::TL* V);
	virtual void		Render			(float LOD);
	virtual void		Copy			(CVisual* pFrom);

	// Functionality
	void				Update			(u32 dt);
	void				Compile			(PS::SDef_RT* source, PS::SEmitter* E);
	IC int				ParticleCount	()	{ return m_Particles.size();}
	IC void				Stop			()	{ m_Particles.clear();      }
};
#endif /*_INCDEF_PSVisual_H_*/

