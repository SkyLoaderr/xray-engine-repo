#include "stdafx.h"
#include "controller.h"
#include "../../ai_monster_utils.h"
#include "controller_state_manager.h"
#include "../../controlled_entity.h"

CController::CController()
{
	StateMan = xr_new<CStateManagerController>(this);
}

CController::~CController()
{
	xr_delete(StateMan);
}

void CController::Load(LPCSTR section)
{
	inherited::Load	(section);

	m_max_controlled_number			= pSettings->r_u8(section,"Max_Controlled_Count");
	m_controlled_objects.resize		(m_max_controlled_number);


	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);

	if (!MotionMan.start_load_shared(SUB_CLS_ID)) return;

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND);

	MotionMan.AddAnim(eAnimLieIdle,			"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);
	MotionMan.AddAnim(eAnimSleep,			"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);

	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND);

	MotionMan.AddAnim(eAnimLookAround,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);

	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun	, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_EAT,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6/6);
	MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, EPS_S); 

	MotionMan.finish_load_shared();
}

bool CController::UpdateStateManager()
{
	UpdateControlled	();
	StateMan->execute	();
	return true;
}


void CController::UpdateControlled()
{
	// если есть враг, проверить может ли быть враг взят под контроль
	if (EnemyMan.get_enemy()) {
		CControlledEntityBase *entity = dynamic_cast<CControlledEntityBase *>(const_cast<CEntityAlive *>(EnemyMan.get_enemy()));
		if (entity) {
			if (!entity->is_under_control() && (m_controlled_objects.size() < m_max_controlled_number)) {
				entity->set_under_control	(this);
				entity->set_task_follow		(this);
			}
		}
	}

	// удалить мертвые объекты
	for (u32 i=m_controlled_objects.size()-1; i>=0;i--) {
		if (!m_controlled_objects[i]->g_Alive()) {
			m_controlled_objects[i] = m_controlled_objects.back();
			m_controlled_objects.pop_back();
		}
	}

	// обновить цели подконтрольных монстров
	// [...]
}

