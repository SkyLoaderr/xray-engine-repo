#include "stdafx.h"
#include "xr_area.h"
#include "xr_object.h"

using namespace	Collide;

const DWORD	clStatic	= clQUERY_STATIC+clGET_TRIS;

void CObjectSpace::BoxQuery(const Fbox& B, const Fmatrix& M, DWORD flags)
{
	dwQueryID++;
	q_result.Clear	();
	XRC.box_options	(
		(flags&clCOARSE?0:CDB::OPT_FULL_TEST)|
		(flags&clQUERY_ONLYFIRST?CDB::OPT_ONLYFIRST:0)
		);

	if ((flags&clStatic) == clStatic)
	{
		Fvector bc,bd;
		Fbox	xf; 
		xf.xform	(B,M);
		xf.get_CD	(bc,bd);
		
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
		if ( GetNearest(M.c, B.getradius()) )
		{
			for (NL_IT nl_item = q_nearest.begin(); nl_item!=q_nearest.end(); nl_item++)
				(*nl_item)->CFORM()->_BoxQuery	(B,M,flags);
		}
	};
	/*
	if (bDebug) {
		q_debug.AddBox(M,B);

		if (q_result.boxes.size())
		{
			q_debug.boxes.append(q_result.boxes.begin(),q_result.boxes.size());
		}
	}
	*/
}

