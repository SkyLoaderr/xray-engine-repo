#include "stdafx.h"
#include "build.h"

extern BOOL	hasImplicitLighting		(Face* F);
extern void Detach					(vecFace* S);

/*
WORD					dwMaterial;		// index of material
svector<_TCF,8>			tc;				// TC
svector<CLightmap*,8>	lmap_layers;
*/

IC BOOL	FaceEqual(Face* F1, Face* F2)
{
	if (F1->dwMaterial  != F2->dwMaterial)					return FALSE;
	if (F1->tc.size()	!= F2->tc.size())					return FALSE;
	if (F1->lmap_layers.size() != F2->lmap_layers.size())	return FALSE;
	for (DWORD it=0; it<F1->lmap_layers.size(); it++)		
		if (F1->lmap_layers[it] != F2->lmap_layers[it])		return FALSE;
		return TRUE;
}

BOOL	NeedMerge		(vecFace& subdiv, Fbox& bb_base)
{
	// 1. Amount of polygons
	if (subdiv.size()>=u32(3*g_params.m_SS_High/4))	return FALSE;
	
	// 2. Bounding box
	bb_base.invalidate	();
	for (DWORD it=0; it<subdiv.size(); it++)
	{
		Face* F = subdiv[it];
		bb_base.modify(F->v[0]->P);
		bb_base.modify(F->v[1]->P);
		bb_base.modify(F->v[2]->P);
	}
	Fvector sz_base;	bb_base.getsize(sz_base);
	if (sz_base.x<(3*g_params.m_SS_maxsize/4))		return TRUE;
	if (sz_base.y<(3*g_params.m_SS_maxsize/4))		return TRUE;
	if (sz_base.z<(3*g_params.m_SS_maxsize/4))		return TRUE;
	return FALSE;
}

BOOL	ValidateMerge	(DWORD f1, Fbox& bb_base, DWORD f2, Fbox& bb, float& volume)
{
	// Polygons
	if ((f1+f2) > u32(4*g_params.m_SS_High/3))		return FALSE;	// Don't exceed limits (POLY)	

	// Size
	Fbox	merge;	merge.merge		(bb_base,bb);
	Fvector sz;		merge.getsize	(sz);
	if (sz.x>(4*g_params.m_SS_maxsize/3))			return FALSE;	// Don't exceed limits (GEOM)
	if (sz.y>(4*g_params.m_SS_maxsize/3))			return FALSE;
	if (sz.z>(4*g_params.m_SS_maxsize/3))			return FALSE;

	// Volume
	float	v1	= bb_base.getvolume	();
	float	v2	= bb.getvolume		();
	volume		= merge.getvolume	();
	if (volume > 2*2*2*(v1+v2))						return FALSE;	// Don't merge too distant groups

	// OK
	return TRUE;
}

void CBuild::xrPhase_MergeGeometry	()
{
	Status("Processing...");
	for (DWORD split=0; split<g_XSplit.size(); split++)
	{
		vecFace&	subdiv	= g_XSplit[split];
		Fbox		bb_base;
		while (NeedMerge(subdiv,bb_base))	
		{
			// **OK**. Let's find the best candidate for merge
			DWORD	selected		= split;
			float	selected_volume	= flt_max;
			for (DWORD test=split+1; test<g_XSplit.size(); test++)
			{
				Fbox		bb;
				float		volume;
				vecFace&	TEST	= g_XSplit[test];

				if (!FaceEqual(subdiv.front(),TEST.front()))						continue;
				if (!NeedMerge(TEST,bb))											continue;
				if (!ValidateMerge(subdiv.size(),bb_base,TEST.size(),bb,volume))	continue;

				if (volume<selected_volume)	{
					selected		= test;
					selected_volume	= volume;
				}
			}
			if (selected == split)	break;	// No candidates for merge

			// **OK**. Perform merge
			subdiv.insert	(subdiv.end(), g_XSplit[selected].begin(), g_XSplit[selected].end());
			g_XSplit.erase	(g_XSplit.begin()+selected);
		}
		Progress(float(split)/float(g_XSplit.size()));
	}
	Msg("%d subdivisions.",g_XSplit.size());
}
