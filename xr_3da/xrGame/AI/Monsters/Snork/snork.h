#pragma once
#include "../BaseMonster/base_monster.h"

class CSnorkJump;

class CSnork :	public CBaseMonster {
	typedef		CBaseMonster		inherited;

	enum EMovementParametersSnork {
		eVelocityParameterJumpOne		= eVelocityParameterCustom << 1,
		eVelocityParameterJumpTwo		= eVelocityParameterCustom << 2,

		eVelocityParamsJump				= eVelocityParameterJumpOne | eVelocityParameterJumpTwo,
	};

	SVelocityParam	m_fsVelocityJumpOne;
	SVelocityParam	m_fsVelocityJumpTwo;

	CSnorkJump		*Jump;

public:
					CSnork				();
	virtual			~CSnork				();	

	virtual void	Load				(LPCSTR section);
	virtual void	reinit				();				
	virtual void	UpdateCL			();
	virtual void	CheckSpecParams		(u32 spec_params);
			
			void	try_to_jump			();

	virtual void	HitEntityInJump		(const CEntity *pEntity);
			
			bool	find_geometry		(Fvector &dir);
			float	trace				(const Fvector &dir);

			bool	trace_geometry		(const Fvector &d, float &range);
};
