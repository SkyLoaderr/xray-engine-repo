#pragma once
#include "../state_manager_second.h"

class CFracture;

class CStateManagerFracture : public CStateManagerSecond<CFracture> {
	typedef CStateManagerSecond<CFracture> inherited;

public:
						CStateManagerFracture	(CFracture *obj);
	virtual				~CStateManagerFracture	();

	virtual	void		execute					();
};
