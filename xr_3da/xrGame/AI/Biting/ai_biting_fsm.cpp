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

const float tempAttackDistance = 2.5f;

//const u32 tempSeenInertion = 5000;
const u32 tempHeardInertion = 5000;


#define _ATTACK_TEST_ON
#define	_TEST_OFF

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


	
//	bool A, B, Bb, C, Cc, D, E,Ee, F;
//	vfUpdateParameters(A, B, Bb, C, Cc, D, E,Ee, F);

//	AI_Path.TravelPath.clear();
//	vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
//							eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);

//
//
//#ifdef _TEST_ON
//	if (Level().iGetKeyState(DIK_Q)) {
//		//MoveBackScared();
//		RunAwayInPanic();
//	}
//#endif


	vfUpdateParameters();
//	B = 0;
//	Bb= 0;

	// Инерция 
	if (A) m_dwTimeLastSeenEnemy = m_dwCurrentUpdate;
	if (Bb) m_dwTimeLastHeardEnemy = m_dwCurrentUpdate;
	
	if ((m_dwTimeLastSeenEnemy + SeenInertion >= m_dwCurrentUpdate) && 
		(m_dwTimeLastHeardEnemy + HeardInertion >= m_dwCurrentUpdate)) 
		m_dwTimeLastSeenEnemy = m_dwCurrentUpdate;

	if (m_dwTimeLastSeenEnemy + SeenInertion >= m_dwCurrentUpdate) A = true;
	if (m_dwTimeLastHeardEnemy + HeardInertion >= m_dwCurrentUpdate) {B = true;	Bb = _Bb;}
	

	Msg("See = %i              Hear = %i" ,A,B,Bb);
	
	E = Ee = false;	// Temp

	

	if (A && !E && !Ee) {				// Видит врага, враг очень опасный 
		//pStateRunAwayInPanic->Execute();
	}
	
	
	
	if (A && !E && !Ee) {				// Видит врага, враг очень опасный 

		switch (m_tActionState) {
				case eMoveBackScared: 
					MoveBackScared();
					break;
				case eRunAwayInPanic: 
					RunAwayInPanic();
					break;
				case eStayInPlaceScared: 
					StayInPlaceScared();
					break;
		}
	}/* else if (A && !E && Ee && D) {				// Видит врага, враг опасный и враг видит

		switch (m_tActionState) {
				case eMoveBackScared: 
					MoveBackScared();
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
//		PeaceOnEarth();	 
	}
	
*/
	vfSetAnimation(true);
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




//////////////////////////////////////////////////////////////////////////////////
//


// пятиться назад 3 сек...
void CAI_Biting::MoveBackScared()
{
	m_fCurSpeed				= PI_DIV_2;
	if (InitState) {
	
		vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
								eMovementDirectionBack, eStateTypeDanger, eActionTypeWalk);

		m_tSelectorFreeHunting.m_tEnemyPosition = m_tSavedEnemyPosition;
		m_tSelectorFreeHunting.m_tpEnemyNode = m_tpSavedEnemyNode;
		m_tSelectorFreeHunting.m_tMyPosition = vPosition;
		m_tSelectorFreeHunting.m_tpMyNode = AI_Node;
		vfSetParameters(&m_tSelectorFreeHunting, 0, true, 0, true);

		SeenInertion = 3000;
		StateStartedTime = m_dwCurrentUpdate;
		InitState = false;
	} else {
		vfSetParameters(0, 0, false, 0,true);
		if (SeenInertion + StateStartedTime < m_dwCurrentUpdate)  {
			SeenInertion = 0;
			InitState = true;
			m_tActionState = eRunAwayInPanic;
		}
	}
}

void CAI_Biting::RunAwayInPanic()
{
	vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
							eMovementDirectionForward, eStateTypePanic, eActionTypeRun);

	if (InitState) {
		// построить путь в обратном направлении 
		m_tSelectorRetreat.m_tEnemyPosition = m_tMySavedPosition;
		m_tSelectorRetreat.m_tpEnemyNode = getAI().Node(m_dwMyNodeID);
		m_tSelectorRetreat.m_tMyPosition = vPosition;
		m_tSelectorRetreat.m_tpMyNode = AI_Node;

		vfSetParameters(&m_tSelectorRetreat, 0, true, 0);
		InitState = false;
		SeenInertion = 20000;
		StateStartedTime = m_dwCurrentUpdate;

	} else  {
		vfSetParameters(0, 0, false, 0);
		if ((AI_Path.TravelPath.empty() || (AI_Path.TravelStart >= (AI_Path.TravelPath.size() - 1))) || 
			(SeenInertion + StateStartedTime < m_dwCurrentUpdate)) {
			SeenInertion = 0;
			InitState = true;
			m_tActionState = eStayInPlaceScared;
		}
	}
	
}

void CAI_Biting::StayInPlaceScared()
{
	if (InitState) {
		InitState = false;
	}
	
	vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
							eMovementDirectionNone, eStateTypePanic, eActionTypeStand);

	vfSetParameters(0, 0, false, 0);
}

//void CAI_Biting::AttackEnemy()
//{
//	if ((m_tEnemy.Enemy && m_tEnemy.bVisible) || 
//		(m_tSavedEnemy && m_dwSeenEnemyLastTime > (m_dwCurrentUpdate - 3000)) ) {
//
//			if (m_tSavedEnemyPosition.distance_to(vPosition) <= tempAttackDistance) {
//				AI_Path.TravelPath.clear();
//
//				vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
//					eMovementDirectionNone, eStateTypeDanger, eActionTypeAttack);
//
//				//			m_fCurSpeed				= 0.f;
//				//			r_torso_speed			= PI_MUL_8;
//				//			vPosition.set() 
//				//			r_torso_speed			= PI;
//
//				/*			if (m_tActionType == eActionTypeAttack) {
//				m_fCurSpeed				= 0.01f;		
//				r_torso_speed			= min_angle;
//				}
//				*/
//
//			}  else {
//				vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
//					eMovementDirectionForward, eStateTypeDanger, eActionTypeRun);
//
//				AI_Path.DestNode = m_dwSavedEnemyNodeID;
//				vfSetParameters(0, &m_tSavedEnemyPosition, false, 0);
//
//			}
//		}
//}
