#include "stdafx.h"
#include "ai_monster_motion.h"
#include "BaseMonster/base_monster.h"
#include "../../PHMovementControl.h"
#include "ai_monster_utils.h"
#include "ai_monster_jump.h"
#include "custom_events.h"
#include "../../detail_path_manager.h"
#include "ai_monster_movement.h"
#include "ai_monster_movement_space.h"

//////////////////////////////////////////////////////////////////////////
// m_tAction processing
//////////////////////////////////////////////////////////////////////////

void CMotionManager::Update()
{
	if (seq_playing) return;
	if (TA_IsActive()) return;

	// ��������� Yaw
	if (pMonster->movement().IsMovingOnPath()) pMonster->DirMan.use_path_direction( ((spec_params & ASP_MOVE_BKWD) == ASP_MOVE_BKWD) );

	SelectAnimation		();
	SelectVelocities	();
}


//////////////////////////////////////////////////////////////////////////
// SelectAnimation
// In:	path, target_yaw, m_tAction
// Out:	���������� �������� � cur_anim_info().motion
void CMotionManager::SelectAnimation()
{
	EAction							action = m_tAction;
	if (pMonster->movement().IsMovingOnPath()) action = GetActionFromPath();

	cur_anim_info().motion			= get_sd()->m_tMotions[action].anim;
	
	pMonster->CheckSpecParams		(spec_params);	
	if (Seq_Active()) return;

	if (prev_motion	!= cur_anim_info().motion) CheckTransition(prev_motion, cur_anim_info().motion);
	if (Seq_Active()) return;

	CheckReplacedAnim				();

	SetTurnAnimation				();
}

#define MOVE_TURN_ANGLE		deg(30)
#define STAND_TURN_ANGLE	deg(1)

void CMotionManager::SetTurnAnimation()
{
	const CDirectionManager::SAxis &yaw = pMonster->DirMan.heading();

	float delta_yaw	= angle_difference(yaw.target, yaw.current);

	bool turn_left = true;
	if (from_right(yaw.target, yaw.current)) turn_left = false; 

	if (IsStandCurAnim() && (delta_yaw > STAND_TURN_ANGLE)) {
		pMonster->SetTurnAnimation(turn_left);
		return;
	}
	
	if (pMonster->movement().IsMovingOnPath() && (delta_yaw > MOVE_TURN_ANGLE)) {
		pMonster->SetTurnAnimation(turn_left);
		return;
	}
}



