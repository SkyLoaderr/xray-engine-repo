////////////////////////////////////////////////////////////////////////////
//	Module 		: xr_graph_merge.cpp
//	Created 	: 25.01.2003
//  Modified 	: 25.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Merging level graphs for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_ini.h"

typedef struct tagSLevel {
	string64	caLevelName;
	u32			tLevelID;
	Fvector		tOffset;
} SLevel;

DEFINE_VECTOR(SLevel, LEVEL_VECTOR, LEVEL_IT)

void xrMergeGraphs()
{
	if (!pSettings->SectionExists("game_levels"))
		THROW;
	if (!pSettings->LineCount("game_levels"))
		THROW;

	LEVEL_VECTOR	tpLevels;
	string64		S1;
	SLevel			tLevel;
	for (tLevel.tLevelID =0; pSettings->LineExists("game_levels",itoa(tLevel.tLevelID,S1,10)); tLevel.tLevelID++) {
		sscanf(pSettings->ReadSTRING("game_levels",itoa(tLevel.tLevelID,S1,10)),"%f,%f,%f,%s",&(tLevel.tOffset.x),&(tLevel.tOffset.y),&(tLevel.tOffset.z),tLevel.caLevelName);
		tpLevels.push_back(tLevel);
	}
}