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

	virtual void			Think					();
	virtual void			UpdateCL				();
	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual	void			Load					(LPCSTR section);
			void			vfAssignBones			();

	static	void __stdcall	BoneCallback	(CBoneInstance *B);

	virtual	void			MotionToAnim			(EMotionAnim motion, int &index1, int &index2, int &index3);
	virtual	void			LoadAttackAnim			();
	virtual	void			CheckTransitionAnims	();

			CBoneInstance*	GetBone					(LPCTSTR bone_name);
			CBoneInstance*	GetBone					(int bone_id);

			void			LookDirection			(Fvector to_dir, float bone_turn_speed);
			void			LookPosition			(Fvector to_point, float bone_turn_speed);

			void			Postprocess				();
	// Flesh-specific FSM
	CBloodsuckerRest		*stateRest;
	CBloodsuckerEat			*stateEat;
	CBloodsuckerAttack		*stateAttack;
	CBloodsuckerHearDNE		*stateHearDNE;
	CBloodsuckerHearNDE		*stateHearNDE;
	CBloodsuckerPanic		*statePanic;

	friend	class			CBloodsuckerRest;
	friend	class			CBloodsuckerEat;
	friend	class			CBloodsuckerAttack;
	friend	class			CBitingPanic;
	friend	class			CBloodsuckerHearDNE;
	friend	class			CBloodsuckerHearNDE;
	friend	class			CBloodsuckerPanic;

	
	bonesManipulation		Bones;

	CBloodsuckerVisibility	m_tVisibility;

	bool					flagEatNow;						// true - сейчас монстр ест
	
	float					m_fInvisibilityDist;			// дистанция до врага, на которой монстр может становиться нивидимым
	float					m_ftrPowerDown;					// коэффициент, на который падает сила монстра при активации невидимости 
	float					m_fPowerThreshold;				// порог силы, после которого нельзя стать невидимым

	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance> m_tSelectorHearSnd;

	u32						m_dwDayTimeBegin;
	u32						m_dwDayTimeEnd;
	float					m_fMinSatiety;
	float					m_fMaxSatiety;

};


