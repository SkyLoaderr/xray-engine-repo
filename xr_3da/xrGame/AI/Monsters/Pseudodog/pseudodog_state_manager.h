#pragma once
#include "../state_manager_first.h"

class CAI_PseudoDog;

class CStateManagerPseudodog : public CStateManagerFirst {
	typedef CStateManagerFirst inherited;
	CAI_PseudoDog *m_object;
public:

	CStateManagerPseudodog	(CAI_PseudoDog *monster); 
	virtual void	update				();
};
