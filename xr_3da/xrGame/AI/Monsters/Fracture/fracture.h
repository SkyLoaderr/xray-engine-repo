#pragma once
#include "../../monsters/BaseMonster/base_monster.h"

class CStateManagerFracture;

class CFracture : public CBaseMonster {
	typedef		CBaseMonster		inherited;
	
	CStateManagerFracture	*StateMan;

public:
					CFracture 			();
	virtual			~CFracture 			();	

	virtual void	Load				(LPCSTR section);
	virtual bool	UpdateStateManager	();
	virtual void	CheckSpecParams		(u32 spec_params);
};

