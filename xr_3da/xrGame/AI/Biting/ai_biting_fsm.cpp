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

const float tempCrouchFactor = 0.5f;
const float tempWalkFactor = 1.7f;
const float tempWalkFreeFactor = 1.7f;
const float tempRunFactor = 5.0f;
const float tempRunFreeFactor = 5.0f;
const float tempPanicFactor = 3.2f;


void CAI_Biting::Think()
{

	m_dwLastUpdateTime		= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();

	// ��������� ������� �����
//	m_tSavedEnemyPosition	= vPosition;
//	m_tpSavedEnemyNode		= AI_Node;
//	m_dwSavedEnemyNodeID	= AI_NodeID;

//	m_dwPathTypeRandomFactor = 50;		// ������������� ����� � ���������� ���� (Straight | Criteria)

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

	
	m_fCurSpeed				= 1.0f;	

	vfUpdateDetourPoint();	
	vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
					eMovementDirectionForward, eStateTypeNormal, eActionTypeRun);
	
	
	AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
	vfSetParameters(0, 0, false, m_tPathType,m_tBodyState, 
			m_tMovementType, m_tStateType, 0);


	vfSetAnimation();

}

// ���������� ������ � ����������� ��������
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


// ���������� ���� � ��������� ���������� ��������
void CAI_Biting::vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, EStateType tStateType, Fvector *tpPoint)
{
	
	m_tPathType		= tPathType;
	m_tBodyState	= tBodyState;
	m_tMovementType = tMovementType;
	m_tStateType	= tStateType;

	vfChoosePointAndBuildPath(tpNodeEvaluator,tpDesiredPosition, bSearchNode);
	
	// ���� ���� ������
	if (AI_Path.TravelPath.size() && ((AI_Path.TravelPath.size() - 1) > AI_Path.TravelStart)) {
		
		// ��� ��������� ����
		switch (m_tBodyState) {
			case eBodyStateStand : 	// �����
				break;
			case eBodyStateSit : 	// �����
				break;
			case eBodyStateLie : 	// �����
				m_fCurSpeed *= tempCrouchFactor;
				break;
			default : NODEFAULT;
		}
	
		// ��� ��������
		switch (m_tMovementType) {
			case eMovementTypeWalk : 	// ���
				
				// ���������
				switch (m_tStateType) {
					case eStateTypeDanger : // ������ ��� ���������
						m_fCurSpeed *= tempWalkFactor;
						break;
				
					case eStateTypeNormal : // ������� ������
						m_fCurSpeed *= tempWalkFreeFactor;
						break;
								
					case eStateTypePanic : // ������������ ��� ������
						VERIFY(false);
						break;
					
				}
				// �������� �������� ����
				r_torso_speed	= PI_MUL_2;
				break;
									
			case eMovementTypeRun :		// �����

				// ���������
				switch (m_tStateType) { 
					case eStateTypeDanger :		// ��� ��� ���������
						m_fCurSpeed *= tempRunFactor;
						break;
								
					case eStateTypeNormal :		// ������� ���
						m_fCurSpeed *= tempRunFreeFactor;
						break;
								
					case eStateTypePanic :		// ��� ��� ������
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

	// ������, ���� ���� �� ������
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
	
	if (!getAI().bfTooSmallAngle(r_torso_current.yaw, r_torso_target.yaw, PI_DIV_6)) {
		m_fCurSpeed		= 0.f;
		r_torso_speed	= PI;
		if (getAI().bfTooSmallAngle(angle_normalize_signed(r_torso_current.yaw + PI_DIV_6), r_torso_target.yaw, 5*PI_DIV_6))
			// right
			vfSetMotionActionParams(m_tBodyState, eMovementTypeStand, eMovementDirectionRight, m_tStateType, eActionTypeTurn);
		else 
			// left
			vfSetMotionActionParams(m_tBodyState, eMovementTypeStand, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);		
	}  




}

// ����� �������� � ������������ � ������� ���������� �������
void CAI_Biting::vfSetAnimation()
{
	EPostureAnim PostureAnim_old = m_tPostureAnim;
	EActionAnim ActionAnim_old = m_tActionAnim;
	
	// ��� ��������� ����
	switch (m_tBodyState) {
		case eBodyStateStand : 	// �����
			m_tPostureAnim = ePostureStand;
			break;
		case eBodyStateSit : 	// �����
			m_tPostureAnim = ePostureSit;
			break;	
		case eBodyStateLie : 	// �����
			m_tPostureAnim = ePostureLie;
			break;
		default : NODEFAULT;
	}


	if ( m_tMovementType == eMovementTypeStand &&  
		 m_tStateType == eStateTypeNormal &&  
		 m_tMovementDir == eMovementDirectionNone &&
		 m_tActionType == eActionTypeStand)
		
		 m_tActionAnim = eActionIdle;		// �� ����� / ��������

	else if (m_tMovementType == eMovementTypeWalk && 
			 m_tStateType == eStateTypeNormal &&  
			 m_tMovementDir == eMovementDirectionForward && 
			 m_tActionType == eActionTypeWalk) 
		
		m_tActionAnim = eActionWalkFwd;		// �������� ������

	else if (m_tMovementType == eMovementTypeStand && 
			 m_tMovementDir == eMovementDirectionLeft && 
			 m_tActionType == eActionTypeTurn) 

		m_tActionAnim = eActionTurnLeft;		// ������� �� ����� �����

	else if (m_tMovementType == eMovementTypeStand && 
			 m_tMovementDir == eMovementDirectionRight && 
			 m_tActionType == eActionTypeTurn) 
		
		// !!!! �������� �� eActionTurnRight ����� ����� ��������
		m_tActionAnim = eActionTurnLeft;		// ������� �� ����� ������
	
	else if (m_tMovementType == eMovementTypeRun && 
			 m_tActionType == eActionTypeRun) 

		m_tActionAnim = eActionRun;		// ���

	
	if ( (PostureAnim_old != m_tPostureAnim) || (ActionAnim_old != m_tActionAnim))
		FORCE_ANIMATION_SELECT();
}


// ��������� ���������� �������� � �������� 
void CAI_Biting::vfSetMotionActionParams(AI_Biting::EBodyState l_body_state, AI_Biting::EMovementType l_move_type, 
								   AI_Biting::EMovementDir l_move_dir, AI_Biting::EStateType l_state_type, AI_Biting::EActionType l_action_type)
{
	m_tMovementType		= l_move_type;
	m_tBodyState		= l_body_state;
	m_tStateType		= l_state_type;
	m_tMovementDir		= l_move_dir;
	m_tActionType		= l_action_type;
}