#include "stdafx.h"
#include "base_monster.h"
#include "../../../script_entity_action.h"
#include "../../../phmovementcontrol.h"
#include "../../../sight_manager.h"
#include "../ai_monster_debug.h"
#include "../../../detail_path_manager.h"
#include "../../../ai_object_location.h"
#include "../ai_monster_movement.h"
#include "../../../sound_player.h"

//////////////////////////////////////////////////////////////////////////
bool CBaseMonster::bfAssignMovement (CScriptEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignMovement(tpEntityAction))
		return		(false);

	CScriptMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	if (l_tMovementAction.completed()) return false;

	if (movement().detail().time_path_built() >= tpEntityAction->m_tActionCondition.m_tStartTime) {
		if ((l_tMovementAction.m_fDistToEnd > 0) && movement().IsPathEnd(l_tMovementAction.m_fDistToEnd))  {
			l_tMovementAction.m_bCompleted = true;
			return false;
		}
	}


	// translate script.action into MotionMan.action
	switch (l_tMovementAction.m_tMoveAction) {
	case eMA_WalkFwd:	MotionMan.m_tAction = ACT_WALK_FWD;		break;
	case eMA_WalkBkwd:	MotionMan.m_tAction = ACT_WALK_BKWD;	break;
	case eMA_Run:		MotionMan.m_tAction = ACT_RUN;			break;
	case eMA_Drag:		MotionMan.m_tAction = ACT_DRAG;			break;
	case eMA_Steal:		MotionMan.m_tAction = ACT_STEAL;		break;
	}

	movement().set_path_targeted();
	
	force_real_speed = (l_tMovementAction.m_tSpeedParam == eSP_ForceSpeed);

	switch (l_tMovementAction.m_tGoalType) {
		
		case CScriptMovementAction::eGoalTypeObject : {
			CGameObject		*l_tpGameObject = smart_cast<CGameObject*>(l_tMovementAction.m_tpObjectToGo);
			movement().set_target_point	(l_tpGameObject->Position(), l_tpGameObject->ai_location().level_vertex_id());
			break;
													  }
		case CScriptMovementAction::eGoalTypePathPosition :
		case CScriptMovementAction::eGoalTypeNoPathPosition :
			movement().set_target_point	(l_tMovementAction.m_tDestinationPosition);
			break;
	}
	return			(true);		
}

//////////////////////////////////////////////////////////////////////////

bool CBaseMonster::bfAssignObject(CScriptEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignObject(tpEntityAction))
		return	(false);

//	CScriptObjectAction	&l_tObjectAction = tpEntityAction->m_tObjectAction;
//	if (!l_tObjectAction.m_tpObject)
//		return	(false == (l_tObjectAction.m_bCompleted = true));
//
//	CEntityAlive	*l_tpEntity		= smart_cast<CEntityAlive*>(l_tObjectAction.m_tpObject);
//	if (!l_tpEntity) return	(false == (l_tObjectAction.m_bCompleted = true));
//
//	switch (l_tObjectAction.m_tGoalType) {
//		case eObjectActionTake: 
//			m_PhysicMovementControl->PHCaptureObject(l_tpEntity);
//			break;
//		case eObjectActionDrop: 
//			m_PhysicMovementControl->PHReleaseObject();
//			break;
//	}
//	
//	l_tObjectAction.m_bCompleted = true;
	return	(true);
}


bool CBaseMonster::bfAssignWatch(CScriptEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignWatch(tpEntityAction))
		return		(false);
	
	CScriptWatchAction	&l_tWatchAction = tpEntityAction->m_tWatchAction;
	if (l_tWatchAction.completed()) return false;

	Fvector new_pos;
	switch (l_tWatchAction.m_tWatchType) {
		case SightManager::eSightTypePosition:
			LookPosition(l_tWatchAction.m_tWatchVector);
			break;
		case SightManager::eSightTypeDirection:
			new_pos.mad(Position(), l_tWatchAction.m_tWatchVector, 2.f);
			LookPosition(new_pos);
			break;
	}


	if (angle_difference(movement().m_body.target.yaw,movement().m_body.current.yaw) < deg(2))
		l_tWatchAction.m_bCompleted = true;
	else
		l_tWatchAction.m_bCompleted = false;
	
	return		(!l_tWatchAction.m_bCompleted);
}

