#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBurerAttackGraviAbstract CStateBurerAttackGravi<_Object>

#define GOOD_DISTANCE_FOR_GRAVI 6.f

TEMPLATE_SPECIALIZATION
CStateBurerAttackGraviAbstract::CStateBurerAttackGravi(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::initialize()
{
	inherited::initialize			();

	m_action						= ACTION_GRAVI_STARTED;

	time_gravi_started				= 0;

	object->set_script_capture		(false);
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::execute()
{
	switch (m_action) {
		/************************/
		case ACTION_GRAVI_STARTED:
			/************************/

			ExecuteGraviStart();
			m_action = ACTION_GRAVI_CONTINUE;

			break;
			/************************/
		case ACTION_GRAVI_CONTINUE:
			/************************/

			ExecuteGraviContinue();

			break;

			/************************/
		case ACTION_GRAVI_FIRE:
			/************************/

			ExecuteGraviFire();
			m_action = ACTION_WAIT_TRIPLE_END;

			break;
			/***************************/
		case ACTION_WAIT_TRIPLE_END:
			/***************************/

			if (!object->MotionMan.TA_IsActive()) {
				m_action = ACTION_COMPLETED; 
			}

			/*********************/
		case ACTION_COMPLETED:
			/*********************/

			break;
	}

	object->MotionMan.m_tAction	= ACT_STAND_IDLE;	
	object->DirMan.face_target	(object->EnemyMan.get_enemy(), 500);
}
TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::finalize()
{
	inherited::finalize();

	object->DeactivateShield	();
	object->set_script_capture	(true);
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::critical_finalize()
{
	inherited::critical_finalize();

	object->MotionMan.TA_Deactivate	();
	object->DeactivateShield		();
	object->StopGraviPrepare		();
	object->set_script_capture		(false);
}

TEMPLATE_SPECIALIZATION
bool CStateBurerAttackGraviAbstract::check_start_conditions()
{
	// обработать объекты
	float dist = object->Position().distance_to(object->EnemyMan.get_enemy()->Position());
	if (dist < GOOD_DISTANCE_FOR_GRAVI) return false;

	if (object->EnemyMan.get_enemy_time_last_seen() != Device.dwTimeGlobal) return false; 

	if (!object->DirMan.is_face_target(object->EnemyMan.get_enemy(), deg(30))) return false;

	// всё ок, можно начать грави атаку
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateBurerAttackGraviAbstract::check_completion()
{
	return (m_action == ACTION_COMPLETED);
}

//////////////////////////////////////////////////////////////////////////

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::ExecuteGraviStart()
{
	object->MotionMan.TA_Activate(&object->anim_triple_gravi);

	time_gravi_started			= Device.dwTimeGlobal;

	object->StartGraviPrepare();
	object->ActivateShield();
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::ExecuteGraviContinue()
{
	// проверить на грави удар
	if (time_gravi_started + object->m_gravi_time_to_hold < Device.dwTimeGlobal) {
		m_action = ACTION_GRAVI_FIRE;
	}
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::ExecuteGraviFire()
{
	object->MotionMan.TA_PointBreak();

	Fvector from_pos;
	Fvector target_pos;
	from_pos	= object->Position();	from_pos.y		+= 0.5f;
	target_pos	= object->EnemyMan.get_enemy()->Position();	target_pos.y	+= 0.5f;

	object->m_gravi_object.activate(object->EnemyMan.get_enemy(), from_pos, target_pos);

	object->StopGraviPrepare	();
	object->sound().play		(MonsterSpace::eMonsterSoundGraviAttack);
	object->DeactivateShield	();
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateBurerAttackGraviAbstract