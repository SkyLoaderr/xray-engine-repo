#include "stdafx.h"
#include "feel_touch.h"
#include "xr_creator.h"
using namespace Feel;

void Touch::feel_touch_update	(Fvector& C, float R)
{
	// Find nearest objects
	pCreator->ObjectSpace.GetNearest					(C,R);
	CObjectSpace::NL_IT		n_begin						= pCreator->ObjectSpace.q_nearest.begin	();
	CObjectSpace::NL_IT		n_end						= pCreator->ObjectSpace.q_nearest.end	();
	if (n_end==n_begin)		return;

	// Process results (NEW)
	for (CObjectSpace::NL_IT it = n_begin; it!=n_end; it++)
	{
		CObject* O = *it;
		if (find(feel_touch.begin(),feel_touch.end(),O) == feel_touch.end())
		{
			// new 
			feel_touch_new			(O);
			feel_touch.push_back	(O);
		}
	}

	// Process results (DELETE)
	for (int d = 0; d<int(feel_touch.size()); d++)
	{
		CObject* O	= feel_touch[d];
		if (find(n_begin,n_end,O) == n_end)
		{
			// delete
			feel_touch_delete		(O);
			feel_touch.erase		(feel_touch.begin()+d);
			d--;
		}
	}
	Engine.Sheduler.Slice	();
}
