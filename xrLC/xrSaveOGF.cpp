#include "stdafx.h"
#include "build.h"
#include "OGF_Face.h"

VBContainer				g_VB;
IBContainer				g_IB;
xr_vector<LPCSTR>		g_Strings;

u32						g_batch_count;
u32						g_batch_verts;
u32						g_batch_faces;

u32						g_batch_50;
u32						g_batch_100;
u32						g_batch_500;
u32						g_batch_1000;
u32						g_batch_5000;

int		RegisterString		(LPCSTR T) 
{
	for (u32 it=0; it<g_Strings.size(); it++)
		if (0==stricmp(T,g_Strings[it]))	return it;
	g_Strings.push_back		(xr_strdup(T));
	return g_Strings.size	()-1;
}

void	geom_batch_average	(u32 verts, u32 faces)
{
	g_batch_count	++;
	g_batch_verts	+=	verts;
	g_batch_faces	+=	faces;

	if (faces<=50)				g_batch_50	++;
	else if (faces<=100)		g_batch_100	++;
	else if (faces<=500)		g_batch_500	++;
	else if (faces<=1000)		g_batch_1000++;
	else if (faces<=5000)		g_batch_5000++;
}

void CBuild::SaveTREE(IWriter &fs)
{
	CMemoryWriter		MFS;

	Status				("Visuals...");
	//mem_Compact			();
	fs.open_chunk		(fsL_VISUALS);
	for (xr_vector<OGF_Base*>::iterator it = g_tree.begin(); it!=g_tree.end(); it++)
	{
		u32			idx = u32(it-g_tree.begin());
		MFS.open_chunk	(idx);
		(*it)->Save		(MFS);
		MFS.close_chunk	();
		Progress		(float(idx)/float(g_tree.size()));
	}
	fs.w				(MFS.pointer(),MFS.size());
	fs.close_chunk		();
	clMsg				("Average: %d verts/%d faces, 50(%2.1f), 100(%2.1f), 500(%2.1f), 1000(%2.1f), 5000(%2.1f)",
		g_batch_verts/g_batch_count,
		g_batch_faces/g_batch_count,
		100.f * float(g_batch_50)/float(g_batch_count),
		100.f * float(g_batch_100)/float(g_batch_count),
		100.f * float(g_batch_500)/float(g_batch_count),
		100.f * float(g_batch_1000)/float(g_batch_count),
		100.f * float(g_batch_5000)/float(g_batch_count)
		);
	//mem_Compact			();

	Status				("Geometry : vertices ...");
	MFS.clear			();
	fs.open_chunk		(fsL_VBUFFERS_DX9 | CFS_CompressMark);
	g_VB.Save			(MFS);
	fs.w_compressed		(MFS.pointer(),MFS.size());
	fs.close_chunk		();
	//mem_Compact			();

	Status				("Geometry : indices ...");
	MFS.clear();
	fs.open_chunk		(fsL_IBUFFERS | CFS_CompressMark);
	g_IB.Save			(MFS);
	fs.w_compressed		(MFS.pointer(),MFS.size());
	fs.close_chunk		();
	//mem_Compact			();

	Status				("String table...");
	fs.open_chunk		(fsL_STRINGS);
	fs.w_u32			(g_Strings.size());
	for (xr_vector<LPCSTR>::iterator T=g_Strings.begin(); T!=g_Strings.end(); T++)
		fs.w_stringZ	(*T);
	fs.close_chunk		();
	//mem_Compact			();
}
