#include "stdafx.h"
#include "build.h"

// Performs simple cross-smooth
void CBuild::CalcNormals()
{
	u32	Vcount	= g_vertices.size();
	float	p_total = 0;
	float	p_cost  = 1.f/(Vcount);

	// Clear temporary flag
	Status			("Processing...");
	float sm_cos	= _cos(deg2rad(g_params.m_sm_angle));

	for (vecFaceIt it = g_faces.begin(); it!=g_faces.end(); it++)
	{
		(*it)->flags.bSplitted	= true;
		(*it)->CalcNormal		();
	}

	// remark:
	//	we use Face's bSplitted value to indicate that face is processed
	//  so bSplitted means bUsed
	for (u32 I=0; I<Vcount; I++)
	{
		Vertex* V = g_vertices[I];

		for (vecFaceIt AFit = V->adjacent.begin(); AFit != V->adjacent.end(); AFit++)
		{
			Face*	F			= *AFit;
			F->flags.bSplitted	= false;
		}
		std::sort(V->adjacent.begin(), V->adjacent.end());

		for (u32 AF = 0; AF < V->adjacent.size(); AF++)
		{
			Face*	F			= V->adjacent[AF];
			if (F->flags.bSplitted)	continue;	// Face already used in calculation

			Vertex*	NV			= V->CreateCopy_NOADJ();
			
			// Calculate it's normal
			NV->N.set(0,0,0);
			for (u32 NF = 0; NF < V->adjacent.size(); NF++)
			{
				Face*	Fn		= V->adjacent[NF];

				float	cosa	= F->N.dotproduct(Fn->N);
				if (cosa>sm_cos) {
					NV->N.add		(Fn->N);
					if (!Fn->flags.bSplitted) {
						Fn->VReplace_not_remove	(V,NV);
						Fn->flags.bSplitted		= true;
					}
				}
			}
			if (NV->adjacent.empty()) VertexPool.destroy(NV);
			else NV->N.normalize_safe();
		}

		Progress(p_total+=p_cost);
	}
	Progress		(1.f);

	// Destroy unused vertices
	g_bUnregister	= false;
	vecVertex		vtmp;
	vtmp.assign		(g_vertices.begin(),g_vertices.begin()+Vcount);
	g_vertices.erase(g_vertices.begin(),g_vertices.begin()+Vcount);
	for (u32 I=0; I<Vcount; I++) VertexPool.destroy	(vtmp[I]);
	g_bUnregister = true;

	// Recalculate normals
	for (vecVertexIt it=g_vertices.begin(); it!=g_vertices.end(); it++)
		(*it)->normalFromAdj	();

	clMsg	("%d vertices was duplicated 'cause of SM groups",g_vertices.size()-Vcount);

	// Clear temporary flag
	for (vecFaceIt it = g_faces.begin(); it!=g_faces.end(); it++)
		(*it)->flags.bSplitted = false;

	// Models
	Status	("Models...");
	for		(u32 m=0; m<mu_models.size(); m++)
		mu_models[m]->calc_normals();
}
