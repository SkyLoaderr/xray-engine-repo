////////////////////////////////////////////////////////////////////////////
//	Module 		: xr_spawn_merge.cpp
//	Created 	: 29.01.2003
//  Modified 	: 29.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Merging level spawns for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrAI.h"
#include "xrLevel.h"

#include "xrGraph.h"
#include "xr_spawn_merge.h"

CVirtualFileStream		*tpGraphVFS = 0;
//SGraphHeader			tGraphHeader;
SCompressedGraphVertex	*tpaGameGraph;

void vfLoadGameGraph()
{
	string256						fName = "gamedata\\game.graph";
	tpGraphVFS						= xr_new<CVirtualFileStream>(fName);
//	tGraphHeader.dwVersion			= tpGraphVFS->Rdword();
//	tGraphHeader.dwVertexCount		= tpGraphVFS->Rdword();
//	tGraphHeader.dwLevelCount		= tpGraphVFS->Rdword();
//	tGraphHeader.tpLevels.resize	(tGraphHeader.dwLevelCount);
//	{
//		vector<SLevel>::iterator	I = tGraphHeader.tpLevels.begin();
//		vector<SLevel>::iterator	E = tGraphHeader.tpLevels.end();
//		for ( ; I != E; I++) {
//			tpGraphVFS->RstringZ	((*I).caLevelName);
//			tpGraphVFS->Rvector		((*I).tOffset);
//		}
//	}
//	R_ASSERT						(tGraphHeader.dwVersion == XRAI_CURRENT_VERSION);
	tpaGameGraph					= (SCompressedGraphVertex *)tpGraphVFS->Pointer();
}

void xrMergeSpawns()
{
	// load all the graphs
	CInifile ENGINE_API *pIni	= xr_new<CInifile>(INI_FILE);
	Phase("Reading level graphs");
	SSpawnHeader				tSpawnHeader;
	tSpawnHeader.dwVersion		= XRAI_CURRENT_VERSION;
	tSpawnHeader.dwLevelCount	= 0;
	vfLoadGameGraph				();
//	string256					S1;
//	float						fDummy;
//	u32							dwLevelID;
//	for (dwLevelID = 0; pIni->LineExists("game_levels",itoa(dwLevelID,S1,10)); dwLevelID++) {
//		sscanf(pIni->ReadSTRING("game_levels",itoa(dwLevelID,S1,10)),"%f,%f,%f,%s",&fDummy,&fDummy,&fDummy,S1);
//		Memory.mem_copy(tLevel.caLevelName,S1,strlen(S1) + 1);
//		strconcat(S2,"gamedata\\levels\\",S1);
//		strconcat(S1,S2,"\\level.spawn");
//		tpGraphs.push_back(xr_new<CLevelGraph>(tLevel,S1,dwOffset,dwLevelID));
//		dwOffset += tpGraphs[tpGraphs.size() - 1]->m_tGraphHeader.dwVertexCount;
//		tGraphHeader.tpLevels.push_back(tLevel);
//	}
//	R_ASSERT(tpGraphs.size());

}