#include "stdafx.h"
#include "xrMU_Model.h"

#define	TRY(a) try { a; } catch (...) { clMsg("* E: %s", #a); }

void xrMU_Model::calc_ogf()
{
	// Build OGFs
	for (xrMU_Model::v_subdivs_it it=m_subdivs.begin(); it!=m_subdivs.end(); it++)
	{
		OGF*		pOGF	= xr_new<OGF> ();
		b_material*	M		= &(pBuild->materials[it->material]);	// and it's material
		R_ASSERT	(M);

		try {
			// Common data
			pOGF->Sector		= 0;
			pOGF->material		= it->material;

			// Collect textures
			OGF_Texture			T;
			TRY					(strcpy(T.name,pBuild->textures[M->surfidx].name));
			TRY					(T.pSurface = &(pBuild->textures[M->surfidx]));
			TRY					(pOGF->textures.push_back(T));

			// Collect faces & vertices
			try {
				xrMU_Model::v_faces_it	_beg	= m_faces.begin() + it->start;
				xrMU_Model::v_faces_it	_end	= _beg + it->count;
				for (xrMU_Model::v_faces_it Fit =_beg; Fit!=_end; Fit++)
				{
					OGF_Vertex			V0,V1,V2;

					xrMU_Model::_face*	FF		= *Fit;
					R_ASSERT			(FF);

					// Vertices
					xrMU_Model::_vertex*	_V0		= FF->v[0];	u32 id0	= (u32)(std::find(m_vertices.begin(),m_vertices.end(),_V0)-m_vertices.begin());
					xrMU_Model::_vertex*	_V1		= FF->v[1];	u32 id1	= (u32)(std::find(m_vertices.begin(),m_vertices.end(),_V1)-m_vertices.begin());
					xrMU_Model::_vertex*	_V2		= FF->v[2];	u32 id2	= (u32)(std::find(m_vertices.begin(),m_vertices.end(),_V2)-m_vertices.begin());

					// Geometry - POS
					V0.P			= _V0->P;
					V1.P			= _V1->P;
					V2.P			= _V2->P;	

					// Geometry - NORMAL (ignore ???)
					V0.N			= _V0->N; 
					V1.N			= _V1->N;
					V2.N			= _V2->N;

					// Geometry - COLOR
					V0.Color		= color[id0].get();
					V1.Color		= color[id1].get();
					V2.Color		= color[id2].get();

					// Geometry - UV
					V0.UV.push_back	(FF->tc[0]);
					V1.UV.push_back	(FF->tc[1]);
					V2.UV.push_back	(FF->tc[2]);

					// build face
					TRY				(pOGF->_BuildFace(V0,V1,V2));
					V0.UV.clear();	V1.UV.clear();	V2.UV.clear();
				}
			} catch (...) {  clMsg("* ERROR: MU2OGF, model %s, *faces*",m_name); }
		} catch (...)
		{
			clMsg("* ERROR: MU2OGF, 1st part, model %s",m_name);
		}

		try {
			pOGF->Optimize		();
			pOGF->CalcBounds	();
			pOGF->Stripify		();
		} catch (...)
		{
			clMsg	("* ERROR: MU2OGF, 2nd part, model %s",m_name);
		}

		it->ogf		=	pOGF;
	}
}
