#ifndef PGObjectH
#define PGObjectH

#include "PS_instance.h"
#include "ParticleSystem.h"

class ENGINE_API CPGObject	: 
	public CPS_Instance
{
	typedef CPS_Instance	inherited;
public:
							CPGObject		(LPCSTR pg_name, IRender_Sector* S=0, BOOL bAutoRemove=true);
	virtual					~CPGObject		();

	virtual float		shedule_Scale		()	{ return Device.vCameraPosition.distance_to(Position())/200.f; }
	virtual void		shedule_Update		(u32 dt);
	virtual void		renderable_Render	();
	virtual void		OnEvent				(EVENT E, u64 P1, u64 P2);

	Fvector&			Position			();
	void				SetTransform		(const Fmatrix& m);
	void				UpdateParent		(const Fmatrix& m, const Fvector& vel);
	void				play_at_pos			(const Fvector& pos);
	void				Play				();
	void				Stop				();

	LPCSTR				dbg_ref_name		();
};
#endif /*PGObjectH*/

