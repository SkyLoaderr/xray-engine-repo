#include "stdafx.h"
#include "compiler.h"

IC BYTE	compress(float c, int max_value)
{
	int	cover = iFloor(c*float(max_value)+.5f);
	clamp(cover,0,max_value);
	return BYTE(cover);
}

struct CNodeCompressed {
	IC	void	compress_node(NodeCompressed& Dest, vertex& Src);
};

IC void	CNodeCompressed::compress_node(NodeCompressed& Dest, vertex& Src)
{
	Dest.light	(compress(Src.LightLevel,15));
	for	(u32 L=0; L<4; ++L)
		Dest.link(L,Src.n[L]);
//	for	(u32 L=0; L<4; ++L)
//		if ((Src.n[L] < g_nodes.size()) && (Dest.link(L) != Src.n[L])) {
//			Dest.link(L,Src.n[L]);
//			Dest.link(L);
//		}
}

void	Compress	(NodeCompressed& Dest, vertex& Src, hdrNODES& H)
{
	// Compress plane (normal)
	Dest.plane	= pvCompress	(Src.Plane.n);
	
	// Compress position
	CNodePositionCompressor(Dest.p,Src.Pos,H);
//	CompressPos	(Dest.p1,Src.P1,H);
	
	// Sector
	// R_ASSERT(Src.sector<=255);
	// Dest.sector = BYTE(Src.sector);

	// Light & Cover
	CNodeCompressed().compress_node(Dest,Src);
//	Dest.cover[0]	= CompressCover(Src.cover[0]);
//	Dest.cover[1]	= CompressCover(Src.cover[1]);
//	Dest.cover[2]	= CompressCover(Src.cover[2]);
//	Dest.cover[3]	= CompressCover(Src.cover[3]);
	Dest.cover0		= compress(Src.cover[0],15);
	Dest.cover1		= compress(Src.cover[1],15);
	Dest.cover2		= compress(Src.cover[2],15);
	Dest.cover3		= compress(Src.cover[3],15);
//	Msg				("[%.3f -> %d][%.3f -> %d][%.3f -> %d][%.3f -> %d]",
//		Src.cover[0],Dest.cover0,
//		Src.cover[1],Dest.cover1,
//		Src.cover[2],Dest.cover2,
//		Src.cover[3],Dest.cover3
//		);

	// Compress links
//	R_ASSERT	(Src.neighbours.size()<64);
//	Dest.links	= BYTE(Src.neighbours.size());
}

float	CalculateHeight(Fbox& BB)
{
	// All nodes
	BB.invalidate();

	for (u32 i=0; i<g_nodes.size(); i++)
	{
		vertex&	N	= g_nodes[i];
		BB.modify	(N.Pos);
	}
	return BB.max.y-BB.min.y+EPS_L;
}

xr_vector<NodeCompressed>	compressed_nodes;

class CNodeRenumberer {
	struct SSortNodesPredicate {

		IC	bool	operator()			(const NodeCompressed &vertex0, const NodeCompressed &vertex1) const
		{
			return		(vertex0.p.xz() < vertex1.p.xz());
		}


		IC	bool	operator()			(u32 vertex_id0, u32 vertex_id1) const
		{
			return		(compressed_nodes[vertex_id0].p.xz() < compressed_nodes[vertex_id1].p.xz());
		}
	};

