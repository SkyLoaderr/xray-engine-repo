////////////////////////////////////////////////////////////////////////////
//	Module 		: xr_graph_merge.cpp
//	Created 	: 25.01.2003
//  Modified 	: 25.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Merging level graphs for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_ini.h"
#include "xrLevel.h"

#include "xrGraph.h"

class CLevelGraph;

DEFINE_VECTOR(CLevelGraph *,GRAPH_P_VECTOR,			GRAPH_P_IT);

class CLevelGraph {
public:
	SGraphHeader				m_tGraphHeader;
	GRAPH_VERTEX_VECTOR			m_tpVertices;
	SLevel						m_tLevel;
	SCompressedGraphVertex		*m_tpaGraph;

								CLevelGraph(SLevel &tLevel, u32 dwOffset, u32 dwLevelID)
	{
		m_tLevel				= tLevel;
		CVirtualFileStream		F(m_tLevel.caLevelName);
		F.Read					(&m_tGraphHeader,sizeof(SGraphHeader));
		m_tpaGraph				= (SCompressedGraphVertex *)F.Pointer();
		m_tpVertices.resize		(m_tGraphHeader.dwVertexCount);
		GRAPH_VERTEX_IT			B = m_tpVertices.begin();
		GRAPH_VERTEX_IT			I = B;
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		for ( ; I != E; I++) {
			(*I).tLocalPoint		= m_tpaGraph[I - B].tLocalPoint;
			(*I).tGlobalPoint.add	(m_tpaGraph[I - B].tGlobalPoint,m_tLevel.tOffset);
			(*I).tNodeID			= m_tpaGraph[I - B].tNodeID;
			(*I).tVertexType		= m_tpaGraph[I - B].tVertexType;
			(*I).tLevelID			= dwLevelID;
			(*I).tNeighbourCount	= m_tpaGraph[I - B].tNeighbourCount;
			(*I).tpaEdges			= (SGraphEdge *)xr_malloc((*I).tNeighbourCount*sizeof(SGraphEdge));
			SGraphEdge				*tpaEdges = (SGraphEdge *)((BYTE *)m_tpaGraph + m_tpaGraph[I - B].dwEdgeOffset);
			for (int i=0; i<(int)(*I).tNeighbourCount; i++) {
				(*I).tpaEdges[i]	= tpaEdges[i];
				(*I).tpaEdges[i].dwVertexNumber += dwOffset;
			}
		}
	};

	virtual							~CLevelGraph()
	{
		GRAPH_VERTEX_IT			I = m_tpVertices.begin();
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		for ( ; I != E; I++)
			xr_free((*I).tpaEdges);
	};

	void						vfAddEdge(u32 dwVertexNumber, SGraphEdge &tGraphEdge)
	{
		R_ASSERT(m_tGraphHeader.dwVertexCount > dwVertexNumber);
		m_tpVertices[dwVertexNumber].tpaEdges = (SGraphEdge *)xr_realloc(m_tpVertices[dwVertexNumber].tpaEdges,sizeof(SGraphEdge)*++m_tpVertices[dwVertexNumber].tNeighbourCount);
		m_tpVertices[dwVertexNumber].tpaEdges[m_tpVertices[dwVertexNumber].tNeighbourCount - 1] = tGraphEdge;
	}

	void						vfSaveVertices(CFS_Memory &tMemoryStream, u32 &dwOffset)
	{
		GRAPH_VERTEX_IT			I = m_tpVertices.begin();
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		for ( ; I != E; I++) {
			SCompressedGraphVertex tVertex;
			tVertex.tLocalPoint		= (*I).tLocalPoint;
			tVertex.tGlobalPoint	= (*I).tGlobalPoint;
			tVertex.tNodeID			= (*I).tNodeID;
			tVertex.tVertexType		= (*I).tVertexType;
			tVertex.tLevelID		= (*I).tLevelID;
			tVertex.dwEdgeOffset	= dwOffset;
			dwOffset				+= (tVertex.tNeighbourCount = (*I).tNeighbourCount)*sizeof(SGraphEdge);
			tMemoryStream.write		(&tVertex,sizeof(tVertex));
		}
	};
	
	void						vfSaveEdges(CFS_Memory &tMemoryStream)
	{
		GRAPH_VERTEX_IT			I = m_tpVertices.begin();
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		for ( ; I != E; I++)
			for (int i=0; i<(int)(*I).tNeighbourCount; i++)
				tMemoryStream.write	((*I).tpaEdges + i,sizeof(SGraphEdge));
	};
};

void xrMergeGraphs()
{
	// load all the graphs
	if (!pSettings->SectionExists("game_levels"))
		THROW;
	GRAPH_P_VECTOR					tpGraphs;
	SGraphHeader					tGraphHeader;
	string256						S1, S2;
	SLevel							tLevel;
	u32								dwOffset = 0;
	u32								dwLevelID;
	for (dwLevelID = 0; pSettings->LineExists("game_levels",itoa(dwLevelID,S1,10)); dwLevelID++) {
		sscanf(pSettings->ReadSTRING("game_levels",itoa(dwLevelID,S1,10)),"%f,%f,%f,%s",&(tLevel.tOffset.x),&(tLevel.tOffset.y),&(tLevel.tOffset.z),S1);
		strconcat(S2,"gamedata\\levels\\",S1);
		strconcat(tLevel.caLevelName,S2,"\\level.graph");
		tpGraphs.push_back(xr_new<CLevelGraph>(tLevel,dwOffset,dwLevelID));
		dwOffset += tpGraphs[tpGraphs.size() - 1]->m_tGraphHeader.dwVertexCount;
		Memory.mem_copy				(tLevel.caLevelName,S1,strlen(S1) + 1);
		tGraphHeader.tpLevels.push_back(tLevel);
	}
	R_ASSERT(tpGraphs.size());
	
	// merge all the graphs, i.e. add connections between vertices from different graphs
	// for all connections do:
	//		determine vertices graph ownership
	//		call vfAddEdge of the 2 corrsponding graphs
	
	// save all the graphs
	CFS_Memory						F;
	tGraphHeader.dwLevelCount		= tpGraphs.size();
	tGraphHeader.dwVersion			= XRAI_CURRENT_VERSION;
	tGraphHeader.dwVertexCount		= dwOffset;
	F.Wdword						(tGraphHeader.dwVersion);
	F.Wdword						(tGraphHeader.dwVertexCount);
	F.Wdword						(tGraphHeader.dwLevelCount);
	{
		vector<SLevel>::iterator	I = tGraphHeader.tpLevels.begin();
		vector<SLevel>::iterator	E = tGraphHeader.tpLevels.end();
		for ( ; I != E; I++) {
			F.write((*I).caLevelName,strlen((*I).caLevelName) + 1);	
			F.Wvector((*I).tOffset);
		}
	}

	dwOffset						*= sizeof(SCompressedGraphVertex);
	{
		GRAPH_P_IT					I = tpGraphs.begin();
		GRAPH_P_IT					E = tpGraphs.end();
		for ( ; I != E; I++)
			(*I)->vfSaveVertices	(F,dwOffset);
	}
	{
		GRAPH_P_IT					I = tpGraphs.begin();
		GRAPH_P_IT					E = tpGraphs.end();
		for ( ; I != E; I++)
			(*I)->vfSaveEdges		(F);
	}
	F.SaveTo("game.graph",0);
	
	// _free all the graphs
	{
		GRAPH_P_IT					I = tpGraphs.begin();
		GRAPH_P_IT					E = tpGraphs.end();
		for ( ; I != E; I++)
			xr_free(*I);
	}
}