#pragma once

#include "..\\biting\\ai_biting.h"
#include "ai_bloodsucker_state.h"
#include "ai_bloodsucker_bones.h"
#include "ai_bloodsucker_misc.h"

class CAI_Bloodsucker : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Bloodsucker	();
	virtual					~CAI_Bloodsucker();	

	virtual void			Init					();

	virtual void			UpdateCL				();
	virtual void			StateSelector			();


	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual	void			Load					(LPCSTR section);
			void			vfAssignBones			();

	static	void __stdcall	BoneCallback			(CBoneInstance *B);

	//virtual	void			CheckTransitionAnims	();

			CBoneInstance*	GetBone					(LPCTSTR bone_name);
			CBoneInstance*	GetBone					(int bone_id);

			void			LookDirection			(Fvector to_dir, float bone_turn_speed);
			void			LookPosition			(Fvector to_point, float bone_turn_speed);

			void			ActivateEffector		(float life_time);
	// Flesh-specific FSM
	CBloodsuckerHearDNE		*stateHearDNE;
	CBloodsuckerHearNDE		*stateHearNDE;
	CBloodsuckerPanic		*statePanic;

	friend	class			CBitingPanic;
	friend	class			CBloodsuckerHearDNE;
	friend	class			CBloodsuckerHearNDE;
	friend	class			CBloodsuckerPanic;

	
	bonesManipulation		Bones;

	CBloodsuckerVisibility	m_tVisibility;
	
	float					m_fInvisibilityDist;			// дистанция до врага, на которой монстр может становиться нивидимым
	float					m_ftrPowerDown;					// коэффициент, на который падает сила монстра при активации невидимости 
	float					m_fPowerThreshold;				// порог силы, после которого нельзя стать невидимым

	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance> m_tSelectorHearSnd;



};


