#ifndef _INCDEF_PSObject_H_
#define _INCDEF_PSObject_H_

#include "PS_instance.h"
#include "ParticleSystem.h"

class ENGINE_API CPSObject	: 
	public CPS_Instance
{
public:
	PS::SEmitter		m_Emitter;
public:
						CPSObject			(LPCSTR ps_name, IRender_Sector* S=0, BOOL bAutoRemove=true);
	virtual				~CPSObject			();

	virtual float		shedule_Scale		()	{ return Device.vCameraPosition.distance_to(m_Emitter.m_Position)/200.f; }
	virtual void		shedule_Update		(u32 dt);
	virtual void		renderable_Render	();
	void				play_at_pos			(const Fvector& pos);
	void				Stop				();
};

#endif /*_INCDEF_PSObject_H_*/

