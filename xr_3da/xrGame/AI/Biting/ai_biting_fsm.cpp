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

	//CalcCommonFunc();
//	DoFeel();

/*	m_fCurSpeed				= 1.0f;	

	
	
	
	if (m_tEnemy.Enemy) {
		vfSaveEnemy();
		m_tStateFSM = eAttack;
	} else if (!m_tSavedEnemy && m_tStateFSM == eAttack){
		m_tStateFSM = eNoState;
	}
 
	switch (m_tStateFSM) {
		case eNoState : InitRest();
			break;
		case eRestIdle:
		case eRestIdleTurn:
		case eWalkAround: Rest();
			break;
		case eAttack: Attack();
	}
	
	vfSetAnimation(false);

	*/


	// проверка на инерцию
	if (m_fInertion >= m_dwCurrentUpdate) return;
	
	bool A, B, Bb, C, Cc, D, E,Ee, F;
	vfUpdateParameters(A, B, Bb, C, Cc, D, E,Ee, F);

	AI_Path.TravelPath.clear();
	vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
							eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);

	if (Level().iGetKeyState(DIK_Q)) {
		MoveBackScared();
	}


	vfSetAnimation(true);

/*
	bool A, B, Bb, C, Cc, D, E,Ee F;

	vfUpdateParameters(A, B, Bb, C, Cc, D, E,Ee, F);

	if (A && !E && !Ee) {				// Видит врага, враг очень опасный 

		switch (m_tActionState) {
				case eMoveBackUncertain: 
					MoveBackUncertain();
					break;
				case eRunAwayInPanic: 
					RunAwayInPanic();
					break;
				case eStayInPlaceScared: 
					StayInPlaceScared();
					break;
		}
	} else if (A && !E && Ee && D) {				// Видит врага, враг опасный и враг видит

		switch (m_tActionState) {
				case eMoveBackUncertain: 
					MoveBackUncertain();
					break;
				case eRunAwayInPanic: 
					RunAwayInPanic();
					break;
				case eStayInPlaceScared30: 
					StayInPlaceScared();
					break;
		}
	} else if (A && !E && Ee && !D && F) {				// Видит врага, враг опасный и враг не видит, враг выгоден
		StealUpAndAttack();
	} else if (A && !E && Ee && !D && !F) {				// Видит врага, враг опасный и враг не видит, враг не выгоден
		RunAwayHidden();
	} else if (A && E && !Ee && D && !F) {				// Видит врага, враг равный и враг видит, враг не выгоден
		WalkAroundPointLeaveEnemy();
	} else if (A && !E && Ee && D && F) {				// Видит врага, враг равный и враг видит, враг выгоден
		AttackEnemy();
	} else if (A && E && !Ee && D && !F) {				// Видит врага, враг равный и враг не видит, враг не выгоден
		WalkAroundPointLeaveEnemy();
	} else if (A && !E && Ee && D && F) {				// Видит врага, враг равный и враг не видит, враг выгоден
		StealUpAndAttack();
	} else if (A && E && !Ee && D && !F) {				// Видит врага, враг слабый и враг видит, враг не выгоден
		WalkAroundPointLeaveEnemy();
	} else if (A && !E && Ee && D && F) {				// Видит врага, враг слабый и враг видит, враг выгоден
		AttackEnemy();
	} else if (A && E && !Ee && D && !F) {				// Видит врага, враг слабый и враг не видит, враг не выгоден
		WalkAroundPointLeaveEnemy();
	} else if (A && !E && Ee && D && F) {				// Видит врага, враг слабый и враг не видит, враг выгоден
		AttackEnemy();
	} else if  (!A & B & Bb & E) {						// Слышит опасный звук и уверен в своей победе
		switch (m_tActionState) {
			case eListenAndLookAround: 
				//ListenAndLookAround();
				break;
			case eGotoPlaceOfSound: 
				//GotoPlaceOfSound();
				break;
			case eStayInPlace: 
				//StayInPlace10();
				break;
		}
	} else if (!A & B & Bb & !E) {				// Слышит опасный звук, но не уверен в своей победе
		//WaitFor10_20();
	} else if (!A & B & !Bb) {					// Слышит неопасный звук
		//GotoPlaceOfSound();
	} else if (!A && C && Cc) {					// Аттакован из огнестрельного оружия
		switch (m_tActionState) {
			case eScaredTurnToAttacker: 
				//Scared();
				break;
			case eRunToCover: 
				//RunAwayToCover();
				break;
			case eStayInPlaceScared: 
				//StayInPlaceScared10_20();
				break;
		}
	} else if (!A && C && !Cc) {				// Аттакован из неогнестрельного оружия
		switch (m_tActionState) {
			case eTurnToAttacker: 
				//TurnToAttacker();
				break;
			case eListenAndLookAround: 
				//eListenAndLookAround();
				break;
		}
	} else if (!A && !B && !C && !Cc) {					// ничего не произошло, гулять!
		PeaceOnEarth();	 
	}
	
*/

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
			m_fCurSpeed		= PI_DIV_4;
			r_torso_speed	= PI_DIV_2;

			if (getAI().bfTooSmallAngle(angle_normalize_signed(r_torso_current.yaw + min_angle), r_torso_target.yaw, 5*min_angle))
				// right
				vfSetMotionActionParams(m_tBodyState, eMovementTypeWalk, eMovementDirectionRight, m_tStateType, eActionTypeTurn);
			else 
				// left
				vfSetMotionActionParams(m_tBodyState, eMovementTypeWalk, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);		
		
		} else {				// поворот в ходьбе
			m_fCurSpeed		= 0;
			r_torso_speed	= PI_DIV_4;
			vfSetMotionActionParams(m_tBodyState, eMovementTypeStand, eMovementDirectionLeft, m_tStateType, eActionTypeTurn);		
		}
	}  
	r_target = r_torso_target;
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

	if (!AI_Path.TravelPath.empty()) {
		m_tStateFSM	= eRestIdle;
	} else 	(::Random.randI(2)) ? m_tStateFSM	= eRestIdle : m_tStateFSM = eWalkAround;
	

	AI_Path.TravelPath.clear();
	AI_Path.DestNode = AI_NodeID;
	vfSetParameters(0, 0, false, 0);

	//m_tPrevStateFSM = m_tStateFSM;
	
	

	if (m_tStateFSM == eWalkAround) {		// бродить по точкам графа?
		vfUpdateDetourPoint();	
		vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
				eMovementDirectionForward, eStateTypeNormal, eActionTypeWalk);

		
		AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
		vfSetParameters(0, 0, false, 0);
	} else {								// отдыхать...
		if (!::Random.randI(1))  m_tStateFSM = eRestIdleTurn;
		vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
			eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);
	}

	fTimeToRestTill = Level().timeServer() + ::Random.randF(5000.f,15000.f);
}


