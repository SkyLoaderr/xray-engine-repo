// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BreakableObjectH
#define BreakableObjectH
#pragma once

#include "gameobject.h"
class CPHStaticGeomShell;

class CBreakableObject: public CGameObject {
	typedef	CGameObject		inherited;
private:
		bool				b_resived_damage;
		float				m_max_frame_damage;
		float				m_damage_threshold;
		Fvector				m_contact_damage_pos;
		Fvector				m_contact_damage_dir;

		float				fHealth;
		CPHStaticGeomShell	*m_pUnbrokenObject;
		CPhysicsShell		*m_Shell;
static	u32					m_remove_time;
		u32					m_break_time;
		bool				bRemoved;
public:
					CBreakableObject	();
	virtual			~CBreakableObject	();

	virtual void	Load				( LPCSTR section);
	virtual BOOL	net_Spawn			( LPVOID DC);
	virtual	void	net_Destroy			();
	virtual void	shedule_Update		( u32 dt);							// Called by sheduler
	virtual void	UpdateCL			( );								// Called each frame, so no need for dt
	virtual void	renderable_Render	( );

	virtual BOOL	renderable_ShadowGenerate	( ) { return TRUE;	}
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
	void			ProcessDamage		();
	void			SendDestroy			();
static 	void __stdcall ObjectContactCallback(bool& /**do_colide/**/,dContact& c);
};

#endif //BreakableObjectH
