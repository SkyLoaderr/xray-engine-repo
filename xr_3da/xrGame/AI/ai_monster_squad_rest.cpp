#include "stdafx.h"
#include "ai_monster_squad.h"
#include "../entity.h"

void CMonsterSquad::ProcessIdle()
{
	m_idle_entities.clear();

	// Выделить элементы с общими врагами и состянием атаки 
	for (MEMBER_GOAL_MAP_IT it_goal = m_goals.begin(); it_goal != m_goals.end(); it_goal++) {
		SMemberGoal goal = it_goal->second;
		
		if (goal.type == MG_Rest) {
			m_idle_entities.push_back(it_goal->first);
		}
	}

	Idle_AssignAction(m_idle_entities);
}


void CMonsterSquad::Idle_AssignAction(ENTITY_VEC &members)
{
	// Следовать за лидером... 	
	
	

}