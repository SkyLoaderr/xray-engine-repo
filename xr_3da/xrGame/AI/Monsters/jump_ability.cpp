#include "stdafx.h"
#include "jump_ability.h"
#include "anim_triple.h"
#include "BaseMonster/base_monster.h"
#include "../../PHMovementControl.h"
#include "../../../skeletonanimated.h"
#include "custom_events.h"
#include "critical_action_info.h"

CJumpingAbility::CJumpingAbility()
{
	m_animation_holder = xr_new<CAnimTriple>();
}

CJumpingAbility::~CJumpingAbility()
{
	xr_delete(m_animation_holder);
}

void CJumpingAbility::init_external(CBaseMonster *obj)
{
	m_object							= obj;
	
	// set callbacks
	m_object->EventMan.add_delegate		(eventTAChange,			typeEvent(this, &CJumpingAbility::on_TA_change));
	m_object->EventMan.add_delegate		(eventVelocityBounce,	typeEvent(this, &CJumpingAbility::on_velocity_bounce));
}

void CJumpingAbility::load(LPCSTR section)
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

void CJumpingAbility::reinit(CMotionDef *m_def1, CMotionDef *m_def2, CMotionDef *m_def3)
{
	m_time_started			= 0;
	m_time_next_allowed		= 0;
	
	m_def_glide				= m_def2;
	
	m_active				= false;

	m_animation_holder->reinit_external	(&m_object->EventMan, m_def1, m_def2, m_def3);
}

void CJumpingAbility::jump(CObject *obj, u32 vel_mask)
{
	if (m_active) return;
	
	m_object->MotionMan.TA_Activate(m_animation_holder);
	
	m_blend_speed		= -1.f;

	m_velocity_mask		= vel_mask;
	m_target_object		= obj;
	m_target_position	= get_target(obj);

	m_active			= true;
	m_velocity_bounced	= false;
	m_object_hitted		= false;

	m_time_started		= 0;

	// lock control blocks
	m_object->CriticalActionInfo->set(CAF_LockFSM | CAF_LockPath);
}

void CJumpingAbility::update_frame()
{
	if (!m_active) return;
	
	if (!m_object->MotionMan.TA_IsActive())	stop();
	if (m_velocity_bounced && m_object->CMonsterMovement::enabled() && m_object->CDetailPathManager::completed(m_object->Position())) stop();

	if (is_landing()) pointbreak();
	
	set_animation_speed	();
	hit_test			();

	m_object->CMonsterMovement::set_velocity_from_path	();
}

bool CJumpingAbility::is_landing()
{
	if (m_time_started == 0) return false;

	if (m_time_started + (m_jump_time*1000) > Level().timeServer()) return false;
	
	Fvector direction;
	direction.set(0.f, -1.f, 0.f);
	Fvector trace_from;
	m_object->Center(trace_from);

	m_object->setEnabled(false);
	Collide::rq_result	l_rq;

	bool on_the_ground = false;
	if (Level().ObjectSpace.RayPick(trace_from, direction, m_trace_ground_range, Collide::rqtStatic, l_rq)) {
		if (l_rq.range < m_trace_ground_range) on_the_ground = true;
	}

	m_object->setEnabled(true);			

	if (!on_the_ground) return false;

	return true;
}

void CJumpingAbility::build_line()
{
	if (m_velocity_mask == u32(-1)) return;
	
	Fvector target_position;
	target_position.mad(m_object->Position(), m_object->Direction(), m_build_line_distance);

	if (!m_object->CMonsterMovement::build_special(target_position, u32(-1), m_velocity_mask)) stop();
	else m_object->CMonsterMovement::enable_path();
}


void CJumpingAbility::set_animation_speed() 
{
	SCurrentAnimationInfo &info = m_object->MotionMan.cur_anim_info();
	if (!info.blend) return;
	
	if ((m_animation_holder->get_state() == eStateExecute) && (info.blend->motionID == m_def_glide->motion)) {
		if (m_blend_speed < 0)	m_blend_speed = info.blend->speed;
		info.speed.current = (info.blend->timeTotal / m_jump_time);
	} else {
		info.speed.current = -1.f;
	}
}

