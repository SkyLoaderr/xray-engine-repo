#include "stdafx.h"
#include "control_jump.h"
#include "BaseMonster/base_monster.h"
#include "control_manager.h"
#include "../../PHMovementControl.h"
#include "../../../skeletonanimated.h"
#include "../../detail_path_manager.h"
#include "../../level.h"


void CControlJump::reinit()
{
	inherited::reinit		();

	m_time_started			= 0;
	m_time_next_allowed		= 0;
}


void CControlJump::load(LPCSTR section)
{
	m_delay_after_jump				= pSettings->r_u32	(section,"jump_delay");
	m_jump_factor					= pSettings->r_float(section,"jump_factor");
	m_trace_ground_range			= pSettings->r_float(section,"jump_ground_trace_range");
	m_hit_trace_range				= pSettings->r_float(section,"jump_hit_trace_range");
	m_build_line_distance			= pSettings->r_float(section,"jump_build_line_distance");
	m_min_distance					= pSettings->r_float(section,"jump_min_distance");
	m_max_distance					= pSettings->r_float(section,"jump_max_distance");
	m_max_angle						= pSettings->r_float(section,"jump_max_angle");
}

bool CControlJump::check_start_conditions()
{
	if (is_active())				return false;	
	if (m_man->is_captured_pure())	return false;

	return true;
}

void CControlJump::activate()
{
	//m_man->capture_pure	(this);
	m_man->capture		(this, ControlCom::eControlPath);
	m_man->capture		(this, ControlCom::eControlDir);
	m_man->capture		(this, ControlCom::eControlMovement);
	m_man->capture		(this, ControlCom::eControlTripleAnimation);
	m_man->subscribe	(this, ControlCom::eventTAChange);
	m_man->subscribe	(this, ControlCom::eventVelocityBounce);

	m_man->path_stop	(this);
	m_man->move_stop	(this);

	if (m_data.target_object)
		start_jump	(get_target(m_data.target_object));
	else 
		start_jump	(m_data.target_position);

}

void CControlJump::on_release()
{
	m_man->unlock		(this, ControlCom::eControlPath);
	m_man->release		(this, ControlCom::eControlPath);
	m_man->release		(this, ControlCom::eControlDir);
	m_man->release		(this, ControlCom::eControlMovement);

	if (m_man->triple_anim().is_active()) m_man->release		(this, ControlCom::eControlTripleAnimation);
	m_man->unsubscribe	(this, ControlCom::eventTAChange);
	m_man->unsubscribe	(this, ControlCom::eventVelocityBounce);
}

void CControlJump::start_jump(const Fvector &point)
{
	m_blend_speed						= -1.f;
	m_target_position					= point;

	m_object->dir().set_heading_speed	(3.f);
	m_object->dir().face_target			(point);

	SControlDirectionData		*ctrl_dir = (SControlDirectionData*)m_man->data(this, ControlCom::eControlDir); 
	VERIFY						(ctrl_dir);
	ctrl_dir->heading.target_speed	= 3.f;
	ctrl_dir->heading.target_acc	= flt_max;
	ctrl_dir->heading.target_angle	= m_man->direction().angle_to_target(point);


	m_velocity_bounced					= false;
	m_object_hitted						= false;

	m_time_started						= 0;
	m_jump_time							= 0;

	m_enable_bounce						= true;

	m_object->set_ignore_collision_hit	(true);

	// activate triple animation
	m_data.triple_anim.skip_prepare	= m_data.skip_prepare;

	SAnimationTripleData	*ctrl_data = (SAnimationTripleData*)m_man->data(this, ControlCom::eControlTripleAnimation); 
	VERIFY					(ctrl_data);

	ctrl_data->pool[0]		= m_data.triple_anim.pool[0];
	ctrl_data->pool[1]		= m_data.triple_anim.pool[1];
	ctrl_data->pool[2]		= m_data.triple_anim.pool[2];
	ctrl_data->skip_prepare	= m_data.triple_anim.skip_prepare;
	ctrl_data->execute_once	= m_data.triple_anim.execute_once;
	ctrl_data->capture_type = 0;

	m_man->activate			(ControlCom::eControlTripleAnimation);

}


