#include "stdafx.h"
#include "compiler.h"
#include "xr_func.h"

IC void	CompressPos	(NodePosition& Dest, Fvector& Src, hdrNODES& H)
{
	float sy = 32767.f/H.size_y;
	float sp = 1/g_params.fPatchSize;

	int px,py,pz;
	px = iFloor(Src.x*sp+EPS_L);
	py = iFloor(Src.y*sy+EPS_L);
	pz = iFloor(Src.z*sp+EPS_L);
	
	clamp	(px,-32767,32767);	Dest[0] = short	(px);
	clamp	(py,-32767,32767);	Dest[1] = short	(py);
	clamp	(pz,-32767,32767);	Dest[2] = short	(pz);
}
IC BYTE	CompressCover(float c)
{
	int	cover = iFloor(c*255.f+EPS_L);
	clamp(cover,0,255);
	return BYTE(cover);
}

void	Compress	(NodeCompressed& Dest, NodeMerged& Src, hdrNODES& H)
{
	// Compress plane (normal)
	Dest.plane	= pvCompress	(Src.plane.n);
	
	// Compress position
	CompressPos	(Dest.p0,Src.P0,H);
	CompressPos	(Dest.p1,Src.P1,H);
	
	// Sector
	R_ASSERT(Src.sector<=255);
	Dest.sector = BYTE(Src.sector);

	// Light & Cover
	Dest.light		= Src.light;
	Dest.cover[0]	= CompressCover(Src.cover[0]);
	Dest.cover[1]	= CompressCover(Src.cover[1]);
	Dest.cover[2]	= CompressCover(Src.cover[2]);
	Dest.cover[3]	= CompressCover(Src.cover[3]);

	// Compress links
	R_ASSERT	(Src.neighbours.size()<256);
	Dest.link_count	= BYTE(Src.neighbours.size());
}

float	CalculateHeight()
{
	// All nodes
	Fbox	BB; BB.invalidate();

	for (DWORD i=0; i<g_merged.size(); i++)
	{
		NodeMerged&	N	= g_merged[i];
		BB.modify	(N.P0);
		BB.modify	(N.P1);
	}
	return BB.max.y-BB.min.y+EPS_L;
}

void xrSaveNodes(LPCSTR N)
{
	Msg("NS: %d, CNS: %d, ratio: %f%%",sizeof(Node),sizeof(NodeCompressed),100*float(sizeof(NodeCompressed))/float(sizeof(Node)));

	FILE_NAME	fName; 
	strconcat	(fName,N,"level.ai");

	CFS_File	fs(fName);

	// Header
	Status("Saving header...");
	hdrNODES	H;
	H.version	= XRAI_CURRENT_VERSION;
	H.count		= g_merged.size();
	H.size		= g_params.fPatchSize;
	H.size_y	= CalculateHeight();
	H.aabb		= LevelBB;
	fs.write	(&H,sizeof(H));

	// All nodes
	Status("Saving nodes...");
	for (DWORD i=0; i<g_merged.size(); i++)
	{
		// write node itself
		NodeMerged&		N	= g_merged[i];
		NodeCompressed	NC;
		Compress		(NC,N,H);
		fs.write		(&NC,sizeof(NC));

		// write links
		for (DWORD L=0; L<N.neighbours.size(); L++)
		{
			DWORD		L_id = N.neighbours[L];
			fs.write	(&L_id,3);
		}
		Progress(float(i)/float(g_merged.size()));
	}

	// Stats
	DWORD	SizeTotal	= fs.tell();
	DWORD	SizeData	= g_merged.size()*sizeof(NodeCompressed);
	DWORD	SizeLinks	= SizeTotal-SizeData;
	Msg		("%dK saved (D:%d / L:%d)",SizeTotal/1024,SizeData/1024,SizeLinks/1024);
}
