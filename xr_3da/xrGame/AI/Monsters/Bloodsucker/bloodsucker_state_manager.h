#pragma once
#include "../state_manager_first.h"

class CAI_Bloodsucker;

class CStateManagerBloodsucker : public CStateManagerFirst {
	typedef CStateManagerFirst inherited;
	CAI_Bloodsucker *m_object;
public:

					CStateManagerBloodsucker	(CAI_Bloodsucker *monster); 

	virtual void	update						();
};
