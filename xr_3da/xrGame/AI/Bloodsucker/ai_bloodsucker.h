#pragma once

#include "..\\biting\\ai_biting.h"
#include "ai_bloodsucker_state.h"

class CAI_Bloodsucker : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Bloodsucker	();
	virtual					~CAI_Bloodsucker();	

	virtual void	Init					();

	virtual void	Think					();
	virtual void	UpdateCL				();

	virtual	void	MotionToAnim			(EMotionAnim motion, int &index1, int &index2, int &index3);

	// Flesh-specific FSM
	CBloodsuckerRest	*stateRest;
	friend	class		CBloodsuckerRest;

};

