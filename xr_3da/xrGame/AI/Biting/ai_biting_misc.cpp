////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_misc.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Miscellanious functions for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\..\\actor.h"

#include "..\\..\\hudmanager.h"

using namespace AI_Biting;

const float tempCrouchFactor = 0.5f;
const float tempWalkFactor = 1.7f;
const float tempWalkFreeFactor = 1.7f;
const float tempRunFactor = 5.0f;
const float tempRunFreeFactor = 5.0f;
const float tempPanicFactor = 5.0f;

const float min_angle = PI_DIV_4;
const float min_turning_angle = PI_DIV_6;

// ����������� �������������� ������� ���� 
void CAI_Biting::vfValidatePosition(Fvector &tPosition, u32 dwNodeID)
{
	if ((dwNodeID <= 0) || (dwNodeID >= getAI().Header().count) || (getAI().dwfCheckPositionInDirection(dwNodeID,getAI().tfGetNodeCenter(dwNodeID),tPosition) == u32(-1)))
		m_tSavedEnemyPosition = getAI().tfGetNodeCenter(dwNodeID);
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

// ���������� ���� � ��������� ���������� �������� 
void CAI_Biting::vfSetParameters(EPathType path_type,IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, Fvector *tpPoint, bool moveback, bool bSelectorPath)
{

	//bool bMoveLeft = false;
	bool bPathBuilt = false;
	
	m_tPathType = path_type;
	vfChoosePointAndBuildPath(tpNodeEvaluator,tpDesiredPosition, bSearchNode, bSelectorPath);
	bPathBuilt = AI_Path.TravelPath.size() && ((AI_Path.TravelPath.size() - 1) > AI_Path.TravelStart);
	
	bool caActive = _CA.Active();
	if (caActive) {
		r_torso_speed			= 0;		// ������� ��������
		m_fCurSpeed				= 0.0f;			// �������� ��������

		return;
	}
	if (!caActive && (m_tActionAnim == eActionIdle) && (m_tPostureAnim == ePostureLie) && (m_tActionType!=eActionTypeLie)) {
		_CA.Set(ePostureLie,eActionStandUp);
	}


	// ������������� ���������
	r_torso_speed			= PI_DIV_2;		// ������� ��������
	m_fCurSpeed				= 1.0f;			// �������� ��������


	// ���� ���� ������
	if (bPathBuilt) {

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
				break;
			default : m_fCurSpeed = 0.f;
		} // switch movement
	
		if (!tpPoint) {
			(moveback) ? SetReversedDirectionLook() : SetDirectionLook();
		}
	
	} // if
	else {		// ���� ���� ���
		m_fCurSpeed		= 0.f;

		if (m_tActionType != eActionTypeAttack && m_tActionType != eActionTypeLie) 
			vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
									eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);
	}

	// �������� � ����� tpPoint
	if (tpPoint) {
		if (m_dwPointCheckLastTime + m_dwPointCheckInterval < m_dwCurrentUpdate) {
			m_dwPointCheckLastTime = m_dwCurrentUpdate;

			Fvector tTemp;
			Fvector tTemp2;
			float pitch;

			clCenter(tTemp2);

			tTemp.sub	(*tpPoint,tTemp2);
			tTemp.getHP	(r_torso_target.yaw,pitch);
			r_torso_target.yaw *= -1;

			r_torso_target.yaw = angle_normalize(r_torso_target.yaw);

		}
	}

	bool bMoveLeft, bMoveRight;
	bMoveLeft = bMoveRight = false;
	if (!getAI().bfTooSmallAngle(r_torso_target.yaw,r_torso_current.yaw,min_angle)) {
		// we need to turn
		if (angle_normalize_signed(angle_normalize_signed(r_torso_current.yaw) - angle_normalize_signed(r_torso_target.yaw)) > 0)
			// turn left
			bMoveLeft = true;
		else
			// turn right
			bMoveRight = true;
	}

	// ��������� �������?
	if (bMoveLeft || bMoveRight) {
		if (m_tMovementType == eMovementTypeRun) { 	// ������� �� ����

			r_torso_speed	= PI;
			m_fCurSpeed		= PI;

			if (!getAI().bfTooSmallAngle(r_torso_target.yaw,r_torso_current.yaw,min_angle * 2))
				if (bMoveLeft) vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, eMovementDirectionLeft, eStateTypeNormal, eActionTypeTurn);
				else vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, eMovementDirectionRight, eStateTypeNormal, eActionTypeTurn);
			
		} else if (m_tMovementType == eMovementTypeWalk) {				// ������� � ������

			if (m_tMovementDir == eMovementDirectionBack) {
				m_fCurSpeed		= PI;
				r_torso_speed	= PI_MUL_2;
			} else {

				m_fCurSpeed		= PI_DIV_4;
				r_torso_speed	= PI_DIV_2;

				if (!getAI().bfTooSmallAngle(r_torso_target.yaw,r_torso_current.yaw,min_angle * 2))
					if (bMoveLeft) vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, eMovementDirectionLeft, eStateTypeNormal, eActionTypeTurn);
					else vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, eMovementDirectionRight, eStateTypeNormal, eActionTypeTurn);
			}
		} else {				// ������� �� �����
				m_fCurSpeed		= 0;
				r_torso_speed	= PI_DIV_4;
				
				if (m_tActionType == eActionTypeAttack) {
					r_torso_speed	= PI;
					vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, eMovementDirectionLeft, eStateTypeDanger, eActionTypeTurn);

				} else {
					if (!getAI().bfTooSmallAngle(r_torso_target.yaw,r_torso_current.yaw,min_turning_angle))
						if (bMoveLeft) vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, eMovementDirectionLeft, eStateTypeNormal, eActionTypeTurn);
						else vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, eMovementDirectionLeft, eStateTypeNormal, eActionTypeTurn);		
				}
		}  
	}

	//vfAssignPitch();
