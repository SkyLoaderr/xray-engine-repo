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

const float min_angle = PI_DIV_6;

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
void CAI_Biting::vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, Fvector *tpPoint, bool moveback)
{
	bool bPathBuilt;
	bool bNeedToTurnInRun;
	bool bNeedToTurnInAttack;
	
	m_fCurSpeed				= 1.0f;	
	
	vfChoosePointAndBuildPath(tpNodeEvaluator,tpDesiredPosition, bSearchNode);

	bPathBuilt = AI_Path.TravelPath.size() && ((AI_Path.TravelPath.size() - 1) > AI_Path.TravelStart);
	

	// �������� �������� ����
	r_torso_speed	= PI_MUL_2;

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
	
		(moveback) ? SetReversedDirectionLook() : SetDirectionLook();
	
	} // if
	else {		// ���� ���� ���
		m_tMovementType = eMovementTypeStand;
		m_fCurSpeed		= 0.f;
	}

	

/*	
	
	// ������, ���� ���� �� ������
	if (!bPathBuilt) {
		
		if (m_tActionType != eActionTypeAttack)
			vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
				eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);

*/

	// �������� � ����� 
	if (tpPoint) {
		Fvector tTemp;
		Fvector tTemp2;

		clCenter(tTemp2);
		//tTemp2.y += 2.f;
			
		tTemp.sub	(*tpPoint,tTemp2);
		tTemp.getHP	(r_torso_target.yaw,r_torso_target.pitch);
		r_torso_target.yaw *= -1;
	}

	bNeedToTurnInRun = !getAI().bfTooSmallAngle(r_torso_current.yaw, r_torso_target.yaw, min_angle / 6);
	bNeedToTurnInAttack = !getAI().bfTooSmallAngle(r_torso_current.yaw, r_torso_target.yaw, min_angle / 10);

	bool bMoveLeft = true;
	//if (getAI().bfTooSmallAngle(r_torso_target.yaw,angle_normalize_signed(r_torso_current.yaw + PI_DIV_2),PI_DIV_4)) bMoveLeft = false;

	bMoveLeft = IsLeftSide(r_torso_current.yaw,r_torso_target.yaw);

	// ��������� �������?
	if (bNeedToTurnInRun) {
		if (m_tMovementType == eMovementTypeRun) { 	// ������� �� ����

			r_torso_speed	= PI;
			m_fCurSpeed		= PI;

			if (bMoveLeft) vfSetMotionActionParams(m_tBodyState, eMovementTypeRun, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);
			else vfSetMotionActionParams(m_tBodyState, eMovementTypeRun, eMovementDirectionRight, m_tStateType, eActionTypeTurn);
			
		} else if (m_tMovementType == eMovementTypeWalk) {				// ������� � ������

			if (m_tMovementDir == eMovementDirectionBack) {
				m_fCurSpeed		= PI;
				r_torso_speed	= PI_MUL_2;
			} else {

				m_fCurSpeed		= PI_DIV_4;
				r_torso_speed	= PI_DIV_2;

				if (bMoveLeft) vfSetMotionActionParams(m_tBodyState, eMovementTypeWalk, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);
				else vfSetMotionActionParams(m_tBodyState, eMovementTypeWalk, eMovementDirectionRight, m_tStateType, eActionTypeTurn);

//				if (getAI().bfTooSmallAngle(angle_normalize_signed(r_torso_current.yaw + min_angle), r_torso_target.yaw, 5*min_angle))
//					// right
//					vfSetMotionActionParams(m_tBodyState, eMovementTypeWalk, eMovementDirectionRight, m_tStateType, eActionTypeTurn);
//				else 
//					// left
//					vfSetMotionActionParams(m_tBodyState, eMovementTypeWalk, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);		
			}

		} else {				// ������� �� �����
				m_fCurSpeed		= 0;
				r_torso_speed	= PI_DIV_4;
				if (getAI().bfTooSmallAngle(angle_normalize_signed(r_torso_current.yaw + min_angle), r_torso_target.yaw, 5*min_angle)) {
					// right
					vfSetMotionActionParams(m_tBodyState, eMovementTypeStand, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);
					
					AnimEx.Set(ePostureStand, eActionEat,false,false);
				}
				else 
					// left
					vfSetMotionActionParams(m_tBodyState, eMovementTypeStand, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);		
		}  
	}

