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
	m_controlled_objects.reserve	(m_max_controlled_number);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);

	if (!MotionMan.start_load_shared(SUB_CLS_ID)) return;

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimStandDamaged,	"stand_idle_dmg_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);
	MotionMan.AddAnim(eAnimEat,				"sit_eat_",				-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND);
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_dmg_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1, &inherited::get_sd()->m_fsVelocityRunFwdDamaged,	PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,			PS_STAND);
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);	
	MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);
	MotionMan.AddAnim(eAnimSleep,			"sit_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);

	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
	MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

	MotionMan.AddTransition(PS_STAND,	PS_LIE,		eAnimStandSitDown,	false);
	MotionMan.AddTransition(PS_LIE,		PS_STAND,	eAnimSitStandUp,	false);

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
				// взять под контроль
				entity->set_under_control	(this);
				entity->set_task_follow		(this);
				m_controlled_objects.push_back(const_cast<CEntityAlive *>(EnemyMan.get_enemy()));
			}
		}
	}

	// удалить мертвые объекты	
	if (!m_controlled_objects.empty()) 
		for (int i=m_controlled_objects.size()-1; i>=0;i--) {
			if (!m_controlled_objects[i]->g_Alive()) {
				m_controlled_objects[i] = m_controlled_objects.back();
				m_controlled_objects.pop_back();
			}  
		}
}

void CController::set_controlled_task(u32 task)
{
	if (!HasUnderControl()) return;
	
	const CEntity *object = ((((ETask)task) == eTaskNone) ? 0 : ((((ETask)task) == eTaskFollow) ? this : EnemyMan.get_enemy()));
	
	for	(u32 i=0; i<m_controlled_objects.size(); i++) {
		CControlledEntityBase *entity = dynamic_cast<CControlledEntityBase *>(m_controlled_objects[i]);		
		entity->get_data().m_object = object;
		entity->get_data().m_task	= (ETask)task;
	}
}

 