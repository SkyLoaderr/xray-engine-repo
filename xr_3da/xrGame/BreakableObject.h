// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BreakableObjectH
#define BreakableObjectH
#pragma once
struct SGameMtl;
#include "physicsshellholder.h"
#include "ode_include.h"
class CPHStaticGeomShell;

class CBreakableObject: public CPhysicsShellHolder {
	typedef	CPhysicsShellHolder		inherited;
private:
		bool				b_resived_damage;
		float				m_max_frame_damage;
static	float				m_damage_threshold;
static	float				m_health_threshhold;
		Fvector				m_contact_damage_pos;
		Fvector				m_contact_damage_dir;

		float				fHealth;
		CPHStaticGeomShell	*m_pUnbrokenObject;
		CPhysicsShell		*m_Shell;
static	u32					m_remove_time;
		u32					m_break_time;
		bool				bRemoved;
//		Fbox				m_saved_box;
public:
					CBreakableObject	();
	virtual			~CBreakableObject	();

	virtual void	Load				( LPCSTR section);
	virtual BOOL	net_Spawn			( LPVOID DC);
	virtual	void	net_Destroy			();
	virtual void	shedule_Update		( u32 dt);							// Called by sheduler
	virtual void	UpdateCL			( );								// Called each frame, so no need for dt
	virtual void	renderable_Render	( );

	virtual BOOL	renderable_ShadowGenerate	( ) { return FALSE;	}
	virtual BOOL	renderable_ShadowReceive	( ) { return TRUE;	}
	
	virtual	void	Hit					(float P,Fvector &dir, CObject* who,s16 element,
										Fvector p_in_object_space, float impulse, ALife::EHitType /**hit_type/**/);

	virtual void	net_Export			(NET_Packet& P);
	virtual void	net_Import			(NET_Packet& P);
	virtual BOOL	UsedAI_Locations	();
private:
	void			Init				();
	void			CreateUnbroken		();
	void			CreateBroken		();
	void			DestroyUnbroken		();
	void			ActivateBroken		();
	void			Split				();
	void			Break				();
	void			ApplyExplosion		(const Fvector &dir,float impulse);
	void			CheckHitBreak		(float power,ALife::EHitType hit_type);
	void			ProcessDamage		();
	void			SendDestroy			();
static 	void __stdcall ObjectContactCallback(bool& /**do_colide/**/,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/);
};

#endif //BreakableObjectH
