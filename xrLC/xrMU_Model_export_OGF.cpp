#include "stdafx.h"

#define	TRY(a) try { a; } catch (...) { clMsg("* E: %s", #a); }

void xrMU_Reference::export_ogf()
{
	vector<OGF*>	generated;
	vector<u32>		generated_ids;

	// Export nodes
	{
		for (xrMU_Model::v_subdivs_it it=model->m_subdivs.begin(); it!=model->m_subdivs.end(); it++)
		{
			OGF*		pOGF	= xr_new<OGF> ();
			b_material*	M		= &(pBuild->materials[it->material]);	// and it's material
			R_ASSERT	(M);

			try {
				// Common data
				pOGF->Sector		= sector;
				pOGF->material		= it->material;

				// Collect textures
				OGF_Texture			T;
				TRY(strcpy(T.name,pBuild->textures[M->surfidx].name));
				TRY(T.pSurface = &(pBuild->textures[M->surfidx]));
				TRY(pOGF->textures.push_back(T));

				// Collect faces & vertices
				try {
					xrMU_Model::v_faces_it	_beg	= model->m_faces.begin() + it->start;
					xrMU_Model::v_faces_it	_end	= _beg + it->count;
					for (xrMU_Model::v_faces_it Fit =_beg; Fit!=_end; Fit++)
					{
						OGF_Vertex	V0,V1,V2;

						xrMU_Model::_face*	FF		= *Fit;
						R_ASSERT			(FF);

						// Vertices
						xrMU_Model::_vertex*	_V0		= FF->v[0];	u32 id0	= find(model->m_vertices.begin(),model->m_vertices.end(),_V0)-model->m_vertices.begin();
						xrMU_Model::_vertex*	_V1		= FF->v[1];	u32 id1	= find(model->m_vertices.begin(),model->m_vertices.end(),_V1)-model->m_vertices.begin();
						xrMU_Model::_vertex*	_V2		= FF->v[2];	u32 id2	= find(model->m_vertices.begin(),model->m_vertices.end(),_V2)-model->m_vertices.begin();

						// Geometry - POS
						xform.transform_tiny(V0.P, _V0->P);
						xform.transform_tiny(V1.P, _V1->P);	
						xform.transform_tiny(V2.P, _V2->P);	

						// Geometry - NORMAL
						xform.transform_dir(V0.N, _V0->N); V0.N.normalize();
						xform.transform_dir(V1.N, _V1->N); V1.N.normalize();
						xform.transform_dir(V2.N, _V2->N); V2.N.normalize();

						// Geometry - COLOR
						V0.Color			= color[id0].get();
						V1.Color			= color[id1].get();
						V2.Color			= color[id2].get();

						// Geometry - UV
						V0.UV.push_back		(FF->tc[0]);
						V1.UV.push_back		(FF->tc[1]);
						V2.UV.push_back		(FF->tc[2]);

						// build face
						TRY					(pOGF->_BuildFace(V0,V1,V2));
						V0.UV.clear();	V1.UV.clear();	V2.UV.clear();
					}
				} catch (...) {  clMsg("* ERROR: Flex2OGF, model# %d, *faces*",g_tree.size()); }
			} catch (...)
			{
				clMsg("* ERROR: Flex2OGF, 1st part, model# %d",g_tree.size());
			}

			try {
				pOGF->treeID		= g_tree.size();
				pOGF->Optimize		();
				pOGF->CalcBounds	();

				if (g_params.m_bConvertProgressive)		pOGF->MakeProgressive	();
				if (g_params.m_bStripify)				pOGF->Stripify			();
			} catch (...)
			{
				clMsg("* ERROR: Flex2OGF, 2nd part, model# %d",g_tree.size());
			}

			generated_ids.push_back	(g_tree.size());
			generated.push_back		(pOGF);
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

				for (u32 c_it=0; c_it<pNode->chields.size(); c_it++)
				{
					OGF*		ogf		= (OGF*)g_tree[pNode->chields[c_it]];
					vecOGF_V&	verts	= ogf->vertices;
					for (u32 v_it=0; v_it<verts.size(); v_it++)
					{
						Fcolor			vC; 
						OGF_Vertex&		oV	= verts[v_it];
						float			oD	= ptPos.distance_to(oV.P);
						float			oA  = 1/(1+100*oD*oD);
						vC.set			(oV.Color); 
						vC.mul_rgb		(oA);
						_C.r			+= vC.r;
						_C.g			+= vC.g;
						_C.b			+= vC.b;
						_N				+= oA;
					}
				}

				_C.mul_rgb		(1/(_N+EPS));
				_C.a			= 1.f;
				ptColor			= _C.get();
			}
		}
	}
}
