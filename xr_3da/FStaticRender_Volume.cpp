#include "stdafx.h"
#include "fstaticrender.h"
#include "fhierrarhyvisual.h"
#include "bodyinstance.h"
#include "fmesh.h"

void CRender::RenderBox			(CSector* S, Fbox& BB, int sh)
{
	lstVisuals.clear		();
	lstVisuals.push_back	(S->Root());
	
	for (u32 test=0; test<lstVisuals.size(); test++)
	{
		CVisual*	V		= 	lstVisuals[test];
		
		// Visual is 100% visible - simply add it
		vector<CVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals
		
		switch (V->Type) {
		case MT_HIERRARHY:
			{
				// Add all children
				FHierrarhyVisual* pV = (FHierrarhyVisual*)V;
				I = pV->children.begin	();
				E = pV->children.end		();
				for (; I!=E; I++)		{
					CVisual* T			= *I;
					if (BB.intersect(T->bv_BBox))	lstVisuals.push_back(T);
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
					CVisual* T			= *I;
					if (BB.intersect(T->bv_BBox))	lstVisuals.push_back(T);
				}
			}
			break;
		default:
			{
				// Renderable visual
				ShaderElement* E	= V->hShader->E[sh];
				for (u32 pass=0; pass<E->Passes.size(); pass++)
				{
					Device.Shader.set_Element	(E,pass);
					V->Render					(-1.f);
				}
			}
			break;
		}
	}
}