	xr_vector<NodeCompressed>	&m_nodes;
	xr_vector<u32>				&m_sorted;
	xr_vector<u32>				&m_renumbering;

public:
					CNodeRenumberer(
						xr_vector<NodeCompressed>	&nodes, 
						xr_vector<u32>				&sorted,
						xr_vector<u32>				&renumbering
					) :
						m_nodes(nodes),
						m_sorted(sorted),
						m_renumbering(renumbering)
	{
		u32					N = (u32)m_nodes.size();
		m_sorted.resize		(N);
		m_renumbering.resize(N);

		for (u32 i=0; i<N; ++i)
			m_sorted[i]		= i;

		std::stable_sort	(m_sorted.begin(),m_sorted.end(),SSortNodesPredicate());

		for (u32 i=0; i<N; ++i)
			m_renumbering	[m_sorted[i]] = i;

		for (u32 i=0; i<N; ++i) {
			for (u32 j=0; j<4; ++j) {
				u32			vertex_id = m_nodes[i].link(j);
				if (vertex_id >= N)
					continue;
				m_nodes[i].link(j,m_renumbering[vertex_id]);
			}
		}

		std::stable_sort	(m_nodes.begin(),m_nodes.end(),SSortNodesPredicate());
	}
};

void xrSaveNodes(LPCSTR N)
{
	Msg				("NS: %d, CNS: %d, ratio: %f%%",sizeof(vertex),sizeof(CLevelGraph::CVertex),100*float(sizeof(CLevelGraph::CVertex))/float(sizeof(vertex)));

	Msg				("Renumbering nodes...");

	string256		fName; 
	strconcat		(fName,N,"level.ai");

	IWriter			*fs = FS.w_open(fName);

	// Header
	Status			("Saving header...");
	hdrNODES		H;
	H.version		= XRAI_CURRENT_VERSION;
	H.count			= g_nodes.size();
	H.size			= g_params.fPatchSize;
	H.size_y		= CalculateHeight(H.aabb);
	fs->w			(&H,sizeof(H));
	
//	fs->w_u32		(g_covers_palette.size());
//	for (u32 j=0; j<g_covers_palette.size(); ++j)
//		fs->w		(&g_covers_palette[j],sizeof(g_covers_palette[j]));

	// All nodes
	Status			("Saving nodes...");
	for (u32 i=0; i<g_nodes.size(); ++i) {
		vertex			&N	= g_nodes[i];
		NodeCompressed	NC;
		Compress		(NC,N,H);
		compressed_nodes.push_back(NC);
	}

	xr_vector<u32>	sorted;
	xr_vector<u32>	renumbering;
	CNodeRenumberer	A(compressed_nodes,sorted,renumbering);

	for (u32 i=0; i<g_nodes.size(); ++i) {
		fs->w			(&compressed_nodes[i],sizeof(NodeCompressed));
		Progress		(float(i)/float(g_nodes.size()));
	}
	// Stats
	u32	SizeTotal	= fs->tell();
	Msg				("%dK saved",SizeTotal/1024);

	FS.w_close		(fs);
}

#include "game_graph.h"
#include "game_level_cross_table.h"
#define VERSION_OFFSET 1

class CRenumbererConverter {
public:

			bool	save_file	(LPCSTR origin, LPCSTR s1, LPCSTR s2)
	{
		string256				original,backup;
		strcpy					(backup,origin);
		strcpy					(original,origin);
		strcat					(backup,s2);
		strcat					(original,s1);
		if (!FS.exist(original))
			return				(false);
		IReader					*fs0 = FS.r_open(original);
		IWriter					*fs1 = FS.w_open(backup);
		fs1->w					(fs0->pointer(),fs0->elapsed());
		FS.r_close				(fs0);
		FS.w_close				(fs1);
		return					(true);
	}

