////////////////////////////////////////////////////////////////////////////
//	Module 		: path_test_old.cpp
//	Created 	: 18.10.2003
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Old path tests
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_nodes.h"

#define TEST_COUNT 1000

class CSearch {
	u32					m_dwAStarStaticCounter;
	SNode				*m_tpHeap;
	SNode				**m_tppHeap;
	SIndexNode			*m_tpIndexes;
	CAStarSearch<CAIMapShortestPathNode,SAIMapData> m_tpMapPath;
	SAIMapData			tData;

public:
	xr_vector<u32>		tpaNodes;

						CSearch		(const CLevelGraph *tAI_Map)
	{
		tData.m_tpAI_Map		= tAI_Map;
		m_dwAStarStaticCounter	= 0;
		u32 S1					= (tAI_Map->header().vertex_count() + 2)*sizeof(SNode);
		m_tpHeap				= (SNode *)xr_malloc(S1);
		ZeroMemory				(m_tpHeap,S1);
		u32 S2					= (tAI_Map->header().vertex_count())*sizeof(SIndexNode);
		m_tpIndexes				= (SIndexNode *)xr_malloc(S2);
		ZeroMemory				(m_tpIndexes,S2);
		u32 S3					= (tAI_Map->header().vertex_count())*sizeof(SNode *);
		m_tppHeap				= (SNode **)xr_malloc(S3);
		ZeroMemory				(m_tppHeap,S3);
	}

						~CSearch	()
	{
		xr_free(m_tpHeap);
		xr_free(m_tpIndexes);
		xr_free(m_tppHeap);
	}

			void		Find		(u32 dwStart, u32 dwEnd, float &f, u32 &v1)
	{
		tData.dwFinishNode	= dwEnd;
		f = 0.f;
		m_tpMapPath.vfFindOptimalPath(
			m_tppHeap,
			m_tpHeap,
			m_tpIndexes,
			m_dwAStarStaticCounter,
			tData,
			dwStart,
			dwEnd,
			tpaNodes,
			f,
			v1);
	}
};

void path_test_old(LPCSTR caLevelName)
{
	CLevelGraph				*graph			= xr_new<CLevelGraph>				(caLevelName);
	CSearch					*search			= xr_new<CSearch>				(graph);

	xr_vector<u32>			a;
//	a.resize				(ROWS*COLUMNS);
//	for (int i=0, n = ROWS*COLUMNS; i<n; ++i)
//		a[i] = (i/COLUMNS + 1)*(COLUMNS + 2) + i%COLUMNS + 1;

	a.resize				(graph->header().vertex_count());
	for (int i=0, n = (int)a.size(); i<n; ++i)
		a[i] = i;
	
	std::random_shuffle		(a.begin(),a.end());
	Msg						("%d times",_min((int)a.size(),TEST_COUNT));

	u64						start, finish;
	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep					(1);
	start					= CPU::GetCycleCount();
	float f;
	u32 v;
	for (int i=0, n=_min((int)a.size(),TEST_COUNT); i<n; i++)
		search->Find		(a[i],a[n - 1 - i],f,v);
	finish					= CPU::GetCycleCount();
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
	Msg						("%f microseconds",float(s64(finish - start))*CPU::cycles2microsec);
	Msg						("%f microseconds per search",float(s64(finish - start))*CPU::cycles2microsec/_min((int)a.size(),TEST_COUNT));

	xr_delete				(graph);
	xr_delete				(search);
}

void path_test_old(CLevelGraph *graph, xr_vector<u32> &path, u32 start, u32 end, float &f, u32 &v1)
{
	CSearch					*search			= xr_new<CSearch>				(graph);

	search->Find			(start,end,f,v1);

	path					= search->tpaNodes;

	xr_delete				(search);
}