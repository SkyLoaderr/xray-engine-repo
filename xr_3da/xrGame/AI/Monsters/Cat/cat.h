#pragma once
#include "../BaseMonster/base_monster.h"

class CCat : public CBaseMonster {
	typedef		CBaseMonster	inherited;
public:
					CCat				();
	virtual			~CCat				();	

	virtual void	Load				(LPCSTR section);
	virtual void	CheckSpecParams		(u32 spec_params);
};


