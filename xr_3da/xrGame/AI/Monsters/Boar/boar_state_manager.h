#pragma once
#include "../state_manager_first.h"

class CAI_Boar;

class CStateManagerBoar : public CStateManagerFirst {
	typedef CStateManagerFirst inherited;
	CAI_Boar *m_object;
public:

					CStateManagerBoar	(CAI_Boar *monster); 

	virtual void	update				();
};
