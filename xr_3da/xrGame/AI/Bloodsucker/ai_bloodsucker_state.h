#pragma once

#include "..\\ai_monster_state.h"

class CAI_Bloodsucker;

class CBloodsuckerRest : public IState {

	CAI_Bloodsucker	*pMonster;

	enum {
		ACTION_RUN
	} m_tAction;

public:
					CBloodsuckerRest(CAI_Bloodsucker *p);

	virtual void	Reset			();
	virtual	bool	CheckCompletion	();

private:
	virtual	void Init();
	virtual	void Run();
};