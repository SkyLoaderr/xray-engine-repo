#pragma once
#include "../state_manager_first.h"

class CPseudoGigant;

class CStateManagerGigant : public CStateManagerFirst {
	typedef CStateManagerFirst inherited;
	CPseudoGigant *m_object;
public:

					CStateManagerGigant	(CPseudoGigant *monster); 
	virtual void	update				();
};
