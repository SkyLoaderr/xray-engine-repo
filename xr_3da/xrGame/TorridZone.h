#pragma once
#include "mosquitobald.h"

class CObjectAnimator;

class CTorridZone :public CMosquitoBald
{
private:
	typedef	CCustomZone	inherited;
	CObjectAnimator		*m_animator;
public:
						CTorridZone			();
	virtual				~CTorridZone		();
	virtual void		UpdateCL			();
	virtual void		shedule_Update		(u32 dt);
	BOOL				net_Spawn			(CSE_Abstract* DC);


	virtual bool		IsVisibleForZones() { return true;		}
};