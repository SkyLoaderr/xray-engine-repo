#pragma once
#include "../state_manager_second.h"

class CChimera;

class CStateManagerChimera : public CStateManagerSecond<CChimera> {
	
	typedef CStateManagerSecond<CChimera> inherited;

public:
						CStateManagerChimera	(CChimera *obj);
	virtual				~CStateManagerChimera	();

	virtual	void		execute					();
};
