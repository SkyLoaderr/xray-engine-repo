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

#define					START_DISTANCE_TO_SEARCH				16.f
#define					MAX_DEPTH								8			// i.e. 2048 meters
DEFINE_VECTOR			(float,			FLOAT_VECTOR,			FLOAT_IT);
DEFINE_VECTOR			(FLOAT_VECTOR,	FLOAT_VECTOR_VECTOR,	FLOAT_VECTOR_IT);


void					vfRecurseUpdate(FLOAT_VECTOR &tDistances, CLevelGraph& tMap, xr_vector<bool> &tMarks, u32 dwStartNodeID, float fValue)
{
	if (tDistances[dwStartNodeID] < fValue)
		return;
	else
		tDistances[dwStartNodeID] = fValue;
	CLevelGraph::CVertex		*tpNode = tMap.vertex(dwStartNodeID);
	CLevelGraph::const_iterator	I, E;
	tMap.begin			(dwStartNodeID,I,E);
	tMarks[dwStartNodeID] = true;
	for ( ; I != E; I++) {
		u32				dwNexNodeID = tpNode->link(I);
		if (!tMarks[dwNexNodeID])
			vfRecurseUpdate(tDistances,tMap,tMarks,dwNexNodeID,fValue + tMap.distance(dwStartNodeID,dwNexNodeID));
	}
	tMarks[dwStartNodeID] = false;
}

void					vfRecurseMark(const CLevelGraph &tMap, xr_vector<bool> &tMarks, u32 dwStartNodeID)
{
	CLevelGraph::CVertex		*tpNode = tMap.vertex(dwStartNodeID);
	CLevelGraph::const_iterator	I, E;
	tMap.begin					(dwStartNodeID,I,E);
	tMarks[dwStartNodeID]		= true;
	for ( ; I != E; I++) {
		u32						dwNexNodeID = tpNode->link(I);
		if (tMap.valid_vertex_id(dwNexNodeID) && !tMarks[dwNexNodeID])
			vfRecurseMark		(tMap,tMarks,dwNexNodeID);
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
	int					iNodeCount		= tMap.header().vertex_count();
	xr_vector<bool>		tMarks;
	tMarks.assign		(tMap.header().vertex_count(),false);
	{
		for (int i=0; i<iVertexCount; i++)
			vfRecurseMark(tMap,tMarks,tGraph.vertex(i).level_vertex_id());
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
				*i			= START_DISTANCE_TO_SEARCH;
		}
	}
	
	Phase				("Building cross table");
	float				fDistanceToSearch = START_DISTANCE_TO_SEARCH, jj = 0.f;
	Progress(0.f);
	for (int ii=0; ii<MAX_DEPTH; ii++, jj += 1.f/float(u32(1) << (MAX_DEPTH - ii))) {
		float kk = (1.f/float(u32(1) << (MAX_DEPTH - ii + 1)))/float(iVertexCount);
		Status("Current maximum search distance is %7.2f",fDistanceToSearch);
		for (int i=0; i<iVertexCount; i++) {
			if (i)
				for (int k=0; k<(int)tMap.header().vertex_count(); k++)
					tDistances[i][k] = tDistances[i - 1][k];
			
			vfRecurseUpdate(tDistances[i],tMap,tMarks,tGraph.vertex(i).level_vertex_id(),tGraph.vertex(i).level_point().distance_to(tMap.vertex_position(tGraph.vertex(i).level_vertex_id())));
			
			Progress(jj + float(i+1)*kk);
		}
		{
			bool			bOk = true;
			float			fNewDistanceToSearch = fDistanceToSearch * 2;
			u32				dwCount = 0;
			{
				FLOAT_VECTOR	&tResult = tDistances[iVertexCount - 1];
				FLOAT_IT		B = tResult.begin(), I = B;
				FLOAT_IT		E = tResult.end();
				for ( ; I != E; I++)
					if ((!tMarks[I - B]) && (*I == fDistanceToSearch)) {
						dwCount++;
						FLOAT_VECTOR_IT i = tDistances.begin();
						FLOAT_VECTOR_IT e = tDistances.end();
						for ( ; i != e; i++)
							(*i)[I - B] = fNewDistanceToSearch;
						bOk		= false;
					}
			}
			
			if (!bOk) {
				fDistanceToSearch = fNewDistanceToSearch;
				Msg("%d nodes don't have a corresponding graph point",dwCount);
			}
			else {
				Msg("All the nodes being connected with the graph point nodes do have a corresponding graph point",dwCount);
				break;
			}
		}
	}
	Progress			(1.f);
	
	Phase				("Saving cross table");
	CMemoryWriter			tMemoryStream;
	CGameLevelCrossTable	tCrossTable;
	
	tCrossTable.m_tCrossTableHeader.dwVersion = XRAI_CURRENT_VERSION;
	tCrossTable.m_tCrossTableHeader.dwNodeCount = iNodeCount;
	tCrossTable.m_tCrossTableHeader.dwGraphPointCount = iVertexCount;
	
	tMemoryStream.open_chunk(CROSS_TABLE_CHUNK_VERSION);
	tMemoryStream.w(&tCrossTable.m_tCrossTableHeader,sizeof(tCrossTable.m_tCrossTableHeader));
	tMemoryStream.close_chunk();
	
	tMemoryStream.open_chunk(CROSS_TABLE_CHUNK_DATA);
	{
		for (int i=0; i<iNodeCount; i++) {
			FLOAT_VECTOR_IT		I = tDistances.begin(), B = I;
			FLOAT_VECTOR_IT		E = tDistances.end();
			CGameLevelCrossTable::CCell tCrossTableCell;
			tCrossTableCell.fDistance = fDistanceToSearch;
			tCrossTableCell.tGraphIndex = u16(-1);
			for ( ; I != E; I++)
				if ((*I)[i] < tCrossTableCell.fDistance) {
					tCrossTableCell.fDistance	= (*I)[i];
					tCrossTableCell.tGraphIndex = ALife::_GRAPH_ID(I - B);
				}
			
			for (int j=0; j<iVertexCount; j++)
				if ((tGraph.vertex(j).level_vertex_id() == i) && (tCrossTableCell.tGraphIndex != j)) {
					Msg("! Warning : graph points are too close, therefore cross table is automatically validated");
					Msg("%d : [%f][%f][%f] %d[%f] -> %d[%f]",i,VPUSH(tGraph.vertex(j).level_point()),tCrossTableCell.tGraphIndex,tCrossTableCell.fDistance,j,tDistances[j][i]);
					tCrossTableCell.fDistance	= tDistances[j][i];
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