void CControlJump::update_frame()
{
	if (m_velocity_bounced && m_object->movement().enabled() && m_object->movement().detail().completed(m_object->Position())) stop();

	if (is_landing()) pointbreak();
	
	set_animation_speed	();
	hit_test			();

	
	// Set Velocity from path
	SControlMovementData		*ctrl_move = (SControlMovementData*)m_man->data(this, ControlCom::eControlMovement); 
	VERIFY						(ctrl_move);
	
	ctrl_move->velocity_target	= m_object->move().get_velocity_from_path();
	ctrl_move->acc				= flt_max;
}

bool CControlJump::is_landing()
{
	if (m_time_started == 0) return false;

	if (m_time_started + (m_jump_time*1000) > Device.dwTimeGlobal) return false;

	Fvector direction;
	direction.set(0.f, -1.f, 0.f);
	Fvector trace_from;
	m_object->Center(trace_from);

	m_object->setEnabled(false);
	collide::rq_result	l_rq;

	bool on_the_ground = false;
	if (Level().ObjectSpace.RayPick(trace_from, direction, m_trace_ground_range, collide::rqtStatic, l_rq)) {
		if (l_rq.range < m_trace_ground_range) on_the_ground = true;
	}

	m_object->setEnabled(true);			

	if (!on_the_ground) return false;

	return true;
}

void CControlJump::build_line()
{
	if (m_data.velocity_mask == u32(-1)) return;
	
	Fvector target_position;
	target_position.mad(m_object->Position(), m_object->Direction(), m_build_line_distance);

	if (!m_man->build_path_line(this, target_position, u32(-1), m_data.velocity_mask)) stop();
	else { 
		// Set Velocity from path
		SControlPathBuilderData		*ctrl_path = (SControlPathBuilderData*)m_man->data(this, ControlCom::eControlPath); 
		VERIFY						(ctrl_path);
	
		ctrl_path->enable			= true;

		m_man->lock					(this, ControlCom::eControlPath);
		
	}
}

void CControlJump::set_animation_speed() 
{
	//if ((m_animation_holder->get_state() == eStateExecute) && (info.blend->motionID == m_def_glide)) {
	//	if (m_blend_speed < 0)	m_blend_speed = info.blend->speed;
	//	info.speed.current = (info.blend->timeTotal / m_jump_time);
	//} else {
	//	info.speed.current = 1.f;
	//}
}

void CControlJump::stop()
{
	if (m_man->triple_anim().is_active())	m_man->release(this, ControlCom::eControlTripleAnimation);
	m_object->path().prepare_builder		();
	m_object->set_ignore_collision_hit		(false);

	m_man->notify							(ControlCom::eventJumpEnd, 0);
}

void CControlJump::pointbreak()
{
	m_man->triple_anim().pointbreak();
	m_time_started	= 0;
}

Fvector CControlJump::get_target(CObject *obj)
{
	u16 bone_id			= smart_cast<CKinematics*>(obj->Visual())->LL_GetBoneRoot			();
	CBoneInstance &bone = smart_cast<CKinematics*>(obj->Visual())->LL_GetBoneInstance		(bone_id);

	Fmatrix	global_transform;
	global_transform.set	(obj->XFORM());
	global_transform.mulB	(bone.mTransform);

	return	(predict_position(obj, global_transform.c));
}

