////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\..\\actor.h"

using namespace AI_Biting;

const float tempCrouchFactor = 0.5f;
const float tempWalkFactor = 1.7f;
const float tempWalkFreeFactor = 1.7f;
const float tempRunFactor = 5.0f;
const float tempRunFreeFactor = 5.0f;
const float tempPanicFactor = 3.2f;
const float tempAttackDistance = 2.5f;

const float min_angle = PI_DIV_6 / 2;

#define _ATTACK_TEST_ON

void CAI_Biting::Think()
{

	m_dwLastUpdateTime		= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();

	// сохранить позиции врага
//	m_tSavedEnemyPosition	= vPosition;
//	m_tpSavedEnemyNode		= AI_Node;
//	m_dwSavedEnemyNodeID	= AI_NodeID;

//	m_dwPathTypeRandomFactor = 50;		// вероятностный выбор в построении пути (Straight | Criteria)

//	if (m_tMovementType != eMovementTypeStand && m_tActionType != eActionTypeTurn) 
//			m_fCurSpeed				= 1.0f;
//
//	if (Level().iGetKeyState(DIK_R)) { // Run
//		
//		vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
//			eMovementDirectionForward, eStateTypeNormal, eActionTypeRun);
//
//		vfSetParameters(&m_tSelectorFreeHunting, 0, false, m_tPathType,m_tBodyState, 
//			m_tMovementType, m_tStateType, 0);
//
//	} 
//
//	if (Level().iGetKeyState(DIK_T)) { // Walk
//		
//		vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
//			eMovementDirectionForward, eStateTypeNormal, eActionTypeWalk);
//		
//		vfSetParameters(&m_tSelectorFreeHunting, 0, false, m_tPathType,m_tBodyState, 
//			m_tMovementType, m_tStateType, 0);
//
//	} 
//
//	
//	if (AI_Path.TravelPath.empty() || (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 1)) {
//		vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
//			eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);
//			m_fCurSpeed				= 1.0f;
//	  
//		vfSetParameters(&m_tSelectorFreeHunting, 0, true, m_tPathType,m_tBodyState, 
//				m_tMovementType, m_tStateType, 0);
//	}
//		
//	vfSetAnimation();

//	m_fCurSpeed				= 1.0f;	
//
//#ifndef _ATTACK_TEST_ON
//
//	// RUN TESTING
//
//	vfUpdateDetourPoint();	
//	vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
//					eMovementDirectionForward, eStateTypeNormal, eActionTypeWalk);
//	
//	
//	AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
//	vfSetParameters(0, 0, false, 0);
//
//#else
//	//////////////////////////////////////////////////////////////////////////
//	// ATTACK TESTING
//
//	vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
//		eMovementDirectionForward, eStateTypeNormal, eActionTypeRun);
//
//	CActor *pActor = dynamic_cast <CActor*> (Level().CurrentEntity());
//	if (pActor) {
//
//
//		if (pActor->Position().distance_to(vPosition) <= tempAttackDistance) {
//			//AI_Path.TravelPath.clear();
//
//			AI_Path.DestNode = AI_NodeID;
//
//			vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
//				eMovementDirectionNone, eStateTypeDanger, eActionTypeAttack);
//
//			vfSetParameters(0, &pActor->Position(), false, 0);
//			
//			if (m_tActionType == eActionTypeAttack) {
//				m_fCurSpeed				= 0.01f;		
//				r_torso_speed			= min_angle;
//			}
//		}  else {
//			AI_Path.DestNode = pActor->AI_NodeID;
//			vfSetParameters(0, &Level().CurrentEntity()->Position(), false, 0);
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//
//#endif
//
//	if (m_tActionType == eActionTypeAttack) vfSetAnimation(false);
//	else vfSetAnimation(true);

	m_fCurSpeed				= 1.0f;	

	switch (m_tStateFSM) {
		case eNoState : InitRest();
			break;
		case eRestIdle:
		case eWalkAround: Rest();
			break;
	}
	vfSetAnimation(true);
}

// Развернуть объект в направление движения
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


