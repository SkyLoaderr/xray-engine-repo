#pragma once
#include "../state_manager_second.h"

class CTushkano;

class CStateManagerTushkano : public CStateManagerSecond<CTushkano> {
	typedef CStateManagerSecond<CTushkano> inherited;

public:
						CStateManagerTushkano	(CTushkano *obj);
	virtual				~CStateManagerTushkano	();

	virtual void		initialize				();
	virtual	void		execute					();
};
