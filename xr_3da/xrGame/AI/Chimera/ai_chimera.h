#pragma once

#include "..\\biting\\ai_biting.h"
#include "ai_chimera_state.h"

#include "..\\bloodsucker\\ai_bloodsucker_bones.h"

class CAI_Chimera : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Chimera			();
	virtual					~CAI_Chimera		();	

	virtual void			Init				();
	virtual void			StateSelector		();

	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual	void			Load				(LPCSTR section);

	// Bone manipulation
			void			vfAssignBones		(CInifile *ini, const char *section);
	static	void __stdcall	SpinCallback		(CBoneInstance *B);
			void			SpinBoneInMotion	(CBoneInstance *B);
			void			SpinBoneInAttack	(CBoneInstance *B);

			
	// реализация плавного поворота	
	float	fSpinYaw;				// угол поворота для боны
	TTime	timeLastSpin;			// последнее время изменения SpinYaw
	float	fStartYaw, fFinishYaw;	// начальный и конечный углы поворота монстра
	float	fPrevMty;				// предыдущее значение target.yaw

	bonesManipulation		Bones;


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
