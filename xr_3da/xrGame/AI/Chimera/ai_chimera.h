#pragma once

#include "..\\biting\\ai_biting.h"
#include "ai_chimera_state.h"

class CAI_Chimera : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Chimera			();
	virtual					~CAI_Chimera		();	

	virtual void			Init				();
 
	virtual void			Think				();
	virtual void			UpdateCL			();
	virtual BOOL			net_Spawn			(LPVOID DC);

	virtual	void			MotionToAnim		(EMotionAnim motion, int &index1, int &index2, int &index3);
	virtual	void			LoadAttackAnim		();

	// Bone manipulation
			void			vfAssignBones		(CInifile *ini, const char *section);
	static	void __stdcall	SpinCallback		(CBoneInstance *B);
			void			SpinBoneInMotion	(CBoneInstance *B);
			void			SpinBoneInAttack	(CBoneInstance *B);

	// ���������� �������� ��������	
	float	fSpinYaw;				// ���� �������� ��� ����
	TTime	timeLastSpin;			// ��������� ����� ��������� SpinYaw
	float	fStartYaw, fFinishYaw;	// ��������� � �������� ���� �������� �������
	float	fPrevMty;				// ���������� �������� target.yaw

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
