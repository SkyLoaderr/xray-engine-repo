#include "stdafx.h"

//-----------------------------------------------------------------------
void xrMU_Model::calc_lighting	(vector<Fcolor>& dest, Fmatrix& xform, CDB::MODEL* M, LPCSTR L_layer)
{
	// trans-map
	typedef	multimap<float,v_vertices>	mapVert;
	typedef	mapVert::iterator			mapVertIt;
	mapVert								g_trans;

	// trans-epsilons
	const float eps			= EPS_L;
	const float eps2		= 2.f*eps;

	// Perform lighting
	CDB::COLLIDER			DB;
	DB.ray_options			(0);
	vector<R_Light>	Lights = pBuild->L_layers.front().lights;
	if (Lights.empty())		return;

	for (DWORD I = 0; I<m_vertices.size(); I++)
	{
		_vertex*	V	= m_vertices[I];

		// Get ambient factor
		float		v_amb	= 0.f;
		float		v_trans	= 0.f;
		for (u32 f=0; f<V->adjacent.size(); f++)
		{
			_face*	F		=	V->adjacent[f];
			v_amb			+=	F->Shader().vert_ambient;
			v_trans			+=	F->Shader().vert_translucency;
		}
		v_amb				/= float(V->adjacent.size());
		float v_inv			= 1.f-v_amb;

		Fcolor				vC;
		Fvector				vP,vN;
		vC.set				(0,0,0,0);
		xform.transform_tiny(vP,V->P);
		xform.transform_dir	(vN,V->N);
		vN.normalize        ();
		LightPoint			(&DB, vC, vP, vN, Lights.begin(), Lights.end(), 0);

		V->C.r				= vC.r*v_inv+v_amb;
		V->C.g				= vC.g*v_inv+v_amb;
		V->C.b				= vC.b*v_inv+v_amb;
		V->C.a				= v_trans;

		// Search
		const float key		= V->P.x;
		mapVertIt	it		= g_trans.lower_bound	(key);
		mapVertIt	it2		= it;

		// Decrement to the start and inc to end
		while (it!=g_trans.begin() && ((it->first+eps2)>key)) it--;
		while (it2!=g_trans.end() && ((it2->first-eps2)<key)) it2++;
		if (it2!=g_trans.end())	it2++;

		// Search
		for (; it!=it2; it++)
		{
			v_vertices&	VL		= it->second;
			_vertex* Front		= VL.front();
			R_ASSERT			(Front);
			if (Front->P.similar(V->P,eps))
			{
				VL.push_back		(V);
			}
		}

		// Register
		mapVertIt	ins			= g_trans.insert(make_pair(key,v_vertices()));
		ins->second.reserve		(32);
		ins->second.push_back	(V);
	}

	// Process all groups
	for (mapVertIt it=g_trans.begin(); it!=g_trans.end(); it++)
	{
		// Unique
		v_vertices&	VL		= it->second;
		std::sort			(VL.begin(),VL.end());
		VL.erase			(unique(VL.begin(),VL.end()),VL.end());

		// Calc summary color
		Fcolor		C;
		C.set		(0,0,0,0);
		for (int v=0; v<int(VL.size()); v++)
		{
			C.r = _max	(C.r,VL[v]->C.r);
			C.g = _max	(C.g,VL[v]->C.g);
			C.b = _max	(C.b,VL[v]->C.b);
		}

		// Calculate final vertex color
		for (v=0; v<int(VL.size()); v++)
		{
			// trans-level
			float	level		= VL[v]->C.a;

			// 
			Fcolor				R;
			R.lerp				(VL[v]->C,C,level);
			R.r					= _max(R.r,VL[v]->C.r);
			R.g					= _max(R.g,VL[v]->C.g);
			R.b					= _max(R.b,VL[v]->C.b);
			VL[v]->C.lerp		(R,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
			VL[v]->C.mul_rgb	(.5f);
			VL[v]->C.a			= 1.f;
		}
	}

	// Transfer colors to destination
	dest.resize				(m_vertices.size());
	for (I = 0; I<m_vertices.size(); I++)
	{
		dest[I]			= m_vertices[I]->C;
	}
}


void xrMU_Reference::calc_lighting()
{
	model->calc_lighting(color,xform,RCAST_Model,0);
}
