////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fuzzy State Machine for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\ai_monsters_misc.h"

#undef	WRITE_TO_LOG
//#define WRITE_TO_LOG(s) m_bStopThinking = true;
//Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);
#define WRITE_TO_LOG(s) {\
	Msg("Path state : %s",(m_tPathState == ePathStateSearchNode) ? "Searching for the node" : (m_tPathState == ePathStateBuildNodePath) ? "Building path" : (m_tPathState == ePathStateBuildTravelLine) ? "Building travel line" : "Dodging travel line");\
	m_bStopThinking = true;\
}

//#ifndef DEBUG
//	#undef	WRITE_TO_LOG
//	#define WRITE_TO_LOG(s) m_bStopThinking = true;
//#endif

void CAI_Stalker::vfUpdateSearchPosition()
{
	if (!g_Alive())
		return;
	INIT_SQUAD_AND_LEADER;
	if (this != Leader)	{
		CAI_Stalker *tpLeader = dynamic_cast<CAI_Stalker*>(Leader);
		if (tpLeader) {
//			if (m_tNextGraphPoint.distance_to(tpLeader->m_tNextGraphPoint) > EPS_L)
//				m_eCurrentState = eStalkerStateSearching;
			m_tNextGraphPoint			= tpLeader->m_tNextGraphPoint;
		}
	}
	else {
		if ((Level().timeServer() >= m_dwTimeToChange) && (getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex == m_tNextGP)) {
			m_tNextGP					= getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;
			vfChooseNextGraphPoint		();
			m_tNextGraphPoint.set		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
			if (m_eCurrentState == eStalkerStateAccomplishingTask)
				AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
		}
		else
			if (m_eCurrentState == eStalkerStateAccomplishingTask)
				AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
	}
}

void CAI_Stalker::Death()
{
	WRITE_TO_LOG("Death");
	
	DropItemSendMessage();
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
}

void CAI_Stalker::BackDodge()
{
	WRITE_TO_LOG("Back dodging");
	
	m_tEnemy.Enemy				= dynamic_cast<CEntity *>(Level().CurrentEntity());

	m_tPathType					= ePathTypeDodge;
	vfChoosePointAndBuildPath	(m_tSelectorRetreat);

	vfSetFire					(false,*getGroup());

	Fvector						tPoint;
	m_tEnemy.Enemy->svCenter	(tPoint);
	vfSetMovementType			(eBodyStateStand,eMovementTypeRun,eLookTypeDirection,tPoint);
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::BackCover()
{
	WRITE_TO_LOG("Back cover");
	
	m_tEnemy.Enemy				= dynamic_cast<CEntity *>(Level().CurrentEntity());

	m_tPathType					= ePathTypeCriteria;
	vfChoosePointAndBuildPath	(m_tSelectorReload);

	vfSetFire					(false,*getGroup());

	Fvector						tPoint;
	m_tEnemy.Enemy->svCenter	(tPoint);
	vfSetMovementType			(eBodyStateStand,eMovementTypeRun,eLookTypeFirePoint,tPoint);
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::Think()
{
	vfUpdateDynamicObjects	();
//	vfUpdateSearchPosition	();
	m_dwUpdateCount++;
	m_dwLastUpdate			= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();
	m_bStopThinking			= false;
	do {
		m_ePreviousState	= m_eCurrentState;
//		Fvector				tPoint;
//		//					 fire	  body state	 movement type		look type     point to view
//		SetStates			(
//							false,
//							eDirectionTypeBack,
//							eBodyStateStand, 
//							eMovementTypeRun, 
//							eLookTypePoint, 
//							tPoint);
		BackCover();
		m_bStateChanged		= m_ePreviousState != m_eCurrentState;
	}
	while (!m_bStopThinking);
};