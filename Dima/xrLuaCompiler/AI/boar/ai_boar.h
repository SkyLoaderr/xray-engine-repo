#pragma once

#include "..\\biting\\ai_biting.h"

class CAI_Boar : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
					CAI_Boar			();
	virtual			~CAI_Boar			();	

	virtual	BOOL	net_Spawn			(LPVOID DC);

	virtual void	Init				();
	virtual void	StateSelector		();
	virtual void	CheckSpecParams		(u32 spec_params);

	virtual void	ProcessTurn			();
	
};
