#pragma once
#include "../../biting/ai_biting.h"

class CStateManagerZombie;

class CZombie :	public CAI_Biting {
	
	typedef		CAI_Biting		inherited;

	CStateManagerZombie *StateMan;

public:
					CZombie		();
	virtual			~CZombie		();	

	virtual void	Load				(LPCSTR section);
	virtual bool	UpdateStateManager	();
};
