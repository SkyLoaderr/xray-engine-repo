#include "stdafx.h"
#include "compiler.h"

IC BYTE	compress(float c)
{
	int	cover = iFloor(c*15.f+EPS_L);
	clamp(cover,0,15);
	return BYTE(cover);
}

struct CNodeCompressed {
	IC	void	compress_node(NodeCompressed& Dest, vertex& Src);
};

IC void	CNodeCompressed::compress_node(NodeCompressed& Dest, vertex& Src)
{
	Dest.light	(compress(Src.LightLevel));
	for	(u32 L=0; L<4; L++)
		Dest.link(L,Src.n[L]);
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
	Dest.cover		= Src.cover_index;

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

bool neighbour(const vertex &v0, const vertex &v1)
{
	return	(v0.Pos.distance_to_xz(v1.Pos) < (0.7f + EPS_L));
}

bool neighbour(u32 index)
{
	for (int i=0; i<4; ++i)
		if ((g_nodes[index].n[i] < g_nodes.size()) && !neighbour(g_nodes[index],g_nodes[g_nodes[index].n[i]]))
			return(false);
	return	(true);
}

bool neighbour()
{
	for (u32 i=0; i<g_nodes.size(); ++i)
		if (!neighbour(i)) {
			return	(neighbour(i));
		}
	return	(true);
}

void xrSaveNodes(LPCSTR N)
{
	Msg				("NS: %d, CNS: %d, ratio: %f%%",sizeof(vertex),sizeof(CLevelGraph::CVertex),100*float(sizeof(CLevelGraph::CVertex))/float(sizeof(vertex)));

	neighbour		();
	string256		fName; 
	strconcat		(fName,N,"level.ai");

	IWriter			*fs = FS.w_open(fName);

	// Header
	Status			("Saving header...");
	hdrNODES		H;
	H.version		= XRAI_CURRENT_VERSION;
	H.count			= g_nodes.size()+1;
	H.size			= g_params.fPatchSize;
	H.size_y		= CalculateHeight(H.aabb);
	fs->w			(&H,sizeof(H));
	
	fs->w_u32		(g_covers_palette.size());
	for (u32 j=0; j<g_covers_palette.size(); ++j)
		fs->w		(&g_covers_palette[j],sizeof(g_covers_palette[j]));

	// Dummy node
	NodeCompressed	NC;
	ZeroMemory		(&NC,sizeof(NC));
	fs->w			(&NC,sizeof(NC));

	// All nodes
	Status			("Saving nodes...");
	for (u32 i=0; i<g_nodes.size(); i++)
	{
		vertex&		N	= g_nodes[i];

		Compress		(NC,N,H);
		
		fs->w			(&NC,sizeof(NC));

		Progress(float(i)/float(g_nodes.size()));
	}

	// Stats
	u32	SizeTotal	= fs->tell();
	u32	SizeData	= g_nodes.size()*sizeof(CLevelGraph::CVertex);
	u32	SizeLinks	= SizeTotal-SizeData;
	Msg		("%dK saved (D:%d / L:%d)",SizeTotal/1024,SizeData/1024,SizeLinks/1024);
}
