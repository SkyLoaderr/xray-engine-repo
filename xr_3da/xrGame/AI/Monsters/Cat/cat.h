#pragma once
#include "../BaseMonster/base_monster.h"
#include "../jump_ability.h"

class CCat : public CBaseMonster, public CJumpingAbility {
	typedef		CBaseMonster	inherited;
public:
					CCat				();
	virtual			~CCat				();	

	virtual void	Load				(LPCSTR section);
	virtual void	reinit				();
	
	virtual void	CheckSpecParams		(u32 spec_params);

			void	try_to_jump			();
};


