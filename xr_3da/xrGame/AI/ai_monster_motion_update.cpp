#include "stdafx.h"
#include "ai_monster_motion.h"
#include "biting/ai_biting.h"
#include "../PHMovementControl.h"

//////////////////////////////////////////////////////////////////////////
// m_tAction processing
//////////////////////////////////////////////////////////////////////////

void CMotionManager::Update()
{
	if (seq_playing) return;
	if (TA_IsActive()) return;

	// ��������� Yaw
	if (pMonster->IsMovingOnPath()) pMonster->SetDirectionLook( ((spec_params & ASP_MOVE_BKWD) == ASP_MOVE_BKWD) );

	b_forced_velocity	= false;

	SelectAnimation		();
	SelectVelocities	();

	spec_params		= 0;
}


//////////////////////////////////////////////////////////////////////////
// SelectAnimation
// In:	path, target_yaw, m_tAction
// Out:	���������� �������� � cur_anim_info().motion
void CMotionManager::SelectAnimation()
{
	EAction							action = m_tAction;
	if (pMonster->IsMovingOnPath()) action = GetActionFromPath();

	cur_anim_info().motion			= get_sd()->m_tMotions[action].anim;
	
	ANIM_ITEM_MAP_IT anim_it = get_sd()->m_tAnims.find(cur_anim_info().motion);

	pMonster->CheckSpecParams		(spec_params);	
	if (Seq_Active()) return;

	if (prev_motion	!= cur_anim_info().motion) CheckTransition(prev_motion, cur_anim_info().motion);
	if (Seq_Active()) return;

	CheckReplacedAnim				();

	pMonster->ProcessTurn			();
	
	anim_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
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
	bool		b_moving = pMonster->IsMovingOnPath();
	SMotionVel	path_vel;	path_vel.set(0.f,0.f);
	SMotionVel	anim_vel;	anim_vel.set(0.f,0.f);

	if (b_moving) {

		u32 cur_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index()].velocity;

		u32 next_point_velocity_index = u32(-1);
		if (pMonster->CDetailPathManager::path().size() > pMonster->curr_travel_point_index() + 1) 
			next_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index() + 1].velocity;

		// ���� ������ ����� �� ����� � ���� ���� ����� (�.�. ������ ���� � ��������),
		// �� ����������� ������� �� �����, � ������ ����������� �������� �� ��������� �����
		if ((cur_point_velocity_index == pMonster->eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
			if (angle_difference(pMonster->m_body.current.yaw, pMonster->m_body.target.yaw) < deg(1)) 
				cur_point_velocity_index = next_point_velocity_index;
		} 

		const CDetailPathManager::STravelParams &current_velocity = pMonster->CDetailPathManager::velocity(cur_point_velocity_index);
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
	// - ��������� �� ����������� ����������
	if (!accel_check_braking(0.f)) {

		pMonster->m_velocity_linear.target	= _abs(anim_vel.linear);
		if (fis_zero(pMonster->m_velocity_linear.target)) pMonster->stop_linear();
	} else {
		pMonster->stop_linear_accel();
	}

	// ���� ���������, �� ���������� �������� �� ����
	if (pMonster->state_invisible) pMonster->m_velocity_linear.target	= _abs(path_vel.linear);

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

		pMonster->m_velocity_angular = item_it->second.velocity->velocity.angular_real;
	}

	// ��������� 
	// ���� ������������� �������� ���������� �� ���������� - ���������� ����� ��������
	if (prev_motion	!= cur_anim_info().motion) {
		prev_motion	= cur_anim_info().motion;
		ForceAnimSelect();		
	}
}

