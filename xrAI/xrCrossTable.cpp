////////////////////////////////////////////////////////////////////////////
//	Module 		: xrCrossTable.cpp
//	Created 	: 25.01.2003
//  Modified 	: 25.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Building cross table for AI nodes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defines.h"
#include "xrCrossTable.h"

DEFINE_VECTOR			(u32,			FLOAT_VECTOR,			FLOAT_IT);
DEFINE_VECTOR			(FLOAT_VECTOR,	FLOAT_VECTOR_VECTOR,	FLOAT_VECTOR_IT);

FLOAT_VECTOR			*g_tDistances;
CLevelGraph				*g_tMap;
xr_vector<bool>			*g_tMarks;

u32 absolute(u32 a, u32 b)
{
	return ((a >= b) ? (a - b): (b - a));
}

//void					vfRecurseUpdate(u32 dwStartNodeID, u32 fValue)
//{
//	if ((*g_tDistances)[dwStartNodeID] <= fValue)
//		return;
//	else
//		(*g_tDistances)[dwStartNodeID]	= fValue;
//	CLevelGraph::CVertex			*tpNode = (*g_tMap).vertex(dwStartNodeID);
//	CLevelGraph::const_iterator		I, E;
//	(*g_tMap).begin					(dwStartNodeID,I,E);
//	(*g_tMarks)[dwStartNodeID]			= true;
//	for ( ; I != E; ++I) {
//		u32							dwNexNodeID = tpNode->link(I);
//		if ((*g_tMap).valid_vertex_id(dwNexNodeID) && !(*g_tMarks)[dwNexNodeID])
//			vfRecurseUpdate			(dwNexNodeID,fValue + 1);
//	}
//	(*g_tMarks)[dwStartNodeID]		= false;
//}

void					vfRecurseUpdate(u32 dwStartNodeID, u32 fValue)
{
//	struct SFloodCell {
//		u32		id;
//		u32		dist;
//	};
//	CLevelGraph::const_iterator	I, E;
//	xr_vector<SFloodCell>		l_stack;
//	l_stack.reserve				(8192);
//	l_stack.push_back			(dwStartNodeID);
//
//	for (;!l_stack.empty();) {
//		dwStartNodeID			= l_stack.back();
//		l_stack.resize			(l_stack.size() - 1);
//		CLevelGraph::CVertex	*node = tMap.vertex(dwStartNodeID);
//		tMap.begin				(dwStartNodeID,I,E);
//		tMarks[dwStartNodeID]	= true;
//		for ( ; I != E; ++I) {
//			u32						dwNexNodeID = node->link(I);
//			if (tMap.valid_vertex_id(dwNexNodeID) && !tMarks[dwNexNodeID])
//				l_stack.push_back	(dwNexNodeID);
//		}
//	}
	g_tDistances->assign(g_tDistances->size(),u32(-1));
	(*g_tDistances)[dwStartNodeID] = 0;
	for (;;) {
		bool bOk = true;
		for (u32 i=0, n=(*g_tMap).header().vertex_count(); i<n; ++i) {
			if ((*g_tMarks)[i])
				continue;
			CLevelGraph::const_iterator	I, E;
			CLevelGraph::CVertex		*node = (*g_tMap).vertex(i);
			for (;;) {
				bool					bOk1 = true;
				(*g_tMap).begin			(i,I,E);
				u32						value = (*g_tDistances)[i];
				for ( ; I != E; ++I) {
					u32						dwNexNodeID = node->link(I);
					if ((*g_tMap).valid_vertex_id(dwNexNodeID) && !(*g_tMarks)[dwNexNodeID] && (absolute((*g_tDistances)[dwNexNodeID],value) > 1)) {
						bOk = false;
						if ((*g_tDistances)[dwNexNodeID] > value)
							(*g_tDistances)[dwNexNodeID] = value + 1;
						else {
							value = (*g_tDistances)[dwNexNodeID] - 1;
							bOk1 = false;
						}
					}
				}
				if (bOk1)
					break;
			}
		}
		if (bOk)
			break;
	}
}

void					vfRecurseMark(const CLevelGraph &tMap, xr_vector<bool> &tMarks, u32 dwStartNodeID)
{
	CLevelGraph::const_iterator	I, E;
	xr_vector<u32>				l_stack;
	l_stack.reserve				(8192);
	l_stack.push_back			(dwStartNodeID);

	for (;!l_stack.empty();) {
		dwStartNodeID			= l_stack.back();
		l_stack.resize			(l_stack.size() - 1);
		CLevelGraph::CVertex	*node = tMap.vertex(dwStartNodeID);
		tMap.begin				(dwStartNodeID,I,E);
		tMarks[dwStartNodeID]	= true;
		for ( ; I != E; ++I) {
			u32						dwNexNodeID = node->link(I);
			if (tMap.valid_vertex_id(dwNexNodeID) && !tMarks[dwNexNodeID])
				l_stack.push_back	(dwNexNodeID);
		}
	}
}

class CCrossTableBuilder {
public:
	CCrossTableBuilder(LPCSTR caProjectName);
};

