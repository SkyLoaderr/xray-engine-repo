#include "stdafx.h"
#include "build.h"
#include "OGF_Face.h"
#include "vbm.h"
#include "std_classes.h"
#include "lightmap.h"

#define	TRY(a) try { a; } catch (...) { Msg("* E: %s", #a); }

void CBuild::Flex2OGF()
{
	float p_total	= 0;
	float p_cost	= 1/float(g_XSplit.size());

	g_tree.clear	();
	g_tree.reserve	(4096);
	for (splitIt it=g_XSplit.begin(); it!=g_XSplit.end(); it++)
	{
		R_ASSERT( ! (*it)->empty() );
		
		DWORD MODEL_ID		= it-g_XSplit.begin();
		
		OGF*		pOGF	= new OGF;
		Face*		F		= *((*it)->begin());			// first face
		b_material*	M		= &(materials[F->dwMaterial]);	// and it's material
		R_ASSERT	(F && M);
		
		try {
			// Common data
			pOGF->Sector		= M->sector;
			
			// Collect textures
			OGF_Texture			T;
			pOGF->shader		= M->shader;
			pOGF->shader_xrlc	= &F->Shader();
			
			TRY(strcpy(T.name,textures[M->surfidx].name));
			TRY(T.pSurface = &(textures[M->surfidx]));
			TRY(pOGF->textures.push_back(T));
			
			try {
				for (DWORD lmit=0; lmit<F->lmap_layers.size(); lmit++)
				{
					// If lightmaps persist
					CLightmap* LM	= F->lmap_layers[lmit];
					R_ASSERT		(LM);
					strcpy			(T.name, LM->lm.name);
					T.pSurface		= &(LM->lm);
					R_ASSERT		(T.pSurface);
					R_ASSERT		(pOGF);
					pOGF->textures.push_back(T);
				}
			} catch (...) {  Msg("* ERROR: Flex2OGF, model# %d, *textures*",MODEL_ID); }
			
			// Collect faces & vertices
			try {
				for (vecFaceIt Fit=(*it)->begin(); Fit!=(*it)->end(); Fit++)
				{
					OGF_Vertex	V1,V2,V3;
					
					Face*	FF = *Fit;
					R_ASSERT(FF);
					
					// Geometry
					V1.P.set(FF->v[0]->P);	V1.N.set(FF->v[0]->N); V1.Color = FF->v[0]->Color.get();
					V2.P.set(FF->v[1]->P);	V2.N.set(FF->v[1]->N); V2.Color = FF->v[1]->Color.get();
					V3.P.set(FF->v[2]->P);	V3.N.set(FF->v[2]->N); V3.Color = FF->v[2]->Color.get();
					
					// Normal order
					svector<_TCF,8>::iterator TC=FF->tc.begin(); 
					for (;TC!=FF->tc.end(); TC++)
					{
						V1.UV.push_back(TC->uv[0]);
						V2.UV.push_back(TC->uv[1]);
						V3.UV.push_back(TC->uv[2]);
					}
					
					// build face
					TRY				(pOGF->_BuildFace(V1,V2,V3));
					V1.UV.clear();	V2.UV.clear();	V3.UV.clear();
				}
			} catch (...) {  Msg("* ERROR: Flex2OGF, model# %d, *faces*",MODEL_ID); }
		} catch (...)
		{
			Msg("* ERROR: Flex2OGF, 1st part, model# %d",MODEL_ID);
		}
		
		try {
			pOGF->treeID	= g_tree.size();
			pOGF->Optimize	();
			pOGF->CalcBounds();
			
			if (g_params.m_bConvertProgressive)		pOGF->MakeProgressive	();
			if (g_params.m_bStripify)				pOGF->Stripify			();
		} catch (...)
		{
			Msg("* ERROR: Flex2OGF, 2nd part, model# %d",MODEL_ID);
		}
		
		g_tree.push_back	(pOGF);
		Progress			(p_total+=p_cost);
	}
}
