#pragma once
#include "../BaseMonster/base_monster.h"
#include "../jump_ability.h"

class CSnork :	public CBaseMonster, public CJumpingAbility {
	typedef		CBaseMonster		inherited;

	enum EMovementParametersSnork {
		eVelocityParameterJumpOne		= eVelocityParameterCustom << 1,
		eVelocityParameterJumpTwo		= eVelocityParameterCustom << 2,

		eVelocityParamsJump				= eVelocityParameterJumpOne | eVelocityParameterJumpTwo,
	};

	SVelocityParam	m_fsVelocityJumpOne;
	SVelocityParam	m_fsVelocityJumpTwo;

public:
					CSnork				();
	virtual			~CSnork				();	

	virtual void	Load				(LPCSTR section);
	virtual void	reinit				();				
	virtual void	UpdateCL			();
	virtual void	CheckSpecParams		(u32 spec_params);
			
			void	try_to_jump			();

	virtual void	HitEntityInJump		(const CEntity *pEntity);

};
