#pragma once
#include "../../biting/ai_biting.h"

class CFracture : public CAI_Biting {
	typedef		CAI_Biting		inherited;

public:
					CFracture 			();
	virtual			~CFracture 			();	

	virtual void	Load				(LPCSTR section);
	virtual void	StateSelector		();
};

