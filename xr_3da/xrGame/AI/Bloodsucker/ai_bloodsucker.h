#pragma once

#include "../biting/ai_biting.h"
#include "../ai_monster_bones.h"
#include "../ai_monster_invisibility.h"

class CAI_Bloodsucker : public CAI_Biting, 
						public CMonsterInvisibility {

	typedef		CAI_Biting	inherited;

	
	static	void __stdcall	BoneCallback			(CBoneInstance *B);
			void			vfAssignBones			();
			void			LookDirection			(Fvector to_dir, float bone_turn_speed);


	bonesManipulation		Bones;

	CBoneInstance			*bone_spine;
	CBoneInstance			*bone_head;

	SMotionVel				invisible_vel;
	LPCSTR					invisible_particle_name;
	bool					visibility_steady;

public:
							CAI_Bloodsucker	();
	virtual					~CAI_Bloodsucker();	

			void			Init					();
	virtual void			UpdateCL				();
	virtual void			shedule_Update			(u32 dt);

	virtual void			StateSelector			();
	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual	void			Load					(LPCSTR section);
	virtual	void			LookPosition			(Fvector to_point, float angular_speed = PI_DIV_3);
	virtual	void			PitchCorrection			() {}
	virtual u8				get_legs_number			() {return BIPEDAL;}			

	virtual	void			CheckSpecParams			(u32 spec_params);
	virtual bool			ability_invisibility	() {return true;}

	float					m_fInvisibilityDist;			// дистанция до врага, на которой монстр может становиться нивидимым
	float					m_ftrPowerDown;					// коэффициент, на который падает сила монстра при активации невидимости 
	float					m_fPowerThreshold;				// порог силы, после которого нельзя стать невидимым
	float					m_fEffectDist;

			
	// PP Effector		
	
			void			LoadEffector			(LPCSTR section);	
			void			ActivateEffector		(float life_time);
			
			SPPInfo			pp_effector;


	virtual	void			set_visible				(bool val);
	virtual	void			ProcessTurn				();
	
};