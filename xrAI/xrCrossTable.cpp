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
#include "ai_map.h"

#define					START_DISTANCE_TO_SEARCH				16.f
#define					MAX_DEPTH								8			// i.e. 2048 meters
DEFINE_VECTOR			(float,			FLOAT_VECTOR,			FLOAT_IT);
DEFINE_VECTOR			(FLOAT_VECTOR,	FLOAT_VECTOR_VECTOR,	FLOAT_VECTOR_IT);

void					vfRecurseUpdate(FLOAT_VECTOR &tDistances, CAI_Map& tMap, vector<bool> &tMarks, u32 dwStartNodeID, float fValue)
{
	if (tDistances[dwStartNodeID] < fValue)
		return;
	else
		tDistances[dwStartNodeID] = fValue;
	NodeCompressed		*tpNode = tMap.Node(dwStartNodeID);
	NodeLink			*I = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));;
	NodeLink			*E = I + tpNode->links;
	tMarks[dwStartNodeID] = true;
	for ( ; I != E; I++) {
		u32				dwNexNodeID = tMap.UnpackLink(*I);
		if (!tMarks[dwNexNodeID])
			vfRecurseUpdate(tDistances,tMap,tMarks,dwNexNodeID,fValue + tMap.ffGetDistanceBetweenNodeCenters(dwStartNodeID,dwNexNodeID));
	}
	tMarks[dwStartNodeID] = false;
}

void					vfRecurseMark(CAI_Map& tMap, vector<bool> &tMarks, u32 dwStartNodeID)
{
	NodeCompressed		*tpNode = tMap.Node(dwStartNodeID);
	NodeLink			*I = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));;
	NodeLink			*E = I + tpNode->links;
	tMarks[dwStartNodeID] = true;
	for ( ; I != E; I++) {
		u32				dwNexNodeID = tMap.UnpackLink(*I);
		if (!tMarks[dwNexNodeID])
			vfRecurseMark(tMap,tMarks,dwNexNodeID);
	}
}

void					xrBuildCrossTable(LPCSTR caProjectName)
{
	FILE_NAME			caFileName;
	strconcat			(caFileName,caProjectName,"level.graph");
	
	Phase				("Loading level graph");
	CGraph				tGraph(caFileName);
	
	Phase				("Loading AI map");
	CAI_Map				tMap(caProjectName);
	
	Phase				("Allocating memory");
	FLOAT_VECTOR_VECTOR	tDistances;
	int					iVertexCount	= tGraph.m_tGraphHeader.dwVertexCount;
	int					iNodeCount		= tMap.m_header.count;
	vector<bool>		tMarks;
	tMarks.resize		(tMap.m_header.count);
	tMarks.assign		(tMap.m_header.count,false);
	{
		for (int i=0; i<iVertexCount; i++)
			vfRecurseMark(tMap,tMarks,tGraph.m_tpaGraph[i].tNodeID);
		tMarks.flip();
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
				for (int k=0; k<(int)tMap.m_header.count; k++)
					tDistances[i][k] = tDistances[i - 1][k];
			
			vfRecurseUpdate(tDistances[i],tMap,tMarks,tGraph.m_tpaGraph[i].tNodeID,tGraph.m_tpaGraph[i].tLocalPoint.distance_to(tMap.tfGetNodeCenter(tGraph.m_tpaGraph[i].tNodeID)));
			
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
						//if (dwCount < 10)
						//	Msg		("%d node doesn't have a correspodning graph point",I - B);
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
//				FLOAT_VECTOR_IT		I = tDistances.begin();
//				FLOAT_VECTOR_IT		E = tDistances.end();
//				for ( ; I != E; I++) {
//					FLOAT_IT		i = (*I).begin();
//					FLOAT_IT		e = (*I).end();
//					for ( ; i != e; i++)
//						*i			= fDistanceToSearch;
//				}
			}
			else
				break;
		}
	}
	Progress			(1.f);
	
	Phase				("Saving cross table");


	Phase				("Freeing resources being allocated");

}
