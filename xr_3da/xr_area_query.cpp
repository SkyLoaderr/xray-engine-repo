#include "stdafx.h"
#include "xr_area.h"
#include "xr_object.h"

using namespace	Collide;

const u32	clStatic	= clQUERY_STATIC+clGET_TRIS;

void CObjectSpace::BoxQuery(const Fbox& B, const Fmatrix& M, u32 flags)
{
	Fvector		bc,bd;
	Fbox		xf; 
	xf.xform	(B,M);
	xf.get_CD	(bc,bd);

	q_result.Clear	();
	XRC.box_options	(
		(flags&clCOARSE?0:CDB::OPT_FULL_TEST)|
		(flags&clQUERY_ONLYFIRST?CDB::OPT_ONLYFIRST:0)
		);

	if ((flags&clStatic) == clStatic)
	{
		XRC.box_query	(&Static, bc, bd);
		if (XRC.r_count())
		{
			CDB::RESULT* it	=XRC.r_begin();
			CDB::RESULT* end=XRC.r_end	();
			for (; it!=end; it++)
				q_result.AddTri(&Static.get_tris() [it->id]);
		}
	};

	if (flags&clQUERY_DYNAMIC)
	{
		// Traverse object database
		g_SpatialSpace.q_box	(0,STYPE_COLLIDEABLE,bc,bd);

		// Determine visibility for dynamic part of scene
		for (u32 o_it=0; o_it<g_SpatialSpace.q_result.size(); o_it++)
		{
			ISpatial*	spatial						= g_SpatialSpace.q_result[o_it];
			CObject*	collidable					= dynamic_cast<CObject*>	(spatial);
			if			(0==collidable)				continue;
			collidable->collidable.model->_BoxQuery	(B,M,flags);
		}
	};
}
