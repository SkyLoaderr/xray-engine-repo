#ifndef _CPS_Instance_H_
#define _CPS_Instance_H_

#include "ispatial.h"
#include "isheduled.h"
#include "irenderable.h"

class ENGINE_API CPS_Instance	:
	public ISpatial,
	public ISheduled,
	public IRenderable
{
protected:
	int						m_iLifeTime			;
	BOOL					m_bAutoRemove		;
	BOOL					m_bDead				;
protected:
	virtual					~CPS_Instance		();
public:
	CPS_Instance			();

	IC BOOL					PSI_alive			()						{	return m_iLifeTime>0;				}
	IC BOOL					PSI_IsAutomatic		()						{	return m_bAutoRemove;				}
	IC void					PSI_SetLifeTime		(float life_time)		{	m_iLifeTime=iFloor(life_time*1000);	}
	void					PSI_destroy			();
	void					PSI_internal_delete	();

	virtual void			Play				()				= 0;
	virtual BOOL			Locked				()				{ return FALSE; }

	virtual	shared_str		shedule_Name		() const		{ return shared_str("particle_instance"); };

	virtual void			shedule_Update		(u32 dt);
	virtual	IRenderable*	dcast_Renderable	()				{ return this;	}
};

#endif
