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

using namespace AI_Biting;

const float tempCrouchFactor = 0.5f;
const float tempWalkFactor = 1.7f;
const float tempWalkFreeFactor = 1.7f;
const float tempRunFactor = 5.0f;
const float tempRunFreeFactor = 5.0f;
const float tempPanicFactor = 5.0f;

const float min_angle = PI_DIV_6 / 2;

// исправление несоответствия позиции узлу 
void CAI_Biting::vfValidatePosition(Fvector &tPosition, u32 dwNodeID)
{
	if ((dwNodeID <= 0) || (dwNodeID >= getAI().Header().count) || (getAI().dwfCheckPositionInDirection(dwNodeID,getAI().tfGetNodeCenter(dwNodeID),tPosition) == u32(-1)))
		m_tSavedEnemyPosition = getAI().tfGetNodeCenter(dwNodeID);
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

// построение пути и установка параметров скорости
void CAI_Biting::vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, Fvector *tpPoint, bool moveback)
{

	m_fCurSpeed				= 1.0f;	

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
	else {		// если пути нет
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
			if (moveback) {
				SetReversedDirectionLook();
			}

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

		} else if (m_tMovementType == eMovementTypeWalk) {				// поворот в ходьбе

			if (m_tMovementDir == eMovementDirectionBack) {
				m_fCurSpeed		= PI;
				r_torso_speed	= PI_MUL_2;
			} else {

				m_fCurSpeed		= PI_DIV_4;
				r_torso_speed	= PI_DIV_2;

				if (getAI().bfTooSmallAngle(angle_normalize_signed(r_torso_current.yaw + min_angle), r_torso_target.yaw, 5*min_angle))
					// right
					vfSetMotionActionParams(m_tBodyState, eMovementTypeWalk, eMovementDirectionRight, m_tStateType, eActionTypeTurn);
				else 
					// left
					vfSetMotionActionParams(m_tBodyState, eMovementTypeWalk, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);		
			}

		} else {				// поворот в ходьбе
			m_fCurSpeed		= 0;
			r_torso_speed	= PI_DIV_4;
			vfSetMotionActionParams(m_tBodyState, eMovementTypeStand, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);		
		}
	}  

	r_target = r_torso_target;
}

void CAI_Biting::vfUpdateParameters()
{
	_A = A; _B = B; _Bb = Bb; _C = C; _Cc = Cc; _D = D; _E = E; _Ee = Ee; _F = F;
	A = B = Bb  = C = Cc = D = E = Ee = F = false;
	
	// does enemy see me?
	SelectEnemy			(m_tEnemy);
	objVisible			&VisibleEnemies = Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
	
	if (m_tEnemy.Enemy) 
		vfSaveEnemy();


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
		
		if (D = (getAI().bfTooSmallAngle(yaw1,yaw2,fYawFov) && (getAI().bfTooSmallAngle(pitch1,pitch2,fPitchFov) || false)))
			break;
	}

	// sound
	if (m_tLastSound.dwTime >= m_dwLastUpdateTime - 1000) {
		B = true;
		if (((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING) || 
			((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_RECHARGING) == SOUND_TYPE_WEAPON_RECHARGING) || 
			((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_RICOCHET) == SOUND_TYPE_WEAPON_BULLET_RICOCHET) || 
			((m_tLastSound.eSoundType & SOUND_TYPE_MONSTER_ATTACKING) == SOUND_TYPE_MONSTER_ATTACKING) ) {

			Bb = true;
		}
	} 

	// аттакован
	if (m_dwHitTime >= m_dwLastUpdateTime - 1000) C = true;

	if (m_tLastSound.dwTime >= m_dwLastUpdateTime - 1000) {
		if (((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)) {
			C = Cc = true;
		}
	} 

	// оценка вероятности победы
	if (VisibleEnemies.size()) {
		switch (dwfChooseAction(0,m_fAttackSuccessProbability0,m_fAttackSuccessProbability1,m_fAttackSuccessProbability2,m_fAttackSuccessProbability3,g_Team(),g_Squad(),g_Group(),0,1,2,3,4,this,30.f)) {
			case 4 :				// очень сильный враг
				E = Ee = false;		
				break;
			case 3 :				// сильный враг
				E = false;			
				Ee = true;
				break;
			case 2 :				// равный враг
				E = true;			
				Ee = false;
				break;
			case 1 :				// слабый враг
			case 0 : 
				E = Ee = true;		
				break;
			
		}
	}
//////////////////////////////////////////////////////////////////////////
	

	// враг виден
	if (m_tEnemy.Enemy && m_tEnemy.bVisible) A = true;


	// является ли враг выгодным?
	F = false;
	getAI().m_tpCurrentMember = this;
	for ( i=0, n=VisibleEnemies.size(); i<n; i++) {
		if (!(getAI().m_tpCurrentEnemy  = dynamic_cast<CEntityAlive*>(VisibleEnemies[i].key)))
			continue;
//		if ((E || F || G) && (H = !!getAI().pfExpediency.dwfGetDiscreteValue(2)))
//			break;
//		else
//			if (ifFindHurtIndex(getAI().m_tpCurrentEnemy) != -1)
//				H = true;

		F = !!getAI().pfExpediency.dwfGetDiscreteValue(2);
	}
}