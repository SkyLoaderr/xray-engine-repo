#pragma once

#include "GameTaskDefs.h"
#include "object_interfaces.h"

class CGameTaskWrapper;
class CGameTask;
class CMapLocation;
class SGameTaskObjective;

class CGameTaskManager
{
	CGameTaskWrapper*		m_gametasks;
	enum		{eChanged	= (1<<0),};
	Flags8					m_flags;
protected:
	void					UpdateActiveTask				();
public:

							CGameTaskManager				();
							~CGameTaskManager				();

	void					initialize						(u16 id);
	GameTasks&				GameTasks						();
	CGameTask*				HasGameTask						(const TASK_ID& id);
	CGameTask*				GiveGameTaskToActor				(const TASK_ID& id, bool bCheckExisting=true);
	CGameTask*				GiveGameTaskToActor				(CGameTask* t, bool bCheckExisting=true);
	void					SetTaskState					(const TASK_ID& id, int objective_num, ETaskState state);
	void					SetTaskState					(CGameTask* t, int objective_num, ETaskState state);

	void					UpdateTasks						();
	void					RemoveUserTask					(CMapLocation* ml);

	SGameTaskObjective*		ActiveTask						();
};