#pragma once
#include "../../biting/ai_biting.h"

class CStateManagerSnork;

class CSnork :	public CAI_Biting {

	typedef		CAI_Biting		inherited;

	CStateManagerSnork		*StateMan;

public:
					CSnork				();
	virtual			~CSnork				();	

	virtual void	Load				(LPCSTR section);
	virtual bool	UpdateStateManager	();
};
