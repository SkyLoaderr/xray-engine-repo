#ifndef PGObjectH
#define PGObjectH

#include "TempObject.h"
#include "PSRuntime.h"

class ENGINE_API IRender_Sector;

class ENGINE_API CPGObject: public CTempObject
{
	typedef CTempObject inherited;
	IRender_Sector*		m_pCurSector;
public:
						CPGObject		(LPCSTR pg_name, IRender_Sector* S=0, BOOL bAutoRemove=true);
	virtual				~CPGObject		();

	virtual float		shedule_Scale	()	{ return Device.vCameraPosition.distance_to(Position())/200.f; }
	virtual void		Update			(u32 dt);
	virtual Fvector&	Position		();
	void				UpdateSector	(IRender_Sector* sect);
	void				UpdateParent	(const Fmatrix& m, const Fvector* vel=0);
	void				play_at_pos		(const Fvector& pos);
	void				Play			();
	void				Stop			();

	LPCSTR				dbg_ref_name	();
};
#endif /*PGObjectH*/

