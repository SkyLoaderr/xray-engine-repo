#pragma once

#include "..\\biting\\ai_biting.h"
#include "ai_bloodsucker_state.h"
#include "ai_bloodsucker_bones.h"


class CAI_Bloodsucker : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Bloodsucker	();
	virtual					~CAI_Bloodsucker();	

	virtual void	Init					();

	virtual void	Think					();
	virtual void	UpdateCL				();
	virtual BOOL	net_Spawn				(LPVOID DC);

			void	vfAssignBones			();

	static	void __stdcall	BoneCallback(CBoneInstance *B);

	virtual	void	MotionToAnim			(EMotionAnim motion, int &index1, int &index2, int &index3);

	// Flesh-specific FSM
	CBloodsuckerRest	*stateRest;
	CBloodsuckerEat		*stateEat;

	friend	class		CBloodsuckerRest;
	friend	class		CBloodsuckerEat;

	bonesManipulation	Bones;

	bool				flagVisibility;					// true - если видим
	TTime				timeStartVisChanged;			// время начала изменение видимости монстра
	TTime				timeInvisible;					// время нахождения в состоянии невидимости
	
};


