////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGraph.cpp
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Building graph for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "xrLevel.h"
#include "xrGraph.h"

CStream*						vfs;			// virtual file
hdrNODES						m_header;		// m_header
BYTE*							m_nodes;		// virtual nodes DATA array
NodeCompressed**				m_nodes_ptr;	// pointers to node's data
vector<bool>					q_mark_bit;		// temporal usage mark for queries

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
	for (u32 I=0; I<m_header.count; I++)
	{
		m_nodes_ptr[I]	= (NodeCompressed*)vfs->Pointer();

		NodeCompressed	C;
		vfs->Read		(&C,sizeof(C));

		u32			L = C.links;
		vfs->Advance	(L*sizeof(NodeLink));
	}

	// special query tables
	q_mark_bit.assign	(m_header.count,false);
}

void xrBuildGraph(LPCSTR name)
{
	Phase("Loading AI map...");
//	Status("");
//	Progress(0..1);

//	Msg("%d nodes created",int(g_nodes.size()));

}