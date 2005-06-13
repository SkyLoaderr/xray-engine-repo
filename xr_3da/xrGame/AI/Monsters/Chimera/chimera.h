#pragma once
#include "../BaseMonster/base_monster.h"
#include "../../../script_export_space.h"
#include "../control_jump.h"

class CChimera : public CBaseMonster {
	typedef		CBaseMonster	inherited;

	bool		b_upper_state;

	
	SVelocityParam		m_fsVelocityWalkUpper;
	SVelocityParam		m_fsVelocityJumpOne;
	SVelocityParam		m_fsVelocityJumpTwo;
	SVelocityParam		m_fsVelocityRunAttack;

	CControlJump		m_jump;
	SControlJumpData	m_jump_data;

public:
					CChimera			();
	virtual			~CChimera			();	

	virtual void	Load				(LPCSTR section);
	virtual void	reinit				();
	virtual	void	UpdateCL			();

	virtual	void	SetTurnAnimation			(bool turn_left);
	virtual void	CheckSpecParams				(u32 spec_params);
	virtual	EAction	CustomVelocityIndex2Action	(u32 velocity_index);
	virtual	void	TranslateActionToPathParams ();
	virtual bool	ability_run_attack			() {return true;}
	virtual bool	ability_jump_over_physics	() {return true;}
	virtual void	jump_over_physics			(const Fvector &target);
			
			void	try_to_jump					();
	virtual void	HitEntityInJump				(const CEntity *pEntity);

	IC		void	SetUpperState				(bool state = true) {b_upper_state = state;}
	
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CChimera)
#undef script_type_list
#define script_type_list save_type_list(CChimera)
