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

/**
	Msg("Path state : %s",(m_tPathState == ePathStateSearchNode) ? "Searching for the node" : (m_tPathState == ePathStateBuildNodePath) ? "Building path" : (m_tPathState == ePathStateBuildTravelLine) ? "Building travel line" : "Dodging travel line");\
	Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\
/**/

#undef	WRITE_TO_LOG
#ifdef SILENCE
#define WRITE_TO_LOG(s) ;

#else
#define WRITE_TO_LOG(s) {\
	Msg("Monster %s : \n* State : %s",cName(),s);\
}
#endif

void CAI_Biting::Death()
{
	WRITE_TO_LOG("Death");
}

void CAI_Biting::Panic()
{
	WRITE_TO_LOG("Panic");

	if (m_bStateChanged) {
		m_tActionState = eActionStateWatchGo;
		m_dwActionStartTime = Level().timeServer() + ::Random.randI(2000,5000);
	}

	m_dwInertion				= 60000;

	if ((AI_Path.fSpeed < EPS_L) && !m_tEnemy.Enemy) { // осматриваемся 
		switch (m_tActionState) {
			case eActionStateWatch : {
				vfSetMotionActionParams	(eBodyStateStand,eMovementTypeRun,eMovementDirectionForward,eStateTypeDanger,eActionTypeRun);
				vfSetParameters			(0,0,false,0);
				if (r_torso_current.yaw == r_torso_target.yaw) {
					m_ls_yaw = r_torso_current.yaw;
					m_tActionState = eActionStateDontWatch;
				}
				break;
									 }
			case eActionStateDontWatch : {
//				Fvector					tPoint;
//				tPoint.setHP			(m_ls_yaw,0);
//				tPoint.mul				(100.f);
//				tPoint.add				(vPosition);
//				vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeLookOver,tPoint,2500);
				break;
										 }
			default : {
				m_tActionState = eActionStateWatch;
				break;
					  }
		}
		return;
	}

	m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tSavedEnemyPosition.distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
	m_tSelectorFreeHunting.m_fOptEnemyDistance = m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(vPosition) + 3.f;

	vfSetMotionActionParams		(eBodyStateStand,eMovementTypeRun,eMovementDirectionForward,eStateTypeDanger,eActionTypeRun);
	vfSetParameters				(&m_tSelectorFreeHunting,0,true,0);
}

void CAI_Biting::BackCover(bool bFire)
{
	WRITE_TO_LOG("Back Cover");
}

void CAI_Biting::ForwardStraight()
{
	WRITE_TO_LOG("Forward Straight");
	
	m_dwInertion				= 6000;
	Fvector						EnemyPosition;
	EActionState				OldState;
	
	EnemyPosition = ((m_tEnemy.Enemy) ? m_tEnemy.Enemy->Position() : m_tSavedEnemy->Position());
	OldState = m_tActionState;
	m_tActionState = ((EnemyPosition.distance_to(vPosition) > 2.4f) ? eActionStateRun : eActionStateStand);
	
	
	if ((OldState == eActionStateStand && m_tActionState == eActionStateRun)&& 
		(EnemyPosition.distance_to(vPosition) < 3.2f)) m_tActionState = OldState;


	switch (m_tActionState) {

		case eActionStateRun:	
			vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
									eMovementDirectionForward, eStateTypeDanger, eActionTypeRun);
			//vfSetParameters			(0,&EnemyPosition,false,&EnemyPosition);
			vfSetParameters			(0,&EnemyPosition,false,0);
			break;
		case eActionStateStand:	
			vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
									eMovementDirectionNone, eStateTypeDanger, eActionTypeAttack);

			vfSetParameters			(0,&EnemyPosition,false,0);
			break;
	}	
	
}


void CAI_Biting::Hide()
{
	WRITE_TO_LOG("Hide");
}

void CAI_Biting::Detour()
{
	WRITE_TO_LOG("Detour");
}

void CAI_Biting::ExploreDE()
{
	WRITE_TO_LOG("Explore danger-expedient enemy");
}

void CAI_Biting::ExploreDNE()
{
	WRITE_TO_LOG("Explore danger-non-expedient enemy");
}

void CAI_Biting::ExploreNDE()
{
	WRITE_TO_LOG("Explore non-danger-expedient enemy");

	vfSetMotionActionParams	(eBodyStateStand,eMovementTypeWalk,eMovementDirectionForward,eStateTypeDanger,eActionTypeWalk);
	
	AI_Path.DestNode		= m_dwLastSoundNodeID;
	vfSetParameters			(0,0,false,0);

}

