#include "stdafx.h"
#include "ai_monster_squad.h"
#include "../../entity.h"

void CMonsterSquad::RegisterMember(CEntity *pE)
{
	// �������� ����
	SMemberGoal			G;
	m_goals.insert		(mk_pair(pE, G));
	
	// �������� �������
	SSquadCommand		C;
	C.type				= SC_NONE;
	m_commands.insert	(mk_pair(pE, C));
	
	// ���������� ������
	if (!leader) leader = pE;
}

void CMonsterSquad::RemoveMember(CEntity *pE)
{
	// ������� �� �����
	MEMBER_GOAL_MAP_IT it_goal = m_goals.find(pE);
	if (it_goal == m_goals.end()) return;	
	m_goals.erase(it_goal);

	// ������� �� ������
	MEMBER_COMMAND_MAP_IT it_command = m_commands.find(pE);
	if (it_command == m_commands.end()) return;	
	m_commands.erase(it_command);

	// ���� ��������� ������� �������� ������� - ������������� ������
	if (leader == pE)  {
		if (m_goals.empty()) leader = 0;
		else leader = m_goals.begin()->first;
	}
}

bool CMonsterSquad::SquadActive()
{
	if (!leader) return false;

	// ��������� ���������� ����� �������� � ������
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

SMemberGoal &CMonsterSquad::GetGoal(CEntity *pE)
{
	MEMBER_GOAL_MAP_IT it = m_goals.find(pE);
	VERIFY(it != m_goals.end());

	return it->second;
}

SSquadCommand &CMonsterSquad::GetCommand(CEntity *pE)
{
	MEMBER_COMMAND_MAP_IT it = m_commands.find(pE);
	VERIFY(it != m_commands.end());
	return it->second;
}

void CMonsterSquad::GetGoal(CEntity *pE, SMemberGoal &goal)
{
	goal = GetGoal(pE);
}

void CMonsterSquad::GetCommand(CEntity *pE, SSquadCommand &com)
{
	com	= GetCommand(pE);
}

void CMonsterSquad::UpdateSquadCommands()
{
	// �������� ��� ������� � ������
	for (MEMBER_COMMAND_MAP_IT it = m_commands.begin(); it != m_commands.end(); it++) {
		it->second.type = SC_NONE;
	}

	// ������� ��� ����, ������� ������� ��������� ��� ���� � �������
	for (MEMBER_GOAL_MAP_IT it_goal = m_goals.begin(); it_goal != m_goals.end(); ++it_goal) {
		SMemberGoal goal = it_goal->second;
		if (!goal.entity || goal.entity->getDestroy()) {
			it_goal->second.type = MG_None;
		}
	}


	ProcessAttack	();
	ProcessIdle		();
}