/*	if (bNeedToTurnInAttack) {
		m_fCurSpeed		= 0;
		r_torso_speed	= PI_DIV_4;
		if (getAI().bfTooSmallAngle(angle_normalize_signed(r_torso_current.yaw + min_angle), r_torso_target.yaw, 5*min_angle))
			// right
			vfSetMotionActionParams(m_tBodyState, eMovementTypeStand, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);		
		else 
			// left
			vfSetMotionActionParams(m_tBodyState, eMovementTypeStand, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);		
	}

	*/
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
	if (K) {
		F = true; C = D = E = false;
	}

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
	// temp!!!
	H = true;
}

//////////////////////////////////////////////////////////////////////////
// Temp!!!
void CAI_Biting::SetText()
{
	HUD().pFontSmall->SetColor	(0xffffffff);
	HUD().pFontSmall->OutSet	(320,400);
	HUD().pFontSmall->OutNext	("Curren = [%f]  Target = [%f] ",rad2deg(r_torso_current.yaw), rad2deg(r_torso_target.yaw));
	HUD().pFontSmall->OutSet	(320,430);
	HUD().pFontSmall->OutNext	("Norm Current = [%f] Norm Target = [%f]",rad2deg(angle_normalize(r_torso_current.yaw)),rad2deg(angle_normalize(r_torso_target.yaw)));
	HUD().pFontSmall->OutSet	(320,450);
	
	
	string128 s;
	strconcat(s,AI_Biting::caStateNames[AnimEx.CurState.m_tPostureAnim],
				AI_Biting::caStateNames[AnimEx.CurState.m_tActionAnim]);

	HUD().pFontSmall->OutSet	(100,300);
	HUD().pFontSmall->OutNext	("Current Animation = [%s]        Stack Size: [%i]",s,AnimEx.Anim.size());
	
	
	
	std::list<TAnimCell>::iterator It;
	It = AnimEx.Anim.begin();

	for (int i=0; (i<(int)AnimEx.Anim.size()) && (i<8); i++) {
		strconcat(s,AI_Biting::caStateNames[It->m_tPostureAnim],
				AI_Biting::caStateNames[It->m_tActionAnim]);
		
		HUD().pFontSmall->OutSet	(100+i*50,350);
		HUD().pFontSmall->OutNext ("SA[%i]=[%s]",i,s);
		It++;
	}
	// Where is enemy

	
	
	if (m_tSavedEnemy){
		HUD().pFontSmall->OutSet	(300,380);	
		if (IsLeftSide(m_tSavedEnemy->Position())) {
			HUD().pFontSmall->OutNext	("LEFT");
		} else  HUD().pFontSmall->OutNext	("RIGHT");

		HUD().pFontSmall->OutSet	(400,380);

/*		
		Fvector v1,v2;
		v1 = mRotate.k;
		v2 = m_tSavedEnemy->Direction();

		float f1,f2,f3;
		f1 = v1.magnitude();
		f2 = v2.magnitude();
		f3 = v1.dotproduct(v2);

		float a = f3 / (f1 * f2);
		a = acosf(a);
*/		
	
		float yaw1;
		CCustomMonster	*tpCustomMonster = dynamic_cast<CCustomMonster *>(m_tSavedEnemy);
		if (tpCustomMonster) yaw1		= -tpCustomMonster->r_current.yaw;
		else {
			CActor		*tpActor = dynamic_cast<CActor *>(m_tSavedEnemy);
			if (tpActor) yaw1	= tpActor->Orientation().yaw;
		}
		

		HUD().pFontSmall->OutNext	("Monster Angle = [%f] Actor Angle = [%f]",R2D(-r_torso_current.yaw),R2D(yaw1));
		HUD().pFontSmall->OutSet	(400,280);
		HUD().pFontSmall->OutNext	("Normalized = [%f] Actor Angle = [%f]",R2D(angle_normalize(-r_torso_current.yaw)),R2D(angle_normalize(yaw1)));
		//float a = angle_normalize(_abs(r_torso_current.yaw - yaw1));

	//	HUD().pFontSmall->OutNext	("Current Angle between = [%f]",rad2deg(a));

		//Msg("ANGLE = [%f]",rad2deg(a));
	}

	
//	Fvector temp;
//	SRotation
//	if ()

//	if (bNeedToTurnInRun) {
//		if (bMoveLeft) HUD().pFontSmall->OutNext	("LEFT");
//		else HUD().pFontSmall->OutNext	("RIGHT");
//	} else HUD().pFontSmall->OutNext	("NONE");

	//	Msg("%Turn Angles: Current = [%f]  Target = [%f]; TurnToLeft = [%d]",r_torso_current.yaw,r_torso_target.yaw,bMoveLeft);
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