void CControlJump::on_event(ControlCom::EEventType type, ControlCom::IEventData *data)
{
	if (type == ControlCom::eventTAChange) {
		STripleAnimEventData *event_data = (STripleAnimEventData *)data;
		if (event_data->m_current_state == eStateExecute) {
			// �������� ����� ���.������
			float ph_time = m_object->m_PhysicMovementControl->JumpMinVelTime(m_target_position);
			// ��������� ������ � ������������ � ��������� �������
			m_object->m_PhysicMovementControl->Jump(m_target_position,ph_time/m_jump_factor);
			m_jump_time			= ph_time/m_jump_factor;
			m_time_started		= Device.dwTimeGlobal;
			m_time_next_allowed	= m_time_started + m_delay_after_jump;
		} else if (event_data->m_current_state == eStateNone) {
			if (m_man->triple_anim().is_active()) m_man->release(this, ControlCom::eControlTripleAnimation);
			stop();
		}
	} else if (type == ControlCom::eventVelocityBounce) {
		SEventVelocityBounce *event_data = (SEventVelocityBounce *)data;
		if ((event_data->m_ratio < 0) && !m_velocity_bounced && (m_jump_time != 0)) {
			if (is_on_the_ground()) {
				m_velocity_bounced	= true;
				build_line			();
			} else {
				if (!m_enable_bounce) {
					m_enable_bounce = true;
				} else stop();
			}
		}
	}
}

void CControlJump::hit_test() 
{
	if (m_object_hitted)	return;
	if (!m_data.target_object)	return;

	// ��������� �� ��������� ���� �� ����� ������
	Fvector trace_from;
	m_object->Center(trace_from);

	m_object->setEnabled(false);
	collide::rq_result	l_rq;

	if (Level().ObjectSpace.RayPick(trace_from, m_object->Direction(), m_hit_trace_range, collide::rqtObject, l_rq)) {
		if ((l_rq.O == m_data.target_object) && (l_rq.range < m_hit_trace_range)) {
			m_object->HitEntityInJump(smart_cast<CEntity*>(m_data.target_object));
			m_object_hitted = true;
		}
	}

	m_object->setEnabled(true);			
	return;
}

bool CControlJump::is_on_the_ground()
{
	bool ret_val = false;

	m_object->setEnabled(false);
	collide::rq_result	l_rq;

	Fvector			trace_from;
	m_object->Center(trace_from);

	Fvector			trace_dir;
	trace_dir.set	(0,-1.f,0);

	float			trace_dist = m_object->Radius() + m_trace_ground_range;

	if (Level().ObjectSpace.RayPick(trace_from, trace_dir, trace_dist, collide::rqtStatic, l_rq)) {
		if ((l_rq.range < trace_dist)) {
			ret_val = true;
		}
	}

	m_object->setEnabled(true);

	return ret_val;
}

bool CControlJump::can_jump(CObject *target)
{
	if (m_time_next_allowed > Device.dwTimeGlobal) return false;

	Fvector source_position		= m_object->Position	();
	Fvector target_position;
	target->Center				(target_position);

	float dist = source_position.distance_to(target_position);
	if ((dist < m_min_distance) || (dist > m_max_distance)) return false;

	// �������� ������ ����������� � ��� ��� ����
	float		dir_yaw = Fvector().sub(target_position, source_position).getH();
	dir_yaw		= angle_normalize(-dir_yaw);

	// �������� �� angle � �� dist
	float yaw_current, yaw_target;
	m_object->control().direction().get_heading(yaw_current, yaw_target);
	if (angle_difference(yaw_current, dir_yaw) > m_max_angle) return false;

	return true;
}

Fvector CControlJump::predict_position(CObject *obj, const Fvector &pos)
{
	float velocity = m_object->movement_control()->GetVelocityActual();
	float jump_time = m_object->movement_control()->JumpMinVelTime(pos);
	float prediction_dist = jump_time * velocity;

	Fvector prediction_pos;
	prediction_pos.mad(pos, obj->Direction(), prediction_dist);

	// ��������� prediction_pos �� ��������� � ����
	float dist = m_object->Position().distance_to(prediction_pos);
	if ((dist < m_min_distance) || (dist > m_max_distance)) return pos;

	// �������� ������ ����������� � ��� ��� ����
	Fvector		dir;
	float		dir_yaw, dir_pitch;

	dir.sub		(prediction_pos, m_object->Position());
	dir.getHP	(dir_yaw, dir_pitch);

	// �������� �� angle � �� dist
	float yaw_current, yaw_target;
	m_object->control().direction().get_heading(yaw_current, yaw_target);
	if (angle_difference(yaw_current, -dir_yaw) > m_max_angle) return pos;

	return prediction_pos;
}
