#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateControlMoveOutAbstract CStateControlMoveOut<_Object>

#define MAX_STATE_TIME 10000

TEMPLATE_SPECIALIZATION
void CStateControlMoveOutAbstract::initialize()
{
	inherited::initialize			();

	object->path().prepare_builder	();
	m_time_started					= time();
}

TEMPLATE_SPECIALIZATION
void CStateControlMoveOutAbstract::execute()
{
	object->path().set_target_point			(Level().CurrentEntity()->Position());
	object->path().set_generic_parameters	();

	object->anim().m_tAction				= ACT_STEAL;
	object->anim().accel_deactivate			();
	object->anim().accel_set_braking		(false);
	object->set_state_sound					(MonsterSpace::eMonsterSoundAttack);

	// look into very open point where we go
	object->set_look_point					(Level().CurrentEntity()->Position());
}

TEMPLATE_SPECIALIZATION
bool CStateControlMoveOutAbstract::check_start_conditions()
{
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateControlMoveOutAbstract::check_completion()
{
	if (m_time_started + 5000 < time()) return true;
	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateControlMoveOutAbstract
