#include "stdafx.h"
#include "build.h"

// Performs simple cross-smooth
void CBuild::CalcNormals()
{
	DWORD	Vcount	= g_vertices.size();
	float	p_total = 0;
	float	p_cost  = 1.f/(Vcount);

	Status("Processing...");
	float sm_cos = cosf(deg2rad(g_params.m_sm_angle));

	for (vecFaceIt it = g_faces.begin(); it!=g_faces.end(); it++)
		(*it)->CalcNormal();

	// remark:
	//	we use Face's bSplitted value to indicate that face is processed
	//  so bSplitted means bUsed
	for (DWORD I=0; I<Vcount; I++)
	{
		Vertex* V = g_vertices[I];

		for (vecFaceIt AFit = V->adjacent.begin(); AFit != V->adjacent.end(); AFit++)
		{
			Face*	F		= *AFit;
			F->bSplitted	= FALSE;
		}
		std::sort(V->adjacent.begin(), V->adjacent.end());

		for (DWORD AF = 0; AF < V->adjacent.size(); AF++)
		{
			Face*	F			= V->adjacent[AF];
			if (F->bSplitted)	continue;	// Face already used in calculation

			Vertex*	NV			= V->CreateCopy_NOADJ();
			
			// Calculate it's normal
			NV->N.set(0,0,0);
			for (DWORD NF = 0; NF < V->adjacent.size(); NF++)
			{
				Face*	Fn		= V->adjacent[NF];

				float	cosa	= F->N.dotproduct(Fn->N);
				if (cosa>sm_cos) {
					NV->N.add		(Fn->N);
					if (!Fn->bSplitted) {
						Fn->VReplace_not_remove	(V,NV);
						Fn->bSplitted			= TRUE;
					}
				}
			}
			if (NV->adjacent.empty()) delete NV;
			else NV->N.normalize_safe();
		}

		Progress(p_total+=p_cost);
	}

	// Destroy unused vertices
	g_bUnregister = false;
	vecVertex vtmp(g_vertices.begin(),g_vertices.begin()+Vcount);
	g_vertices.erase(g_vertices.begin(),g_vertices.begin()+Vcount);
	for (I=0; I<Vcount; I++) delete vtmp[I];
	g_bUnregister = true;

	// Clear temporary flag
	for (it = g_faces.begin(); it!=g_faces.end(); it++)
		(*it)->bSplitted = FALSE;

	Msg("%d vertices was duplicated 'cause of SM groups",g_vertices.size()-Vcount);
}
