#pragma once
#include "../state_manager_second.h"

class CAI_Dog;

class CStateManagerDog : public CStateManagerSecond<CAI_Dog> {
	typedef CStateManagerSecond<CAI_Dog> inherited;

public:

					CStateManagerDog	(CAI_Dog *monster); 
	virtual void	execute				();
};