CCrossTableBuilder::CCrossTableBuilder(LPCSTR caProjectName)
{
	FILE_NAME			caFileName;
	strconcat			(caFileName,caProjectName,"level.graph");
	
	Phase				("Loading level graph");
	CGameGraph			tGraph(caFileName);
	
	Phase				("Loading AI map");
	CLevelGraph			tMap(caProjectName);
	
	Phase				("Building dynamic objects");
	FLOAT_VECTOR_VECTOR	tDistances;
	int					iVertexCount	= tGraph.header().vertex_count();
	R_ASSERT2			(iVertexCount > 0,"There are no graph points in the graph!");
	int					iNodeCount		= tMap.header().vertex_count();
	xr_vector<bool>		tMarks;
	tMarks.assign		(tMap.header().vertex_count(),false);
	{
		for (int i=0; i<iVertexCount; i++)
			vfRecurseMark(tMap,tMarks,tGraph.vertex(i)->level_vertex_id());
		tMarks.flip		();
	}

	tDistances.resize	(iVertexCount);
	{
		FLOAT_VECTOR_IT		I = tDistances.begin();
		FLOAT_VECTOR_IT		E = tDistances.end();
		for ( ; I != E; I++) {
			(*I).resize		(iNodeCount);
			FLOAT_IT		i = (*I).begin();
			FLOAT_IT		e = (*I).end();
			for ( ; i != e; i++)
				*i			= u32(-1);
		}
	}
	
	Phase				("Building cross table");
//	float				fDistanceToSearch = START_DISTANCE_TO_SEARCH, jj = 0.f;
	Progress(0.f);
//	for (int ii=0; ii<MAX_DEPTH; ii++, jj += 1.f/float(u32(1) << (MAX_DEPTH - ii))) {
//		float kk = (1.f/float(u32(1) << (MAX_DEPTH - ii + 1)))/float(iVertexCount);
//		Status("Current maximum search distance is %7.2f",fDistanceToSearch);
		for (int i=0; i<iVertexCount; i++) {
			if (i)
				for (int k=0; k<(int)tMap.header().vertex_count(); k++)
					tDistances[i][k] = tDistances[i - 1][k];
			
			g_tDistances	= &tDistances[i];
			g_tMap			= &tMap;
			g_tMarks		= &tMarks;
			vfRecurseUpdate(tGraph.vertex(i)->level_vertex_id(),0);//tGraph.vertex(i)->level_point().distance_to(tMap.vertex_position(tGraph.vertex(i)->level_vertex_id())));
//			
//			Progress(jj + float(i+1)*kk);
			Progress(float(i)/float(iVertexCount));
		}
//		{
//			bool			bOk = true;
//			float			fNewDistanceToSearch = fDistanceToSearch * 2;
//			u32				dwCount = 0;
//			{
//				FLOAT_VECTOR	&tResult = tDistances[iVertexCount - 1];
//				FLOAT_IT		B = tResult.begin(), I = B;
//				FLOAT_IT		E = tResult.end();
//				for ( ; I != E; I++)
//					if ((!tMarks[I - B]) && (*I == fDistanceToSearch)) {
//						dwCount++;
//						FLOAT_VECTOR_IT i = tDistances.begin();
//						FLOAT_VECTOR_IT e = tDistances.end();
//						for ( ; i != e; i++)
//							(*i)[I - B] = fNewDistanceToSearch;
//						bOk		= false;
//					}
//			}
//			
//			if (!bOk) {
//				fDistanceToSearch = fNewDistanceToSearch;
//				Msg("%d nodes don't have a corresponding graph point",dwCount);
//			}
//			else {
//				Msg("All the nodes being connected with the graph point nodes do have a corresponding graph point",dwCount);
//				break;
//			}
//		}
//	}
	Progress			(1.f);
	
	Phase				("Saving cross table");
	CMemoryWriter					tMemoryStream;
	CGameLevelCrossTable::CHeader	tCrossTableHeader;
	
	tCrossTableHeader.dwVersion = XRAI_CURRENT_VERSION;
	tCrossTableHeader.dwNodeCount = iNodeCount;
	tCrossTableHeader.dwGraphPointCount = iVertexCount;
	
	tMemoryStream.open_chunk(CROSS_TABLE_CHUNK_VERSION);
	tMemoryStream.w(&tCrossTableHeader,sizeof(tCrossTableHeader));
	tMemoryStream.close_chunk();
	
	tMemoryStream.open_chunk(CROSS_TABLE_CHUNK_DATA);
	{
		for (int i=0; i<iNodeCount; i++) {
			FLOAT_VECTOR_IT		I = tDistances.begin(), B = I;
			FLOAT_VECTOR_IT		E = tDistances.end();
			CGameLevelCrossTable::CCell tCrossTableCell;
			tCrossTableCell.fDistance = flt_max;
			tCrossTableCell.tGraphIndex = u16(-1);
			for ( ; I != E; I++)
				if (float((*I)[i])*tMap.header().cell_size() < tCrossTableCell.fDistance) {
					tCrossTableCell.fDistance	= float((*I)[i])*tMap.header().cell_size();
					tCrossTableCell.tGraphIndex = ALife::_GRAPH_ID(I - B);
				}
			
			for (int j=0; j<iVertexCount; j++)
				if ((tGraph.vertex(j)->level_vertex_id() == i) && (tCrossTableCell.tGraphIndex != j)) {
					Msg("! Warning : graph points are too close, therefore cross table is automatically validated");
					Msg("%d : [%f][%f][%f] %d[%f] -> %d[%f]",i,VPUSH(tGraph.vertex(j)->level_point()),tCrossTableCell.tGraphIndex,tCrossTableCell.fDistance,j,tDistances[j][i]);
					tCrossTableCell.fDistance	= float(tDistances[j][i])*tMap.header().cell_size();
					tCrossTableCell.tGraphIndex = j;
				}
			tMemoryStream.w(&tCrossTableCell,sizeof(tCrossTableCell));
		}
	}
	tMemoryStream.close_chunk();
	
	strconcat			(caFileName,caProjectName,CROSS_TABLE_NAME_RAW);
	tMemoryStream.save_to(caFileName);
}

void					xrBuildCrossTable(LPCSTR caProjectName)
{
	CCrossTableBuilder	A(caProjectName);
}
