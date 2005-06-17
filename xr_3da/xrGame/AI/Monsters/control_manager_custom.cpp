#include "stdafx.h"
#include "control_manager_custom.h"
#include "BaseMonster/base_monster.h"
#include "control_sequencer.h"
#include "control_rotation_jump.h"
#include "../../PhysicsShell.h"
#include "../../detail_path_manager.h"
#include "../../level.h"


CControlManagerCustom::CControlManagerCustom()
{
	m_sequencer		= 0;
	m_triple_anim	= 0;
	m_rotation_jump	= 0;
	m_jump			= 0;
}

CControlManagerCustom::~CControlManagerCustom()
{
	xr_delete	(m_sequencer);
	xr_delete	(m_triple_anim);
	xr_delete	(m_rotation_jump);
	xr_delete	(m_jump);
}	

void CControlManagerCustom::add_ability(ControlCom::EContolType type)
{
	if (type == ControlCom::eControlSequencer) {

		m_sequencer		= xr_new<CAnimationSequencer>();
		m_man->add		(m_sequencer, ControlCom::eControlSequencer);

	} else if (type == ControlCom::eControlTripleAnimation) {
		
		m_triple_anim	= xr_new<CAnimationTriple>();
		m_man->add		(m_triple_anim, ControlCom::eControlTripleAnimation);
	
	} else if (type == ControlCom::eControlRotationJump) {

		m_rotation_jump = xr_new<CControlRotationJump>();
		m_man->add		(m_rotation_jump, ControlCom::eControlRotationJump);

	} else if (type == ControlCom::eControlJump) {

		m_jump			= xr_new<CControlJump>();
		m_man->add		(m_jump, ControlCom::eControlJump);

	}	
}

//////////////////////////////////////////////////////////////////////////

void CControlManagerCustom::on_start_control(ControlCom::EContolType type)
{
	switch (type) {
	case ControlCom::eControlSequencer:			m_man->subscribe	(this, ControlCom::eventSequenceEnd);	break;
	case ControlCom::eControlTripleAnimation:	m_man->subscribe	(this, ControlCom::eventTAChange);		break;
	case ControlCom::eControlJump:				m_man->subscribe	(this, ControlCom::eventJumpEnd);		break;
	}
}

void CControlManagerCustom::on_stop_control	(ControlCom::EContolType type)
{
	switch (type) {
	case ControlCom::eControlSequencer:			m_man->unsubscribe	(this, ControlCom::eventSequenceEnd);	break;
	case ControlCom::eControlTripleAnimation:	m_man->unsubscribe	(this, ControlCom::eventTAChange);		break;
	case ControlCom::eControlJump:				m_man->unsubscribe	(this, ControlCom::eventJumpEnd);		break;
	}
}

void CControlManagerCustom::on_event(ControlCom::EEventType type, ControlCom::IEventData *data)
{
	switch (type) {
	case ControlCom::eventSequenceEnd:	m_man->release(this, ControlCom::eControlSequencer); break;
	case ControlCom::eventTAChange: 
		{
			STripleAnimEventData *event_data = (STripleAnimEventData *)data;
			if (event_data->m_current_state == eStateNone) 
										m_man->release(this, ControlCom::eControlTripleAnimation); 

			break;
		}
	case ControlCom::eventJumpEnd:		m_man->release(this, ControlCom::eControlJump); break;
	}
}


void CControlManagerCustom::update_frame()
{

}

void CControlManagerCustom::update_schedule()
{
	if (m_jump)	{
		check_attack_jump		();
		check_jump_over_physics	();
	}
}

//////////////////////////////////////////////////////////////////////////
void CControlManagerCustom::ta_fill_data(SAnimationTripleData &data, LPCSTR s1, LPCSTR s2, LPCSTR s3, bool execute_once, bool skip_prep)
{
	// Load triple animations
	CSkeletonAnimated	*skel_animated = smart_cast<CSkeletonAnimated*>(m_object->Visual());
	data.pool[0]		= skel_animated->ID_Cycle_Safe(s1);	VERIFY(data.pool[0]);
	data.pool[1]		= skel_animated->ID_Cycle_Safe(s2);	VERIFY(data.pool[1]);
	data.pool[2]		= skel_animated->ID_Cycle_Safe(s3);	VERIFY(data.pool[2]);
	data.execute_once	= execute_once;
	data.skip_prepare	= skip_prep;
}


