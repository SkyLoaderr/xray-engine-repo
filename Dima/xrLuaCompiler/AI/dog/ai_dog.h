#pragma once

#include "..\\biting\\ai_biting.h"

class CAI_Dog : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
					CAI_Dog				();
	virtual			~CAI_Dog			();	

	virtual	BOOL	net_Spawn			(LPVOID DC);
	
	virtual void	Init				();
	virtual void	StateSelector		();
	virtual void	CheckSpecParams		(u32 spec_params);
	
};
