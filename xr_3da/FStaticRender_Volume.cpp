#include "stdafx.h"
#include "fstaticrender.h"
#include "fhierrarhyvisual.h"
#include "bodyinstance.h"

void CRender::RenderBox			(CSector* S, Fbox& BB, int sh)
{
	CVisual*	V	= 	S->Root	();
	lstVisuals.clear		();
	lstVisuals.push_back	(V);
	
	for (int test=0; test<lstVisuals.size(); test++)
	{
		if (BB.intersect(V->bv_BBox))
		{
			// Visual is 100% visible - simply add it
			vector<CVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals
			
			switch (V->Type) {
			case MT_HIERRARHY:
				{
					// Add all chields, doesn't perform any tests
					FHierrarhyVisual* pV = (FHierrarhyVisual*)V;
					I = pV->chields.begin	();
					E = pV->chields.end		();
					for (; I!=E; I++)		lstVisuals.push_back	(*I);
				}
				break;
			case MT_SKELETON:
				{
					// Add all chields, doesn't perform any tests
					CKinematics * pV = (CKinematics*)V;
					pV->Calculate			();
					I = pV->chields.begin	();
					E = pV->chields.end		();
					for (; I!=E; I++)		lstVisuals.push_back	(*I);
				}
				break;
			default:
				{
					// Renderable visual

				}
				break;
			}
		}
	}
}

