#ifndef _CM_VIS_TRACK
#define _CM_VIS_TRACK

class ENGINE_API CEntity;

#include "..\portal.h"

namespace AI {
	const float fuzzy_update_vis	= 2.0f;		// speed of fuzzy-logic desisions
	const float fuzzy_update_novis	= 1.5f;		// speed of fuzzy-logic desisions
	const float fuzzy_guaranteed	= 4.f;		// distance which is supposed 100% visible

	class VisiTrack 
	{
	private:
		struct Item {
			CObject*			E;
			Collide::ray_cache	Cache;
			float				fuzzy;		// note range: (-1[no]..1[yes])
		};
		vector<Item>	track;
		objSET			query;

		void			o_new	(CObject* E);
		void			o_delete(CObject* E);
	public:
		void			o_update(objSET& seen, CEntity* parent, Fvector& P, float dt);
		void			o_dump	();
		void			o_get	(objSET& R)
		{
			R.clear		();
			vector<Item>::iterator I=track.begin(),E=track.end();
			for (; I!=E; I++)	if (!negative(I->fuzzy)) R.push_back(I->E);
		}
		void			o_get	(objVisible& R)
		{
			vector<Item>::iterator I=track.begin(),E=track.end();
			for (; I!=E; I++)	if (!negative(I->fuzzy)) R.insert(I->E);
		}
	};
};

#endif