//	r_torso_target.pitch = PI_DIV_6;

	r_target = r_torso_target;
}

void CAI_Biting::vfUpdateParameters()
{
	// sound
	A = B = false;
	
	if (_A && !A && ((m_tLastSound.dwTime + m_dwInertion) > m_dwCurrentUpdate))
		A = true;
	
	if (_B && !B && ((m_tLastSound.dwTime + m_dwInertion) > m_dwCurrentUpdate))
		B = true;
	
	if (!A && !B && (m_tLastSound.dwTime >= m_dwLastUpdateTime)) {
		B = true;
		if (((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING) || 
			((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_RECHARGING) == SOUND_TYPE_WEAPON_RECHARGING) || 
			((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_RICOCHET) == SOUND_TYPE_WEAPON_BULLET_RICOCHET) || 
			((m_tLastSound.eSoundType & SOUND_TYPE_MONSTER_ATTACKING) == SOUND_TYPE_MONSTER_ATTACKING) )
				A = true;
	} 
	J = A | B;

	// victory probability
	C = D = E = F = G	= false;
	objVisible			&VisibleEnemies = Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
	if (bfIsAnyAlive(VisibleEnemies)) {
		switch (dwfChooseAction(0,m_fAttackSuccessProbability0,m_fAttackSuccessProbability1,m_fAttackSuccessProbability2,m_fAttackSuccessProbability3,g_Team(),g_Squad(),g_Group(),0,1,2,3,4,this,30.f)) {
			case 4 : 
				C = true;
				break;
			case 3 : 
				D = true;
				break;
			case 2 : 
				E = true;
				break;
			case 1 : 
			case 0 : 
				F = true;
				break;
		}
	}
	K					= C | D | E | F;
	
	// temp!!!!
//	if (K) {
//		E = true;
//		C = F = D = false;
//	}

	// does enemy see me?
	SelectEnemy			(m_tEnemy);

	I					= false;
	for (int i=0, n=VisibleEnemies.size(); i<n; i++) {
		float			yaw1, pitch1, yaw2, pitch2, fYawFov, fPitchFov, fRange;
		Fvector			tPosition = VisibleEnemies[i].key->Position();

		CCustomMonster	*tpCustomMonster = dynamic_cast<CCustomMonster *>(VisibleEnemies[i].key);
		if (tpCustomMonster) {
			yaw1		= -tpCustomMonster->r_current.yaw;
			pitch1		= -tpCustomMonster->r_current.pitch;
			fYawFov		= angle_normalize_signed(tpCustomMonster->ffGetFov()*PI/180.f);
			fRange		= tpCustomMonster->ffGetRange();
		}
		else {
			CActor		*tpActor = dynamic_cast<CActor *>(VisibleEnemies[i].key);
			if (tpActor) {
				yaw1	= tpActor->Orientation().yaw;
				pitch1	= tpActor->Orientation().pitch;
				fYawFov	= angle_normalize_signed(tpActor->ffGetFov()*PI/180.f);
				fRange	= tpActor->ffGetRange();
			}
			else
				continue;
		}

		if (tPosition.distance_to(vPosition) > fRange)
			continue;

		fYawFov			= angle_normalize_signed((_abs(fYawFov) + _abs(atanf(1.f/tPosition.distance_to(vPosition))))/2.f);
		fPitchFov		= angle_normalize_signed(fYawFov*1.f);
		tPosition.sub	(vPosition);
		tPosition.mul	(-1);
		tPosition.getHP	(yaw2,pitch2);
		yaw1			= angle_normalize_signed(yaw1);
		pitch1			= angle_normalize_signed(pitch1);
		yaw2			= angle_normalize_signed(yaw2);
		pitch2			= angle_normalize_signed(pitch2);
		if (0 != (I = (getAI().bfTooSmallAngle(yaw1,yaw2,fYawFov) && (getAI().bfTooSmallAngle(pitch1,pitch2,fPitchFov) || false))))
			break;
	}

	// is enemy expedient?
	H = false;
	getAI().m_tpCurrentMember = this;
	for ( i=0, n=VisibleEnemies.size(); i<n; i++) {
		if (0 == (getAI().m_tpCurrentEnemy  = dynamic_cast<CEntityAlive*>(VisibleEnemies[i].key)))
			continue;
		if ((E || F || G) && (0 != (H = !!getAI().pfExpediency.dwfGetDiscreteValue(2))))
			break;
		else
			if (ifFindHurtIndex(getAI().m_tpCurrentEnemy) != -1)
				H = true;
	}

//	if (Level().iGetKeyState(DIK_H)) {
//		K = C = D = E = F = false;
//		H = true; I = false;

	// temp
	H = true;
}

//////////////////////////////////////////////////////////////////////////
// Temp!!!
void CAI_Biting::SetText()
{

#ifndef SILENCE
//	HUD().pFontSmall->OutSet	(300,380);	
//	HUD().pFontSmall->OutNext	("Health = [%f]", g_Health());
	
	
	if (m_tCorpse.Enemy) 
		if (m_tCorpse.Enemy->m_fFood >= 0) {	
			HUD().pFontSmall->OutSet	(300,400);	
			HUD().pFontSmall->OutNext	("Food = [%f]", m_tCorpse.Enemy->m_fFood);
		}
/*
	HUD().pFontSmall->OutSet	(300,420);	
	HUD().pFontSmall->OutNext	("Current = [%f] Target = [%f]", R2D(r_torso_current.yaw), R2D(r_torso_target.yaw));

	HUD().pFontSmall->OutSet	(300,440);	
	HUD().pFontSmall->OutNext	("Current = [%f] Target = [%f]", R2D(r_torso_current.pitch), R2D(r_torso_target.pitch));*/
#endif

}


bool CAI_Biting::IsLeftSide(const Fvector &Position)
{
	Fvector iV; // i-������������ ������� mRotate
	Fvector temp;

	iV = mRotate.i;
	temp = vPosition;
	temp.sub(Position);
	float f = temp.dotproduct(iV);

	if (f >= 0) return true;
	else return false;
}

bool CAI_Biting::IsLeftSide(float current_yaw,float target_yaw)
{
	float cy = angle_normalize(current_yaw);	
	float ty = angle_normalize(target_yaw);

	if (((cy < ty) && (cy + PI > ty) && (cy>0) && (cy<PI)) && 
		!((cy > ty) && (ty > cy-PI) && (cy > PI) && (cy < PI_MUL_2))) 
			return true;
	return false;
}
