#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateControllerControlHitAbstract CStateControlAttack<_Object>

#define GOOD_DISTANCE_FOR_CONTROL_HIT	5.f
#define CONTROL_PREPARE_TIME			3500

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
	time_enemy_last_faced	= 0;
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
	face_enemy();
}

TEMPLATE_SPECIALIZATION
bool CStateControllerControlHitAbstract::check_start_conditions()
{
	float dist = object->Position().distance_to(object->EnemyMan.get_enemy_position());
	if (dist < GOOD_DISTANCE_FOR_CONTROL_HIT) return false;

	if (!object->EnemyMan.see_enemy_now()) return false; 

	// �� ��, ����� ������ �����
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

TEMPLATE_SPECIALIZATION
void CStateControllerControlHitAbstract::face_enemy()
{
	if (time_enemy_last_faced + 1200 < object->m_current_update) {
		object->FaceTarget(object->EnemyMan.get_enemy());
		time_enemy_last_faced = object->m_current_update;
	}
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
	// ��������� �� ����� ����
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