void CJumpingAbility::stop()
{
	m_active = false;
	if (m_object->MotionMan.TA_IsActive())	m_object->MotionMan.TA_Deactivate();
	m_object->CMonsterMovement::stop_now();

	// unlock control blocks
	m_object->CriticalActionInfo->clear(CAF_LockFSM | CAF_LockPath);
}

void CJumpingAbility::pointbreak()
{
	m_object->MotionMan.TA_PointBreak();
	m_time_started	= 0;
}

Fvector CJumpingAbility::get_target(CObject *obj)
{
	u16 bone_id			= smart_cast<CKinematics*>(obj->Visual())->LL_GetBoneRoot			();
	CBoneInstance &bone = smart_cast<CKinematics*>(obj->Visual())->LL_GetBoneInstance		(bone_id);

	Fmatrix	global_transform;
	global_transform.set	(obj->XFORM());
	global_transform.mulB	(bone.mTransform);
	
	return global_transform.c;
}

void CJumpingAbility::on_TA_change(IEventData *p_data)
{
	if (!m_active) return;
	
	CEventTAPrepareAnimation	*data = (CEventTAPrepareAnimation	*)p_data;
	
	if (data->m_current_state == eStateExecute) {
		// получить время физ.прыжка
		float ph_time = m_object->m_PhysicMovementControl->JumpMinVelTime(m_target_position);
		// выполнить прыжок в соответствии с делителем времени
		m_object->m_PhysicMovementControl->Jump(m_target_position,ph_time/m_jump_factor);
		m_jump_time			= ph_time/m_jump_factor;
		m_time_started		= Level().timeServer();
		m_time_next_allowed	= m_time_started + m_delay_after_jump;
	}
}

void CJumpingAbility::on_velocity_bounce(IEventData *p_data)
{
	if (!m_active) return;
	
	CEventVelocityBounce *data = (CEventVelocityBounce *)p_data;
	if ((data->m_ratio < 0) && !m_velocity_bounced) {
		if (is_on_the_ground()) {
			m_velocity_bounced	= true;
			build_line			();
		} else stop();
	}
}

void CJumpingAbility::hit_test() 
{
	if (m_object_hitted) return;
	
	// Проверить на нанесение хита во время прыжка
	Fvector trace_from;
	m_object->Center(trace_from);
	
	m_object->setEnabled(false);
	Collide::rq_result	l_rq;

	if (Level().ObjectSpace.RayPick(trace_from, m_object->Direction(), m_hit_trace_range, Collide::rqtObject, l_rq)) {
		if ((l_rq.O == m_target_object) && (l_rq.range < m_hit_trace_range)) {
			m_object->HitEntityInJump(smart_cast<CEntity*>(m_target_object));
			m_object_hitted = true;
		}
	}
	
	m_object->setEnabled(true);			
	return;
}

bool CJumpingAbility::is_on_the_ground()
{
	bool ret_val = false;
	
	m_object->setEnabled(false);
	Collide::rq_result	l_rq;
	
	Fvector			trace_from;
	m_object->Center(trace_from);
	
	Fvector			trace_dir;
	trace_dir.set	(0,-1.f,0);
	
	float			trace_dist = m_object->Radius() + m_trace_ground_range;

	if (Level().ObjectSpace.RayPick(trace_from, trace_dir, trace_dist, Collide::rqtStatic, l_rq)) {
		if ((l_rq.range < trace_dist)) {
			ret_val = true;
		}
	}
	
	m_object->setEnabled(true);

	return ret_val;
}

bool CJumpingAbility::can_jump(CObject *target)
{
	if (m_active || (m_time_next_allowed > Level().timeServer())) return false;
	
	Fvector source_position		= m_object->Position	();
	Fvector target_position;
	target->Center				(target_position);
	
	float dist = source_position.distance_to(target_position);
	if ((dist < m_min_distance) || (dist > m_max_distance)) return false;
	
	// получить вектор направления и его мир угол
	Fvector		dir;
	float		yaw, pitch;

	dir.sub		(target_position, source_position);
	dir.getHP	(yaw, pitch);
	yaw			*= -1;
	yaw			= angle_normalize(yaw);

	// проверка на angle и на dist
	if (angle_difference(m_object->CMovementManager::m_body.current.yaw, yaw) > m_max_angle) return false;

	return true;
}
