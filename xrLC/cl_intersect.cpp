#include "stdafx.h"
#pragma hdrstop

#include "cl_RAPID.h"
#include "cl_intersect.h"

namespace RAPID {
	bool XRCollide::Intersect_BBoxTri(const bbox& bb,Fvector** p)
	{
		return TestBBoxTri(bb,p,bbox_flags&BBOX_CULL);
	}


};
