#include "stdafx.h"
#include "xrMU_Model.h"

void xrMU_Model::calc_normals()
{
	DWORD	Vcount	= m_vertices.size();
	float	p_total = 0;
	float	p_cost  = 1.f/(Vcount);

	// Clear temporary flag
	float	sm_cos	= _cos(deg2rad(89.f));

	for (v_faces_it it = m_faces.begin(); it!=m_faces.end(); it++)
	{
		(*it)->bSplitted	= FALSE;
		(*it)->CalcNormal	();
	}

	// remark:
	//	we use Face's bSplitted value to indicate that face is processed
	//  so bSplitted means bUsed
	for (DWORD I=0; I<Vcount; I++)
	{
		_vertex* V	= m_vertices[I];

		for (v_faces_it AFit = V->adjacent.begin(); AFit != V->adjacent.end(); AFit++)
		{
			_face*	F		= *AFit;
			F->bSplitted	= FALSE;
		}
		std::sort	(V->adjacent.begin(), V->adjacent.end());

		for (DWORD AF = 0; AF < V->adjacent.size(); AF++)
		{
			_face*	F			= V->adjacent[AF];
			if (F->bSplitted)	continue;	// Face already used in calculation

			// Create new vertex (except adjacency)
			_vertex*	NV		= mu_vertices.create();
			NV->P				= V->P;

			// Calculate it's normal
			NV->N.set	(0,0,0);
			for (DWORD NF = 0; NF < V->adjacent.size(); NF++)
			{
				_face*	Fn		= V->adjacent[NF];

				float	cosa	= F->N.dotproduct(Fn->N);
				if (cosa>sm_cos) {
					NV->N.add		(Fn->N);
					if (!Fn->bSplitted) {
						Fn->VReplace_NoRemove	(V,NV);
						Fn->bSplitted			= TRUE;
					}
				}
			}
			if (NV->adjacent.empty()) mu_vertices.destroy(NV);
			else {
				NV->N.normalize_safe();
				m_vertices.push_back(NV);
			}
		}
	}

	// Destroy unused vertices
	for (I=0; I<Vcount; I++) mu_vertices.destroy	(m_vertices[I]);
	m_vertices.erase(m_vertices.begin(),m_vertices.begin()+Vcount);

	// Clear temporary flag
	for (it = m_faces.begin(); it!=m_faces.end(); it++)
		(*it)->bSplitted = FALSE;

	Msg("model "%20s":  %d vertices duplicated",g_vertices.size()-Vcount);
}
