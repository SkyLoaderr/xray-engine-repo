#include "stdafx.h"
#include "base_monster.h"
#include "../ai_monster_debug.h"
#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"
#include "../profiler.h"

void CBaseMonster::Think()
{
	START_PROFILE("AI/Base Monster/Think");

	if (!g_Alive())		return;
	if (getDestroy())	return;

	m_current_update						= Level().timeServer();

	// ����������������
	InitThink								();
	MotionMan.ScheduledInit					();
	CMonsterMovement::Update_Initialize		();

	// �������� ������
	START_PROFILE("AI/Base Monster/Think/Update Memory");
	vfUpdateParameters						();
	STOP_PROFILE;

	// �������� �����
	START_PROFILE("AI/Base Monster/Think/Update Squad");
	monster_squad().update					(this);
	STOP_PROFILE;

	// ��������� FSM
	START_PROFILE("AI/Base Monster/Think/FSM");
	if (MotionMan.IsCriticalAction()) disable_path();
	else {
		StateMan->update				();		
		TranslateActionToPathParams		();
		
		//if (!UpdateStateManager()) {
		//	StateSelector				();
		//	CurrentState->Execute		(m_current_update);
		//	squad_notify				();
		//}
		//
		// TranslateActionToPathParams		();

		//StateMan->update				();
		//squad_notify					();	
		
		//TranslateActionToPathParams		();
	}
	STOP_PROFILE;

	// ��������� ����
	START_PROFILE("AI/Base Monster/Think/Build Path");
	CMonsterMovement::Update_Execute		();
	STOP_PROFILE;

	// �������� ��������
	MotionMan.UpdateScheduled				();

	// ���������� ������� ��������
	CMonsterMovement::Update_Finalize		();

	// Debuging
#ifdef DEBUG
	HDebug->SetActive						(true);
#endif

	STOP_PROFILE;
}

//void CBaseMonster::squad_notify()
//{
//	CMonsterSquad	*squad = monster_squad().get_squad(this);
//	SMemberGoal		goal;
//
//	if (CurrentState == stateAttack) {
//		goal.type	= MG_AttackEnemy;
//		goal.entity	= const_cast<CEntityAlive*>(EnemyMan.get_enemy());
//	}  
//	
//	squad->UpdateGoal(this, goal);
//}

