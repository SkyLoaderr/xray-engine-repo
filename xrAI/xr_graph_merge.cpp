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
#include "xrAI.h"
#include "xrGraph.h"
#include "game_base.h"
#include "xrServer_Objects_ALife_All.h"
#include "xrSE_Factory_import_export.h"
#include "xrCrossTable.h"
#include "level_graph.h"
#include "net_utils.h"
#include "object_broker.h"
#include "xr_graph_merge.h"
using namespace ALife;

class CLevelGameGraph;

typedef struct tagSConnectionVertex {
	LPSTR		caConnectName;
	_GRAPH_ID	tGraphID;
	_GRAPH_ID	tOldGraphID;
	u32			dwLevelID;
} SConnectionVertex;

extern  HWND logWindow;

class CCompareVertexPredicate {
public:
	IC bool operator()(LPCSTR S1, LPCSTR S2) const
	{
		return(xr_strcmp(S1,S2) < 0);
	}
};

u32 dwfGetIDByLevelName(CInifile *Ini, LPCSTR caLevelName)
{
	LPCSTR				N,V;
	for (u32 k = 0; Ini->r_line("levels",k,&N,&V); k++) {
		R_ASSERT3		(Ini->section_exist(N),"Fill section properly!",N);
		R_ASSERT3		(Ini->line_exist(N,"caption"),"Fill section properly!",N);
		R_ASSERT3		(Ini->line_exist(N,"id"),"Fill section properly!",N);
		if (!xr_strcmp(Ini->r_string_wb(N,"caption"),caLevelName))
			return		(Ini->r_u32(N,"id"));
	}
	return				(u32(-1));
}

DEFINE_MAP		(u32,	::CLevelGameGraph*,	GRAPH_P_MAP,	GRAPH_P_PAIR_IT);
DEFINE_MAP_PRED	(LPSTR,	SConnectionVertex,	VERTEX_MAP,		VERTEX_PAIR_IT,	CCompareVertexPredicate);

class CLevelGameGraph {
public:
	GRAPH_VERTEX_VECTOR			m_tpVertices;
	CGameGraph::SLevel			m_tLevel;
	VERTEX_MAP					m_tVertexMap;
	u32							m_dwOffset;
	xr_vector<CGameGraph::CLevelPoint>	m_tpLevelPoints;
	CGameGraph					*m_tpGraph;

