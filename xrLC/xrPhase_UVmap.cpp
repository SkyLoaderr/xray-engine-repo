#include "stdafx.h"
#include "build.h"

extern BOOL	hasImplicitLighting		(Face* F);

void Face::OA_Unwarp()
{
	if (pDeflector)					return;
	if (!Deflector->OA_Place(this))	return;
	
	// now iterate on all our neigbours
	for (int i=0; i<3; i++) 
		for (vecFaceIt it=v[i]->adjacent.begin(); it!=v[i]->adjacent.end(); it++)
			(*it)->OA_Unwarp();
}
void Detach			(vecFace* S)
{
	static map_v2v	verts;
	verts.clear		();
	
	// Collect vertices
	for (vecFaceIt F=S->begin(); F!=S->end(); F++)
	{
		for (int i=0; i<3; i++) {
			Vertex*		V=(*F)->v[i];
			Vertex*		VC;
			map_v2v_it	W=verts.find(V);	// iterator
			
			if (W==verts.end()) 
			{	// where is no such-vertex
				VC = V->CreateCopy_NOADJ();	// make copy
				verts.insert(make_pair(V, VC));
			} else {
				// such vertex(key) already exists - update its adjacency
				VC = W->second;
			}
			VC->prep_add		(*F);
			V->prep_remove		(*F);
			(*F)->v[i]=VC;
		}
	}
	// vertices are already registered in container
	// so we doesn't need "vers" for this time
	verts.clear	();
}

void	CBuild::xrPhase_UVmap()
{
	// Main loop
	Status("Processing...");
	float p_cost	= 1.f / float(g_XSplit.size());
	float p_total	= 0.f;
	for (int SP = 0; SP<int(g_XSplit.size()); SP++) 
	{
		Progress	(p_total+=p_cost);
		
		// Detect vertex-lighting and avoid this subdivision
		R_ASSERT(!g_XSplit[SP].empty());
		Face*		Fvl = g_XSplit[SP][0];
		if (Fvl->Shader().flags.bLIGHT_Vertex) 	continue;	// do-not touch (skip)
		if (hasImplicitLighting(Fvl))			continue;
		
		//   find first poly that doesn't has mapping and start recursion
		while (TRUE) {
			// Select maximal sized poly
			Face *	msF		= NULL;
			float	msA		= 0;
			for (vecFaceIt it = g_XSplit[SP].begin(); it!=g_XSplit[SP].end(); it++)
			{
				if ( (*it)->pDeflector == NULL ) {
					float a = (*it)->CalcArea();
					if (a>msA) {
						msF = (*it);
						msA = a;
					}
				}
			}
			if (msF) {
				g_deflectors.push_back	(new CDeflector);
				
				// Start recursion from this face
				Deflector->OA_SetNormal	(msF->N);
				msF->OA_Unwarp			();
				
				// break the cycle to startup again
				Deflector->OA_Export	();
				
				// Detach affected faces
				static vecFace faces_affected;
				faces_affected.reserve	(1024);
				for (int i=0; i<int(g_XSplit[SP].size()); i++) {
					Face *F = g_XSplit[SP][i];
					if ( F->pDeflector==Deflector ) {
						faces_affected.push_back(F);
						g_XSplit[SP].erase(g_XSplit[SP].begin()+i);
						i--;
					}
				}
				
				// detaching itself
				Detach				(&faces_affected);
				g_XSplit.push_back	(faces_affected);
				faces_affected.clear();
			} else {
				if (g_XSplit[SP].empty()) 
				{
					g_XSplit.erase(g_XSplit.begin()+SP);
					SP--;
				}
				// Cancel infine loop (while)
				break;
			}
		}
	}
	Msg("%d subdivisions...",g_XSplit.size());
	mem_CompactSubdivs	();
}

void CBuild::mem_CompactSubdivs()
{
	// Memory compact
	DWORD dwT = timeGetTime	();
	vecFace		temp;
	for (int SP = 0; SP<int(g_XSplit.size()); SP++) 
	{
		temp	= g_XSplit[SP];
		g_XSplit[SP].clear	();
		g_XSplit[SP].reserve(temp.size());
		g_XSplit[SP].insert	(g_XSplit[SP].begin(),temp.begin(),temp.end());
		_heapmin			();
		HeapCompact			(GetProcessHeap(),0);
	}
	Msg("%d ms for memory compacting...",timeGetTime()-dwT);
}
