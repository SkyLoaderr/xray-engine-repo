#pragma once

#include "..\\biting\\ai_biting.h"

class CAI_Dog : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Dog			();
	virtual					~CAI_Dog		();	

	virtual void	Init					();
	virtual void	StateSelector			();

	virtual	void	MotionToAnim			(EMotionAnim motion, int &index1, int &index2, int &index3);
	virtual	void	LoadAttackAnim			();
};
