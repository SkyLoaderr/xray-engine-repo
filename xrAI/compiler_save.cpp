#include "stdafx.h"
#include "compiler.h"

IC void	CompressPos	(NodePosition& Pdest, Fvector& Psrc, hdrNODES& H)
{
	float sp = 1/g_params.fPatchSize;
	int row_length = iFloor((H.aabb.max.z - H.aabb.min.z)/H.size + EPS_L + .5f);
	int pxz	= iFloor((Psrc.x - H.aabb.min.x)*sp + EPS_L + .5f)*row_length + iFloor((Psrc.z - H.aabb.min.z)*sp   + EPS_L + .5f);
	int py	= iFloor(65535.f*(Psrc.y-H.aabb.min.y)/(H.size_y)+EPS_L);
	clamp	(pxz,0,(1 << 24) - 1);	Pdest.xz = pxz;
	clamp	(py,0,     65535);	Pdest.y = u16	(py);
}

IC BYTE	CompressCover(float c)
{
	int	cover = iFloor(c*15.f+EPS_L);
	clamp(cover,0,15);
	return BYTE(cover);
}

void	Compress	(NodeCompressed& Dest, vertex& Src, hdrNODES& H)
{
	// Compress plane (normal)
	Dest.plane	= pvCompress	(Src.Plane.n);
	
	// Compress position
	CompressPos	(Dest.p,Src.Pos,H);
//	CompressPos	(Dest.p1,Src.P1,H);
	
	// Sector
	// R_ASSERT(Src.sector<=255);
	// Dest.sector = BYTE(Src.sector);

	// Light & Cover
	Dest.light		= CompressCover(Src.LightLevel);
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

void xrSaveNodes(LPCSTR N)
{
	Msg				("NS: %d, CNS: %d, ratio: %f%%",sizeof(vertex),sizeof(CLevelGraph::CVertex),100*float(sizeof(CLevelGraph::CVertex))/float(sizeof(vertex)));

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
		NC.link0		= 0xffffffff;
		NC.link1		= 0xffffffff;
		NC.link2		= 0xffffffff;
		NC.link3		= 0xffffffff;
		for	(u32 L=0; L<4; L++)
			switch (L) {
				case 0 : {
					NC.link0	= N.n[L];
					break;
				}
				case 1 : {
					NC.link1	= N.n[L];
					break;
				}
				case 2 : {
					NC.link2	= N.n[L];
					break;
				 }
				case 3 : {
					NC.link3	= N.n[L];
					break;
				}
			}
		
		fs->w			(&NC,sizeof(NC));

		Progress(float(i)/float(g_nodes.size()));
	}

	// Stats
	u32	SizeTotal	= fs->tell();
	u32	SizeData	= g_nodes.size()*sizeof(CLevelGraph::CVertex);
	u32	SizeLinks	= SizeTotal-SizeData;
	Msg		("%dK saved (D:%d / L:%d)",SizeTotal/1024,SizeData/1024,SizeLinks/1024);
}
