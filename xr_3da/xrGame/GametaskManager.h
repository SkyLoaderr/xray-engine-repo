#pragma once

#include "GameTaskDefs.h"
#include "object_interfaces.h"

class CGameTaskWrapper;
class CGameTask;

class CGameTaskManager
{
	CGameTaskWrapper*		m_gametasks;
public:

							CGameTaskManager				();
							~CGameTaskManager				();

	void					initialize						(u16 id);
	GameTasks&				GameTasks						();
	CGameTask*				HasGameTask						(const TASK_ID& id);
	void					GiveGameTaskToActor				(const TASK_ID& id);

	void					HighlightOnMap					(CGameTask* t, int objective_id, bool bHighlight);
};