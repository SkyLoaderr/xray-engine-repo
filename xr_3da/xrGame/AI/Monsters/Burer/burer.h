#pragma once
#include "../../biting/ai_biting.h"

class CBurer : public CAI_Biting {
	typedef		CAI_Biting	inherited;

public:
					CBurer				();
	virtual			~CBurer				();	

	virtual void	Load				(LPCSTR section);

			void	Init				();
	virtual void	StateSelector		();
};

