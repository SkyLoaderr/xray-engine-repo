#pragma once

#include "xr_collide_defs.h"
#include "portal.h"

class ENGINE_API CObject;

namespace Feel
{
	const float fuzzy_update_vis	= 1000.f;		// speed of fuzzy-logic desisions
	const float fuzzy_update_novis	= 1000.f;		// speed of fuzzy-logic desisions
	const float fuzzy_guaranteed	= 1.f;			// distance which is supposed 100% visible
	const float lr_granularity		= 0.1f;			// assume similar positions

	class ENGINE_API Vision
	{
	private:
		objSET						query;

		void						o_new	(CObject* E);
		void						o_delete(CObject* E);
		void						o_trace	(Fvector& P, float dt);
	public:
		struct feel_visible_Item 
		{
			float				fuzzy;		// note range: (-1[no]..1[yes])
			CObject*			O;
			Collide::ray_cache	Cache;
			Fvector				cp_LP;
			Fvector				cp_LR_src;
			Fvector				cp_LR_dst;
		};
		vector<feel_visible_Item>	feel_visible;
	public:
		void						feel_vision_update	(objSET& seen, CObject* parent, Fvector& P, float dt);
		void						feel_vision_get		(objSET& R)
		{
			R.clear		();
			vector<feel_visible_Item>::iterator I=feel_visible.begin(),E=feel_visible.end();
			for (; I!=E; I++)	if (positive(I->fuzzy)) R.push_back(I->O);
		}
	};
};
