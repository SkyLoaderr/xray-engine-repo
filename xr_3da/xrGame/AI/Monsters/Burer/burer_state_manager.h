#pragma once
#include "../state_manager_first.h"

class CBurer;

class CStateManagerBurer : public CStateManagerFirst {
	typedef CStateManagerFirst inherited;
	CBurer *m_object;
public:

					CStateManagerBurer	(CBurer *monster); 
	virtual void	update				();
};

