#include "stdafx.h"

#define	TRY(a) try { a; } catch (...) { clMsg("* E: %s", #a); }

void xrMU_Reference::export_ogf()
{
	vector<u32>		generated_ids;

	// Export nodes
	{
		for (xrMU_Model::v_subdivs_it it=model->m_subdivs.begin(); it!=model->m_subdivs.end(); it++)
		{
			OGF_Reference*	pOGF	= xr_new<OGF_Reference> ();
			b_material*	M			= &(pBuild->materials[it->material]);	// and it's material
			R_ASSERT	(M);

			// Common data
			pOGF->Sector		= sector;
			pOGF->material		= it->material;

			// Collect textures
			OGF_Texture			T;
			TRY(strcpy(T.name,pBuild->textures[M->surfidx].name));
			TRY(T.pSurface = &(pBuild->textures[M->surfidx]));
			TRY(pOGF->textures.push_back(T));

			// Special
			pOGF->model			= it->ogf;
			pOGF->vb_id			= it->vb_id;
			pOGF->vb_start		= it->vb_start;
			pOGF->ib_id			= it->ib_id;
			pOGF->ib_start		= it->ib_start;
			pOGF->xform.set		(xform);
			pOGF->c_scale		= c_scale;
			pOGF->c_bias		= c_bias;

			pOGF->CalcBounds		();
			generated_ids.push_back	(g_tree.size());
			g_tree.push_back		(pOGF);
		}
	}

	// Now, let's fuck with LODs
	if (u16(-1) == model->m_lod_ID)	return;
	{
		// Create Node and fill it with information
		b_lod&		LOD		= pBuild->lods	[model->m_lod_ID];
		OGF_LOD*	pNode	= xr_new<OGF_LOD> (1,sector);
		pNode->lod_Material	= LOD.dwMaterial;
		for (int lf=0; lf<8; lf++)
		{
			b_lod_face&		F = LOD.faces[lf];
			OGF_LOD::_face& D = pNode->lod_faces[lf];
			for (int lv=0; lv<4; lv++)
			{
				xform.transform_tiny(D.v[lv].v,F.v[lv]);
				D.v[lv].t			= F.t[lv];
				D.v[lv].c			= 0xffffffff;
			}
		}

		// Add all 'OGFs' with such LOD-id
		for (u32 o=0; o<generated_ids.size(); o++)
			pNode->AddChield(generated_ids[o]);

		// Register node
		R_ASSERT						(pNode->chields.size());
		pNode->CalcBounds				();
		g_tree.push_back				(pNode);

		// Calculate colors
		const float sm_range		= 5.f;
		for (int lf=0; lf<8; lf++)
		{
			OGF_LOD::_face& F = pNode->lod_faces[lf];
			for (int lv=0; lv<4; lv++)
			{
				Fvector	ptPos	= F.v[lv].v;
				DWORD&	ptColor	= F.v[lv].c;

				Fcolor	_C;		_C.set(0,0,0,0);
				float 	_N		= 0;

				for (u32 v_it=0; v_it<model->m_vertices.size(); v_it++)
				{
					// get base
					Fvector baseP;	xform.transform_tiny	(baseP,model->m_vertices[v_it]->P);
					Fcolor	baseC	= color[v_it];

					Fcolor			vC;
					float			oD	= ptPos.distance_to	(baseP);
					float			oA  = 1/(1+100*oD*oD);
					vC.set			(baseC); 
					vC.mul_rgb		(oA);
					_C.r			+= vC.r;
					_C.g			+= vC.g;
					_C.b			+= vC.b;
					_N				+= oA;
				}

				_C.mul_rgb		(1/(_N+EPS));
				_C.a			= 1.f;
				ptColor			= _C.get();
			}
		}
	}
}
