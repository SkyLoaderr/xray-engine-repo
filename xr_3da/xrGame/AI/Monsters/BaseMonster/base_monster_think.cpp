#include "stdafx.h"
#include "base_monster.h"
#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"
#include "../../../profiler.h"
#include "../critical_action_info.h"
#include "../ai_monster_movement.h"
#include "../state_manager.h"
#include "../ai_monster_utils.h"

void CBaseMonster::Think()
{
	START_PROFILE("AI/Base Monster/Think");

	if (!g_Alive())		return;
	if (getDestroy())	return;

	m_current_update						= Device.dwTimeGlobal;

	// Инициализировать
	InitThink								();
	MotionMan.ScheduledInit					();
	movement().Update_Initialize			();

	// Обновить память
	START_PROFILE("AI/Base Monster/Think/Update Memory");
	UpdateMemory							();
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
	movement().Update_Execute				();
	STOP_PROFILE;

	// Обновить анимации в соответствие с путем и action
	MotionMan.UpdateScheduled				();

	// установить текущую скорость
	movement().Update_Finalize				();

	STOP_PROFILE;
}

void CBaseMonster::update_fsm()
{
	if (CriticalActionInfo->is_fsm_locked()) return;

	StateMan->update				();
	
	// завершить обработку установленных в FSM параметров
	post_fsm_update					();
	
	TranslateActionToPathParams		();

	// информировать squad о своих целях
	squad_notify					();

#ifdef DEBUG
	debug_fsm						();
#endif
}

void CBaseMonster::post_fsm_update()
{
	// обработать ROTATION JUMP, RUN_LOOK_ENEMY and so on

	//if (state_id == eStateAttack) {
	//	object->look_at_enemy = true;
	//	// calc new target delta
	//	float yaw, pitch;
	//	Fvector().sub(object->EnemyMan.get_enemy()->Position(), object->Position()).getHP(yaw,pitch);
	//	yaw *= -1;
	//	yaw = angle_normalize(yaw);

	//	if (from_right(yaw,object->movement().m_body.current.yaw)) {
	//		object->_target_delta = angle_difference(yaw,object->movement().m_body.current.yaw);
	//	} else object->_target_delta = -angle_difference(yaw,object->movement().m_body.current.yaw);

	//	clamp(object->_target_delta, -PI_DIV_4, PI_DIV_4);
	//}

	EMonsterState state = StateMan->get_state_type();

	// Look at enemy while running
	m_bRunTurnLeft = m_bRunTurnRight = false;
	
	if (is_state(state, eStateAttack)) {
		
		float	dir_yaw = Fvector().sub(EnemyMan.get_enemy_position(), Position()).getH();
		dir_yaw	= angle_normalize(-dir_yaw);

		const CDirectionManager::SAxis &yaw = DirMan.heading();
		float angle_diff	= angle_difference(yaw.current, dir_yaw);

		if ((angle_diff > PI_DIV_3) && (angle_diff < 5 * PI_DIV_6)) {
			if (from_right(-dir_yaw, yaw.current))	m_bRunTurnRight = true;
			else									m_bRunTurnLeft	= true;
		}
	}

	// Rotation Jump
	if (is_state(state, eStateAttack) && ability_rotation_jump()) check_rotation_jump();
}

void CBaseMonster::squad_notify()
{
	CMonsterSquad	*squad = monster_squad().get_squad(this);
	SMemberGoal		goal;

	EMonsterState state = StateMan->get_state_type();

	if (is_state(state, eStateAttack)) {
		
		goal.type	= MG_AttackEnemy;
		goal.entity	= const_cast<CEntityAlive*>(EnemyMan.get_enemy());

	} else if (is_state(state, eStateRest)) {
		if (is_state(state, eStateRest_Idle))					goal.type	= MG_Rest;
		else if (is_state(state, eStateRest_WalkGraphPoint)) 	goal.type	= MG_WalkGraph;
	} else if (is_state(state, eStateSquad)) {
		goal.type	= MG_Rest;
	}
	
	squad->UpdateGoal(this, goal);
}


void CBaseMonster::check_rotation_jump()
{
	if (MotionMan.IsCriticalAction()) return;
	
	// check time
	// [todo]
	
	Fvector							enemy_position;
	enemy_position.set				(EnemyMan.get_enemy()->Position());

	if (DirMan.is_face_target(enemy_position, 120 * PI / 180)) return;

	// everything is good, so set animation
	EMotionAnim						anim = DirMan.is_from_right(enemy_position) ? eAnimJumpRight : eAnimJumpLeft;

	// set sequence
	MotionMan.Seq_Add				(anim);
	MotionMan.Seq_Switch			();

	DirMan.face_target				(enemy_position);
	
	// calculate angular speed according to animation speed and angle difference
	float	dir_yaw = Fvector().sub(enemy_position, Position()).getH();
	const CDirectionManager::SAxis &yaw = DirMan.heading();	

	float new_angular_velocity; 
	float delta_yaw					= angle_difference(yaw.current, dir_yaw);
	float time						= MotionMan.GetCurAnimTime	();
	new_angular_velocity			= delta_yaw / time;

	MotionMan.ForceAngularSpeed		(new_angular_velocity);
}
