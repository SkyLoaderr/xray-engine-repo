#pragma once
#include "../state_manager_second.h"

class CZombie;

class CStateManagerZombie : public CStateManagerSecond<CZombie> {
	typedef CStateManagerSecond<CZombie> inherited;

public:
						CStateManagerZombie		(CZombie *obj);
	virtual				~CStateManagerZombie	();

	virtual	void		execute					();
	
};
