// GameObject.h: interface for the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)
#define AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_
#pragma once

class CPhysicsShell;
#include "PhysicsRefObject.h"

class CGameObject : 
	public CObject, 
	public CPhysicsRefObject
{
	typedef CObject inherited;
protected:
	CSE_ALifeSimulator		*m_tpALife;
public:
	// AI connection
	u32						AI_NodeID;
	NodeCompressed			*AI_Node;
	CPhysicsShell			*m_pPhysicsShell;
	IC CPhysicsShell		*&PPhysicsShell()		
	{
		return m_pPhysicsShell;
	}
	// Utilities
	void					u_EventGen			(NET_Packet& P, u32 type, u32 dest	);
	void					u_EventSend			(NET_Packet& P, BOOL sync=TRUE		);
	
	// Methods
	virtual void			Load				(LPCSTR section);
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual BOOL			net_Relevant		()	{ return getLocal();	}	// send messages only if active and local
	virtual void			spatial_move		();
	virtual BOOL			Ready				()	{ return getReady();	}	// update only if active and fully initialized by/for network
	virtual float			renderable_Ambient	();

	virtual void			renderable_Render			();
	virtual void			OnEvent				(NET_Packet& P, u16 type);
	virtual void			UpdateCL			();
	
	virtual	void			Hit					(float P, Fvector &dir,	CObject* who, 
												 s16 element,Fvector p_in_object_space, 
												 float impulse, ALife::EHitType hit_type = eHitTypeWound);
	//virtual void			OnH_A_Independent	();
	virtual void			OnH_B_Chield		();
	virtual void			OnH_B_Independent	();
	virtual void			PHGetLinearVell		(Fvector& velocity);
	virtual void			PHSetMaterial		(LPCSTR m);
	virtual void			PHSetMaterial		(u32 m);
	virtual void			PHSetPushOut		(u32 time = 5000);


	virtual bool			IsVisibleForZones() { return true; }

	// Position stack
	virtual	SavedPosition	ps_Element			(u32 ID);

	// Game-specific events
	CGameObject();
	virtual ~CGameObject();

	virtual f32 ExplosionEffect(const Fvector &expl_centre, const f32 expl_radius, xr_list<s16> &elements, xr_list<Fvector> &bs_positions);
	virtual f32 GetMass();

#ifdef DEBUG
	virtual void			OnRender			();
#endif
};

#endif // !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)
