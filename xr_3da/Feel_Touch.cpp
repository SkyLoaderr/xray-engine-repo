#include "stdafx.h"
#include "feel_touch.h"
using namespace Feel;

void CTouch::feel_touch_process	(Fvector& C, float R)
{
	// Find nearest objects
	Level().ObjectSpace.GetNearest						(C,R);
	CObjectSpace::NL_IT		n_begin						= Level().ObjectSpace.q_nearest.begin	();
	CObjectSpace::NL_IT		n_end						= Level().ObjectSpace.q_nearest.end		();
	if (n_end==n_begin)		return;

	// Process results (NEW)
	for (CObjectSpace::NL_IT it = n_begin; it!=n_end; it++)
	{
		CObject* O = *it;
		if (find(Nearest.begin(),Nearest.end(),O) == Nearest.end())
		{
			// new 
			g_sv_Feel_near_new		(O);
			Nearest.push_back		(O);
		}
	}

	// Process results (DELETE)
	for (int d = 0; d<int(Nearest.size()); d++)
	{
		CObject* O	= Nearest[d];
		if (find(n_begin,n_end,O) == n_end)
		{
			// delete
			g_sv_Feel_near_delete	(O);
			Nearest.erase			(Nearest.begin()+d);
			d--;
		}
	}
	Engine.Sheduler.Slice	();
}
