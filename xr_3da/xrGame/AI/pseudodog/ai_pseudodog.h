#pragma once

#include "..\\biting\\ai_biting.h"

class CAI_PseudoDog : public CAI_Biting {
	typedef		CAI_Biting	inherited;

public:
					CAI_PseudoDog		();
	virtual			~CAI_PseudoDog		();	

	virtual void	Load				(LPCSTR section);

	virtual void	Init				();
	virtual void	StateSelector		();
};
