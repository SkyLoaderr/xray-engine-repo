#ifndef ParticlesObjectH
#define ParticlesObjectH

#include "../PS_instance.h"


extern const Fvector zero_vel;

class CParticlesObject		:	public CPS_Instance
{
	Fmatrix				m_XFORM;
	typedef CPS_Instance	inherited;
	u32					dwLastTime;
	void				Init				(LPCSTR p_name, IRender_Sector* S, BOOL bAutoRemove);
	void				UpdateSpatial		();
public:
						CParticlesObject	(LPCSTR p_name, IRender_Sector* S=0, BOOL bAutoRemove=true);
						CParticlesObject	(LPCSTR p_name, BOOL bAutoRemove);
	virtual				~CParticlesObject	();

	virtual float		shedule_Scale		()	{ return Device.vCameraPosition.distance_to(Position())/200.f; }
	virtual void		shedule_Update		(u32 dt);
	virtual void		renderable_Render	();

	Fvector&			Position			();
	void				SetXFORM			(const Fmatrix& m);
	void				UpdateParent		(const Fmatrix& m, const Fvector& vel);

	void				play_at_pos			(const Fvector& pos, BOOL xform=FALSE);
	void				Play				();
	void				Stop				();
	
	bool				IsLooped			() {return m_bLooped;}
	bool				IsAutoRemove		();
	bool				IsPlaying			();
	void				SetAutoRemove		(bool auto_remove);

	LPCSTR				dbg_ref_name		();

protected:
	bool m_bLooped;
};

#endif /*ParticlesObjectH*/
