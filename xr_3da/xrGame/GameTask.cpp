///////////////////////////////////////////////////////////////
// GameTask.cpp
// Класс игрового задания задания
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameTask.h"

#include "gameobject.h"
#include "script_engine.h"
#include "ai_space.h"

CGameTask::CGameTask		()
{
}

CGameTask::~CGameTask		()
{
}

void CGameTask::Load		(CUIXml& uiXml, XML_NODE* task_node)
{
	int tag_num = uiXml.GetNodesNum(task_node, "objective");
	
	m_Objectives.clear		();
	m_Objectives.reserve	(tag_num);

	SGameTaskObjective objective;
	for(int i=0; i<tag_num; i++)
	{
		LPCSTR tag_text = uiXml.Read(task_node, "objective:text", i, NULL);
		objective.description = tag_text;
		tag_text = uiXml.Read(task_node, "objective:script_state", i, NULL);
		objective.script_state = tag_text;
		m_Objectives.push_back(objective);
	}
}

ETaskState CGameTask::CalcObjectiveState  (u32 index, CGameObject* task_accomplisher)
{
	SGameTaskObjective& objective = m_Objectives[index];

	luabind::functor<ETaskState>	lua_function;
	bool functor_exists = ai().script_engine().functor(*objective.script_state ,lua_function);
	R_ASSERT3(functor_exists, "Cannot find task script state", *objective.script_state);
	objective.state = lua_function	(task_accomplisher->lua_game_object());
	return objective.state;
}


void CGameTask::CalcState (CGameObject* task_accomplisher)
{
	for(u32 i=0; i<ObjectivesNum(); i++)
		CalcObjectiveState(i, task_accomplisher);
}