#pragma once
#include "../BaseMonster/base_monster.h"
#include "../jump_ability.h"

class CSnork :	public CBaseMonster, public CJumpingAbility {
	typedef		CBaseMonster		inherited;

public:
					CSnork				();
	virtual			~CSnork				();	

	virtual void	Load				(LPCSTR section);
	virtual void	reinit				();				
	virtual void	UpdateCL			();
	virtual void	CheckSpecParams		(u32 spec_params);
			void	test				();

};
