////////////////////////////////////////////////////////////////////////////
//	Module 		: path_test_old.cpp
//	Created 	: 18.10.2003
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Old path tests
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_nodes.h"

class CSearch {
	u32					m_dwAStarStaticCounter;
	SNode				*m_tpHeap;
	SNode				**m_tppHeap;
	SIndexNode			*m_tpIndexes;
	xr_vector<u32>		tpaNodes;
	CAStarSearch<CAIMapShortestPathNode,SAIMapData> m_tpMapPath;
	SAIMapData			tData;

public:
						CSearch		(const CAI_Map *tAI_Map)
	{
		tData.m_tpAI_Map		= tAI_Map;
		m_dwAStarStaticCounter	= 0;
		u32 S1					= (tAI_Map->m_header.count + 2)*sizeof(SNode);
		m_tpHeap				= (SNode *)xr_malloc(S1);
		ZeroMemory				(m_tpHeap,S1);
		u32 S2					= (tAI_Map->m_header.count)*sizeof(SIndexNode);
		m_tpIndexes				= (SIndexNode *)xr_malloc(S2);
		ZeroMemory				(m_tpIndexes,S2);
		u32 S3					= (tAI_Map->m_header.count)*sizeof(SNode *);
		m_tppHeap				= (SNode **)xr_malloc(S3);
		ZeroMemory				(m_tppHeap,S3);
	}

						~CSearch	()
	{
		xr_free(m_tpHeap);
		xr_free(m_tpIndexes);
		xr_free(m_tppHeap);
	}

			void		Find		(u32 dwStart, u32 dwEnd)
	{
		tData.dwFinishNode	= dwEnd;
		m_tpMapPath.vfFindOptimalPath(
			m_tppHeap,
			m_tpHeap,
			m_tpIndexes,
			m_dwAStarStaticCounter,
			tData,
			dwStart,
			dwEnd,
			tpaNodes);
	}
};

void path_test_old(LPCSTR caLevelName)
{
	CAI_Map					*graph			= xr_new<CAI_Map>				(caLevelName);
	CSearch					*search			= xr_new<CSearch>				(graph);

	u64						start, finish;
	Sleep					(1);
	start					= CPU::GetCycleCount();
	for (int i=0; i<100; ++i)
		search->Find		(1+i,graph->get_node_count() - 1 - i);
	finish					= CPU::GetCycleCount();
	Msg						("%f microseconds",float(s64(finish - start))*CPU::cycles2microsec);
	
	xr_delete				(graph);
	xr_delete				(search);
}