void CControlManagerCustom::ta_activate(const SAnimationTripleData &data)
{
	if (!m_man->check_start_conditions(ControlCom::eControlTripleAnimation)) 
		return;

	m_man->capture			(this,	ControlCom::eControlTripleAnimation);

	SAnimationTripleData	*ctrl_data = (SAnimationTripleData*)m_man->data(this, ControlCom::eControlTripleAnimation); 
	VERIFY					(ctrl_data);

	ctrl_data->pool[0]		= data.pool[0];
	ctrl_data->pool[1]		= data.pool[1];
	ctrl_data->pool[2]		= data.pool[2];
	ctrl_data->skip_prepare	= data.skip_prepare;
	ctrl_data->execute_once	= data.execute_once;
	ctrl_data->capture_type	= ControlCom::eCaptureDir | ControlCom::eCapturePath | ControlCom::eCaptureMovement;

	m_man->activate			(ControlCom::eControlTripleAnimation);
}

void CControlManagerCustom::ta_pointbreak()
{
	m_triple_anim->pointbreak();
}

bool CControlManagerCustom::ta_is_active()
{
	return (m_triple_anim->is_active());
}

void CControlManagerCustom::ta_deactivate()
{
	m_man->release(this, ControlCom::eControlTripleAnimation);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Работа с последовательностями
void CControlManagerCustom::seq_init()
{
	if (!m_man->check_start_conditions(ControlCom::eControlSequencer)) 
		return;

	m_man->capture				(this,	ControlCom::eControlSequencer);

	SAnimationSequencerData		*ctrl_data = (SAnimationSequencerData*)m_man->data(this, ControlCom::eControlSequencer); 
	if (!ctrl_data) return;

	ctrl_data->motions.clear	();
}

void CControlManagerCustom::seq_add(MotionID motion)
{
	if (!m_man->check_start_conditions(ControlCom::eControlSequencer)) 
		return;

	SAnimationSequencerData		*ctrl_data = (SAnimationSequencerData*)m_man->data(this, ControlCom::eControlSequencer); 
	if (!ctrl_data) return;

	ctrl_data->motions.push_back(motion);
}

void CControlManagerCustom::seq_switch()
{
	if (!m_man->check_start_conditions(ControlCom::eControlSequencer)) 
		return;

	m_man->activate(ControlCom::eControlSequencer);
}

void CControlManagerCustom::seq_run(MotionID motion)
{
	if (!m_man->check_start_conditions(ControlCom::eControlSequencer)) 
		return;

	m_man->capture				(this,	ControlCom::eControlSequencer);

	SAnimationSequencerData		*ctrl_data = (SAnimationSequencerData*)m_man->data(this, ControlCom::eControlSequencer); 
	if (!ctrl_data) return;

	ctrl_data->motions.push_back(motion);

	m_man->activate(ControlCom::eControlSequencer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Jumping
void CControlManagerCustom::jump(CObject *obj, const SControlJumpData &ta)
{
	if (!m_man->check_start_conditions(ControlCom::eControlJump)) 
		return;

	m_man->capture		(this, ControlCom::eControlJump);

	SControlJumpData	*ctrl_data = (SControlJumpData *) m_man->data(this, ControlCom::eControlJump);
	VERIFY				(ctrl_data);

	ctrl_data->target_object	= obj;
	ctrl_data->velocity_mask	= ta.velocity_mask;
	ctrl_data->target_position	= obj->Position();
	ctrl_data->skip_prepare		= ta.skip_prepare;
	ctrl_data->play_glide_once	= ta.play_glide_once;
	ctrl_data->pool[0]			= ta.pool[0];
	ctrl_data->pool[1]			= ta.pool[1];
	ctrl_data->pool[2]			= ta.pool[2];

	m_man->activate		(ControlCom::eControlJump);
}

void CControlManagerCustom::load_jump_data(LPCSTR s1, LPCSTR s2, LPCSTR s3, u32 vel_mask)
{
	// Load triple animations
	CSkeletonAnimated	*skel_animated = smart_cast<CSkeletonAnimated*>(m_object->Visual());
	m_jump->setup_data().pool[0]		= skel_animated->ID_Cycle_Safe(s1);	VERIFY(m_jump->setup_data().pool[0]);
	m_jump->setup_data().pool[1]		= skel_animated->ID_Cycle_Safe(s2);	VERIFY(m_jump->setup_data().pool[1]);
	m_jump->setup_data().pool[2]		= skel_animated->ID_Cycle_Safe(s3);	VERIFY(m_jump->setup_data().pool[2]);
	m_jump->setup_data().skip_prepare	= false;
	m_jump->setup_data().play_glide_once = true;
	m_jump->setup_data().velocity_mask	= vel_mask;
}

void CControlManagerCustom::jump(const SControlJumpData &ta)
{
	if (!m_man->check_start_conditions(ControlCom::eControlJump)) 
		return;

	m_man->capture		(this, ControlCom::eControlJump);

	SControlJumpData	*ctrl_data = (SControlJumpData *) m_man->data(this, ControlCom::eControlJump);
	VERIFY				(ctrl_data);

	ctrl_data->target_object	= ta.target_object;
	ctrl_data->velocity_mask	= ta.velocity_mask;
	ctrl_data->target_position	= ta.target_position;
	ctrl_data->skip_prepare		= ta.skip_prepare;
	ctrl_data->play_glide_once	= ta.play_glide_once;
	ctrl_data->pool[0]			= ta.pool[0];
	ctrl_data->pool[1]			= ta.pool[1];
	ctrl_data->pool[2]			= ta.pool[2];

	m_man->activate		(ControlCom::eControlJump);
}

bool CControlManagerCustom::jump(CObject *obj)
{
	if (!m_man->check_start_conditions(ControlCom::eControlJump)) return false;
	if (!m_jump->can_jump(obj)) return false;

	m_man->capture		(this, ControlCom::eControlJump);

	SControlJumpData	*ctrl_data = (SControlJumpData *) m_man->data(this, ControlCom::eControlJump);
	VERIFY				(ctrl_data);

	ctrl_data->target_object	= obj;
	ctrl_data->target_position	= obj->Position();
	ctrl_data->skip_prepare		= false;

	m_man->activate		(ControlCom::eControlJump);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Services
void CControlManagerCustom::check_attack_jump()
{
	if (!m_object->EnemyMan.get_enemy()) return;

	CEntityAlive *target = const_cast<CEntityAlive*>(m_object->EnemyMan.get_enemy());
	if (!m_jump->can_jump(target)) return;

	if (m_man->check_start_conditions(ControlCom::eControlJump)) {
		
		m_jump->setup_data().skip_prepare		= false;
		m_jump->setup_data().target_object		= target;
		m_jump->setup_data().target_position	= target->Position();

		jump(m_jump->setup_data());
	}
}

#define MAX_DIST_SUM	6.f

void CControlManagerCustom::check_jump_over_physics()
{
	if (!m_man->path_builder().is_moving_on_path()) return;
	if (!m_man->check_start_conditions(ControlCom::eControlJump)) return;

	Fvector prev_pos	= m_object->Position();
	float	dist_sum	= 0.f;

	for(u32 i = m_man->path_builder().detail().curr_travel_point_index(); i<m_man->path_builder().detail().path().size();i++) {
		const DetailPathManager::STravelPathPoint &travel_point = m_man->path_builder().detail().path()[i];

		// получить список объектов вокруг врага
		Level().ObjectSpace.GetNearest	(travel_point.position, m_object->Radius());
		xr_vector<CObject*> &tpObjects	= Level().ObjectSpace.q_nearest;

		for (u32 k=0;k<tpObjects.size();k++) {
			CPhysicsShellHolder *obj = smart_cast<CPhysicsShellHolder *>(tpObjects[k]);
			if (!obj || !obj->PPhysicsShell() || !obj->PPhysicsShell()->bActive || (obj->Radius() < 0.5f)) continue;
			if (m_object->Position().distance_to(obj->Position()) < MAX_DIST_SUM / 2) continue;

			Fvector dir = Fvector().sub(travel_point.position, m_object->Position());

			// проверка на  Field-Of-View
			float	my_h	= m_object->Direction().getH();
			float	h		= dir.getH();

			float from	= angle_normalize(my_h - deg(8));
			float to	= angle_normalize(my_h + deg(8));

			if (!is_angle_between(h, from, to)) continue;

			dir = Fvector().sub(obj->Position(), m_object->Position());

			// вычислить целевую позицию для прыжка
			Fvector target;
			obj->Center(target);
			target.y += obj->Radius();
			// --------------------------------------------------------

			m_jump->setup_data().skip_prepare		= true;
			m_jump->setup_data().target_object		= 0;
			m_jump->setup_data().target_position	= target;

			jump(m_jump->setup_data());

			return;
		}

		dist_sum += prev_pos.distance_to(travel_point.position);
		if (dist_sum > MAX_DIST_SUM) break;

		prev_pos = travel_point.position;
	}
}
