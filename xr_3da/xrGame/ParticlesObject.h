#ifndef ParticlesObjectH
#define ParticlesObjectH

#include "../PS_instance.h"
#include "script_export_space.h"

extern const Fvector zero_vel;

class CParticlesObject		:	public CPS_Instance
{
	typedef CPS_Instance	inherited;

	u32					dwLastTime;
	void				Init				(LPCSTR p_name, IRender_Sector* S, BOOL bAutoRemove);
	void				UpdateSpatial		();
protected:
	//флаг, что система зациклена
	bool				m_bLooped;
	//вызвана функция Stop()
	bool				m_bStoppig;
public:
						CParticlesObject	(LPCSTR p_name, IRender_Sector* S=0, BOOL bAutoRemove=TRUE);
						CParticlesObject	(LPCSTR p_name, BOOL bAutoRemove);
	virtual				~CParticlesObject	();

	virtual float		shedule_Scale		()	{ return Device.vCameraPosition.distance_to(Position())/200.f; }
	virtual void		shedule_Update		(u32 dt);
	virtual void		renderable_Render	();
	void				PerformAllTheWork	(u32 dt);

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

	const shared_str			Name		();
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
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CParticlesObject)
#undef script_type_list
#define script_type_list save_type_list(CParticlesObject)

#endif /*ParticlesObjectH*/
