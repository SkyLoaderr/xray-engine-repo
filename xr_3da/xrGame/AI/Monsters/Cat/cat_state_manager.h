#pragma once
#include "../state_manager_second.h"

class CCat;

class CStateManagerCat : public CStateManagerSecond<CCat> {

	typedef CStateManagerSecond<CCat> inherited;

	u32					m_rot_jump_last_time;

public:
						CStateManagerCat	(CCat *obj);	
	virtual				~CStateManagerCat	();

	virtual	void		execute				();
};
