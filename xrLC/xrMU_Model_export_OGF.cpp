#include "stdafx.h"

void xrMU_Reference::export_ogf()
{
	for (xrMU_Model::v_subdivs_it it=model->m_subdivs.begin(); it!=model->m_subdivs.end(); it++)
	{
		OGF*		pOGF	= xr_new<OGF> ();
		b_material*	M		= &(materials[it->material]);	// and it's material
		R_ASSERT	(M);

		try {
			// Common data
			pOGF->Sector		= sector;
			pOGF->material		= it->material;

			// Collect textures
			OGF_Texture			T;
			TRY(strcpy(T.name,textures[M->surfidx].name));
			TRY(T.pSurface = &(textures[M->surfidx]));
			TRY(pOGF->textures.push_back(T));

			// Collect faces & vertices
			try {
				xrMU_Model::v_faces_it	_beg	= model->m_faces.begin() + it->start;
				xrMU_Model::v_faces_it	_end	= _beg + it->count;
				for (xrMU_Model::v_faces_it Fit=_beg; Fit!=_end; Fit++)
				{
					OGF_Vertex	V1,V2,V3;

					xrMU_Model::_face*	FF		= *Fit;
					R_ASSERT			(FF);

					// Geometry - POS
					xform.transform_tiny(V1.P, FF->v[0]->P);
					xform.transform_tiny(V2.P, FF->v[1]->P);	
					xform.transform_tiny(V3.P, FF->v[2]->P);	

					// Geometry - NORMAL
					xform.transform_dir(V1.N, FF->v[0]->N); V1.N.normalize();
					xform.transform_dir(V2.N, FF->v[1]->N); V2.N.normalize();
					xform.transform_dir(V3.N, FF->v[2]->N); V3.N.normalize();

					// Geometry - COLOR
					V1.Color			= 0xffffffff;//FF->v[0]->Color.get();
					V2.Color			= 0xffffffff;//FF->v[1]->Color.get();
					V3.Color			= 0xffffffff;//FF->v[2]->Color.get();

					// Geometry - UV
					V1.UV.push_back		(FF->tc[0]);
					V2.UV.push_back		(FF->tc[1]);
					V3.UV.push_back		(FF->tc[2]);

					// build face
					TRY					(pOGF->_BuildFace(V1,V2,V3));
					V1.UV.clear();	V2.UV.clear();	V3.UV.clear();
				}
			} catch (...) {  clMsg("* ERROR: Flex2OGF, model# %d, *faces*",MODEL_ID); }
		} catch (...)
		{
			clMsg("* ERROR: Flex2OGF, 1st part, model# %d",MODEL_ID);
		}

		try {
			pOGF->treeID	= g_tree.size();
			pOGF->Optimize	();
			pOGF->CalcBounds();

			if (g_params.m_bConvertProgressive)		pOGF->MakeProgressive	();
			if (g_params.m_bStripify)				pOGF->Stripify			();
		} catch (...)
		{
			clMsg("* ERROR: Flex2OGF, 2nd part, model# %d",MODEL_ID);
		}

		g_tree.push_back	(pOGF);
	}
}
