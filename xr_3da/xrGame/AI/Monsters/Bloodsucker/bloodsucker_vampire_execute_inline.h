#pragma once

#include "../../../../skeletoncustom.h"
#include "../../../actor.h"
#include "../../../../CameraBase.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBloodsuckerVampireExecuteAbstract CStateBloodsuckerVampireExecute<_Object>

#define VAMPIRE_TIME_HOLD		4000
#define VAMPIRE_HIT_IMPULSE		400.f
#define VAMPIRE_MIN_DIST		0.5f
#define VAMPIRE_MAX_DIST		1.5f

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::initialize()
{
	inherited::initialize					();

	object->CControlledActor::take_control	();

	CKinematics *pK = smart_cast<CKinematics*>(object->Visual());
	Fmatrix bone_transform;
	bone_transform = pK->LL_GetTransform(pK->LL_BoneID("bip01_head"));	

	Fmatrix global_transform;
	global_transform.set(object->XFORM());
	global_transform.mulB(bone_transform);

	object->CControlledActor::look_point	(2.0f, global_transform.c);

	m_action				= eActionPrepare;
	time_vampire_started	= 0;

	object->CInvisibility::manual_deactivate();

	m_effector_activated			= false;
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::execute()
{
	if (!object->CControlledActor::is_turning() && !m_effector_activated) {
		float dist = object->EnemyMan.get_enemy()->Position().distance_to(object->Position());

		object->ActivateVampireEffector	(_abs(dist - 0.6f));
		m_effector_activated			= true;
	}
	
	
	CKinematics *pK = smart_cast<CKinematics*>(object->Visual());
	Fmatrix bone_transform;
	bone_transform = pK->LL_GetTransform(pK->LL_BoneID("bip01_head"));	

	Fmatrix global_transform;
	global_transform.set(object->XFORM());
	global_transform.mulB(bone_transform);

	if (object->CControlledActor::is_controlled())
		object->CControlledActor::update_look_point(global_transform.c);

	switch (m_action) {
		case eActionPrepare:
			execute_vampire_prepare();
			m_action = eActionContinue;
			break;

		case eActionContinue:
			execute_vampire_continue();
			break;

		case eActionFire:
			execute_vampire_hit();
			m_action = eActionWaitTripleEnd;
			break;

		case eActionWaitTripleEnd:
			if (!object->MotionMan.TA_IsActive()) {
				m_action = eActionCompleted; 
			}

		case eActionCompleted:
			break;
	}

	object->set_action			(ACT_STAND_IDLE);
	object->DirMan.face_target	(object->EnemyMan.get_enemy(), 1200);
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::finalize()
{
	inherited::finalize();

	object->CControlledActor::free_from_control	();
	object->CInvisibility::manual_activate		();
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::critical_finalize()
{
	inherited::critical_finalize();

	object->CControlledActor::free_from_control	();
	object->MotionMan.TA_Deactivate				();
	object->CInvisibility::manual_activate		();
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireExecuteAbstract::check_start_conditions()
{
	const CEntityAlive	*enemy = object->EnemyMan.get_enemy();
	
	// ��������� ���������
	float dist		= object->MeleeChecker.distance_to_enemy	(enemy);
	if ((dist > VAMPIRE_MAX_DIST) || (dist < VAMPIRE_MIN_DIST))	return false;

	// ��������� ����������� �� �����
	if (!object->DirMan.is_face_target(enemy, PI_DIV_6)) return false;

	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireExecuteAbstract::check_completion()
{
	return (m_action == eActionCompleted);
}

//////////////////////////////////////////////////////////////////////////

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::execute_vampire_prepare()
{
	object->MotionMan.TA_Activate	(&object->anim_triple_vampire);
	time_vampire_started			= object->m_current_update;
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::execute_vampire_continue()
{
	// ��������� �� ����� ����
	if (time_vampire_started + VAMPIRE_TIME_HOLD < object->m_current_update) {
		m_action = eActionFire;
	}
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::execute_vampire_hit()
{
	object->MotionMan.TA_PointBreak				();
}

//////////////////////////////////////////////////////////////////////////


#undef TEMPLATE_SPECIALIZATION
#undef CStateBloodsuckerVampireExecuteAbstract

