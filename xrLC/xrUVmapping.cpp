#include "stdafx.h"
#include "build.h"

const int	edge2idx	[3][2]	= { {0,1},		{1,2},		{2,0}	};
const int	edge2idx3	[3][3]	= { {0,1,2},	{1,2,0},	{2,0,1}	};
const int	idx2edge	[3][3]  = {
	{-1,  0,  2},
	{ 0, -1,  1},
	{ 2,  1, -1}
};

IC	void GetVIdxByEdge(int edge, int &e1, int &e2, int &v)
{
	e1 = edge2idx3[edge][0];
	e2 = edge2idx3[edge][1];
	v  = edge2idx3[edge][2];
}
/*
void Face::Unwarp(int connect_edge, float u1, float v1, float u2, float v2)
{
	VERIFY(connect_edge>=0 && connect_edge<3);

	//***** firstly we need to get third point UV
	int e1,e2,vo;	// e1(u1,v1), e2(u2,v2), v(?,?)
	GetVIdxByEdge(connect_edge, e1,e2,vo);

	Vertex* E1 = v[e1];
	Vertex* E2 = v[e2];
	Vertex*  V = v[vo];

	Fvector	eD,pD;

	eD.sub(E1->P, E2->P);				// edge direction
	float	eL	= eD.magnitude();		// edge length
	eD.div(eL);							// normalize it!

	pD.sub(V->P,  E2->P);				// edge with another vertex
	float	pL	= pD.magnitude();		// len
	pD.div(pL);							// norm

	float	_cos= eD.dotproduct(pD);	// Cos(angle)
	float	_sin= sqrtf(1-_cos*_cos);	// Sin(angle)

	float	du = u1-u2, dv = v1-v2;		// get edge direction 
	float	dl = sqrtf(du*du + dv*dv);	// get edge length
	du/=dl; dv/=dl;						// normalize it

	float	pU = (du*_cos - dv*_sin)*pL+u2;	// rotate U
	float	pV = (du*_sin + dv*_cos)*pL+v2;	// rotate V

	//*****
	if (Deflector->Place(this, u1,v1, u2,v2, pU,pV))
	{
		// !!! Recursion takes place here !!!
		for (vecFaceIt it=V->adjacent.begin(); it!=V->adjacent.end(); it++)
		{
			Face* F = (*it);
			if ( F->pDeflector ) continue;

			if ( F->VContains(E1) ) {
				// Face contains both e1 and vo
				F->Unwarp(
					idx2edge[F->VIndex(E1)][F->VIndex(V)],
					u1,v1,pU,pV);
			}
			if ( F->VContains(E2) ) {
				// Face contains both e2 and vo
				F->Unwarp(
					idx2edge[F->VIndex(E2)][F->VIndex(V)],
					u2,v2,pU,pV);
			}
		}
	} else {
		// try different dirrection
		// -- rotate about 180 degree
		pU = (du*_cos + dv*_sin)*pL+u2;	// rotate U
		pV = (dv*_cos - du*_sin)*pL+v2;	// rotate V
		if (Deflector->Place(this, u1,v1, u2,v2, pU,pV))
		{
			// !!! Recursion takes place here !!!
			for (vecFaceIt it=V->adjacent.begin(); it!=V->adjacent.end(); it++)
			{
				Face* F = (*it);
				if ( F->pDeflector ) continue;
				
				if ( F->VContains(E1) ) {
					// Face contains both e1 and vo
					F->Unwarp(
						idx2edge[F->VIndex(E1)][F->VIndex(V)],
						u1,v1,pU,pV);
				}
				if ( F->VContains(E2) ) {
					// Face contains both e2 and vo
					F->Unwarp(
						idx2edge[F->VIndex(E2)][F->VIndex(V)],
						u2,v2,pU,pV);
				}
			}
		}
	}
}
*/

void Face::OA_Unwarp()
{
	if (pDeflector)					return;
	if (!Deflector->OA_Place(this))	return;

	// now iterate on all our neigbours
	for (int i=0; i<3; i++) 
	{
		for (vecFaceIt it=v[i]->adjacent.begin(); it!=v[i]->adjacent.end(); it++)
		{
			(*it)->OA_Unwarp();
		}
	}
}


extern void Detach(vecFace* S);
extern BOOL	hasImplicitLighting(Face* F);

void CBuild::BuildUVmap()
{
	// Take a copy of g_XSplit;
	g_XMerge = g_XSplit;
	
	// Main loop
	Status("Processing...");
	float p_cost	= 1.f / float(g_XSplit.size());
	float p_total	= 0.f;
	for (int SP = 0; SP<int(g_XSplit.size()); SP++) 
	{
		Progress(p_total+=p_cost);
		
		// Detect vertex-lighting and avoid this subdivision
		R_ASSERT(!g_XSplit[SP].empty());
		Face*		Fvl = g_XSplit[SP][0];
		if (Fvl->Shader().CL != SH_ShaderDef::clLightmap) 	continue; // do-not touch (skip)
		if (hasImplicitLighting(Fvl))
		{
			continue;
		} else {
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
					g_deflectors.push_back(new CDeflector);
					
					// Start recursion from this face
					Deflector->OA_SetNormal(msF->N);
					msF->OA_Unwarp();
					
					// break the cycle to startup again
					Deflector->OA_Export();
					
					// Detach affected faces
					static vecFace faces_affected;
					for (int i=0; i<int(g_XSplit[SP].size()); i++) {
						Face *F = g_XSplit[SP][i];
						if ( F->pDeflector==Deflector ) {
							faces_affected.push_back(F);
							g_XSplit[SP].erase(g_XSplit[SP].begin()+i);
							i--;
						}
					}
					
					// detaching itself
					Detach(&faces_affected);
					
					g_XSplit.push_back(faces_affected);
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
	}
	Msg("%d subdivisions...",g_XSplit.size());
}
