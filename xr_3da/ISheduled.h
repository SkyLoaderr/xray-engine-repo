#pragma once

class	ENGINE_API	ISheduled
{
public:
	struct {
		u32		t_min		:	14;		// minimal bound of update time (sample: 20ms)
		u32		t_max		:	14;		// maximal bound of update time (sample: 200ms)
		u32		b_RT		:	1;
		u32		b_locked	:	1;
	}	shedule;

	ISheduled			();
	virtual ~ISheduled	();

	void								shedule_Register	();
	void								shedule_Unregister	();

	virtual float						shedule_Scale		()			= 0;
	virtual void						shedule_Update		(u32 dt)	= 0;
};