								CLevelGameGraph(const CGameGraph::SLevel &tLevel, LPCSTR S, u32 dwOffset, u32 dwLevelID, CInifile *Ini)
	{
		m_tLevel				= tLevel;
		m_dwOffset				= dwOffset;
		m_tpLevelPoints.clear	();
		
		FILE_NAME				caFileName;
		
		// loading graph
		strconcat				(caFileName,S,"level.graph");
		m_tpGraph				= xr_new<CGameGraph>(caFileName);

		strconcat				(caFileName,S,CROSS_TABLE_NAME_RAW);
		CGameLevelCrossTable	*l_tpCrossTable = xr_new<CGameLevelCrossTable>(caFileName);

		CLevelGraph				*l_tpAI_Map = xr_new<CLevelGraph>(S);

		VERIFY					(l_tpAI_Map->header().vertex_count() == l_tpCrossTable->header().level_vertex_count());
		VERIFY					(m_tpGraph->header().vertex_count() == l_tpCrossTable->header().game_vertex_count());

		{
			for (_GRAPH_ID i=0, n = m_tpGraph->header().vertex_count(); i<n; ++i)
				if ((!l_tpAI_Map->valid_vertex_id(m_tpGraph->vertex(i)->level_vertex_id()) ||
					(l_tpCrossTable->vertex(m_tpGraph->vertex(i)->level_vertex_id()).game_vertex_id() != i) ||
					!l_tpAI_Map->inside(m_tpGraph->vertex(i)->level_vertex_id(),m_tpGraph->vertex(i)->level_point()))) {
						Msg				("! Graph doesn't correspond to the cross table");
						R_ASSERT2		(false,"Graph doesn't correspond to the cross table");
				}
		}

		m_tpVertices.resize		(m_tpGraph->header().vertex_count());
		GRAPH_VERTEX_IT			B = m_tpVertices.begin();
		GRAPH_VERTEX_IT			I = B;
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		for ( ; I != E; I++) {
			(*I).tLocalPoint		= m_tpGraph->vertex(int(I - B))->level_point();
			(*I).tGlobalPoint.add	(m_tpGraph->vertex(int(I - B))->game_point(),m_tLevel.offset());
			(*I).tLevelID			= dwLevelID;
			(*I).tNodeID			= m_tpGraph->vertex(int(I - B))->level_vertex_id();
			Memory.mem_copy			((*I).tVertexTypes,m_tpGraph->vertex(int(I - B))->vertex_type(),LOCATION_TYPE_COUNT*sizeof(_LOCATION_ID));
			(*I).tNeighbourCount	= m_tpGraph->vertex(int(I - B))->edge_count();
			CGameGraph::const_iterator	b,i,e;
			m_tpGraph->begin		(int(I - B),i,e);
			(*I).tpaEdges			= (CGameGraph::CEdge*)xr_malloc((*I).tNeighbourCount*sizeof(CGameGraph::CEdge));
			b						= i;
			for ( ; i != e; ++i) {
				(*I).tpaEdges[i - b]	= *i;
				(*I).tpaEdges[i - b].dwVertexNumber += dwOffset;
			}
			(*I).dwPointOffset		= 0;
			vfGenerateDeathPoints	(int(I - B),l_tpCrossTable,l_tpAI_Map,(*I).tDeathPointCount);
		}

		xr_delete					(l_tpCrossTable);
		xr_delete					(l_tpAI_Map);
		
		// updating cross-table
		{
			strconcat				(caFileName,S,CROSS_TABLE_NAME_RAW);
			CGameLevelCrossTable	*tpCrossTable = xr_new<CGameLevelCrossTable>(caFileName);
			xr_vector<CGameLevelCrossTable::CCell> tCrossTableUpdate;
			tCrossTableUpdate.resize(tpCrossTable->header().level_vertex_count());
			for (int i=0; i<(int)tpCrossTable->header().level_vertex_count(); i++) {
				tCrossTableUpdate[i] = tpCrossTable->vertex(i);
				VERIFY				(u32(tCrossTableUpdate[i].tGraphIndex) < tpCrossTable->header().game_vertex_count());
				tCrossTableUpdate[i].tGraphIndex = tCrossTableUpdate[i].tGraphIndex + (ALife::_GRAPH_ID)dwOffset;
			}

			CMemoryWriter					tMemoryStream;
			CGameLevelCrossTable::CHeader	tCrossTableHeader;

			tCrossTableHeader.dwVersion = XRAI_CURRENT_VERSION;
			tCrossTableHeader.dwNodeCount = tpCrossTable->m_tCrossTableHeader.dwNodeCount;
			tCrossTableHeader.dwGraphPointCount = tpCrossTable->m_tCrossTableHeader.dwGraphPointCount;

			xr_delete			(tpCrossTable);

			tMemoryStream.open_chunk(CROSS_TABLE_CHUNK_VERSION);
			tMemoryStream.w(&tCrossTableHeader,sizeof(tCrossTableHeader));
			tMemoryStream.close_chunk();

			tMemoryStream.open_chunk(CROSS_TABLE_CHUNK_DATA);
			for (int i=0; i<(int)tCrossTableHeader.dwNodeCount; i++)
				tMemoryStream.w(&(tCrossTableUpdate[i]),sizeof(tCrossTableUpdate[i]));
			tMemoryStream.close_chunk();

			strconcat			(caFileName,S,CROSS_TABLE_NAME);
			tMemoryStream.save_to(caFileName);
		}

		// fill vertex map
		{
			string256								fName;
			strconcat								(fName,S,"level.spawn");
			IReader									*F = FS.r_open(fName);
			IReader									*O = 0;
			for (int id=0, i=0; 0!=(O = F->open_chunk(id)); id++)	{
				NET_Packet							P;
				P.B.count							= O->length();
				O->r								(P.B.data,P.B.count);
				O->close							();
				u16									ID;
				P.r_begin							(ID);
				R_ASSERT							(M_SPAWN==ID);
				P.r_stringZ							(fName);
				CSE_Abstract						*E = F_entity_Create(fName);
				R_ASSERT2							(E,"Can't create entity.");
				E->Spawn_Read						(P);
				CSE_ALifeGraphPoint					*tpGraphPoint = smart_cast<CSE_ALifeGraphPoint*>(E);
				if (tpGraphPoint) {
					Fvector							tVector;
					tVector							= tpGraphPoint->o_Position;
					_GRAPH_ID						tGraphID = _GRAPH_ID(-1);
					float							fMinDistance = 1000000.f;
					{
						GRAPH_VERTEX_IT					B = m_tpVertices.begin();
						GRAPH_VERTEX_IT					I = B;
						GRAPH_VERTEX_IT					E = m_tpVertices.end();
						for ( ; I != E; I++) {
							float fDistance = (*I).tLocalPoint.distance_to(tVector);
							if (fDistance < fMinDistance) {
								fMinDistance	= fDistance;
								tGraphID		= _GRAPH_ID(I - B);
								if (fMinDistance < EPS_L)
									break;
							}
						}
					}
					if (fMinDistance < EPS_L) {
						SConnectionVertex				T;
						LPSTR							S;
						S								= (char *)xr_malloc((xr_strlen(tpGraphPoint->s_name_replace) + 1)*sizeof(char));
						T.caConnectName					= (char *)xr_malloc((xr_strlen(tpGraphPoint->m_caConnectionPointName) + 1)*sizeof(char));
						T.dwLevelID						= dwfGetIDByLevelName(Ini,*tpGraphPoint->m_caConnectionLevelName);
						T.tGraphID						= (ALife::_GRAPH_ID)i;
						T.tOldGraphID					= tGraphID;
						Memory.mem_copy					(S,tpGraphPoint->s_name_replace,(u32)xr_strlen(tpGraphPoint->s_name_replace) + 1);
						Memory.mem_copy					(T.caConnectName,*tpGraphPoint->m_caConnectionPointName,(u32)xr_strlen(tpGraphPoint->m_caConnectionPointName) + 1);

						bool							ok = true;
						VERTEX_MAP::const_iterator		II = m_tVertexMap.begin();
						VERTEX_MAP::const_iterator		EE = m_tVertexMap.end();
						for ( ; II != EE; ++II)
							if (T.tOldGraphID == (*II).second.tOldGraphID) {
								ok						= false;
								Msg						("Graph point %s is removed,because it has the same position as some another graph point",E->s_name_replace);
								break;
							}

						if (ok) {
							m_tVertexMap.insert			(mk_pair(S,T));
							i++;
						}
					}
				}
				F_entity_Destroy					(E);
			}
			if (i != m_tpGraph->header().vertex_count())
				Msg									("Graph for the level %s doesn't correspond to the graph points from Level Editor! (%d : %d)",m_tLevel.name(),i,m_tpGraph->header().vertex_count());
			
			VERTEX_MAP::const_iterator				I = m_tVertexMap.begin();
			VERTEX_MAP::const_iterator				E = m_tVertexMap.end();
			for ( ; I != E; ++I) {
				R_ASSERT3	(!xr_strlen((*I).second.caConnectName) || ((*I).second.tGraphID < m_tpVertices.size()),"Rebuild graph for the level",m_tLevel.name());
			}

//			VERIFY3									(i == m_tpGraph->header().vertex_count(), "Rebuild graph for the level ",m_tLevel.name());
			O->close								();
		}
	};

