#pragma once
#include "../state_manager_first.h"

class CAI_Flesh;

class CStateManagerFlesh : public CStateManagerFirst {
	typedef CStateManagerFirst inherited;
	CAI_Flesh *m_object;
public:

					CStateManagerFlesh	(CAI_Flesh *monster); 
	virtual void	update				();
};
