////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"

using namespace AI_Biting;


void CAI_Biting::Think()
{
	static int i = 0;

	//TurnAround();

	TMotionState state;

	if (i==0) {
		state.angle = PI_DIV_2;
		state.motion = eTurnLeft;

		if (ExecMovement(state)) i++;
	} else if (i== 1) {
		state.dest=vPosition;
		
		state.dest.add(10.f);
		state.motion = eMoveFwd;

		if (ExecMovement(state)) i++;

	} else if (i== 2) {
		state.angle = PI_DIV_2;
		state.motion = eTurnLeft;

		if (ExecMovement(state)) i++;
	}

	
/*	m_dwLastUpdateTime		= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();

	m_tPathType				= ePathTypeStraightCriteria;

	m_tSavedEnemyPosition	= vPosition;
	m_tpSavedEnemyNode		= AI_Node;
	m_dwSavedEnemyNodeID	= AI_NodeID;


	m_fCurSpeed				= 0.0f;

	m_dwPathTypeRandomFactor = 50;		// вероятностный выбор в построении пути (Straight | Criteria)

	r_torso_speed = PI_DIV_4;

	SetDirectionLook();

	if (AI_Path.TravelPath.empty() || (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 1))
		vfChoosePointAndBuildPath(&m_tSelectorFreeHunting,0,true);
		*/
}

void CAI_Biting::SetDirectionLook()
{
	int i = ps_Size();		// position stack size
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.magnitude() > EPS_L) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);
		}
	}
	else
		r_torso_target.pitch = 0;
	r_target = r_torso_target;
}



//void CAI_Biting::vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, EWeaponState tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, EStateType tStateType, ELookType tLookType, Fvector &tPointToLook, u32 dwLookOverDelay)
//{
//	
//}


// Info: r_target - для головы

void CAI_Biting::TurnAround()
{
	
	r_torso_speed = 3*PI_DIV_4;		// рад/сек
	if (getAI().bfTooSmallAngle(r_torso_target.yaw,r_torso_current.yaw,EPS_L)) {
		r_torso_target.yaw = angle_normalize(angle_normalize(r_torso_target.yaw) - PI_DIV_2);	// рад
		Msg("%f",R2D(r_torso_target.yaw));
	}

	m_tPostureAnim = ePostureStand;
	m_tActionAnim = eActionTurnLeft;
	
	//FORCE_ANIMATION_SELECT();
}

// true - Полностью обработан
bool CAI_Biting::ExecMovement(TMotionState &motion)
{
//	if ((motion.time_started + motion.time) > Level().timeServer())
//		return true;


	m_fCurSpeed = 0.0f;
	r_torso_speed = 0.0f;
	r_torso_target.yaw = 0.0f;

	switch (motion.motion) {
		case eNothing:
			m_tActionAnim = eActionIdle;
			m_tPostureAnim = ePostureStand;
			break;
		case eMoveFwd:
			
			if (motion.dest.similar(vPosition,0.5f)) return true;  // уже пришли

			m_fCurSpeed = 0.5f;
			m_tActionAnim = eActionWalkFwd;
			m_tPostureAnim = ePostureStand;
			break;

		case eTurnLeft:
			if (r_torso_current.yaw == r_torso_target.yaw) return true;  // уже повернули

			m_fCurSpeed = 0.1f;
			r_torso_speed = 3*PI_DIV_4;		// рад/сек
			r_torso_target.yaw = angle_normalize(r_torso_current.yaw + motion.angle);

			m_tActionAnim = eActionTurnLeft;
			m_tPostureAnim = ePostureStand;
			break;
	}
	

	return false;
}