	virtual							~CLevelGameGraph()
	{
		{
			GRAPH_VERTEX_IT			I = m_tpVertices.begin();
			GRAPH_VERTEX_IT			E = m_tpVertices.end();
			for ( ; I != E; I++)
				xr_free((*I).tpaEdges);
		}
		delete_data					(m_tVertexMap);
		xr_delete					(m_tpGraph);
	};

	void						vfAddEdge(u32 dwVertexNumber, CGameGraph::CEdge &tGraphEdge)
	{
		R_ASSERT(m_tpGraph->header().vertex_count() > dwVertexNumber);
		m_tpVertices[dwVertexNumber].tpaEdges = (CGameGraph::CEdge *)xr_realloc(m_tpVertices[dwVertexNumber].tpaEdges,sizeof(CGameGraph::CEdge)*++m_tpVertices[dwVertexNumber].tNeighbourCount);
		m_tpVertices[dwVertexNumber].tpaEdges[m_tpVertices[dwVertexNumber].tNeighbourCount - 1] = tGraphEdge;
	}

	void						vfSaveVertices(CMemoryWriter &tMemoryStream, u32 &dwOffset, u32 &dwPointOffset, xr_vector<CGameGraph::CLevelPoint> *tpLevelPoints)
	{
		GRAPH_VERTEX_IT			I = m_tpVertices.begin();
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		CGameGraph::CVertex		tVertex;
		for ( ; I != E; I++) {
			tVertex.tLocalPoint		= (*I).tLocalPoint;
			tVertex.tGlobalPoint	= (*I).tGlobalPoint;
			tVertex.tNodeID			= (*I).tNodeID;
			Memory.mem_copy			(tVertex.tVertexTypes,(*I).tVertexTypes,LOCATION_TYPE_COUNT*sizeof(_LOCATION_ID));
			tVertex.tLevelID		= (*I).tLevelID;
			tVertex.dwEdgeOffset	= dwOffset;
			tVertex.dwPointOffset	= dwPointOffset;
			tVertex.tNeighbourCount = (*I).tNeighbourCount;
			tVertex.tDeathPointCount = (*I).tDeathPointCount;
			tMemoryStream.w			(&tVertex,sizeof(tVertex));
			dwOffset				+= (*I).tNeighbourCount*sizeof(CGameGraph::CEdge);
			dwPointOffset			+= (*I).tDeathPointCount*sizeof(CGameGraph::CLevelPoint);
		}
	};
	
