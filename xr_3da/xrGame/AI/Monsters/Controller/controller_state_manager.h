#pragma once
#include "../state_manager_second.h"

class CController;

class CStateManagerController : public CStateManagerSecond<CController> {

	typedef CStateManagerSecond<CController> inherited;

public:
						CStateManagerController		(CController *obj);
	virtual				~CStateManagerController	();

	virtual	void		execute						();
};
