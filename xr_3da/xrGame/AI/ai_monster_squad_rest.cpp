#include "stdafx.h"
#include "ai_monster_squad.h"
#include "../entity.h"

void CMonsterSquad::ProcessIdle()
{
	m_temp_entities.clear();

	// ¬ыделить элементы с общими врагами и сост€нием атаки 
	for (MEMBER_GOAL_MAP_IT it_goal = m_goals.begin(); it_goal != m_goals.end(); it_goal++) {
		SMemberGoal goal = it_goal->second;
		
		if ((goal.type == MG_Rest) || (goal.type == MG_WalkGraph)) {
			m_temp_entities.push_back(it_goal->first);
		}
	}

	Idle_AssignAction(m_temp_entities);
}


void CMonsterSquad::Idle_AssignAction(ENTITY_VEC &members)
{
	// получить цель лидера
	SMemberGoal goal;
	GetGoal(leader, goal);

	// пересчитать положение в команде в соответствие с целью лидера
	for (ENTITY_VEC_IT it = members.begin(); it != members.end(); it++) {
		if ((*it) == leader) continue;
			
		SSquadCommand command;
		command.position	= leader->Position();
		command.node		= leader->level_vertex_id();
		
		if (goal.type == MG_Rest) {
			command.type		= SC_REST;
			command.entity		= 0;
		} else if (goal.type == MG_WalkGraph){
			command.type		= SC_FOLLOW;
			command.entity		= leader;	
			command.direction	= leader->Direction();
		}

		UpdateCommand(*it, command);
	}
}

