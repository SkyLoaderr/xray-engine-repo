////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_misc.cpp
//	Created 	: 27.02.2003
//  Modified 	: 27.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\..\\a_star.h"

void CAI_Stalker::vfBuildPathToDestinationPoint(CAISelectorBase *S)
{
	// building a path from and to
	if (S)
		getAI().m_tpAStar->ffFindOptimalPath(AI_NodeID,AI_Path.DestNode,AI_Path,S->m_dwEnemyNode,S->fOptEnemyDistance);
	else
		getAI().m_tpAStar->ffFindOptimalPath(AI_NodeID,AI_Path.DestNode,AI_Path);
	
	if (AI_Path.Nodes.size() > 0) {
		// if path is long enough then build travel line
		AI_Path.BuildTravelLine(Position());
		AI_Path.TravelStart = 0;
	}
	else {
		// if path is too short then clear it (patch for ExecMove)
		AI_Path.TravelPath.clear();
	}
	AI_Path.bNeedRebuild = FALSE;
}