	void						vfSaveEdges(CMemoryWriter &tMemoryStream)
	{
		GRAPH_VERTEX_IT			I = m_tpVertices.begin();
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		for ( ; I != E; I++)
			for (int i=0; i<(int)(*I).tNeighbourCount; i++)
				tMemoryStream.w	((*I).tpaEdges + i,sizeof(CGameGraph::CEdge));
	};

	u32							dwfGetEdgeCount()
	{
		u32						l_dwResult = 0;
		GRAPH_VERTEX_IT			I = m_tpVertices.begin();
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		for ( ; I != E; I++)
			l_dwResult += (*I).tNeighbourCount;
		return					(l_dwResult);
	}

	u32							dwfGetDeathPointCount()
	{
		u32						l_dwResult = 0;
		GRAPH_VERTEX_IT			I = m_tpVertices.begin();
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		for ( ; I != E; I++)
			l_dwResult += (*I).tDeathPointCount;
		return					(l_dwResult);
	}

	void						vfGenerateDeathPoints(int iGraphIndex, CGameLevelCrossTable *tpCrossTable, CLevelGraph *tpAI_Map, u32 &dwDeathPointCount)
	{
		xr_vector<u32>			l_dwaNodes;
		l_dwaNodes.clear		();
		{
			for (u32 i=0, n = tpCrossTable->m_tCrossTableHeader.dwNodeCount; i<n; i++)
				if (tpCrossTable->m_tpaCrossTable[i].tGraphIndex == iGraphIndex)
					l_dwaNodes.push_back(i);
		}

		R_ASSERT2				(!l_dwaNodes.empty(),"Can't create at least one death point for specified graph point");

		random_shuffle			(l_dwaNodes.begin(),l_dwaNodes.end());

		u32						m = l_dwaNodes.size() > 10 ? _min(iFloor(.1f*l_dwaNodes.size()),255) : l_dwaNodes.size(), l_dwStartIndex = m_tpLevelPoints.size();
		m_tpLevelPoints.resize	(l_dwStartIndex + m);
		xr_vector<CGameGraph::CLevelPoint>::iterator I = m_tpLevelPoints.begin() + l_dwStartIndex;
		xr_vector<CGameGraph::CLevelPoint>::iterator E = m_tpLevelPoints.end();
		xr_vector<u32>::iterator		 i = l_dwaNodes.begin();

		dwDeathPointCount		= m;

		for ( ; I != E; I++, i++) {
			(*I).tNodeID	= *i;
			(*I).tPoint		= tpAI_Map->vertex_position(*i);
			(*I).fDistance	= tpCrossTable->vertex(*i).distance();
		}
	}

};

//void send_message()
//{
//	LPCSTR							mailslot = 	"\\\\*\\mailslot\\game";
//
//	HANDLE							handle	= CreateFile(
//		mailslot,
//		GENERIC_WRITE,
//		FILE_SHARE_READ,
//		0,
//		OPEN_EXISTING,
//		FILE_ATTRIBUTE_NORMAL,
//		0
//		);
//
//	if (INVALID_HANDLE_VALUE != handle) {
//		BOOL			result; 
//		DWORD			bytes_written; 
//
//		result			= WriteFile(
//			handle,
//			"quit",
//			(DWORD)xr_strlen("quit") + 1,
//			&bytes_written,
//			0
//			);
//	}
//}

