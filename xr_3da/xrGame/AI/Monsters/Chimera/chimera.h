#pragma once
#include "../../biting/ai_biting.h"

class CChimera : public CAI_Biting {
	typedef		CAI_Biting	inherited;

public:
					CChimera			();
	virtual			~CChimera			();	

	virtual void	Load				(LPCSTR section);

			void	Init				();
	virtual void	StateSelector		();
};