bool CBaseMonster::bfAssignAnimation(CScriptEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignAnimation(tpEntityAction))
		return			(false);

	CScriptAnimationAction	&l_tAnimAction	= tpEntityAction->m_tAnimationAction;
	if (l_tAnimAction.completed()) return false;
	
	// translate animation.action into MotionMan.action
	switch (l_tAnimAction.m_tAnimAction) {
	case eAA_StandIdle:		MotionMan.m_tAction = ACT_STAND_IDLE;	break;
	case eAA_SitIdle:		MotionMan.m_tAction = ACT_SIT_IDLE;		break;
	case eAA_LieIdle:		MotionMan.m_tAction = ACT_LIE_IDLE;		break;
	case eAA_Eat:			MotionMan.m_tAction = ACT_EAT;			break;
	case eAA_Sleep:			MotionMan.m_tAction = ACT_SLEEP;		break;
	case eAA_Rest:			MotionMan.m_tAction = ACT_REST;			break;
	case eAA_Attack:		MotionMan.m_tAction = ACT_ATTACK;		break;
	case eAA_LookAround:	MotionMan.m_tAction = ACT_LOOK_AROUND;	break;
	}

	return				(true);
}

bool CBaseMonster::bfAssignSound(CScriptEntityAction *tpEntityAction)
{
	CScriptSoundAction	&l_tAction = tpEntityAction->m_tSoundAction;
	if (l_tAction.completed()) return false;

	if (l_tAction.m_monster_sound == MonsterSpace::eMonsterSoundDummy) {
		if (!inherited::bfAssignSound(tpEntityAction))
			return			(false);
	}

	switch (l_tAction.m_monster_sound) {
	case	eMonsterSoundIdle:			sound().play(eMonsterSoundIdle,		0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? get_sd()->m_dwIdleSndDelay		: l_tAction.m_monster_sound_delay);		break;
	case 	eMonsterSoundEat:			sound().play(eMonsterSoundEat,		0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? get_sd()->m_dwEatSndDelay		: l_tAction.m_monster_sound_delay);		break;
	case 	eMonsterSoundAttack:		sound().play(eMonsterSoundAttack,		0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? get_sd()->m_dwAttackSndDelay	: l_tAction.m_monster_sound_delay);		break;
	case	eMonsterSoundAttackHit:		sound().play(eMonsterSoundAttackHit);		break;
	case	eMonsterSoundTakeDamage:	sound().play(eMonsterSoundTakeDamage);	break;
	case	eMonsterSoundDie:			sound().play(eMonsterSoundDie);			break;
	case	eMonsterSoundThreaten:		sound().play(eMonsterSoundThreaten,	0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? get_sd()->m_dwAttackSndDelay : l_tAction.m_monster_sound_delay);		break;
	case	eMonsterSoundSteal:			sound().play(eMonsterSoundSteal,		0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? get_sd()->m_dwAttackSndDelay : l_tAction.m_monster_sound_delay);		break;
	case	eMonsterSoundPanic:			sound().play(eMonsterSoundPanic,		0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? get_sd()->m_dwAttackSndDelay : l_tAction.m_monster_sound_delay);		break;
	case	eMonsterSoundGrowling:		sound().play(eMonsterSoundGrowling,	0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? get_sd()->m_dwAttackSndDelay : l_tAction.m_monster_sound_delay);		break;
	}

	return				(true);
}

