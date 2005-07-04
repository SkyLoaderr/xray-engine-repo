#include "stdafx.h"
#include "control_animation_base.h"
#include "control_direction_base.h"
#include "control_movement_base.h"
#include "BaseMonster/base_monster.h"
#include "../../PHMovementControl.h"
#include "../../detail_path_manager.h"
#include "monster_velocity_space.h"


//////////////////////////////////////////////////////////////////////////
// m_tAction processing
//////////////////////////////////////////////////////////////////////////

void CControlAnimationBase::update_frame()
{
	update				();

	// raise event on velocity bounce
	CheckVelocityBounce	();
}

void CControlAnimationBase::update()
{
	// Установка Yaw
	if (m_object->control().path_builder().is_moving_on_path()) m_object->dir().use_path_direction( ((spec_params & ASP_MOVE_BKWD) == ASP_MOVE_BKWD) );

	SelectAnimation		();
	SelectVelocities	();
}


//////////////////////////////////////////////////////////////////////////
// SelectAnimation
// In:	path, target_yaw, m_tAction
// Out:	установить анимацию в cur_anim_info().motion
void CControlAnimationBase::SelectAnimation()
{
	EAction							action = m_tAction;
	if (m_object->control().path_builder().is_moving_on_path()) action = GetActionFromPath();

	cur_anim_info().motion			= get_sd()->m_tMotions[action].anim;

	m_object->CheckSpecParams		(spec_params);	
	if (prev_motion	!= cur_anim_info().motion) 
		CheckTransition				(prev_motion, cur_anim_info().motion);

	CheckReplacedAnim				();
	SetTurnAnimation				();
}

#define MOVE_TURN_ANGLE		deg(30)

void CControlAnimationBase::SetTurnAnimation()
{
	float yaw_current, yaw_target;
	m_man->direction().get_heading(yaw_current, yaw_target);
	float delta_yaw	= angle_difference(yaw_target, yaw_current);

	bool turn_left = true;
	if (from_right(yaw_target, yaw_current)) turn_left = false; 

	if (IsStandCurAnim() && (!fis_zero(delta_yaw))) {
		m_object->SetTurnAnimation(turn_left);
		return;
	}

	if (m_object->control().path_builder().is_moving_on_path() && (delta_yaw > MOVE_TURN_ANGLE)) {
		m_object->SetTurnAnimation(turn_left);
		return;
	}
}

//////////////////////////////////////////////////////////////////////////
// SelectVelocities
// In:	path, target_yaw, анимация
// Out:	установить linear и angular velocities, 
//		по скорости движения выбрать финальную анимацию из Velocity_Chain
//		установить скорость анимации в соответствие с физ скоростью
void CControlAnimationBase::SelectVelocities()
{
	// получить скорости движения по пути
	bool		b_moving = m_object->control().path_builder().is_moving_on_path();
	SMotionVel	path_vel;	path_vel.set(0.f,0.f);
	SMotionVel	anim_vel;	anim_vel.set(0.f,0.f);

	if (b_moving) {

		u32 cur_point_velocity_index = m_object->movement().detail().path()[m_object->movement().detail().curr_travel_point_index()].velocity;

		u32 next_point_velocity_index = u32(-1);
		if (m_object->movement().detail().path().size() > m_object->movement().detail().curr_travel_point_index() + 1) 
			next_point_velocity_index = m_object->movement().detail().path()[m_object->movement().detail().curr_travel_point_index() + 1].velocity;

		// если сейчас стоит на месте и есть след точка (т.е. должен быть в движении),
		// то реализовать поворот на месте, а дальше форсировать скорость со следующей точки
		if ((cur_point_velocity_index == MonsterMovement::eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
			if (!m_object->control().direction().is_turning()) 
				cur_point_velocity_index = next_point_velocity_index;
		} 

		const CDetailPathManager::STravelParams &current_velocity = m_object->movement().detail().velocity(cur_point_velocity_index);
		path_vel.set(_abs(current_velocity.linear_velocity), current_velocity.real_angular_velocity);
	}

	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	// получить скорости движения по анимации
	anim_vel.set(item_it->second.velocity.velocity.linear, item_it->second.velocity.velocity.angular_real);

	//	// проверить на совпадение
	//	R_ASSERT(fsimilar(path_vel.linear,	anim_vel.linear));
	//	R_ASSERT(fsimilar(path_vel.angular,	anim_vel.angular));

	// установка линейной скорости	
	if (m_object->state_invisible) {
		// если невидимый, то установить скорость из пути
		m_object->move().set_velocity(_abs(path_vel.linear));
	} else {

		// - проверить на возможность торможения
		if (!accel_check_braking(0.f)) {
			if (fis_zero(_abs(anim_vel.linear))) stop_now();
			else m_object->move().set_velocity(_abs(anim_vel.linear));
		} else {
			m_object->move().stop_accel();
		}
	}

	// финальная корректировка скорости анимации по физической скорости
	if (b_moving) {
		Fvector temp_vec;
		m_object->m_PhysicMovementControl->GetCharacterVelocity(temp_vec);
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

	set_animation_speed	();

	// установка угловой скорости
	item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	m_object->dir().set_heading_speed(item_it->second.velocity.velocity.angular_real);

	// применить 
	// если установленная анимация отличается от предыдущей - установить новую анимацию
	if (prev_motion	!= cur_anim_info().motion) {
		prev_motion	= cur_anim_info().motion;
		select_animation();
	}
}

#define VELOCITY_BOUNCE_THRESHOLD 1.5f

void CControlAnimationBase::CheckVelocityBounce()
{
	Fvector		temp_vec;
	m_object->m_PhysicMovementControl->GetCharacterVelocity(temp_vec);
	float		prev_speed	= m_prev_character_velocity;
	float		cur_speed	= temp_vec.magnitude();

	// prepare 
	if (fis_zero(prev_speed))	prev_speed	= 0.01f;
	if (fis_zero(cur_speed))	cur_speed	= 0.01f;

	float ratio = ((prev_speed > cur_speed) ? (prev_speed / cur_speed) : (cur_speed / prev_speed));

	if (ratio > VELOCITY_BOUNCE_THRESHOLD) {
		if (prev_speed > cur_speed) ratio = -ratio;

		// prepare event
		SEventVelocityBounce		event(ratio);
		m_man->notify				(ControlCom::eventVelocityBounce, &event);

	}
	m_prev_character_velocity = cur_speed;
}


void CControlAnimationBase::ScheduledInit()
{
	spec_params			= 0;
	accel_deactivate	();
}
