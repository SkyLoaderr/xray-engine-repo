#ifndef _INCDEF_PSVisual_H_
#define _INCDEF_PSVisual_H_

#include "FBasicVisual.h"

#define MAX_PARTICLES	1024

class ENGINE_API CPSVisual: public FBasicVisual{
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
	virtual void		Render			(float LOD);
	virtual void		Copy			(FBasicVisual* pFrom);

	// Functionality
	void				Update			(DWORD dt);
    void				Compile			(LPCSTR name, PS::SEmitter* E);
	IC int				ParticleCount	(){return m_Particles.size();}
	IC void				Stop			(){m_Particles.clear();}
};
#endif /*_INCDEF_PSVisual_H_*/

