#pragma once
#include "../state_manager_second.h"

class CSnork;

class CStateManagerSnork : public CStateManagerSecond<CSnork> {
	typedef CStateManagerSecond<CSnork> inherited;

public:
						CStateManagerSnork		(CSnork *obj);
	virtual				~CStateManagerSnork		();

	virtual	void		execute					();
};
