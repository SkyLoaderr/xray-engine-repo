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
	int					m_iLifeTime;
	BOOL				m_bAutoRemove;
	string32			rm_event_desc;
	EVENT				rm_event;
public:
	CPS_Instance		();
	virtual				~CPS_Instance	();

	IC BOOL				PSI_alive		()						{	return m_iLifeTime>0;				}
	IC BOOL				PSI_IsAutomatic	()						{	return m_bAutoRemove;				}
	IC void				PSI_SetLifeTime	(float life_time)		{	m_iLifeTime=iFloor(life_time*1000);	}

	virtual void		shedule_Update	(u32 dt);
};

#endif