			void check_consistency(LPCSTR origin)
	{
		string256				original;
		CLevelGraph				*level_graph = xr_new<CLevelGraph>(origin,XRAI_CURRENT_VERSION - VERSION_OFFSET);
		if (level_graph->header().version() != XRAI_CURRENT_VERSION - VERSION_OFFSET) {
			xr_delete			(level_graph);
			return;
		}
		strcpy					(original,origin);
		strcat					(original,"level.graph");
		if (!FS.exist(original)) {
			xr_delete			(level_graph);
			return;
		}
		CGameGraph				*game_graph = xr_new<CGameGraph>(original,XRAI_CURRENT_VERSION - VERSION_OFFSET);
		if (game_graph->header().version() != XRAI_CURRENT_VERSION - VERSION_OFFSET) {
			xr_delete			(level_graph);
			xr_delete			(game_graph);
			return;
		}
		strcpy					(original,origin);
		strcat					(original,"level.gct.raw");
		if (!FS.exist(original)) {
			xr_delete			(level_graph);
			return;
		}
		CGameLevelCrossTable	*cross_table = xr_new<CGameLevelCrossTable>(original,XRAI_CURRENT_VERSION - VERSION_OFFSET);
		if (cross_table->header().version() != XRAI_CURRENT_VERSION - VERSION_OFFSET) {
			xr_delete			(level_graph);
			xr_delete			(game_graph);
			xr_delete			(cross_table);
			return;
		}
		for (ALife::_GRAPH_ID i=0, n = game_graph->header().vertex_count(); i<n; ++i)
			if ((!level_graph->valid_vertex_id(game_graph->vertex(i)->level_vertex_id()) ||
				(cross_table->vertex(game_graph->vertex(i)->level_vertex_id()).game_vertex_id() != i)
//				!level_graph->inside(game_graph->vertex(i)->level_vertex_id(),game_graph->vertex(i)->level_point())
				)) {
					Msg				("! Graph doesn't correspond to the cross table");
					R_ASSERT2		(false,"Graph doesn't correspond to the cross table");
				}

		xr_delete				(level_graph);
		xr_delete				(game_graph);
		xr_delete				(cross_table);
	}

