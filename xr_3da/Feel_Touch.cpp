#include "stdafx.h"
#include "feel_touch.h"
#include "xr_creator.h"
#include "xr_object.h"
using namespace Feel;

void Touch::feel_touch_deny		(CObject* O, DWORD T)
{
	DenyTouch						D;
	D.O								= O;
	D.Expire						= Device.dwTimeGlobal + T;
	feel_touch_disable.push_back	(D);
}

void Touch::feel_touch_update	(Fvector& C, float R)
{
	// Check if denied objects expire in time
	DWORD	dwT			= Device.dwTimeGlobal;
	for (u32 dit=0; dit<feel_touch_disable.size(); dit++)
	{
		if (feel_touch_disable[dit].Expire>dwT)
		{
			feel_touch_disable.erase	(feel_touch_disable.begin()+dit);
			dit--;
		}
	}

	// Find nearest objects
	pCreator->ObjectSpace.GetNearest	(C,R);
	CObjectSpace::NL_IT		n_begin		= pCreator->ObjectSpace.q_nearest.begin	();
	CObjectSpace::NL_IT		n_end		= pCreator->ObjectSpace.q_nearest.end	();
	if (n_end==n_begin)		return;

	// Process results (NEW)
	for (CObjectSpace::NL_IT it = n_begin; it!=n_end; it++)
	{
		CObject* O = *it;
		if (O->getDestroy())	continue;							// Don't touch candidates for destroy
		if (find(feel_touch.begin(),feel_touch.end(),O) == feel_touch.end())
		{
			// check for deny
			BOOL bDeny = FALSE;
			for (dit=0; dit<feel_touch_disable.size(); dit++)
				if (O == feel_touch_disable[dit].O)	{ bDeny=TRUE; break; }

			// new 
			if (!bDeny)
			{
				feel_touch_new			(O);
				feel_touch.push_back	(O);
			}
		}
	}

	// Process results (DELETE)
	for (int d = 0; d<int(feel_touch.size()); d++)
	{
		CObject* O	= feel_touch[d];
		if (O->getDestroy() || (find(n_begin,n_end,O) == n_end))	// Don't touch candidates for destroy
		{
			// delete
			feel_touch_delete		(O);
			feel_touch.erase		(feel_touch.begin()+d);
			d--;
		}
	}
	Engine.Sheduler.Slice	();
}
