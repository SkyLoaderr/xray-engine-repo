#pragma once

#include "..\\biting\\ai_biting.h"
#include "ai_chimera_state.h"

class CAI_Chimera : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Chimera		();
	virtual					~CAI_Chimera	();	

	virtual void	Init();

	virtual void	Think				();
	virtual void	UpdateCL			();
	virtual BOOL	net_Spawn			(LPVOID DC);


	virtual	void	MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3);
	virtual void	FillAttackStructure(u32 i, TTime t);

	// Bone manipulatio
			void			vfAssignBones		(CInifile *ini, const char *section);
	static	void __stdcall	SpinCallback		(CBoneInstance *B);
			void			SpinBone			(CBoneInstance *B);

	// реализация плавного поворота	
	float	fSpinYaw;				// угол поворота для боны
	TTime	timeLastSpin;		// последнее время изменения SpinYaw
	float	fStartYaw, fFinishYaw;	// начальный и конечный углы поворота монстра
	float	fPrevMty;				// предыдущее значение target.yaw

	// Flesh-specific FSM
	CChimeraAttack		*stateAttack;

	CBitingRest			*stateRest;
	CBitingEat			*stateEat;
	CBitingHide			*stateHide;
	CBitingDetour		*stateDetour;
	CBitingPanic		*statePanic;
	CBitingExploreDNE	*stateExploreDNE;
	CBitingExploreDE	*stateExploreDE;
	CBitingExploreNDE	*stateExploreNDE;

	friend	class		CBitingRest;
	friend	class 		CBitingEat;
	friend	class 		CBitingHide;
	friend	class 		CBitingDetour;
	friend	class 		CBitingPanic;
	friend	class 		CBitingExploreDNE;
	friend	class 		CBitingExploreDE;
	friend	class 		CBitingExploreNDE;

	friend  class 		CChimeraAttack;
};
