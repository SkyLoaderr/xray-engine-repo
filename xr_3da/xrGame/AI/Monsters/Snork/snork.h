#pragma once
#include "../BaseMonster/base_monster.h"
#include "../jump_ability.h"

FASTDELEGATEDECLARE(CSnork);

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
			void	test2				();



	DEFINE_DELEGATE	on_test_1			(IEventData *data);
	DEFINE_DELEGATE	on_test_2			(IEventData *data);
	DEFINE_DELEGATE	on_test_3			(IEventData *data);

};
