#include "stdafx.h"
#include "build.h"
#include "OGF_Face.h"
#include "vbm.h"
#include "std_classes.h"
#include "xrOcclusion.h"

void CBuild::Flex2OGF()
{
	float p_total	= 0;
	float p_cost	= 1/float(g_XSplit.size());

	g_tree.clear	();
	g_tree.reserve	(4096);
	for (splitIt it=g_XSplit.begin(); it!=g_XSplit.end(); it++)
	{
		R_ASSERT( ! it->empty() );

		DWORD MODEL_ID = it-g_XSplit.begin();
		set_status	("Converting",MODEL_ID,it->size(),0);

		OGF*		pOGF	= new OGF;
		Face*		F		= *(it->begin());				// first face
		b_material*	M		= &(materials[F->dwMaterial]);	// and it's material

		// Common data
		pOGF->Sector		= M->sector;

		// Collect textures
		OGF_Texture			T;
		pOGF->shader		= &F->Shader();
		if (F->pDeflector)
		{
			// If lightmaps persist
			CDeflector* D = (CDeflector*)F->pDeflector;

			strcpy(T.name, D->lm.name);
			T.pSurface = &(D->lm);
			pOGF->textures.push_back(T);
		}
		for (int i=0; i<M->dwTexCount; i++)
		{
			strcpy(T.name,	textures[M->surfidx[i]].name);
			T.pSurface = &(textures[M->surfidx[i]]);
			pOGF->textures.push_back(T);
		}

		// Collect faces & vertices
		for (vecFaceIt Fit=it->begin(); Fit!=it->end(); Fit++)
		{
			OGF_Vertex	V1,V2,V3;

			Face*	FF = *Fit;

			// Geometry
			V1.P.set(FF->v[0]->P);	V1.N.set(FF->v[0]->N); V1.Color = FF->v[0]->Color;
			V2.P.set(FF->v[1]->P);	V2.N.set(FF->v[1]->N); V2.Color = FF->v[1]->Color;
			V3.P.set(FF->v[2]->P);	V3.N.set(FF->v[2]->N); V3.Color = FF->v[2]->Color;

			// Textures
			if (FF->pDeflector) {
				// Move last stage TC at first stage
				_TCF T = FF->tc.back();
				FF->tc.pop_back();
				FF->tc.insert(0,T);
			}

			// Normal order
			svector<_TCF,8>::iterator TC=FF->tc.begin(); 
			for (;TC!=FF->tc.end(); TC++)
			{
				V1.UV.push_back(TC->uv[0]);
				V2.UV.push_back(TC->uv[1]);
				V3.UV.push_back(TC->uv[2]);
			}

			// build face
			pOGF->_BuildFace(V1,V2,V3);
		}

		pOGF->treeID	= g_tree.size();
		pOGF->Optimize	();
		pOGF->CalcBounds();

		if (g_params.m_bConvertProgressive)		pOGF->MakeProgressive	();
		if (g_params.m_bStripify)				pOGF->Stripify			();

		g_tree.push_back(pOGF);
		Progress(p_total+=p_cost);
	}
}
