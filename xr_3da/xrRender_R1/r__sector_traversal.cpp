#include "stdafx.h"

CPortalTraverser	PortalTraverser;

CPortalTraverser::CPortalTraverser	()
{
	i_marker			=	0xffffffff;
}

#ifdef DEBUG
xr_vector<IRender_Sector*>				dbg_sectors;
#endif

void CPortalTraverser::traverse			(IRender_Sector* start, CFrustum& F, Fvector& vBase, Fmatrix& mXFORM, u32 options)
{
	Fmatrix			m_viewport_01	= {
		1.f/2.f,			0.0f,				0.0f,		0.0f,
		0.0f,				-1.f/2.f,			0.0f,		0.0f,
		0.0f,				0.0f,				1.0f,		0.0f,
		1.f/2.f + 0 + 0,	1.f/2.f + 0 + 0,	0.0f,		1.0f
	};

	VERIFY				(start);
	i_marker			++;
	i_options			= options;
	i_vBase				= vBase;
	i_mXFORM			= mXFORM;
	i_mXFORM_01.mul		(m_viewport_01,mXFORM);
	i_start				= (CSector*)start;
	r_sectors.clear		();
	_scissor			scissor;
	scissor.set			(0,0,1,1);
	scissor.depth		= 0;
	i_start->traverse	(F,scissor);

	if (options & VQ_SCISSOR)		{
		// dbg_sectors					= r_sectors;
		// merge scissor info
		for (u32 s=0; s<r_sectors.size(); s++)
		{
			CSector*	S		= (CSector*)r_sectors[s];
			S->r_scissor_merged.invalidate	();
			S->r_scissor_merged.depth		= flt_max;
			for (u32 it=0; it<S->r_scissors.size(); it++)
			{
				S->r_scissor_merged.merge(S->r_scissors[it]);
				if (S->r_scissors[it].depth < S->r_scissor_merged.depth)
					S->r_scissor_merged.depth = S->r_scissors[it].depth;
			}
		}
	}
}

#ifdef DEBUG
void CPortalTraverser::dbg_draw()
{
	RCache.OnFrameEnd		();
	RCache.set_xform_world	(Fidentity);
	RCache.set_xform_view	(Fidentity);
	RCache.set_xform_project(Fidentity);
	for (u32 s=0; s<dbg_sectors.size(); s++)	{
		CSector*	S		= (CSector*)dbg_sectors[s];
		FVF::L		verts	[5];
		Fbox2		bb		= S->r_scissor_merged;
		bb.min.x			= bb.min.x * 2 - 1;
		bb.max.x			= bb.max.x * 2 - 1;
		bb.min.y			= (1-bb.min.y) * 2 - 1;
		bb.max.y			= (1-bb.max.y) * 2 - 1;

		verts[0].set(bb.min.x,bb.min.y,EPS,0xffffffff);
		verts[1].set(bb.max.x,bb.min.y,EPS,0xffffffff);
		verts[2].set(bb.max.x,bb.max.y,EPS,0xffffffff);
		verts[3].set(bb.min.x,bb.max.y,EPS,0xffffffff);
		verts[4].set(bb.min.x,bb.min.y,EPS,0xffffffff);
		RCache.dbg_Draw		(D3DPT_LINESTRIP,verts,4);
	}
}
#endif
