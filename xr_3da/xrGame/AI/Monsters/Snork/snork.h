#pragma once
#include "../BaseMonster/base_monster.h"
#include "../../../script_export_space.h"
#include "../anim_triple.h"

class CSnorkJump;
class CControlJump;

class CSnork :	public CBaseMonster {
	typedef		CBaseMonster		inherited;

	SVelocityParam	m_fsVelocityJumpOne;
	SVelocityParam	m_fsVelocityJumpTwo;

	CControlJump	*m_jump;

	SAnimationTripleData anim_triple_jump;

public:
					CSnork				();
	virtual			~CSnork				();	

	virtual void	Load				(LPCSTR section);
	virtual void	reinit				();				
	virtual void	UpdateCL			();
	virtual void	CheckSpecParams		(u32 spec_params);
	virtual bool	jump				(CObject *enemy);

			void	try_to_jump			();

	virtual void	HitEntityInJump		(const CEntity *pEntity);
			
			bool	find_geometry		(Fvector &dir);
			float	trace				(const Fvector &dir);

			bool	trace_geometry		(const Fvector &d, float &range);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CSnork)
#undef script_type_list
#define script_type_list save_type_list(CSnork)
