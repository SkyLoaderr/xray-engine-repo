#pragma once

#include "../../../clsid_game.h"
#include "../../../../skeletoncustom.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBloodsuckerVampireAbstract CStateBloodsuckerVampire<_Object>

#define VAMPIRE_TIME_HOLD		2000
#define VAMPIRE_HIT_IMPULSE		400.f
#define VAMPIRE_MIN_DIST		1.f
#define VAMPIRE_MAX_DIST		2.f

TEMPLATE_SPECIALIZATION
CStateBloodsuckerVampireAbstract::CStateBloodsuckerVampire(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateBloodsuckerVampireAbstract::~CStateBloodsuckerVampire()
{
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::initialize()
{
	inherited::initialize					();
	
	object->CControlledActor::take_control	();
	
	CKinematics *pK = smart_cast<CKinematics*>(object->Visual());
	Fmatrix bone_transform;
	bone_transform = pK->LL_GetTransform(pK->LL_BoneID("bip01_head"));	

	Fmatrix global_transform;
	global_transform.set(object->XFORM());
	global_transform.mulB(bone_transform);
	
	object->CControlledActor::look_point	(5.f, global_transform.c);


	m_action				= eActionPrepare;
	time_vampire_started	= 0;
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::execute()
{
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


#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0, "State Vampire", D3DCOLOR_XRGB(255,0,0));
	}
#endif

}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::finalize()
{
	inherited::finalize();
	
	object->CControlledActor::free_from_control();
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::critical_finalize()
{
	inherited::critical_finalize();
	
	object->CControlledActor::free_from_control();
	object->MotionMan.TA_Deactivate				();
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireAbstract::check_start_conditions()
{
	// �������� �� ���� �������
	const CEntityAlive *enemy = object->EnemyMan.get_enemy();
	if (enemy->SUB_CLS_ID != CLSID_OBJECT_ACTOR) return false;
	
	// ��������� ���������
	float dist		= object->MeleeChecker.distance_to_enemy	(enemy);
	if ((dist > VAMPIRE_MAX_DIST) || (dist < VAMPIRE_MIN_DIST))	return false;

	// ��������� ����������� �� �����
	if (!object->DirMan.is_face_target(enemy, PI_DIV_6)) return false;

	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireAbstract::check_completion()
{
	return (m_action == eActionCompleted);
}

//////////////////////////////////////////////////////////////////////////

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::execute_vampire_prepare()
{
	object->MotionMan.TA_Activate	(&object->anim_triple_vampire);
	time_vampire_started			= object->m_current_update;
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::execute_vampire_continue()
{
	// ��������� �� ����� ����
	if (time_vampire_started + VAMPIRE_TIME_HOLD < object->m_current_update) {
		m_action = eActionFire;
	}
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::execute_vampire_hit()
{
	object->MotionMan.TA_PointBreak();
	
	const CEntityAlive *enemy = object->EnemyMan.get_enemy();

	Fvector				dir;
	dir.sub				(enemy->Position(), object->Position());
	
	float				h,p;
	dir.getHP			(h,p);
	p					+= PI_DIV_6;
	dir.setHP			(h,p);
	dir.normalize_safe	();
	
	object->HitEntity	(enemy, 0.01f, VAMPIRE_HIT_IMPULSE, dir);
}

//////////////////////////////////////////////////////////////////////////


#undef TEMPLATE_SPECIALIZATION
#undef CStateBloodsuckerVampireAbstract

