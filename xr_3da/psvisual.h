#ifndef _INCDEF_PSVisual_H_
#define _INCDEF_PSVisual_H_

#include "FBasicVisual.h"
#include "ParticleSystem.h"

#define MAX_PARTICLES	1024

class ENGINE_API CPSVisual: public CVisual
{
protected:
	CVertexStream*		m_Stream;
	PS::Particles	  	m_Particles;
	PS::SParams*		m_Definition;
public:
    PS::SEmitter*		m_Emitter;
public:
						CPSVisual		();
	virtual				~CPSVisual		();

	// Visual
	DWORD				RenderTO		(FVF::TL* V);
	virtual void		Render			(float LOD);
	virtual void		Copy			(CVisual* pFrom);

	// Functionality
	void				Update			(DWORD dt);
	void				Compile			(PS::SDef* source, PS::SEmitter* E);
	IC int				ParticleCount	()	{ return m_Particles.size();}
	IC void				Stop			()	{ m_Particles.clear();      }
	IC CVertexStream*	GetStream		()	{ return m_Stream;			}
};
#endif /*_INCDEF_PSVisual_H_*/

