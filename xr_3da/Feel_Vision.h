#pragma once

class ENGINE_API CObject;

namespace Feel
{
	const float fuzzy_update_vis	= 1000.f;		// speed of fuzzy-logic desisions
	const float fuzzy_update_novis	= 1000.f;		// speed of fuzzy-logic desisions
	const float fuzzy_guaranteed	= 1.f;			// distance which is supposed 100% visible
	const float lr_granularity		= 0.1f;			// assume similar positions

	class Vision
	{
	private:
		struct Item {
			CObject*			E;
			Collide::ray_cache	Cache;
			float				fuzzy;		// note range: (-1[no]..1[yes])
			Fvector				cp_LP;
			Fvector				cp_LR_src;
			Fvector				cp_LR_dst;
		};
		vector<Item>	track;
		objSET			query;

		void			o_new	(CObject* E);
		void			o_delete(CObject* E);
		void			o_trace	(Fvector& P, float dt);
	public:
		void			o_update(objSET& seen, CEntity* parent, Fvector& P, float dt);
		void			o_get	(objSET& R)
		{
			R.clear		();
			vector<Item>::iterator I=track.begin(),E=track.end();
			for (; I!=E; I++)	if (positive(I->fuzzy)) R.push_back(I->E);
		}
		void			o_get	(objVisible& R)
		{
			vector<Item>::iterator I=track.begin(),E=track.end();
			for (; I!=E; I++)	if (positive(I->fuzzy)) R.insert(I->E);
		}
	};
};
