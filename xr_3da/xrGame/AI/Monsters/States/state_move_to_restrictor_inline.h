#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterMoveToRestrictorAbstract CStateMonsterMoveToRestrictor<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToRestrictorAbstract::initialize()
{
	inherited::initialize();
	object->path().prepare_builder();	
}
TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToRestrictorAbstract::execute()
{
	object->set_action					(ACT_RUN);
	
	object->path().set_target_point		(random_position(object->Position(), 10.f));
	object->anim().accel_activate		(EAccelType(eAT_Aggressive));
	object->anim().accel_set_braking	(false);
	object->set_state_sound				(MonsterSpace::eMonsterSoundIdle);
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterMoveToRestrictorAbstract::check_start_conditions()
{
	return (!object->control().path_builder().accessible(object->Position()));
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterMoveToRestrictorAbstract::check_completion()
{
	return (object->control().path_builder().accessible(object->Position()));
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterMoveToRestrictorAbstract