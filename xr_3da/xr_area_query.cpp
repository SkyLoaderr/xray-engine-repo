#include "stdafx.h"
#include "xr_area.h"

using namespace	Collide;

const DWORD	clStatic	= clQUERY_STATIC+clGET_TRIS;

void CObjectSpace::BoxQuery(const Fbox& B, const Fmatrix& M, DWORD flags)
{
	dwQueryID++;
	q_result.Clear	();
	XRC.BBoxMode	(
		(flags&clCOARSE?0:BBOX_TRITEST)|
		(flags&clQUERY_ONLYFIRST?BBOX_ONLYFIRST:0)
		);

	if ((flags&clStatic) == clStatic)
	{
		XRC.BBoxCollide	(precalc_identity, &Static, M, B );
		if (XRC.GetBBoxContactCount())
		{
			if (flags&clQUERY_ONLYFIRST) 
			{
				q_result.AddTri(&Static.tris[XRC.BBoxContact[0].id]);
				return;
			} else {
				for (DWORD i=0; i<XRC.GetBBoxContactCount(); i++)
					q_result.AddTri(&Static.tris[XRC.BBoxContact[i].id]);
			}
		}
	};
	if (flags&clQUERY_DYNAMIC)
	{
		if ( GetNearest(M.c, B.getradius()) )
		{
			for (NL_IT nl_item = nearest_list.begin(); nl_item!=nearest_list.end(); nl_item++)
				(*nl_item)->_BoxQuery	(B,M,flags);
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

