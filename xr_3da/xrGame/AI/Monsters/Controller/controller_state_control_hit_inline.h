#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateControllerControlHitAbstract CStateControlAttack<_Object>

#define GOOD_DISTANCE_FOR_CONTROL_HIT	8.f
#define CONTROL_PREPARE_TIME			2900

TEMPLATE_SPECIALIZATION
CStateControllerControlHitAbstract::CStateControlAttack(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateControllerControlHitAbstract::~CStateControlAttack()
{
}

TEMPLATE_SPECIALIZATION
void CStateControllerControlHitAbstract::initialize()
{
	inherited::initialize();
	
	m_action				= eActionPrepare;
	time_control_started	= 0;
}

TEMPLATE_SPECIALIZATION
void CStateControllerControlHitAbstract::execute()
{
	switch (m_action) {
		case eActionPrepare:
			execute_hit_prepare();
			m_action = eActionContinue;
			break;

		case eActionContinue:
			execute_hit_continue();
			break;

		case eActionFire:
			execute_hit_fire();
			m_action = eActionWaitTripleEnd;
			break;

		case eActionWaitTripleEnd:
			if (!object->MotionMan.TA_IsActive()) {
				m_action = eActionCompleted; 
			}

		case eActionCompleted:
			break;
	}

	object->MotionMan.m_tAction = ACT_STAND_IDLE;	
	object->DirMan.face_target(object->EnemyMan.get_enemy(), 1200);

	object->sound().play(MonsterSpace::eMonsterSoundAttack, 0,0,object->get_sd()->m_dwAttackSndDelay);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Attack :: Control Hit", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif

}

TEMPLATE_SPECIALIZATION
bool CStateControllerControlHitAbstract::check_start_conditions()
{
	float dist = object->Position().distance_to(object->EnemyMan.get_enemy_position());
	if (dist < GOOD_DISTANCE_FOR_CONTROL_HIT) return false;

	if (!object->EnemyMan.see_enemy_now()) return false; 

	// всё ок, можно начать атаку
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateControllerControlHitAbstract::check_completion()
{
	return (m_action == eActionCompleted);
}

TEMPLATE_SPECIALIZATION
void CStateControllerControlHitAbstract::finalize()
{
	inherited::finalize();
}

TEMPLATE_SPECIALIZATION
void CStateControllerControlHitAbstract::critical_finalize()
{
	inherited::critical_finalize();
	
	object->MotionMan.TA_Deactivate();
}

//////////////////////////////////////////////////////////////////////////
// Processing
//////////////////////////////////////////////////////////////////////////


TEMPLATE_SPECIALIZATION
void CStateControllerControlHitAbstract::execute_hit_prepare()
{
	object->MotionMan.TA_Activate(&object->anim_triple_control);
	object->play_control_sound_start();

	time_control_started = object->m_current_update;
}

TEMPLATE_SPECIALIZATION
void CStateControllerControlHitAbstract::execute_hit_continue()
{
	// проверить на грави удар
	if (time_control_started + CONTROL_PREPARE_TIME < object->m_current_update) {
		m_action = eActionFire;
	}
}

TEMPLATE_SPECIALIZATION
void CStateControllerControlHitAbstract::execute_hit_fire()
{
	object->MotionMan.TA_PointBreak();
	
	if (object->EnemyMan.see_enemy_now()) object->control_hit();
}

