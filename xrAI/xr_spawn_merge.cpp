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
#include "ai_nodes.h"
#include "xr_spawn_merge.h"

CVirtualFileStream				*tpGraphVFS = 0;
SCompressedGraphVertex			*tpaGameGraph;

class CSpawn {
public:
	SLevel						m_tLevel;
	CStream*					vfs;			// virtual file
	hdrNODES					m_header;		// m_header
	BYTE*						m_nodes;		// virtual nodes DATA array
	NodeCompressed**			m_nodes_ptr;	// pointers to node's data
	vector<xrServerEntity *>	m_tpSpawnPoints;

								CSpawn(SLevel &tLevel)
	{
		// loading AI map
		m_tLevel				= tLevel;
		string256				fName;
		strconcat				(fName,"gamedata\\levels\\",m_tLevel.caLevelName);
		strconcat				(fName,fName,"\\level.ai");
		vfs						= xr_new<CVirtualFileStream>(fName);
		
		// m_header & data
		vfs->Read				(&m_header,sizeof(m_header));
		R_ASSERT				(m_header.version == XRAI_CURRENT_VERSION);
		m_nodes					= (BYTE*) vfs->Pointer();

		m_fSize2				= _sqr(m_header.size)/4;
		m_fYSize2				= _sqr((float)(m_header.size_y/32767.0))/4;
		// dispatch table
		m_nodes_ptr				= (NodeCompressed**)xr_malloc(m_header.count*sizeof(void*));
		Progress				(0.0f);
		for (u32 I=0; I<m_header.count; Progress(float(++I)/m_header.count)) {
			m_nodes_ptr[I]		= (NodeCompressed*)vfs->Pointer();

			NodeCompressed		C;
			vfs->Read			(&C,sizeof(C));

			u32					L = C.links;
			vfs->Advance		(L*sizeof(NodeLink));
		}
		// loading spawn points
		
	};
							~CSpawn()
	{
		xr_delete(vfs);
	};
	
};

void vfLoadGameGraph()
{
	string256						fName = "gamedata\\game.graph";
	tpGraphVFS						= xr_new<CVirtualFileStream>(fName);
	tGraphHeader.dwVersion			= tpGraphVFS->Rdword();
	tGraphHeader.dwVertexCount		= tpGraphVFS->Rdword();
	tGraphHeader.dwLevelCount		= tpGraphVFS->Rdword();
	tGraphHeader.tpLevels.resize	(tGraphHeader.dwLevelCount);
	{
		vector<SLevel>::iterator	I = tGraphHeader.tpLevels.begin();
		vector<SLevel>::iterator	E = tGraphHeader.tpLevels.end();
		for ( ; I != E; I++) {
			tpGraphVFS->RstringZ	((*I).caLevelName);
			tpGraphVFS->Rvector		((*I).tOffset);
		}
	}
	R_ASSERT						(tGraphHeader.dwVersion == XRAI_CURRENT_VERSION);
	tpaGameGraph					= (SCompressedGraphVertex *)tpGraphVFS->Pointer();
}

void xrMergeSpawns()
{
	// load all the graphs
	CInifile ENGINE_API *pIni	= xr_new<CInifile>(INI_FILE);
	Phase						("Reading level graphs");
	vfLoadGameGraph				();
	SSpawnHeader				tSpawnHeader;
	tSpawnHeader.dwVersion		= XRAI_CURRENT_VERSION;
	tSpawnHeader.dwLevelCount	= 0;
	tSpawnHeader.dwSpawnCount	= 0;
	vector<CSpawn *>			tpLevels;
	string256					S1;
	SLevel						tLevel;
	float						fDummy;
	u32							dwLevelID;
	for (dwLevelID = 0; pIni->LineExists("game_levels",itoa(dwLevelID,S1,10)); dwLevelID++) {
		sscanf(pIni->ReadSTRING("game_levels",itoa(dwLevelID,S1,10)),"%f,%f,%f,%s",&fDummy,&fDummy,&fDummy,S1);
		Memory.mem_copy(tLevel.caLevelName,S1,strlen(S1) + 1);
		tpLevels.push_back(xr_new<CSpawn>(tLevel));
	}
	R_ASSERT(tpLevels.size());
}