#include "stdafx.h"
#include "ai_biting.h"
#include "../../ai_script_actions.h"
#include "../../phmovementcontrol.h"

//////////////////////////////////////////////////////////////////////////
bool CAI_Biting::bfAssignMovement (CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignMovement(tpEntityAction))
		return		(false);

	CMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	// translate script.action into MotionMan.action
	switch (l_tMovementAction.m_tMoveAction) {
	case eMA_WalkFwd:	MotionMan.m_tAction = ACT_WALK_FWD;		break;
	case eMA_WalkBkwd:	MotionMan.m_tAction = ACT_WALK_BKWD;	break;
	case eMA_Run:		MotionMan.m_tAction = ACT_RUN;			break;
	case eMA_Drag:		MotionMan.m_tAction = ACT_DRAG;			break;
	case eMA_Jump:		MotionMan.m_tAction = ACT_JUMP;			break;
	case eMA_Steal:		MotionMan.m_tAction = ACT_STEAL;		break;
	}

	TranslateActionToVelocityMasks(l_tMovementAction.m_tSpeedParam == eSP_ForceSpeed);
	CMonsterMovement::set_path_targeted();
		
	return			(true);		
}

//////////////////////////////////////////////////////////////////////////

bool CAI_Biting::bfAssignObject(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignObject(tpEntityAction))
		return	(false);

//	CObjectAction	&l_tObjectAction = tpEntityAction->m_tObjectAction;
//	if (!l_tObjectAction.m_tpObject)
//		return	(false == (l_tObjectAction.m_bCompleted = true));
//
//	CEntityAlive	*l_tpEntity		= dynamic_cast<CEntityAlive*>(l_tObjectAction.m_tpObject);
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


bool CAI_Biting::bfAssignWatch(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignWatch(tpEntityAction))
		return		(false);
	
	CWatchAction	&l_tWatchAction = tpEntityAction->m_tWatchAction;

	Fvector new_pos;
	switch (l_tWatchAction.m_tWatchType) {
		case MonsterSpace::eLookTypePoint:
			LookPosition(l_tWatchAction.m_tWatchVector);
			break;
		case MonsterSpace::eLookTypeDirection:
			new_pos.mad(Position(), l_tWatchAction.m_tWatchVector, 2.f);
			LookPosition(new_pos);
			break;
	}

	if (angle_difference(m_body.target.yaw,m_body.current.yaw) < EPS_L)
		l_tWatchAction.m_bCompleted = true;
	else
		l_tWatchAction.m_bCompleted = false;
	
	return		(!l_tWatchAction.m_bCompleted);
}

bool CAI_Biting::bfAssignAnimation(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignAnimation(tpEntityAction))
		return			(false);

	CAnimationAction	&l_tAnimAction	= tpEntityAction->m_tAnimationAction;
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
	case eAA_Turn:			MotionMan.m_tAction = ACT_TURN;			break;
	}
	
	return				(true);
}

bool CAI_Biting::bfAssignMonsterAction(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignMonsterAction(tpEntityAction)) return false;
	
	CMonsterAction	&l_tAction = tpEntityAction->m_tMonsterAction;	

	switch(l_tAction.m_tAction) {
		case eGA_Rest:		
			SetState(stateRest, true);	
			break;
		case eGA_Eat:		
			SetState(stateEat, true);	
			SetForcedCorpse(dynamic_cast<CEntity *>(l_tAction.m_tObject));
			break;
		case eGA_Attack:
			SetState(stateAttack, true);	
			SetForcedEnemy(dynamic_cast<CEntity *>(l_tAction.m_tObject));
			break;
		case eGA_Panic:		
			SetState(statePanic, true);
			SetForcedEnemy(dynamic_cast<CEntity *>(l_tAction.m_tObject));
			break;
	}
	
	if (CurrentState->CheckCompletion()) l_tAction.m_bCompleted = true;
	else l_tAction.m_bCompleted = false;

	b_script_state_must_execute = true;
	return (!l_tAction.m_bCompleted);
}



void CAI_Biting::ProcessScripts()
{
	// »нициализировать параметры движени€
	CMonsterMovement::Frame_Init				();
	
	// ¬ыполнить скриптовые actions
	b_script_state_must_execute					= false;
	inherited::ProcessScripts					();
	
	// обновить мир (пам€ть, враги, объекты)
	vfUpdateParameters							();
	
	// если из скрипта выбрано действие по универсальной схеме, выполнить его
	if (b_script_state_must_execute && !CurrentState->CheckCompletion())			
		CurrentState->Execute					(m_current_update);
	
	// ”далить все враги и объекты, которые были принудительно установлены
	// во врем€ выполнени€ скриптового действи€
	ClearForcedEntities							();
	
	// ¬ыбрать анимацию и соотв. параметры согласно текущему действию
	MotionMan.ProcessAction						();

	// Backend processing
	// получена анимаци€ - проверить состо€ни€ в соотв. с данной анимацией
	
	// ќбновить параметры пути
	UpdatePathWithAction						();

	// обновить путь
	CMonsterMovement::Frame_Update				();
	
	// по анимации и пути установить скорости движени€
	MotionMan.FinalizeProcessing					();

	// ќбновить скорости движени€ в соответствии с текущим путем и анимацией
	if (CMonsterMovement::is_path_targeted())
		UpdateTargetVelocityWithPath			();

	// установить текущую скорость
	CMonsterMovement::Frame_Finalize			();

	if (!CDetailPathManager::path().empty()) {
		CDetailPathManager::STravelPathPoint point = CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index()];
		xr_map<u32,CDetailPathManager::STravelParams>::iterator it = m_movement_params.find(point.velocity);
		R_ASSERT(it != m_movement_params.end());
		Msg("Cur index = [%u] velocity index = [%u] :: linear = [%f], angular = [%f] ", CDetailPathManager::curr_travel_point_index(), point.velocity, it->second.linear_velocity, it->second.angular_velocity);
	}

	Msg("speeds: linear[%f] angular[%f]", m_velocity_linear.current, m_velocity_angular.current);
}

CEntity *CAI_Biting::GetCurrentEnemy()
{
	VisionElem ve;
	if (GetEnemy(ve)) return (const_cast<CEntity *>(ve.obj));
	else return (0);
}
CEntity *CAI_Biting::GetCurrentCorpse()
{
	VisionElem ve;
	
	if (!GetEnemy(ve)) {
		GetCorpse(ve);
		return (const_cast<CEntity *>(ve.obj));
	} else return (0);
}

