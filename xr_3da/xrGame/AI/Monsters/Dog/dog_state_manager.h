#pragma once
#include "../state_manager_first.h"

class CAI_Dog;

class CStateManagerDog : public CStateManagerFirst {
	typedef CStateManagerFirst inherited;
	CAI_Dog *m_object;
public:

					CStateManagerDog	(CAI_Dog *monster); 
	virtual void	update				();
};
