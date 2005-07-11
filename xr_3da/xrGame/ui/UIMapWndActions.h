#pragma once
#include "../action_planner.h"

class CUIMapWnd;

class CMapActionPlanner :public CActionPlanner<CUIMapWnd> {
private:
	typedef CActionPlanner<CUIMapWnd> inherited;

public:
					CMapActionPlanner	();
	virtual			~CMapActionPlanner	();
	virtual	void	setup				(CUIMapWnd *object);
			void	add_evaluators		();
			void	add_actions			();
	virtual	LPCSTR	object_name			() const;
};