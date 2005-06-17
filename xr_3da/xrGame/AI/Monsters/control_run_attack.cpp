#include "stdafx.h"
#include "control_run_attack.h"
#include "BaseMonster/base_monster.h"
#include "monster_velocity_space.h"

void CControlRunAttack::reinit()
{
	CControl_ComCustom<>::reinit();

	
}

void CControlRunAttack::activate()
{
	m_man->capture_pure	(this);
	m_man->subscribe	(this, ControlCom::eventAnimationEnd);

	// to know the position we will be we need to build path
	Fvector target_position = m_object->EnemyMan.get_enemy()->Position();
	m_object->dir().face_target			(target_position);

	// everything is good, so set animation sequence
	MotionID								anim = m_object->anim().get_motion_id(m_object->control().direction().is_from_right(target_position) ? eAnimJumpRight : eAnimJumpLeft);

	// start new animation
	SControlAnimationData					*ctrl_data = (SControlAnimationData*)m_man->data(this, ControlCom::eControlAnimation); 
	VERIFY									(ctrl_data);
	ctrl_data->motion						= anim;
	ctrl_data->start_animation				= true;

	// disable path builder and movement
	m_man->path_stop						(this);
	m_man->move_stop						(this);


	// set angular speed in exclusive force mode
	SControlDirectionData					*ctrl_data_dir = (SControlDirectionData*)m_man->data(this, ControlCom::eControlDir); 
	VERIFY									(ctrl_data_dir);	
	ctrl_data_dir->heading.target_speed		= 3.f;
	ctrl_data_dir->heading.target_angle		= m_object->dir().heading().target;

}

void CControlRunAttack::deactivate()
{
	m_man->release_pure	(this);
	m_man->unsubscribe	(this, ControlCom::eventAnimationEnd);
}

bool CControlRunAttack::check_start_conditions()
{
	if (is_active())				return false;	
	if (m_man->is_captured_pure())	return false;


	Fvector									enemy_position;
	enemy_position.set						(m_object->EnemyMan.get_enemy()->Position());

	return true;
}

void CControlRunAttack::on_event(ControlCom::EEventType type, ControlCom::IEventData *dat)
{
	m_man->deactivate	(ControlCom::eControlRotationJump);
}
