#pragma once
#include "../../biting/ai_biting.h"

class CStateManagerFracture;

class CFracture : public CAI_Biting {
	typedef		CAI_Biting		inherited;
	
	CStateManagerFracture	*StateMan;

public:
					CFracture 			();
	virtual			~CFracture 			();	

	virtual void	Load				(LPCSTR section);
	virtual bool	UpdateStateManager	();
	virtual void	CheckSpecParams		(u32 spec_params);
};

