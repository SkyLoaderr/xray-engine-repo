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
	CGameTask*				GiveGameTaskToActor				(const TASK_ID& id, bool bCheckExisting=true);
	void					SetTaskState					(const TASK_ID& id, int objective_num, ETaskState state);

	void					ShowSpotOnMap					(CGameTask* t, int objective_id, bool bShow);
	void					ShowSpotPointerOnMap			(CGameTask* t, int objective_id, bool bShow);
};