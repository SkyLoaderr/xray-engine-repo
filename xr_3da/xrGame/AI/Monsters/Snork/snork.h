#pragma once
#include "../../monsters/BaseMonster/base_monster.h"

class CStateManagerSnork;

class CSnork :	public CBaseMonster {

	typedef		CBaseMonster		inherited;

public:
					CSnork				();
	virtual			~CSnork				();	

	virtual void	Load				(LPCSTR section);
};
