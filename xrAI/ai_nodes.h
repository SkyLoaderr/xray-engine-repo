////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_nodes.h
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : AI nodes structures and functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "compiler.h"
#include "xrThread.h"

#include "xrGraph.h"
#include "level_graph.h"
#include "graph_engine.h"

class CNodeThread : public CThread
{
	u32					m_dwStart;
	u32					m_dwEnd;
	const CLevelGraph	*m_tpAI_Map;

public:
	CNodeThread	(u32 ID, u32 dwStart, u32 dwEnd, const CLevelGraph *tpAI_Map) : CThread(ID)
	{
		m_dwStart  = dwStart;
		m_dwEnd	   = dwEnd;
		m_tpAI_Map = tpAI_Map;
	}
	
	virtual void Execute()
	{
		u32 dwSize = m_dwEnd - m_dwStart + 1;
		thProgress = 0.0f;
		for (int i = (int)m_dwStart; i<(int)m_dwEnd; thProgress = float(++i - (int)m_dwStart)/dwSize) {
			tpaGraph[i].tNodeID = m_tpAI_Map->vertex(tpaGraph[i].tLocalPoint);
			if (m_tpAI_Map->valid_vertex_id(tpaGraph[i].tNodeID) && !m_tpAI_Map->inside(tpaGraph[i].tNodeID,tpaGraph[i].tLocalPoint,true))
				tpaGraph[i].tNodeID = u32(-1);
		}
		thProgress = 1.0f;
	}
};

class CGraphThread : public CThread
{
	u32										m_dwStart;
	u32										m_dwEnd;
	xr_vector<u32>							m_path;
	xrCriticalSection						*m_tpCriticalSection;
	const CLevelGraph						*m_graph;
	CGraphSearchEngine						*m_graph_search_engine;
	CGraphSearchEngine::CStraightLineParams	*m_parameters;

public:
	CGraphThread(u32 ID, u32 dwStart, u32 dwEnd, float fMaxDistance, xrCriticalSection *tpCriticalSection, const CLevelGraph *graph) : CThread(ID)
	{
		m_dwStart				= dwStart;
		m_dwEnd					= dwEnd;
		m_tpCriticalSection		= tpCriticalSection;
		m_graph					= graph;
		VERIFY					(m_graph);
		m_path.reserve			(10000);
		m_graph_search_engine	= xr_new<CGraphSearchEngine>(m_graph->header().vertex_count());
		m_parameters			= xr_new<CGraphSearchEngine::CStraightLineParams>(Fvector().set(0,0,0),Fvector().set(0,0,0),fMaxDistance,u32(-1),u32(-1));
	}

	virtual ~CGraphThread()
	{
		xr_delete				(m_graph_search_engine);
		xr_delete				(m_parameters);
	}
	
	virtual void Execute()
	{
		u32 dwSize = m_dwEnd - m_dwStart + 1;
		float fDistance;

		u32 N = tpaGraph.size() - 1, M = N + 1, K = N*M/2;
		u32 a = M*m_dwStart - m_dwStart*(m_dwStart + 1)/2, b = M*m_dwEnd - m_dwEnd*(m_dwEnd + 1)/2, c = b - a;
		thProgress = 0.0f;
		for (int i=(int)m_dwStart; i<(int)m_dwEnd; i++) {
			SDynamicGraphVertex &tCurrentGraphVertex = tpaGraph[i];
			for (int j = (i + 1); j<(int)M; thProgress = (float(M)*i - i*(i + 1)/2 + ++j - i - 1 - a)/c) {
				SDynamicGraphVertex &tNeighbourGraphVertex = tpaGraph[j];
				float fStraightDistance = tCurrentGraphVertex.tLocalPoint.distance_to(tNeighbourGraphVertex.tLocalPoint);
				if (fStraightDistance < m_parameters->max_range) {
					try {
						if (fStraightDistance < 200.f)
							fDistance = m_graph->check_position_in_direction(tCurrentGraphVertex.tNodeID,tCurrentGraphVertex.tLocalPoint,tNeighbourGraphVertex.tLocalPoint,m_parameters->max_range);
						else
							fDistance = m_parameters->max_range;
						if (fDistance == m_parameters->max_range) {
							VERIFY						(m_parameters);
							m_parameters->m_start_point = tCurrentGraphVertex.tLocalPoint;
							m_parameters->m_dest_point	= tNeighbourGraphVertex.tLocalPoint;
							if (m_graph_search_engine->build_path(
								*m_graph,
								tCurrentGraphVertex.tNodeID,
								tNeighbourGraphVertex.tNodeID,
								&m_path,
								*m_parameters
							))
								fDistance = m_parameters->m_distance;
//							SAIMapData			tData;
//							tData.dwFinishNode	= tNeighbourGraphVertex.tNodeID;
//							tData.m_tpAI_Map	= m_tpAI_Map;
//							m_tpMapPath.vfFindOptimalPath(
//								m_tppHeap,
//								m_tpHeap,
//								m_tpIndexes,
//								m_dwAStarStaticCounter,
//								tData,
//								tCurrentGraphVertex.tNodeID,
//								tNeighbourGraphVertex.tNodeID,
//								fDistance,
//								m_parameters->max_range,
//								tCurrentGraphVertex.tLocalPoint,
//								tNeighbourGraphVertex.tLocalPoint,
//								tpaNodes,
//								dwMaxNodeCount);

						}
						if (fDistance < m_parameters->max_range) {
							m_tpCriticalSection->Enter();
							int ii = tCurrentGraphVertex.tNeighbourCount;
							int jj = tNeighbourGraphVertex.tNeighbourCount;
							tCurrentGraphVertex.tNeighbourCount++;
							tNeighbourGraphVertex.tNeighbourCount++;
							m_tpCriticalSection->Leave();

							tCurrentGraphVertex.tpaEdges[ii].dwVertexNumber = j;
							tCurrentGraphVertex.tpaEdges[ii].fPathDistance  = fDistance;
							
							tNeighbourGraphVertex.tpaEdges[jj].dwVertexNumber = i;
							tNeighbourGraphVertex.tpaEdges[jj].fPathDistance  = fDistance;
						}
					}
					catch(char *) {
						Msg("%d, %d",i,j);
						R_ASSERT(false);
					}
				}
			}
		}
		thProgress = 1.0f;
	}
};