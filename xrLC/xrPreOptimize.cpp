#include "stdafx.h"
#include "build.h"

const int	 HDIM_X = 64;
const int	 HDIM_Y = 32;
const int	 HDIM_Z = 64;

extern bool			g_bUnregister;

IC bool				FaceEqual(Face& F1, Face& F2)
{
	// Test for 6 variations
	if ((F1.v[0]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[0]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	return false;
}

void CBuild::PreOptimize()
{
	// We use overlapping hash table to avoid boundary conflicts
	static vecVertex	HASH	[HDIM_X+1][HDIM_Y+1][HDIM_Z+1];
	Fvector				VMmin,	VMscale, VMeps, scale;

	// Calculate offset,scale,epsilon
	Fbox				bb = scene_bb;
	VMscale.set			(bb.max.x-bb.min.x, bb.max.y-bb.min.y, bb.max.z-bb.min.z);
	VMmin.set			(bb.min);
	VMeps.set			(VMscale.x/HDIM_X/2,VMscale.y/HDIM_Y/2,VMscale.z/HDIM_Z/2);
	VMeps.x				= (VMeps.x<EPS_L)?VMeps.x:EPS_L;
	VMeps.y				= (VMeps.y<EPS_L)?VMeps.y:EPS_L;
	VMeps.z				= (VMeps.z<EPS_L)?VMeps.z:EPS_L;
	scale.set			(float(HDIM_X),float(HDIM_Y),float(HDIM_Z));
	scale.div			(VMscale);

	DWORD	Vcount		= g_vertices.size();
	DWORD	Fcount		= g_faces.size();
 
	Status("Processing...");
	g_bUnregister		= false;
	for (int it = 0; it<(int)g_vertices.size(); it++)
	{
		once_more:

		if (0==(it%128)) {
			Progress(float(it)/float(g_vertices.size()));
			Status	("Processing... (%d verts removed)",Vcount-g_vertices.size());
		}

		if (it>=(int)g_vertices.size()) break;

		Vertex	*pTest	= g_vertices[it];
		Fvector	&V		= pTest->P;

		// Hash
		DWORD ix,iy,iz;
		ix = iFloor((V.x-VMmin.x)*scale.x);
		iy = iFloor((V.y-VMmin.y)*scale.y);
		iz = iFloor((V.z-VMmin.z)*scale.z);
		R_ASSERT(ix<=HDIM_X && iy<=HDIM_Y && iz<=HDIM_Z);
		vecVertex &H	= HASH[ix][iy][iz];

		// Search similar vertices in hash table
		for (vecVertexIt T=H.begin(); T!=H.end(); T++)
		{
			Vertex *pBase = *T;
			if (pBase->similar(*pTest,g_params.m_weld_distance)) 
			{
				while (pTest->adjacent.size())	pTest->adjacent.front()->VReplace(pTest, pBase);
				g_vertices.erase(g_vertices.begin()+it);
				delete pTest;
				goto once_more;
			}
		}

		// If we get here - there is no similar vertices - register in hash tables
		H.push_back	(pTest);

		DWORD ixE,iyE,izE;
		ixE = iFloor((V.x+VMeps.x-VMmin.x)*scale.x);
		iyE = iFloor((V.y+VMeps.y-VMmin.y)*scale.y);
		izE = iFloor((V.z+VMeps.z-VMmin.z)*scale.z);
		R_ASSERT(ixE<=HDIM_X && iyE<=HDIM_Y && izE<=HDIM_Z);
		
		if (ixE!=ix)							HASH[ixE][iy][iz].push_back		(pTest);
		if (iyE!=iy)							HASH[ix][iyE][iz].push_back		(pTest);
		if (izE!=iz)							HASH[ix][iy][izE].push_back		(pTest);
		if ((ixE!=ix)&&(iyE!=iy))				HASH[ixE][iyE][iz].push_back	(pTest);
		if ((ixE!=ix)&&(izE!=iz))				HASH[ixE][iy][izE].push_back	(pTest);
		if ((iyE!=iy)&&(izE!=iz))				HASH[ix][iyE][izE].push_back	(pTest);
		if ((ixE!=ix)&&(iyE!=iy)&&(izE!=iz))	HASH[ixE][iyE][izE].push_back	(pTest);
	}

	Status("Removing degenerated/duplicated faces...");
	g_bUnregister = true;
	for (it=0; it<(int)g_faces.size(); it++)
	{
		R_ASSERT(it>=0 && it<(int)g_faces.size());
		Face* F = g_faces[it];
		if ( F->isDegenerated()) {
			delete F;
			it--;
		}
		Progress(float(it)/float(g_faces.size()));
	}

	Status("Removing isolated vertices...");
	g_bUnregister = true;
	for (it = 0; it<(int)g_vertices.size(); it++)
	{
		Status("%d",it);
		R_ASSERT(it>=0 && it<(int)g_vertices.size());
		if (g_vertices[it]->adjacent.empty()) {
			// isolated
			delete g_vertices[it];
//			g_vertices.erase(g_vertices.begin()+it);
			it--;
		}
	}
	g_bUnregister = true;


	Msg("%d vertices removed.",Vcount-g_vertices.size());
	Msg("%d faces removed.",Fcount-g_faces.size());
}
