#include "stdafx.h"
#include "base_monster.h"
#include "../ai_monster_debug.h"
#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"
#include "../../profiler.h"
#include "../critical_action_info.h"
#include "../ai_monster_movement.h"

void CBaseMonster::Think()
{
	START_PROFILE("AI/Base Monster/Think");

	if (!g_Alive())		return;
	if (getDestroy())	return;

	m_current_update						= Device.dwTimeGlobal;

	// Инициализировать
	InitThink								();
	MotionMan.ScheduledInit					();
	movement().Update_Initialize		();

	// Обновить память
	START_PROFILE("AI/Base Monster/Think/Update Memory");
	vfUpdateParameters						();
	STOP_PROFILE;

	// Обновить сквад
	START_PROFILE("AI/Base Monster/Think/Update Squad");
	monster_squad().update					(this);
	STOP_PROFILE;

	// Запустить FSM
	START_PROFILE("AI/Base Monster/Think/FSM");
	update_fsm();
	STOP_PROFILE;	
	
	// Построить путь
	START_PROFILE("AI/Base Monster/Think/Build Path");
	movement().Update_Execute		();
	STOP_PROFILE;

	// Обновить анимации в соответствие с путем и action
	MotionMan.UpdateScheduled				();

	// установить текущую скорость
	movement().Update_Finalize		();

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

void CBaseMonster::update_fsm()
{
	if (CriticalActionInfo->is_fsm_locked()) return;

	StateMan->update				();		
	TranslateActionToPathParams		();

	//if (!UpdateStateManager()) {
	//	StateSelector				();
	//	CurrentState->Execute		(m_current_update);
	//	squad_notify				();
	//}
	//

	//StateMan->update				();
	//squad_notify					();	

	//TranslateActionToPathParams		();

}

