////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGraph.cpp
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Building graph for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "levelgamedef.h"
#include "xrLevel.h"
#include "xrGraph.h"

CStream*						vfs;			// virtual file
hdrNODES						m_header;		// m_header
BYTE*							m_nodes;		// virtual nodes DATA array
NodeCompressed**				m_nodes_ptr;	// pointers to node's data
vector<bool>					q_mark_bit;		// temporal usage mark for queries

typedef struct tagRPoint {
	Fvector	P;
	Fvector A;
} RPoint;

typedef struct tagSGraphEdge {
	u32		dwVertexNumber;
	float	fPathDistance;
} SGraphEdge;

typedef struct tagSGraphVertex {
	Fvector				tPoint;
	u32					dwNodeID;
	unsigned char		ucVertexType;
	tagSGraphVertex		*tpaNeighbourVertexes;
} SGraphVertex;

vector<SGraphVertex> tpaGraph;

void vfLoafAIMap(LPCSTR name)
{
	FILE_NAME	fName;
	strconcat	(fName,name,"level.ai");
	vfs			= new CVirtualFileStream	(fName);
	
	// m_header & data
	vfs->Read	(&m_header,sizeof(m_header));
	R_ASSERT	(m_header.version == XRAI_CURRENT_VERSION);
	m_nodes		= (BYTE*) vfs->Pointer();

	// dispatch table
	m_nodes_ptr	= (NodeCompressed**)xr_malloc(m_header.count*sizeof(void*));
	Progress(0.0f);
	for (u32 I=0; I<m_header.count; I++)
	{
		m_nodes_ptr[I]	= (NodeCompressed*)vfs->Pointer();

		NodeCompressed	C;
		vfs->Read		(&C,sizeof(C));

		u32			L = C.links;
		vfs->Advance	(L*sizeof(NodeLink));
		Progress(float(I + 1)/m_header.count);
	}

	// special query tables
	q_mark_bit.assign	(m_header.count,false);
}

void vfLoadAIPoints(LPCSTR name)
{
	FILE_NAME	fName;
	strconcat	(fName,name,"level.game");
	CVirtualFileStream	F(fName);

	CStream *O = 0;

	if (0!=(O = F.OpenChunk	(AIPOINT_CHUNK)))
	{
		for (int id=0; O->FindChunk(id); id++)
		{
			SGraphVertex tGraphVertex;
			O->Rvector(tGraphVertex.tPoint);
			tpaGraph.push_back(tGraphVertex);
		}
		O->Close();
	}
}

void vfLoadRespawnPoints(LPCSTR name)
{
	FILE_NAME	fName;
	strconcat	(fName,name,"level.game");
	CVirtualFileStream	F(fName);

	CStream *O = 0;

	if (0 != (O = F.OpenChunk(RPOINT_CHUNK))) {
		for (int i = 0; O->FindChunk(i); i++) {
			RPoint			R;
			int				team;

			O->Rvector		(R.P);
			O->Rvector		(R.A);
			team			= O->Rdword	();
			
			SGraphVertex	tGraphVertex;
			tGraphVertex.tPoint = R.P;
			tpaGraph.push_back(tGraphVertex);
		}
		O->Close();
	}
}

void vfBuildGraph(LPCSTR name)
{
	FILE_NAME	fName;
	strconcat	(fName,name,"level.spawn");
	CVirtualFileStream	FN(fName);
}

void vfSaveGraph(LPCSTR name)
{
	FILE_NAME	fName;
	strconcat	(fName,name,"level.spawn");
	CVirtualFileStream	FN(fName);
}

void xrBuildGraph(LPCSTR name)
{
	Phase("Loading AI map");
	vfLoafAIMap(name);
	Msg("%d nodes loaded",int(m_header.count));
	
	tpaGraph.clear();

	Phase("Loading AI points");
	vfLoadAIPoints(name);
	u32 dwAIPoints;
	Msg("%d vertexes loaded",int(dwAIPoints = tpaGraph.size()));

	Phase("Loading respawn points");
	vfLoadRespawnPoints(name);
	Msg("%d vertexes loaded",int(tpaGraph.size() - dwAIPoints));

	Phase("Building graph");
	vfBuildGraph(name);
	Msg("%d edges built",int(m_header.count));

	Phase("Saving graph");
	vfSaveGraph(name);
	Msg("%d bytes saved",int(m_header.count));
}