//////////////////////////////////////////////////////////////////////////
// SelectVelocities
// In:	path, target_yaw, ��������
// Out:	���������� linear � angular velocities, 
//		�� �������� �������� ������� ��������� �������� �� Velocity_Chain
//		���������� �������� �������� � ������������ � ��� ���������
void CMotionManager::SelectVelocities()
{
	// �������� �������� �������� �� ����
	bool		b_moving = pMonster->movement().IsMovingOnPath();
	SMotionVel	path_vel;	path_vel.set(0.f,0.f);
	SMotionVel	anim_vel;	anim_vel.set(0.f,0.f);

	if (b_moving) {

		u32 cur_point_velocity_index = pMonster->movement().detail().path()[pMonster->movement().detail().curr_travel_point_index()].velocity;

		u32 next_point_velocity_index = u32(-1);
		if (pMonster->movement().detail().path().size() > pMonster->movement().detail().curr_travel_point_index() + 1) 
			next_point_velocity_index = pMonster->movement().detail().path()[pMonster->movement().detail().curr_travel_point_index() + 1].velocity;

		// ���� ������ ����� �� ����� � ���� ���� ����� (�.�. ������ ���� � ��������),
		// �� ����������� ������� �� �����, � ������ ����������� �������� �� ��������� �����
		if ((cur_point_velocity_index == MonsterMovement::eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
			if (!pMonster->DirMan.is_turning()) 
				cur_point_velocity_index = next_point_velocity_index;
		} 

		const CDetailPathManager::STravelParams &current_velocity = pMonster->movement().detail().velocity(cur_point_velocity_index);
		path_vel.set(_abs(current_velocity.linear_velocity), current_velocity.real_angular_velocity);
	}

	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	// �������� �������� �������� �� ��������
	anim_vel.set(item_it->second.velocity->velocity.linear, item_it->second.velocity->velocity.angular_real);

	//	// ��������� �� ����������
	//	R_ASSERT(fsimilar(path_vel.linear,	anim_vel.linear));
	//	R_ASSERT(fsimilar(path_vel.angular,	anim_vel.angular));

	// ��������� �������� ��������	
	if (pMonster->state_invisible) {
		// ���� ���������, �� ���������� �������� �� ����
		pMonster->movement().m_velocity_linear.target	= _abs(path_vel.linear);
	} else {
		
		// - ��������� �� ����������� ����������
		if (!accel_check_braking(0.f)) {
			pMonster->movement().m_velocity_linear.target	= _abs(anim_vel.linear);
			if (fis_zero(pMonster->movement().m_velocity_linear.target)) pMonster->movement().stop_linear();
		} else {
			pMonster->movement().stop_linear_accel();
		}
	}

	// ��������� ������������� �������� �������� �� ���������� ��������
	if (b_moving) {
		Fvector temp_vec;
		pMonster->m_PhysicMovementControl->GetCharacterVelocity(temp_vec);
		float  real_speed = temp_vec.magnitude();

		EMotionAnim new_anim;
		float		a_speed;

		if (accel_chain_get(real_speed, cur_anim_info().motion, new_anim, a_speed)) {
			cur_anim_info().motion			= new_anim;
			cur_anim_info().speed.target	= a_speed;
		} else 
			cur_anim_info().speed.target	= -1.f;
	} else 
		cur_anim_info().speed.target	= -1.f;


	// ��������� ������� ��������
	if (!b_forced_velocity) {
		item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
		VERIFY(get_sd()->m_tAnims.end() != item_it);

		pMonster->DirMan.set_angular_speed(item_it->second.velocity->velocity.angular_real);
	}

	// ��������� 
	// ���� ������������� �������� ���������� �� ���������� - ���������� ����� ��������
	if (prev_motion	!= cur_anim_info().motion) {
		prev_motion	= cur_anim_info().motion;
		ForceAnimSelect();		
	}
}

#define ANIM_CHANGE_SPEED_VELOCITY 10.f

void CMotionManager::FrameUpdate()
{
	// update animation speed 
	if (m_cur_anim.speed.target > 0) {
		if (m_cur_anim.speed.current < 0) m_cur_anim.speed.current = 0.f;
		velocity_lerp(m_cur_anim.speed.current, m_cur_anim.speed.target, ANIM_CHANGE_SPEED_VELOCITY, Device.fTimeDelta);
	} else m_cur_anim.speed.current = -1.f;

	if (!seq_playing && !TA_IsActive() && (!pJumping || (pJumping && !pJumping->IsActive()))) {
		Update();
	} else {
		pMonster->movement().disable_path();	
	}

	// raise event on velocity bounce
	CheckVelocityBounce();
}

#define VELOCITY_BOUNCE_THRESHOLD 1.5f


void CMotionManager::CheckVelocityBounce()
{
	Fvector		temp_vec;
	pMonster->m_PhysicMovementControl->GetCharacterVelocity(temp_vec);
	float		prev_speed	= m_prev_character_velocity;
	float		cur_speed	= temp_vec.magnitude();
	
	// prepare 
	if (fis_zero(prev_speed))	prev_speed	= 0.01f;
	if (fis_zero(cur_speed))	cur_speed	= 0.01f;

	float ratio = ((prev_speed > cur_speed) ? (prev_speed / cur_speed) : (cur_speed / prev_speed));
	
	if (ratio > VELOCITY_BOUNCE_THRESHOLD) {
		if (prev_speed > cur_speed) ratio = -ratio;
		
		// prepare event
		CEventVelocityBounce		event(ratio);
		pMonster->EventMan.raise	(eventVelocityBounce, &event);

	}
	m_prev_character_velocity = cur_speed;
}


void CMotionManager::ScheduledInit()
{
	spec_params			= 0;
	accel_deactivate	();
}

void CMotionManager::UpdateScheduled()
{
	if (!seq_playing && !TA_IsActive() && (!pJumping || (pJumping && !pJumping->IsActive()))) {
		b_forced_velocity	= false;
		Update				();	
	} else {
		pMonster->movement().disable_path();
	}
}