					CRenumbererConverter(LPCSTR folder)
	{
		// gathering existing information
		string256				original,origin;
		FS.update_path			(origin,"$game_levels$",folder);
		
		check_consistency		(origin);

		if (!save_file(origin,"level.ai","level.ai.backup"))
			return;
		// loading level graph
		CLevelGraph				*level_graph = xr_new<CLevelGraph>(origin,XRAI_CURRENT_VERSION - VERSION_OFFSET);
		if (level_graph->header().version() != XRAI_CURRENT_VERSION - VERSION_OFFSET) {
			xr_delete			(level_graph);
			return;
		}
		hdrNODES				level_header = level_graph->header();
		u32						N = level_header.count;
		NodeCompressed6			*nodes = (NodeCompressed6*)(level_graph->m_nodes);
		compressed_nodes.resize	(N);
		for (u32 i=0; i<N; ++i) {
			NodeCompressed6				node = nodes[i];
			compressed_nodes[i].cover0	= node.cover0;
			compressed_nodes[i].cover1	= node.cover1;
			compressed_nodes[i].cover2	= node.cover2;
			compressed_nodes[i].cover3	= node.cover3;
			compressed_nodes[i].plane	= node.plane;
			compressed_nodes[i].p		= node.p;
			compressed_nodes[i].link	(0,node.link(0) == 0x001fffff ? u32(-1) : node.link(0));
			compressed_nodes[i].link	(1,node.link(1) == 0x001fffff ? u32(-1) : node.link(1));
			compressed_nodes[i].link	(2,node.link(2) == 0x001fffff ? u32(-1) : node.link(2));
			compressed_nodes[i].link	(3,node.link(3) == 0x001fffff ? u32(-1) : node.link(3));
			compressed_nodes[i].light	(node.light());

			VERIFY						(compressed_nodes[i].cover0	  == node.cover0);
			VERIFY						(compressed_nodes[i].cover1	  == node.cover1);
			VERIFY						(compressed_nodes[i].cover2	  == node.cover2);
			VERIFY						(compressed_nodes[i].cover3	  == node.cover3);
			VERIFY						(compressed_nodes[i].plane	  == node.plane);
			VERIFY						(compressed_nodes[i].p.xz()	  == node.p.xz());
			VERIFY						(compressed_nodes[i].p.y()	  == node.p.y());
			VERIFY						((compressed_nodes[i].link(0) == node.link(0)) || ((node.link(0) == 0x001fffff) && (compressed_nodes[i].link(0) == 0x007fffff)));
			VERIFY						((compressed_nodes[i].link(1) == node.link(1)) || ((node.link(1) == 0x001fffff) && (compressed_nodes[i].link(1) == 0x007fffff)));
			VERIFY						((compressed_nodes[i].link(2) == node.link(2)) || ((node.link(2) == 0x001fffff) && (compressed_nodes[i].link(2) == 0x007fffff)));
			VERIFY						((compressed_nodes[i].link(3) == node.link(3)) || ((node.link(3) == 0x001fffff) && (compressed_nodes[i].link(3) == 0x007fffff)));
			VERIFY						(compressed_nodes[i].light()  == node.light());
		}
		xr_delete				(level_graph);

//		// renumbering level nodes
//		xr_vector<u32>			sorted;
//		xr_vector<u32>			renumbering;
//		CNodeRenumberer			A(compressed_nodes,sorted,renumbering);
//
		// changing level
		level_header.version	= XRAI_CURRENT_VERSION;
		// writing level copy
		{
			strcpy					(original,origin);
			strcat					(original,"level.ai");
			IWriter					*fs = FS.w_open(original);
			fs->w					(&level_header,sizeof(level_header));
			for (u32 i=0; i<level_header.count; ++i)
				fs->w				(&compressed_nodes[i],sizeof(NodeCompressed));
			FS.w_close				(fs);
		}

		if (!save_file(origin,"level.graph","level.graph.backup"))
			return;
		// loading game graph
		strcpy					(original,origin);
		strcat					(original,"level.graph");
		CGameGraph				*game_graph = xr_new<CGameGraph>(original,XRAI_CURRENT_VERSION - VERSION_OFFSET);
		if (game_graph->header().version() != XRAI_CURRENT_VERSION - VERSION_OFFSET) {
			xr_delete			(game_graph);
			return;
		}
		CGameGraph::CHeader		game_header = game_graph->header();
		xr_vector<CGameGraph::CVertex>	game_nodes;
		xr_vector<CGameGraph::CEdge>	game_edges;
		u32						edge_count = 0;
		game_nodes.resize		(game_header.vertex_count());
		for (u32 i=0; i<game_header.vertex_count(); ++i) {
			game_nodes[i]		= *game_graph->vertex(i);
			edge_count			+= game_graph->vertex(i)->edge_count();
		}
		game_header.dwEdgeCount	= edge_count;
		game_edges.resize		(edge_count);
		for (u32 i=0; i<edge_count; ++i)
			game_edges[i]		= ((CGameGraph::CEdge*)(game_graph->m_nodes + game_nodes.size()))[i];
		xr_delete				(game_graph);

		// changing game graph
		game_header.dwVersion	= XRAI_CURRENT_VERSION;
//		for (u32 i=0; i<game_header.vertex_count(); ++i)
//			game_nodes[i].tNodeID = renumbering[game_nodes[i].tNodeID];

		// writing graph copy
		{
			strcpy					(original,origin);
			strcat					(original,"level.graph");
			IWriter					*fs = FS.w_open(original);

			fs->w_u32				(game_header.dwVersion);
			fs->w_u32				(game_header.dwLevelCount);
			fs->w_u32				(game_header.dwVertexCount);
			fs->w_u32				(game_header.dwEdgeCount);
			fs->w_u32				(game_header.dwDeathPointCount);
			CGameGraph::LEVEL_PAIR_IT	I = game_header.tpLevels.begin();
			CGameGraph::LEVEL_PAIR_IT	E = game_header.tpLevels.end();
			for ( ; I != E; I++) {
				fs->w_stringZ		((*I).second.name());
				fs->w_fvector3		((*I).second.offset());
				fs->w_u32			((*I).second.id());
			}
			for (u32 i=0; i<game_header.vertex_count(); ++i)
				fs->w				(&game_nodes[i],sizeof(CGameGraph::CVertex));
			for (u32 i=0; i<edge_count; ++i)
				fs->w				(&game_edges[i],sizeof(CGameGraph::CEdge));
			FS.w_close				(fs);
		}

		if (!save_file(origin,"level.gct.raw","level.gct.raw.backup"))
			return;
		// loading cross table
		strcpy					(original,origin);
		strcat					(original,"level.gct.raw");
		CGameLevelCrossTable	*cross_table = xr_new<CGameLevelCrossTable>(original,XRAI_CURRENT_VERSION - VERSION_OFFSET);
		if (cross_table->header().version() != XRAI_CURRENT_VERSION - VERSION_OFFSET) {
			xr_delete			(cross_table);
			return;
		}
		CGameLevelCrossTable::CHeader			cross_header = cross_table->header();
		xr_vector<CGameLevelCrossTable::CCell>	cross_nodes;
		cross_nodes.resize		(cross_header.level_vertex_count());
		for (u32 i=0; i<cross_header.level_vertex_count(); ++i)
//			cross_nodes[renumbering[i]]	= cross_table->m_tpaCrossTable[i];
			cross_nodes[i]		= cross_table->m_tpaCrossTable[i];
		xr_delete				(cross_table);

		// changing cross header
		cross_header.dwVersion	= XRAI_CURRENT_VERSION;;
		
		// writing cross copy
		{
			strcpy					(original,origin);
			strcat					(original,"level.gct.raw");
			IWriter					*fs = FS.w_open(original);
			
			fs->open_chunk			(CROSS_TABLE_CHUNK_VERSION);
			fs->w					(&cross_header,sizeof(cross_header));
			fs->close_chunk			();

			fs->open_chunk			(CROSS_TABLE_CHUNK_DATA);
			for (u32 i=0; i<N; ++i)
				fs->w				(&cross_nodes[i],sizeof(CGameLevelCrossTable::CCell));
			fs->close_chunk			();
			FS.w_close				(fs);
		}

		{
			CLevelGraph				*level_graph = xr_new<CLevelGraph>(origin);
			strcpy					(original,origin);
			strcat					(original,"level.graph");
			CGameGraph				*game_graph = xr_new<CGameGraph>(original);
			strcpy					(original,origin);
			strcat					(original,"level.gct.raw");
			CGameLevelCrossTable	*cross_table = xr_new<CGameLevelCrossTable>(original);
			for (ALife::_GRAPH_ID i=0, n = game_graph->header().vertex_count(); i<n; ++i)
				if ((!level_graph->valid_vertex_id(game_graph->vertex(i)->level_vertex_id()) ||
					(cross_table->vertex(game_graph->vertex(i)->level_vertex_id()).game_vertex_id() != i) ||
					!level_graph->inside(game_graph->vertex(i)->level_vertex_id(),game_graph->vertex(i)->level_point()))) {
						Msg				("! Graph doesn't correspond to the cross table");
						R_ASSERT2		(false,"Graph doesn't correspond to the cross table");
					}

			xr_delete				(level_graph);
			xr_delete				(game_graph);
			xr_delete				(cross_table);
		}

	}
};

void xrConvertMaps	()
{
	VERIFY				(XRAI_CURRENT_VERSION == 7);
//	CRenumbererConverter("fog_dima\\");
//	CRenumbererConverter("fog_dima1\\");
//	CRenumbererConverter("critical\\podval_np_04_r2\\");

	string256			path,drive,folder,file,extension;
	FS.update_path		(path,"$game_levels$","");
	xr_vector<char*>	*file_list = FS.file_list_open(path);
	if (!file_list)
		return;
	xr_vector<char*>::const_iterator	I = file_list->begin();
	xr_vector<char*>::const_iterator	E = file_list->end();
	for ( ; I != E; ++I) {
		_splitpath		(*I,drive,folder,file,extension);
		if (xr_strcmp(".ai",extension))
			continue;
		Msg						("%s",*I);
		CRenumbererConverter	A(folder);
	}
	xr_delete			(file_list);
}