bool CBaseMonster::bfAssignMonsterAction(CScriptEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignMonsterAction(tpEntityAction)) return false;
	
	CScriptMonsterAction	&l_tAction = tpEntityAction->m_tMonsterAction;	
	if (l_tAction.completed()) return false;

	CEntityAlive *pE = smart_cast<CEntityAlive *>(l_tAction.m_tObject);

	switch(l_tAction.m_tAction) {
		case eGA_Rest:		
			StateMan->force_script_state(eStateRest);	
			break;
		case eGA_Eat:		
			if (pE && !pE->getDestroy() && !pE->g_Alive()){
				CorpseMan.force_corpse(pE);
				StateMan->force_script_state(eStateEat);	
			} else StateMan->force_script_state(eStateRest);	

			break;
		case eGA_Attack:
			if (pE && !pE->getDestroy() && pE->g_Alive()){
				EnemyMan.force_enemy(pE);
				StateMan->force_script_state(eStateAttack);
			} else StateMan->force_script_state(eStateRest);

			break;
		case eGA_Panic:		
			if (pE && !pE->getDestroy() && pE->g_Alive()){
				EnemyMan.force_enemy			(pE);
				StateMan->force_script_state	(eStatePanic);
			} else StateMan->force_script_state	(eStateRest);	
			break;
	}

	b_script_state_must_execute = true;
	return (!l_tAction.m_bCompleted);
}



void CBaseMonster::ProcessScripts()
{
	if (!g_Alive()) return;
	if (script_processing_active) return;
	
	script_processing_active = true;

	// Инициализировать action
	MotionMan.m_tAction = ACT_STAND_IDLE;

	movement().Update_Initialize			();
	
	// Выполнить скриптовые actions
	b_script_state_must_execute					= false;
	inherited::ProcessScripts					();
	
	m_current_update							= Device.dwTimeGlobal;

	// обновить мир (память, враги, объекты)
	vfUpdateParameters							();
	
	MotionMan.accel_deactivate					();

	// если из скрипта выбрано действие по универсальной схеме, выполнить его
	if (b_script_state_must_execute) 	
		StateMan->execute_script_state			();		
	
	TranslateActionToPathParams					();

	// обновить путь
	movement().Update_Execute			();

	MotionMan.Update							();
	
	// установить текущую скорость
	movement().Update_Finalize			();

	// Удалить все враги и объекты, которые были принудительно установлены
	// во время выполнения скриптового действия
	if (b_script_state_must_execute) {
		EnemyMan.unforce_enemy();
		CorpseMan.unforce_corpse();
	}

	force_real_speed	= false;

	// Debuging
#ifdef DEBUG
	HDebug->SetActive						(true);
#endif


	script_processing_active = false;
}

CEntity *CBaseMonster::GetCurrentEnemy()
{
	CEntity *enemy = 0;
	
	if (EnemyMan.get_enemy()) 
		enemy = const_cast<CEntity *>(smart_cast<const CEntity*>(EnemyMan.get_enemy()));

	if (!enemy || enemy->getDestroy() || !enemy->g_Alive()) enemy = 0;

	return (enemy);
}

CEntity *CBaseMonster::GetCurrentCorpse()
{
	CEntity *corpse = 0;

	if (CorpseMan.get_corpse()) 
		corpse = const_cast<CEntity *>(smart_cast<const CEntity*>(CorpseMan.get_corpse()));

	if (!corpse || corpse->getDestroy() || corpse->g_Alive()) corpse = 0;

	return (corpse);
}

void CBaseMonster::SetScriptControl(const bool bScriptControl, shared_str caScriptName)
{
	//if (CurrentState) CurrentState->Done();

	CScriptEntity::SetScriptControl(bScriptControl, caScriptName);
}


int	CBaseMonster::get_enemy_strength()
{
	if (EnemyMan.get_enemy()) {
		switch (EnemyMan.get_danger_type()) {
			case eVeryStrong	: 	return (4);
			case eStrong		: 	return (3);
			case eNormal		: 	return (2);
			case eWeak			:	return (1);
		}
	}
	
	return (0);
}

void CBaseMonster::vfFinishAction(CScriptEntityAction *tpEntityAction)
{
	inherited::vfFinishAction(tpEntityAction);
	MotionMan.ForceAnimSelect();
}