/////////////////////////////////////////////////////////////////////////////////
// Rest
void CAI_Biting::Rest()
{
	if (m_dwCurrentUpdate < fTimeToRestTill) {
		if (m_tStateFSM == eRestIdle || m_tStateFSM == eRestIdleTurn) {
			
			if (m_tActionType != eActionTypeTurn && m_tStateFSM == eRestIdleTurn) { 
				r_target.yaw = (r_torso_target.yaw += 2*PI_DIV_2);
				fTimeToRestTill += 1000;
				m_tStateFSM = eRestIdle;
			} else {
				vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
					eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);
			}
		} else {  // бродить
			vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
				eMovementDirectionForward, eStateTypeNormal, eActionTypeWalk);
		}
		
		vfSetParameters(0, 0, false, 0);
	} else {
		m_tStateFSM = eNoState;
	}
}


/////////////////////////////////////////////////////////////////////////////////
// Attack
void CAI_Biting::Attack()
{
	if ((m_tEnemy.Enemy && m_tEnemy.bVisible) || 
		(m_tSavedEnemy && m_dwSeenEnemyLastTime > (m_dwCurrentUpdate - 3000)) ) {
		
		if (m_tSavedEnemyPosition.distance_to(vPosition) <= tempAttackDistance) {
			AI_Path.TravelPath.clear();

			vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
				eMovementDirectionNone, eStateTypeDanger, eActionTypeAttack);

//			m_fCurSpeed				= 0.f;
//			r_torso_speed			= PI_MUL_8;
//			vPosition.set() 
//			r_torso_speed			= PI;

/*			if (m_tActionType == eActionTypeAttack) {
				m_fCurSpeed				= 0.01f;		
				r_torso_speed			= min_angle;
			}
*/

		}  else {
			vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
				eMovementDirectionForward, eStateTypeDanger, eActionTypeRun);

			AI_Path.DestNode = m_dwSavedEnemyNodeID;
			vfSetParameters(0, &m_tSavedEnemyPosition, false, 0);

		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
// SelectEnemy
void CAI_Biting::SelectEnemy(SEnemySelected& S)
{
	// Initiate process
	objVisible&	Known	= Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
	S.Enemy				= 0;
	S.bVisible			= FALSE;

	if (Known.size()==0)
		return;

	// получить список видимых объектов
	feel_vision_get	(m_tpaVisibleObjects);
	std::sort		(m_tpaVisibleObjects.begin(),m_tpaVisibleObjects.end()); 

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	for (u32 i=0; i<Known.size(); i++) {
		CEntityAlive*	E = dynamic_cast<CEntityAlive*>(Known[i].key);
		if (!E) continue;
		
		bool bVisible = false;
		for (int i=0; i<(int)m_tpaVisibleObjects.size(); i++)  // TODO: make through std::find
			if (m_tpaVisibleObjects[i] == E) {
				bVisible = true;
				break;
			}
		S.Enemy		= E;
		S.bVisible	= bVisible;
		Group.m_bEnemyNoticed = true;
		m_dwSeenEnemyLastTime = m_dwCurrentUpdate;
	}
}

void CAI_Biting::DoFeel()
{
	// Просмотреть видимые объекты и выбрать врага
	SelectEnemy(m_tEnemy);
	
	if (!(m_tEnemy.Enemy) && m_tSavedEnemy && (Level().timeServer() - m_dwSeenEnemyLastTime < 4000)) 
		m_tEnemy.Enemy = m_tSavedEnemy;

	if (m_tEnemy.Enemy && m_tEnemy.bVisible) bEnemySighted = true;
	else bEnemySighted = false;

	// Слышит звук
//	if (m_tLastSound.dwTime <= m_dwLastUpdateTime - 1000) b

}


void CAI_Biting::feel_sound_new(CObject* who, int eType, const Fvector &Position, float power)
{
	if (!g_Alive())
		return;

	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;//expf(.1f*log(power));

	if (power >= m_fSoundThreshold) {
		if ((this != who) && ((m_tLastSound.dwTime <= m_dwLastUpdateTime) || (m_tLastSound.fPower <= power))) {
			m_tLastSound.eSoundType		= ESoundTypes(eType);
			m_tLastSound.dwTime			= Level().timeServer();
			m_tLastSound.fPower			= power;
			m_tLastSound.tSavedPosition = Position;
			m_tLastSound.tpEntity		= dynamic_cast<CEntity *>(who);
			//float fDistance = (Position.distance_to(vPosition) < 1.f ? 1.f : Position.distance_to(vPosition));
			//			if ((eType & SOUND_TYPE_MONSTER_DYING) == SOUND_TYPE_MONSTER_DYING)
			//				m_fMorale += m_fMoraleDeathQuant;///fDistance;
			//			else


/*
			if (((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING) && (!m_tEnemy.Enemy))
				m_fMorale += m_fMoraleFearQuant;///fDistance;
			else
				if ((eType & SOUND_TYPE_MONSTER_ATTACKING) == SOUND_TYPE_MONSTER_ATTACKING)
					m_fMorale += m_fMoraleSuccessAttackQuant;///fDistance;
*/
		}
	}
}



//////////////////////////////////////////////////////////////////////////////////
//
void CAI_Biting::vfUpdateParameters(bool &A, bool &B, bool &Bb, bool &C, bool &Cc, bool &D, bool &E, bool &Ee, bool &F)
{

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

void CAI_Biting::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	m_dwHitTime = Level().timeServer();
	m_tHitDir.set(D);
	m_tHitDir.normalize();
	m_tHitPosition = who->Position();
	
/*
	// Play hit-sound
	sound& S				= m_tpaSoundHit[Random.randI(SND_HIT_COUNT)];
	
	if (g_Health() > 0) {
		if (S.feedback)
			return;
		if (Random.randI(2))
			return;
		::Sound->play_at_pos		(S,this,vPosition);
	}
	
	if (g_Health() - amount <= 0) {
		if ((m_tpCurrentGlobalAnimation) && (!m_tpCurrentGlobalBlend->playing))
			if (m_tpCurrentGlobalAnimation != m_tRatAnimations.tNormal.tGlobal.tpaDeath[0])
				m_tpCurrentGlobalBlend = PKinematics(pVisual)->PlayCycle(m_tpCurrentGlobalAnimation = m_tRatAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,2)]);
	}
*/


}



// пятиться назад 2 сек...
void CAI_Biting::MoveBackScared()
{
	m_fInertion = m_dwCurrentUpdate + 3000;
	vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
		eMovementDirectionBack, eStateTypeDanger, eActionTypeWalk);

	// построить путь в обратном направлении 
	m_tSelectorRetreat.m_tEnemyPosition = m_tMySavedPosition;
	m_tSelectorRetreat.m_tpEnemyNode = getAI().Node(m_dwMyNodeID);
	m_tSelectorRetreat.m_tMyPosition = vPosition;
	m_tSelectorRetreat.m_tpMyNode = AI_Node;
	

	vfSetParameters(&m_tSelectorRetreat, 0, true, 0);
	
}




