#ifndef ParticlesObjectH
#define ParticlesObjectH

#include "../PS_instance.h"


extern const Fvector zero_vel;

class CParticlesObject		:	public CPS_Instance
{
	typedef CPS_Instance	inherited;

	u32					dwLastTime;
	void				Init				(LPCSTR p_name, IRender_Sector* S, BOOL bAutoRemove);
	void				UpdateSpatial		();
protected:
	bool m_bLooped;
public:
						CParticlesObject	(LPCSTR p_name, IRender_Sector* S=0, BOOL bAutoRemove=TRUE);
						CParticlesObject	(LPCSTR p_name, BOOL bAutoRemove);
	virtual				~CParticlesObject	();

	virtual float		shedule_Scale		()	{ return Device.vCameraPosition.distance_to(Position())/200.f; }
	virtual void		shedule_Update		(u32 dt);
	virtual void		renderable_Render	();

	Fvector&			Position			();
	void				SetXFORM			(const Fmatrix& m);
	IC	Fmatrix&		XFORM				()	{return renderable.xform;}
	void				UpdateParent		(const Fmatrix& m, const Fvector& vel);

	void				play_at_pos			(const Fvector& pos, BOOL xform=FALSE);
	virtual void		Play				();
	void				Stop				(BOOL bDefferedStop=TRUE);
	
	bool				IsLooped			() {return m_bLooped;}
	bool				IsAutoRemove		();
	bool				IsPlaying			();
	void				SetAutoRemove		(bool auto_remove);

	const ref_str&		Name				();
public:
	static CParticlesObject*	Create		(LPCSTR p_name, IRender_Sector* S=0, BOOL bAutoRemove=TRUE)
	{
		return xr_new<CParticlesObject>(p_name, S, bAutoRemove);
	}
	static void					Destroy		(CParticlesObject*& p)
	{
		if (p){ 
			p->PSI_destroy		();
			p					= 0;
		}
	}
};

#endif /*ParticlesObjectH*/
