#pragma once
#include "../../monsters/BaseMonster/base_monster.h"

class CStateManagerChimera;

class CChimera : public CBaseMonster {

	typedef		CBaseMonster	inherited;

	bool		b_upper_state;

	CStateManagerChimera	*StateMan;
public:
					CChimera			();
	virtual			~CChimera			();	

	virtual void	Load				(LPCSTR section);

			void	init				();
	virtual void	reinit				();
	virtual void	StateSelector		();

	virtual	void	ProcessTurn			();

	virtual bool	UpdateStateManager	();
	virtual void	CheckSpecParams		(u32 spec_params);

			void	SetUpperState		(bool state = true) {b_upper_state = state;}

};


