#include "stdafx.h"
#include "ai_monster_squad.h"
#include "../entity.h"
#include "../level.h"

void CMonsterSquad::RegisterMember(CEntity *pE)
{
	// ƒобавить цель
	SMemberGoal			G;
	G.type				= MG_None;
	m_goals.insert		(mk_pair(pE, G));
	
	// ƒобавить команду
	SSquadCommand		C;
	C.type				= SC_NONE;
	m_commands.insert	(mk_pair(pE, C));
	
	// установить лидера
	if (!leader) leader = pE;
}

void CMonsterSquad::RemoveMember(CEntity *pE)
{
	// удалить из целей
	MEMBER_GOAL_MAP_IT it_goal = m_goals.find(pE);
	if (it_goal == m_goals.end()) return;	
	m_goals.erase(it_goal);

	// удалить из команд
	MEMBER_COMMAND_MAP_IT it_command = m_commands.find(pE);
	if (it_command == m_commands.end()) return;	
	m_commands.erase(it_command);

	// если удал€емый елемент €вл€етс€ лидером - переназначить лидера
	if (leader == pE)  {
		if (m_goals.empty()) leader = 0;
		else leader = m_goals.begin()->first;
	}
}

bool CMonsterSquad::SquadActive()
{
	if (!leader) return false;

	// проверить количество живых объектов в группе
	u32 alive_num = 0;
	for (MEMBER_GOAL_MAP_IT it = m_goals.begin(); it != m_goals.end(); it++) 
		if (it->first->g_Alive()) alive_num++;
	
	if (alive_num < 2) return false;
	
	return true;
}


void CMonsterSquad::UpdateGoal(CEntity *pE, const SMemberGoal &goal)
{
	MEMBER_GOAL_MAP_IT it = m_goals.find(pE);
	VERIFY(it != m_goals.end());

	it->second = goal;
}

void CMonsterSquad::UpdateCommand(CEntity *pE, const SSquadCommand &com)
{
	MEMBER_COMMAND_MAP_IT it = m_commands.find(pE);
	VERIFY(it != m_commands.end());

	it->second = com;
}

void CMonsterSquad::GetGoal(CEntity *pE, SMemberGoal &goal)
{
	MEMBER_GOAL_MAP_IT it = m_goals.find(pE);
	VERIFY(it != m_goals.end());

	goal = it->second;
}

void CMonsterSquad::GetCommand(CEntity *pE, SSquadCommand &com)
{
	MEMBER_COMMAND_MAP_IT it = m_commands.find(pE);
	VERIFY(it != m_commands.end());

	com = it->second;
}

void CMonsterSquad::UpdateSquadCommands()
{
	// Set Command To None
	for (MEMBER_COMMAND_MAP_IT it = m_commands.begin(); it != m_commands.end(); it++) {
		it->second.type = SC_NONE;
	}

	ProcessAttack	();
	ProcessIdle		();

}
