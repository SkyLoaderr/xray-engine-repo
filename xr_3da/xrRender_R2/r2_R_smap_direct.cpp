#include "stdafx.h"

/*
void	CRender::render_smap_direct		()
{
	// Dumb!!! Very brute-force rendering!!!
	for (u32 s=0; s<Sectors.size(); s++)
	{
		CSector*	S			= (CSector*)Sectors[s];
		lstVisuals.clear		();
		lstVisuals.push_back	(S->Root());

		for (u32 test=0; test<lstVisuals.size(); test++)
		{
			IVisual*	V		= 	lstVisuals[test];

			// Visual is 100% visible - simply add it
			vector<IVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

			switch (V->Type) {
		case MT_HIERRARHY:
			{
				// Add all children
				FHierrarhyVisual* pV = (FHierrarhyVisual*)V;
				I = pV->children.begin	();
				E = pV->children.end		();
				for (; I!=E; I++)		{
					IVisual* T			= *I;
					if (BB.intersect(T->vis.box))	lstVisuals.push_back(T);
				}
			}
			break;
		case MT_SKELETON:
			{
				// Add all children
				CKinematics * pV = (CKinematics*)V;
				pV->Calculate			();
				I = pV->children.begin	();
				E = pV->children.end		();
				for (; I!=E; I++)		{
					IVisual* T			= *I;
					if (BB.intersect(T->vis.box))	lstVisuals.push_back(T);
				}
			}
			break;
		default:
			{
				// Renderable visual
				ShaderElement* E	= V->hShader->E[sh];
				for (u32 pass=0; pass<E->Passes.size(); pass++)
				{
					RCache.set_Element			(E,pass);
					V->Render					(-1.f);
				}
			}
			break;
			}
		}
	}
}
*/
