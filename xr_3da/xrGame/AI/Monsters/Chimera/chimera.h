#pragma once
#include "../../monsters/BaseMonster/base_monster.h"

class CStateManagerChimera;

class CChimera : public CBaseMonster {

	typedef		CBaseMonster	inherited;

	bool		b_upper_state;

public:
					CChimera			();
	virtual			~CChimera			();	

	virtual void	Load				(LPCSTR section);

	virtual void	reinit				();

	virtual	void	ProcessTurn			();

	virtual void	CheckSpecParams		(u32 spec_params);

			void	SetUpperState		(bool state = true) {b_upper_state = state;}

};