void CAI_Biting::ExploreNDNE()
{
	WRITE_TO_LOG("Explore non-danger-non-expedient enemy");
}
// Walking Around
void CAI_Biting::AccomplishTask(IBaseAI_NodeEvaluator *tpNodeEvaluator)
{
	WRITE_TO_LOG("Accomplishing task");

	// проверка на видимость трупов
	SelectCorp(m_tEnemy);
	
	Fvector *tpDesiredPosition = 0;
	if (m_tEnemy.Enemy) m_tCorpse = m_tEnemy;

	if (m_tCorpse.Enemy) {
		AI_Path.DestNode		= m_tCorpse.Enemy->AI_NodeID;
		Fvector l_tCorpsePosition;
		m_tCorpse.Enemy->clCenter(l_tCorpsePosition);
		tpDesiredPosition = &l_tCorpsePosition;
		
		m_tActionState = eActionStateRun;			
		m_dwActionStartTime = 0;

	} else {
		m_tActionState	= eActionStateStand;

		if (m_bStateChanged || m_dwActionStartTime < m_dwCurrentUpdate) {
			
			if (!AI_Path.TravelPath.empty()) {
				m_tActionState	= eActionStateStand;
//			} else 	(::Random.randI(10)) ? m_tActionState = eActionStateStand : 
//										m_tActionState = eActionStateWatchGo; // walking
				m_tActionState = eActionStateStand;
			}

			AI_Path.TravelPath.clear();
			AI_Path.DestNode = AI_NodeID;
			vfSetParameters(0, 0, false, 0);


			if (m_tActionState == eActionStateWatchGo) {		// бродить по точкам графа?
				vfUpdateDetourPoint();	

				AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
				vfSetParameters(0, 0, false, 0);
			} else {								// отдыхать...
				//if (!::Random.randI(1))  m_tStateFSM = eRestIdleTurn;
			}
			
			m_dwActionStartTime = m_dwCurrentUpdate + ::Random.randF(5000.f,15000.f); // время брожения или стояния на месте
		}
	}

	switch (m_tActionState) {
		case eActionStateStand :
					vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
											eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);
					break;
		case eActionStateWatchGo:   // бродить
					vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
											eMovementDirectionForward, eStateTypeNormal, eActionTypeWalk);
					break;

		case eActionStateRun:   // бежать к трупу
			if (m_tCorpse.Enemy->Position().distance_to(vPosition) > 1.5f)
				vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
					eMovementDirectionForward, eStateTypeNormal, eActionTypeRun);
			else 
				vfSetMotionActionParams(eBodyStateLie, eMovementTypeStand, 
				eMovementDirectionNone, eStateTypeNormal, eActionTypeEat);
			break;
	}
	vfSetParameters(0, tpDesiredPosition, false, 0);	
}


void CAI_Biting::Think()
{

	m_dwLastUpdateTime		= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();

	m_tEnemy.Enemy			= 0;
	vfUpdateParameters		();

	if (!K && _K && m_tSavedEnemy && m_tSavedEnemy->g_Alive() && ((m_dwCurrentUpdate - m_dwLostEnemyTime) < m_dwInertion)) {
		K = true;
		C = _C;
		E = _E;
		D = _D;
		F = _F;
		H = _H;
		I = _I;
		m_bStateChanged = false;
		//vfUpdateVisibilityBySensitivity();
	}

	m_bStateChanged			= ((_A	!= A) || (_B	!= B) || (_C	!= C) || (_D	!= D) || (_E	!= E) || (_F	!= F) || (_H	!= H) || (_I	!= I) || (_J	!= J) || (_K	!= K));

#ifndef SILENCE
	if (g_Alive())
		Msg("%s : [S=%d][A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][H=%d][I=%d][J=%d][K=%d]",cName(),m_bStateChanged ? 1 : 0,A,B,C,D,E,F,H,I,J,K);
#endif
	

// A - я слышу опасный звук
// B - я слышу неопасный звук
// С - я вижу очень опасного врага
// D - я вижу опасного врага
// E - я вижу равного врага
// F - я вижу слабого врага
// H - враг выгодный
// I - враг видит меня
// J - A | B
// K - C | D | E | F 


	if (!g_Alive()) {
		Death				();
	}				   
	else
	if (C && H && I) {
		m_dwRandomFactor	= 50;
		Panic			();
	} else
	if (C && H && !I) {
		Hide();
	} else
	if (C && !H && I) {
		m_dwRandomFactor	= 50;
		Panic			();
	} else
	if (C && !H && !I) {
		Hide();
	} else
	
	if (D && H && I) {
		m_dwRandomFactor	= 50;
		BackCover	(true);
	} else
	if (D && H && !I) {
		Hide		();
	} else
	if (D && !H && I) {
		m_dwRandomFactor	= 50;
		BackCover	(false);
	} else
	if (D && !H && !I) {
		Hide	();
	} else
	
	if (E && H && I) {
		m_dwRandomFactor	= 0;
		ForwardStraight	();
	} else
	if (E && H && !I) {
		m_dwRandomFactor	= 0;
		ForwardStraight	();
	} else
	if (E && !H && I) {
		Detour			();
	} else
	if (E && !H && !I) {
		Detour			();
	} else
	
	if (F && H && I) {
		m_dwRandomFactor	= 75;
		ForwardStraight	();
	} else
	if (F && H && !I) {
		m_dwRandomFactor	= 100;
		ForwardStraight	();
	} else
	if (F && !H && I) {
		Detour			();
	} else
	if (F && !H && !I) {
		Detour			();
	} else
	
	if (A && !K && !H) {	// слышу опасный звук, но не вижу, враг не выгодный
		ExploreDNE();
	} else
	if (A && !K && H) {		// слышу опасный звук, но не вижу, враг выгодный
		ExploreDE();
	} else
	
	if (B && !K && !H) {
		ExploreNDNE();
	} else
	if (B && !K && H) {
		ExploreNDE();
	} else {
		AccomplishTask();
	}
	
	_A	= A;
	_B	= B;
	_C	= C;
	_D	= D;
	_E	= E;
	_F	= F;
	_H	= H;
	_I	= I;
	_J	= J;
	_K	= K;
}