// построение пути и установка параметров скорости
void CAI_Biting::vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, Fvector *tpPoint)
{
	
	vfChoosePointAndBuildPath(tpNodeEvaluator,tpDesiredPosition, bSearchNode);
	
	// если путь выбран
	if (AI_Path.TravelPath.size() && ((AI_Path.TravelPath.size() - 1) > AI_Path.TravelStart)) {
		
		// тип положения тела
		switch (m_tBodyState) {
			case eBodyStateStand : 	// стоит
				break;
			case eBodyStateSit : 	// сидит
				break;
			case eBodyStateLie : 	// ползёт
				m_fCurSpeed *= tempCrouchFactor;
				break;
			default : NODEFAULT;
		}
	
		// тип движения
		switch (m_tMovementType) {
			case eMovementTypeWalk : 	// идёт
				
				// состояние
				switch (m_tStateType) {
					case eStateTypeDanger : // ходьба при опасности
						m_fCurSpeed *= tempWalkFactor;
						break;
				
					case eStateTypeNormal : // обычная ходьба
						m_fCurSpeed *= tempWalkFreeFactor;
						break;
								
					case eStateTypePanic : // передвижение при панике
						VERIFY(false);
						break;
					
				}
				// скорость поворота тела
				r_torso_speed	= PI_MUL_2;
				break;
									
			case eMovementTypeRun :		// бежит

				// состояние
				switch (m_tStateType) { 
					case eStateTypeDanger :		// бег при опасности
						m_fCurSpeed *= tempRunFactor;
						break;
								
					case eStateTypeNormal :		// обычный бег
						m_fCurSpeed *= tempRunFreeFactor;
						break;
								
					case eStateTypePanic :		// бег при панике
						m_fCurSpeed *= tempPanicFactor;
						break;
				}
				r_torso_speed	= PI_MUL_2;
				break;
			default : m_fCurSpeed = 0.f;
		} // switch movement
	} // if
	else {
		m_tMovementType = eMovementTypeStand;
		r_torso_speed	= PI_MUL_2;
		m_fCurSpeed		= 0.f;
	}

	// стоять, если путь не выбран
	if (AI_Path.TravelPath.empty() || (AI_Path.TravelStart >= (AI_Path.TravelPath.size() - 1))) {
		r_torso_speed	= PI_MUL_2;
		m_fCurSpeed		= 0.f;
		
		vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
			eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);

	}  else {
		if (tpPoint) {
			Fvector tTemp;
			tTemp.sub	(*tpPoint,eye_matrix.c);
			tTemp.getHP	(r_torso_target.yaw,r_torso_target.pitch);
			r_torso_target.yaw *= -1;
		} else  { 
			SetDirectionLook();
		}
	}
	
	// необходим поворот?
	if (!getAI().bfTooSmallAngle(r_torso_current.yaw, r_torso_target.yaw, min_angle)) {
		if (m_tMovementType == eMovementTypeRun) { 	// поворот на бегу

			r_torso_speed	= PI;
			m_fCurSpeed		= 3.0f;

			if (getAI().bfTooSmallAngle(angle_normalize_signed(r_torso_current.yaw + min_angle), r_torso_target.yaw, 5*min_angle))
				// right
				vfSetMotionActionParams(m_tBodyState, eMovementTypeRun, eMovementDirectionRight, m_tStateType, eActionTypeTurn);
			else 
				// left
				vfSetMotionActionParams(m_tBodyState, eMovementTypeRun, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);		

		} else {									// поворот на месте
			m_fCurSpeed		= PI_DIV_4;
			r_torso_speed	= PI_DIV_2;

			if (getAI().bfTooSmallAngle(angle_normalize_signed(r_torso_current.yaw + min_angle), r_torso_target.yaw, 5*min_angle))
				// right
				vfSetMotionActionParams(m_tBodyState, eMovementTypeStand, eMovementDirectionRight, m_tStateType, eActionTypeTurn);
			else 
				// left
				vfSetMotionActionParams(m_tBodyState, eMovementTypeStand, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);		
		}
	}  

}

// установка параметров движения и действий 
void CAI_Biting::vfSetMotionActionParams(AI_Biting::EBodyState l_body_state, AI_Biting::EMovementType l_move_type, 
								   AI_Biting::EMovementDir l_move_dir, AI_Biting::EStateType l_state_type, AI_Biting::EActionType l_action_type)
{
	m_tMovementType		= l_move_type;
	m_tBodyState		= l_body_state;
	m_tStateType		= l_state_type;
	m_tMovementDir		= l_move_dir;
	m_tActionType		= l_action_type;
}


void CAI_Biting::InitRest()
{
	m_tPrevStateFSM = m_tStateFSM;
	(::Random.randI(0,1)) ? m_tStateFSM	= eRestIdle : m_tStateFSM = eWalkAround;

	if (m_tStateFSM == eWalkAround) {
		vfUpdateDetourPoint();	
		vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
				eMovementDirectionForward, eStateTypeNormal, eActionTypeWalk);

		
		AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
		vfSetParameters(0, 0, false, 0);
	}

	fTimeToRestTill = Level().timeServer() + ::Random.randF(5000.f,30000.f);
}


void CAI_Biting::Rest()
{
	if (m_dwCurrentUpdate < fTimeToRestTill) {
		if (m_tStateFSM == eRestIdle) {
			vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
				eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);
			vfSetParameters(0, 0, false, 0);
		} else {
			vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
				eMovementDirectionForward, eStateTypeNormal, eActionTypeWalk);
			vfSetParameters(0, 0, false, 0);
		}
	} else m_tStateFSM = eNoState;
}
