#include "stdafx.h"
#include "ai_biting.h"
#include "../../ai_script_actions.h"
#include "../../phmovementcontrol.h"
#include "../../sight_manager.h"

//////////////////////////////////////////////////////////////////////////
bool CAI_Biting::bfAssignMovement (CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignMovement(tpEntityAction))
		return		(false);

	CScriptMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	// translate script.action into MotionMan.action
	switch (l_tMovementAction.m_tMoveAction) {
	case eMA_WalkFwd:	MotionMan.m_tAction = ACT_WALK_FWD;		break;
	case eMA_WalkBkwd:	MotionMan.m_tAction = ACT_WALK_BKWD;	break;
	case eMA_Run:		MotionMan.m_tAction = ACT_RUN;			break;
	case eMA_Drag:		MotionMan.m_tAction = ACT_DRAG;			break;
	case eMA_Jump:		MotionMan.m_tAction = ACT_JUMP;			break;
	case eMA_Steal:		MotionMan.m_tAction = ACT_STEAL;		break;
	}

	CMonsterMovement::set_path_targeted();
		
	return			(true);		
}

//////////////////////////////////////////////////////////////////////////

bool CAI_Biting::bfAssignObject(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignObject(tpEntityAction))
		return	(false);

//	CScriptObjectAction	&l_tObjectAction = tpEntityAction->m_tObjectAction;
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
	
	CScriptWatchAction	&l_tWatchAction = tpEntityAction->m_tWatchAction;

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

	CScriptAnimationAction	&l_tAnimAction	= tpEntityAction->m_tAnimationAction;
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
	// ���������������� ��������� ��������
	CMonsterMovement::Frame_Init				();
	
	// ��������� ���������� actions
	b_script_state_must_execute					= false;
	inherited::ProcessScripts					();
	
	m_dwLastUpdateTime						= m_current_update;
	m_current_update						= Level().timeServer();

	// �������� ��� (������, �����, �������)
	vfUpdateParameters							();
	

	MotionMan.accel_deactivate					();

	// ���� �� ������� ������� �������� �� ������������� �����, ��������� ���
	if (b_script_state_must_execute && !CurrentState->CheckCompletion())			
		CurrentState->Execute					(m_current_update);
	
	TranslateActionToPathParams					();

	// �������� ����
	CMonsterMovement::Frame_Update				();

	MotionMan.Update							();
	
	// ���������� ������� ��������
	CMonsterMovement::Frame_Finalize			();

	// ������� ��� ����� � �������, ������� ���� ������������� �����������
	// �� ����� ���������� ����������� ��������
	ClearForcedEntities							();
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


void CAI_Biting::TranslateActionToPathParams()
{
	bool bEnablePath = true;
	u32 vel_mask = 0;
	u32 des_mask = 0;

	switch (MotionMan.m_tAction) {
	case ACT_STAND_IDLE: 
	case ACT_SIT_IDLE:	 
	case ACT_LIE_IDLE:
	case ACT_EAT:
	case ACT_SLEEP:
	case ACT_REST:
	case ACT_LOOK_AROUND:
	case ACT_ATTACK:
	case ACT_TURN:
		bEnablePath = false;
		break;

	case ACT_WALK_FWD:
		if (m_bDamaged) {
			vel_mask = eVelocityParamsWalkDamaged;
			des_mask = eVelocityParameterWalkDamaged;
		} else {
			vel_mask = eVelocityParamsWalk;
			des_mask = eVelocityParameterWalkNormal;
		}
		break;
	case ACT_WALK_BKWD:
		break;
	case ACT_RUN:
		if (m_bDamaged) {
			vel_mask = eVelocityParamsRunDamaged;
			des_mask = eVelocityParameterRunDamaged;
		} else {
			vel_mask = eVelocityParamsRun;
			des_mask = eVelocityParameterRunNormal;
		}
		break;
	case ACT_DRAG:
		vel_mask = eVelocityParameterDrag;
		des_mask = eVelocityParameterDrag;

		MotionMan.SetSpecParams(ASP_MOVE_BKWD);

		break;
	case ACT_STEAL:
		vel_mask = eVelocityParameterSteal;
		des_mask = eVelocityParameterSteal;
		break;
	case ACT_JUMP:
		break;
	}

	//if (force_real_speed) vel_mask = des_mask;

	if (bEnablePath) {
		set_velocity_mask	(vel_mask);	
		set_desirable_mask	(des_mask);
		enable_path			();		
	} else {
		disable_path		();
	}
}


void CAI_Biting::SetScriptControl(const bool bScriptControl, ref_str caScriptName)
{
	if (CurrentState) CurrentState->Done();

	CScriptMonster::SetScriptControl(bScriptControl, caScriptName);
}


int	CAI_Biting::get_enemy_strength()
{
	if (m_tEnemy.obj) {
		switch (dwfChooseAction(0,m_fAttackSuccessProbability[0],m_fAttackSuccessProbability[1],m_fAttackSuccessProbability[2],m_fAttackSuccessProbability[3],g_Team(),g_Squad(),g_Group(),0,1,2,3,4,this,30.f)) {
			case 4 : 	return (1);
			case 3 : 	return (2);
			case 2 : 	return (3);
			case 1 : 
			case 0 : 	return (4);
		}
	} 
	
	return (0);
}