class CGraphMerger {
public:
	CGraphMerger(LPCSTR name);
};

void read_levels(CInifile *Ini, xr_set<CLevelInfo> &levels)
{
	LPCSTR				N,V;
	string256			caFileName, file_name;
	for (u32 k = 0; Ini->r_line("levels",k,&N,&V); k++) {
		R_ASSERT3		(Ini->section_exist(N),"Fill section properly!",N);
		R_ASSERT3		(Ini->line_exist(N,"name"),"Fill section properly!",N);
		R_ASSERT3		(Ini->line_exist(N,"id"),"Fill section properly!",N);
		// ai
		strconcat		(caFileName,Ini->r_string(N,"name"),"\\level.ai");
		FS.update_path	(file_name,"$game_levels$",caFileName);
		if (!FS.exist(file_name)) {
			Msg			("There is no ai-map for the level %s! (level is not included into the game graph)",Ini->r_string(N,"name"));
			continue;
		}
		// graph
		strconcat		(caFileName,Ini->r_string(N,"name"),"\\level.graph");
		FS.update_path	(file_name,"$game_levels$",caFileName);
		if (!FS.exist(file_name)) {
			Msg			("There is no graph for the level %s! (level is not included into the game graph)",Ini->r_string(N,"name"));
			continue;
		}
		// cross table
		strconcat		(caFileName,Ini->r_string(N,"name"),"\\",CROSS_TABLE_NAME_RAW);
		FS.update_path	(file_name,"$game_levels$",caFileName);
		if (!FS.exist(file_name)) {
			Msg			("There is no cross table for the level %s! (level is not included into the game graph)",Ini->r_string(N,"name"));
			continue;
		}
		
		u32				id = Ini->r_s32(N,"id");
		LPCSTR			S = Ini->r_string(N,"name");
		{
			xr_set<CLevelInfo>::const_iterator	I = levels.begin();
			xr_set<CLevelInfo>::const_iterator	E = levels.end();
			for ( ; I != E; ++I) {
				VERIFY3	((*I).id != id,"Duplicated level id in the game.ltx",S);
			}
		}
		levels.insert	(CLevelInfo(id,S,Ini->r_fvector3(N,"offset"),N));
	}
}

