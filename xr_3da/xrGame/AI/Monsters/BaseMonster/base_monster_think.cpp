#include "stdafx.h"
#include "base_monster.h"
#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"
#include "../../../profiler.h"
#include "../critical_action_info.h"
#include "../state_manager.h"
#include "../../../PhysicsShell.h"
#include "../../../detail_path_manager.h"
#include "../monster_velocity_space.h"
#include "../../../level.h"


void CBaseMonster::Think()
{
	START_PROFILE("AI/Base Monster/Think");

	if (!g_Alive() || getDestroy())			return;

	// Инициализировать
	InitThink								();
	anim().ScheduledInit					();

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
	update_fsm								();
	STOP_PROFILE;	
	
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

	//	if (from_right(yaw,object->control().path_builder().m_body.current.yaw)) {
	//		object->_target_delta = angle_difference(yaw,object->control().path_builder().m_body.current.yaw);
	//	} else object->_target_delta = -angle_difference(yaw,object->control().path_builder().m_body.current.yaw);

	//	clamp(object->_target_delta, -PI_DIV_4, PI_DIV_4);
	//}

	EMonsterState state = StateMan->get_state_type();

	// Look at enemy while running
	m_bRunTurnLeft = m_bRunTurnRight = false;
	
	if (is_state(state, eStateAttack)) {
		
		float	dir_yaw = Fvector().sub(EnemyMan.get_enemy_position(), Position()).getH();
		dir_yaw	= angle_normalize(-dir_yaw);

		float yaw_current, yaw_target;
		control().direction().get_heading(yaw_current, yaw_target);

		float angle_diff	= angle_difference(yaw_current, dir_yaw);

		if ((angle_diff > PI_DIV_3) && (angle_diff < 5 * PI_DIV_6)) {
			if (from_right(-dir_yaw, yaw_current))	m_bRunTurnRight = true;
			else									m_bRunTurnLeft	= true;
		}
	}

	// Rotation Jump
//	if (is_state(state, eStateAttack) && ability_rotation_jump()) check_rotation_jump();

	// Jump over physics
	if (ability_jump_over_physics()) {
		check_jump_over_physics();
	}
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

#define ROTATION_JUMP_DELAY		6000

void CBaseMonster::check_rotation_jump()
{
	//if (anim().IsCriticalAction()) return;
	//if (m_time_last_rotation_jump + ROTATION_JUMP_DELAY > Device.dwTimeGlobal)	return;

	//Fvector							enemy_position;
	//enemy_position.set				(EnemyMan.get_enemy()->Position());

	//if (dir().is_face_target(enemy_position, 120 * PI / 180)) return;

	//dir().face_target				(enemy_position);
	//
	//// everything is good, so set animation sequence
	//EMotionAnim						anim = dir().is_from_right(enemy_position) ? eAnimJumpRight : eAnimJumpLeft;

	//anim().Seq_Add				(anim);
	//anim().Seq_Switch			();
	//
	//// calculate angular speed according to animation speed and angle difference
	//const CControlDirectionBase::SAxis &yaw = dir().heading();	

	//float angular_velocity; 
	//float delta_yaw					= angle_difference(yaw.current, yaw.target);
	//float time						= anim().GetCurAnimTime	();
	//angular_velocity				= delta_yaw / time;

	//// set angular speed in exclusive force mode, coz it has already locked by Seq_Switch
	//dir().set_heading_speed		(angular_velocity, true);

	//m_time_last_rotation_jump		= Device.dwTimeGlobal;
}

#define MAX_DIST_SUM	6.f

void CBaseMonster::check_jump_over_physics()
{
	if (!control().path_builder().is_moving_on_path()) return;

	Fvector prev_pos	= Position();
	float	dist_sum	= 0.f;

	for(u32 i = movement().detail().curr_travel_point_index(); i<movement().detail().path().size();i++) {
		const DetailPathManager::STravelPathPoint &travel_point = movement().detail().path()[i];

		// получить список объектов вокруг врага
		Level().ObjectSpace.GetNearest	(travel_point.position, Radius());
		xr_vector<CObject*> &tpObjects	= Level().ObjectSpace.q_nearest;

		for (u32 k=0;k<tpObjects.size();k++) {
			CPhysicsShellHolder *obj = smart_cast<CPhysicsShellHolder *>(tpObjects[k]);
			if (!obj || !obj->PPhysicsShell() || !obj->PPhysicsShell()->bActive || (obj->Radius() < 1.f)) continue;
			if (Position().distance_to(obj->Position()) < MAX_DIST_SUM / 2) continue;

			Fvector dir = Fvector().sub(travel_point.position, Position());

			// проверка на  Field-Of-View
			float	my_h	= Direction().getH();
			float	h		= dir.getH();

			float from	= angle_normalize(my_h - deg(8));
			float to	= angle_normalize(my_h + deg(8));

			if (!is_angle_between(h, from, to)) continue;

			dir = Fvector().sub(obj->Position(), Position());

			// вычислить целевую позицию для прыжка
			
			// --------------------------------------------------------
			//Fvector center;
			//obj->Center(center);
			//float height = Fvector().add(center, obj->Radius()).y;

			//Fvector target;
			//target.mad(Position(), Direction(), dir.magnitude());
			//target.y = height;
			
			// --------------------------------------------------------

			Fvector target;
			obj->Center(target);
			target.y += obj->Radius();
			// --------------------------------------------------------

			jump_over_physics(target);
			return;
		}

		dist_sum += prev_pos.distance_to(travel_point.position);
		if (dist_sum > MAX_DIST_SUM) break;

		prev_pos = travel_point.position;
	}
}
