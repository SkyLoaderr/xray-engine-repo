#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterLookToUnprotectedAreaAbstract CStateMonsterLookToUnprotectedArea<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterLookToUnprotectedAreaAbstract::CStateMonsterLookToUnprotectedArea(_Object *obj) : inherited(obj, ST_LookUnprotectedArea, &data)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterLookToUnprotectedAreaAbstract::~CStateMonsterLookToUnprotectedArea()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterLookToUnprotectedAreaAbstract::initialize()
{
	inherited::initialize();
	
	Fvector position;
	position = object->Position();
	position.y += 0.3f;

	float angle = ai().level_graph().vertex_less_cover(object->level_vertex_id(),PI_DIV_6);

	Fvector dir;
	dir.set(1.f,0.f,0.f);
	dir.setHP(angle_normalize(angle+PI), 0.f);
	dir.normalize();

	target_point.mad(object->Position(),dir, 1.f);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterLookToUnprotectedAreaAbstract::execute()
{
	object->MotionMan.m_tAction				= data.action;
	object->MotionMan.SetSpecParams			(data.spec_params);
	object->FaceTarget						(target_point);

	if (data.sound_type != u32(-1)) {
		if (data.sound_delay != u32(-1))
			object->CSoundPlayer::play(data.sound_type, 0,0,data.sound_delay);
		else 
			object->CSoundPlayer::play(data.sound_type);
	}

}

TEMPLATE_SPECIALIZATION
bool CStateMonsterLookToUnprotectedAreaAbstract::check_completion()
{	
	if (data.time_out !=0) {
		if (time_state_started + data.time_out < object->m_current_update) return true;
	} else 	if (angle_difference(object->m_body.current.yaw, object->m_body.target.yaw) < deg(1)) return true;

	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterLookToUnprotectedAreaAbstract
