////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_graph.cpp
//	Created 	: 18.02.2003
//  Modified 	: 23.06.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life graph
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_graph.h"

#ifdef AI_COMPILER
	#include "xrLevel.h"
#else
	#include "..\\xrLevel.h"
#endif

void CSE_ALifeGraph::Load				(LPCSTR fName)
{ 
	m_tpGraphVFS						= FS.r_open(fName);
	m_tGraphHeader.dwVersion			= m_tpGraphVFS->r_u32();
	m_tGraphHeader.dwLevelCount			= m_tpGraphVFS->r_u32();
	m_tGraphHeader.dwVertexCount		= m_tpGraphVFS->r_u32();
	m_tGraphHeader.dwEdgeCount			= m_tpGraphVFS->r_u32();
	m_tGraphHeader.dwDeathPointCount	= m_tpGraphVFS->r_u32();
	m_tGraphHeader.tpLevels.resize		(m_tGraphHeader.dwLevelCount);
	{
		xr_vector<SLevel>::iterator		I = m_tGraphHeader.tpLevels.begin();
		xr_vector<SLevel>::iterator		E = m_tGraphHeader.tpLevels.end();
		for ( ; I != E; I++) {
			m_tpGraphVFS->r_stringZ		((*I).caLevelName);
			m_tpGraphVFS->r_fvector3	((*I).tOffset);
			(*I).dwLevelID				= m_tpGraphVFS->r_u32();
		}
	}
	R_ASSERT2							(m_tGraphHeader.dwVersion == XRAI_CURRENT_VERSION,"Graph version mismatch!");
	m_tpaGraph							= (SGraphVertex*)m_tpGraphVFS->pointer();
};