CGraphMerger::CGraphMerger(LPCSTR name)
{
	// load all the graphs
	Phase("Processing level graphs");
	
	CInifile *Ini = xr_new<CInifile>(INI_FILE);
	if (!Ini->section_exist("levels"))
		THROW;
	R_ASSERT						(Ini->section_exist("levels"));

	GRAPH_P_MAP						tpGraphs;
	string256						S1, S2;
	CGameGraph::SLevel				tLevel;
	u32								dwOffset = 0;
	u32								l_dwPointOffset = 0;
	xr_vector<CGameGraph::CLevelPoint>			l_tpLevelPoints;
	l_tpLevelPoints.clear			();

	xr_set<CLevelInfo>				levels;
	read_levels						(Ini,levels);

	xr_set<CLevelInfo>::const_iterator	I = levels.begin();
	xr_set<CLevelInfo>::const_iterator	E = levels.end();
	for ( ; I != E; ++I) {
		tLevel.tOffset				= (*I).offset;
		tLevel.caLevelName			= (*I).name;
		strcpy						(S1,(*I).name);
		strconcat					(S2,name,S1);
		strconcat					(S1,S2,"\\");
		tLevel.tLevelID				= (*I).id;
		tLevel.m_section			= (*I).m_section;
		Msg							("%9s %2d %s","level",tLevel.tLevelID,(*I).name);
		::CLevelGameGraph			*tpLevelGraph = xr_new<::CLevelGameGraph>(tLevel,S1,dwOffset,tLevel.id(), Ini);
		dwOffset					+= tpLevelGraph->m_tpGraph->header().vertex_count();
		R_ASSERT2					(tpGraphs.find(tLevel.id()) == tpGraphs.end(),"Level ids _MUST_ be different!");
		tpGraphs.insert				(mk_pair(tLevel.id(),tpLevelGraph));
		tGraphHeader.tpLevels.insert(std::make_pair(tLevel.id(),tLevel));
	}
	
//	for (u32 k = 0; Ini->r_line("levels",k,&N,&V); k++) {
//		R_ASSERT					(Ini->section_exist(N));
//		tLevel.tOffset				= Ini->r_fvector3(N,"offset");
//		V							= Ini->r_string(N,"name");
//		Memory.mem_copy				(tLevel.caLevelName,V,(u32)xr_strlen(V) + 1);
//		Memory.mem_copy				(S1,V,(u32)xr_strlen(V) + 1);
//		strconcat					(S2,name,S1);
//		strconcat					(S1,S2,"\\");//level.graph");
//		tLevel.tLevelID				= Ini->r_s32(N,"id");
//		Msg							("%9s %2d %s","level",tLevel.tLevelID,V);
//		::CLevelGameGraph			*tpLevelGraph = xr_new<::CLevelGameGraph>(tLevel,S1,dwOffset,tLevel.id(), Ini);
//		dwOffset					+= tpLevelGraph->m_tpGraph->header().vertex_count();
//		R_ASSERT2					(tpGraphs.find(tLevel.id()) == tpGraphs.end(),"Level ids _MUST_ be different!");
//		tpGraphs.insert				(mk_pair(tLevel.id(),tpLevelGraph));
//		tGraphHeader.tpLevels.insert(std::make_pair(tLevel.id(),tLevel));
//    }
	R_ASSERT(tpGraphs.size());
	
	Phase("Adding interconnection points");
	{
		GRAPH_P_PAIR_IT				I = tpGraphs.begin();
		GRAPH_P_PAIR_IT				E = tpGraphs.end();
		for ( ; I != E; I++) {
			VERTEX_PAIR_IT			i = (*I).second->m_tVertexMap.begin();
			VERTEX_PAIR_IT			e = (*I).second->m_tVertexMap.end();
			for ( ; i != e; i++)
				if ((*i).second.caConnectName[0]) {
					GRAPH_P_PAIR_IT				K;
					VERTEX_PAIR_IT				M;
					CGameGraph::CEdge			tGraphEdge;
					SConnectionVertex			&tConnectionVertex = (*i).second;
					K							= tpGraphs.find(tConnectionVertex.dwLevelID);
					if (K == tpGraphs.end()) {
						Msg						("Cannot find level with level_id %d. Connection point will not be generated!",tConnectionVertex.dwLevelID);
						continue;
					}
					R_ASSERT					(K != tpGraphs.end());
					M							= (*K).second->m_tVertexMap.find(tConnectionVertex.caConnectName);
					if (M == (*K).second->m_tVertexMap.end()) {
						Msg						("Level %s with id %d has an INVALID connection point %s,\nwhich references to graph point %s on the level %s with id %d\n",(*I).second->m_tLevel.name(),(*I).second->m_tLevel.id(),(*i).first,tConnectionVertex.caConnectName,(*K).second->m_tLevel.name(),(*K).second->m_tLevel.id());
						R_ASSERT				(M != (*K).second->m_tVertexMap.end());
					}
					tGraphEdge.dwVertexNumber	= (*M).second.tGraphID + (*K).second->m_dwOffset;
					VERIFY3						(tConnectionVertex.tGraphID < (*I).second->m_tpVertices.size(),"Rebuild graph for the level",(*I).second->m_tLevel.name());
					VERIFY3						((*M).second.tGraphID < (*K).second->m_tpVertices.size(),"Rebuild graph for the level",(*K).second->m_tLevel.name());
					tGraphEdge.fPathDistance	= (*I).second->m_tpVertices[tConnectionVertex.tGraphID].tGlobalPoint.distance_to((*K).second->m_tpVertices[(*M).second.tGraphID].tGlobalPoint);
					(*I).second->vfAddEdge		((*i).second.tGraphID,tGraphEdge);
					tGraphEdge.dwVertexNumber	= (*i).second.tGraphID + (*I).second->m_dwOffset;
					(*K).second->vfAddEdge		((*M).second.tGraphID,tGraphEdge);
				}
		}
	}
	// counting edges
	{
		tGraphHeader.dwEdgeCount			= 0;
		tGraphHeader.dwDeathPointCount		= 0;
		GRAPH_P_PAIR_IT						I = tpGraphs.begin();
		GRAPH_P_PAIR_IT						E = tpGraphs.end();
		for ( ; I != E; I++) {
			tGraphHeader.dwEdgeCount		+= (*I).second->dwfGetEdgeCount();
			tGraphHeader.dwDeathPointCount	+= (*I).second->dwfGetDeathPointCount();
		}
	}

	///////////////////////////////////////////////////
	
	// save all the graphs
	Phase("Saving graph being merged");
	CMemoryWriter				F;
	tGraphHeader.dwLevelCount	= tpGraphs.size();
	tGraphHeader.dwVersion		= XRAI_CURRENT_VERSION;
	tGraphHeader.dwVertexCount	= dwOffset;
	F.w_u32						(tGraphHeader.dwVersion);
	F.w_u32						(tGraphHeader.dwLevelCount);
	F.w_u32						(tGraphHeader.dwVertexCount);
	F.w_u32						(tGraphHeader.dwEdgeCount);
	F.w_u32						(tGraphHeader.dwDeathPointCount);
	{
		CGameGraph::LEVEL_PAIR_IT	I = tGraphHeader.tpLevels.begin();
		CGameGraph::LEVEL_PAIR_IT	E = tGraphHeader.tpLevels.end();
		for ( ; I != E; I++) {
			F.w_stringZ	((*I).second.name());
			F.w_fvector3((*I).second.offset());
			F.w_u32		((*I).second.id());
			F.w_stringZ	(*(*I).second.section());
		}
	}

	u32							vertex_count = 0;
	dwOffset					*= sizeof(CGameGraph::CVertex);
	u32							l_dwOffset = F.size();
	l_dwPointOffset				= dwOffset + tGraphHeader.edge_count()*sizeof(CGameGraph::CEdge);
	u32							l_dwStartPointOffset = l_dwPointOffset;
	{
		GRAPH_P_PAIR_IT			I = tpGraphs.begin();
		GRAPH_P_PAIR_IT			E = tpGraphs.end();
		for ( ; I != E; I++) {
			(*I).second->vfSaveVertices	(F,dwOffset,l_dwPointOffset,&l_tpLevelPoints);
			vertex_count		+= (*I).second->m_tpGraph->header().vertex_count();
		}
	}
	{
		GRAPH_P_PAIR_IT			I = tpGraphs.begin();
		GRAPH_P_PAIR_IT			E = tpGraphs.end();
		for ( ; I != E; I++)
			(*I).second->vfSaveEdges(F);
	}
	{
		l_tpLevelPoints.clear	();
		GRAPH_P_PAIR_IT			I = tpGraphs.begin();
		GRAPH_P_PAIR_IT			E = tpGraphs.end();
		for ( ; I != E; I++)
			l_tpLevelPoints.insert(l_tpLevelPoints.end(),(*I).second->m_tpLevelPoints.begin(),(*I).second->m_tpLevelPoints.end());
	}
	R_ASSERT2						(l_dwStartPointOffset == F.size() - l_dwOffset,"Graph file format is corrupted");
	{
		xr_vector<CGameGraph::CLevelPoint>::const_iterator	I = l_tpLevelPoints.begin();
		xr_vector<CGameGraph::CLevelPoint>::const_iterator	E = l_tpLevelPoints.end();
		for ( ; I != E; ++I)
			save_data				(*I,F);
	}
	
	string256						l_caFileName;
	FS.update_path					(l_caFileName,"$game_data$","game.graph");

//	send_message					();
//
//	while (!DeleteFile(l_caFileName)) {
//		string512					S;
//		sprintf						(S,"Cannot delete file\"%s\"!\nClose all the applications which are using it and press 'Retry'.\nIf you don't want to save the game graph (i.e. all the levels can be INVALID)- press 'Cancel'",l_caFileName);
//		int							result = MessageBox(
//			logWindow,
//			S,
//			"Error!",
//			MB_RETRYCANCEL|MB_ICONWARNING
//		);
//		if (result == IDCANCEL)
//			break;
//		
//		send_message				();
//	}
	F.save_to						(l_caFileName);

	// free all the graphs
	Phase("Freeing resources being allocated");
	{
		GRAPH_P_PAIR_IT				I = tpGraphs.begin();
		GRAPH_P_PAIR_IT				E = tpGraphs.end();
		for ( ; I != E; I++)
			xr_free((*I).second);
	}
	xr_delete						(Ini);
}

void xrMergeGraphs(LPCSTR name)
{
	CGraphMerger	A(name);
}
