// GameObject.h: interface for the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)
#define AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_
#pragma once

#include "PhysicsRefObject.h"
#include "level_graph.h"
#include "ai_object_location.h"
#include "prefetch_manager.h"
#include "ParticlesPlayer.h"
#include "xrServer_Space.h"
#include "ai_alife_space.h"

class CPhysicsShell;
class CSE_Abstract;
class CPHSynchronize;
class CLuaGameObject;

class CGameObject : 
	public CObject, 
	public CPhysicsRefObject,
	virtual public CAI_ObjectLocation,
	public CPrefetchManager,
	public CParticlesPlayer

{
	typedef CObject inherited;
	u32						m_dwFrameLoad;
	u32						m_dwFrameReload;
	u32						m_dwFrameReinit;
	u32						m_dwFrameSpawn;
	u32						m_dwFrameDestroy;
	u32						m_dwFrameClient;
protected:
	CSE_ALifeSimulator		*m_tpALife;
public:
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
												 float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);

	//��� ������������ CParticlesPlayer
	virtual IRender_Sector*	Sector				()		{return inherited::Sector();}
	
	//virtual void			OnH_A_Independent	();
	virtual void			OnH_B_Chield		();
	virtual void			OnH_B_Independent	();
	virtual void			PHGetLinearVell		(Fvector& velocity);
	virtual void			PHSetLinearVell		(Fvector& velocity);
	virtual void			PHSetMaterial		(LPCSTR m);
	virtual void			PHSetMaterial		(u16 m);
	virtual void			PHSetPushOut		(u32 time = 5000);
///////////////////////////////////////////////////////////////////////
	virtual u16				PHGetSyncItemsNumber();
	virtual CPHSynchronize*	PHGetSyncItem		(u16 item);
	virtual void			PHUnFreeze			();
	virtual void			PHFreeze			();
///////////////////// network /////////////////////////////////////////
	virtual void			make_Interpolation () {}; //interpolation from last visible to corrected position/rotation
	virtual void			PH_B_CrPr		() {}; // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr		() {}; // actions & operations after correction before prediction steps
	virtual void			PH_A_CrPr		() {}; // actions & operations after phisic correction-prediction steps
///////////////////////////////////////////////////////////////////////
	virtual bool			IsVisibleForZones() { return true; }

	// Position stack
	virtual	SavedPosition	ps_Element			(u32 ID);

			void			setup_parent_ai_locations(bool assign_position = true);
			void			validate_ai_locations(bool decrement_reference = true);

	// Game-specific events
	CGameObject();
	virtual ~CGameObject();

	virtual f32 ExplosionEffect(const Fvector &expl_centre, const f32 expl_radius, xr_list<s16> &elements, xr_list<Fvector> &bs_positions);
	virtual f32 GetMass();
	virtual BOOL			UsedAI_Locations	();

#ifdef DEBUG
	virtual void			OnRender			();
#endif

			void			Init				();
	virtual	void			reinit				();
	virtual	void			reload				(LPCSTR section);

	virtual const SRotation	Orientation			() const
	{
		SRotation			rotation;
		float				h,p,b;
		XFORM().getHPB		(h,p,b);
		rotation.yaw		= h;
		rotation.pitch		= p;
		return				(rotation);
	};

	virtual bool			use_parent_ai_locations	() const
	{
		return				(true);
	}

public:
	typedef void __stdcall visual_callback(CKinematics *);
	typedef svector<visual_callback*,6>			CALLBACK_VECTOR;
	typedef CALLBACK_VECTOR::iterator			CALLBACK_VECTOR_IT;

	CALLBACK_VECTOR			m_visual_callback;

public:
			void			add_visual_callback		(visual_callback *callback);
			void			remove_visual_callback	(visual_callback *callback);
	IC		CALLBACK_VECTOR &visual_callbacks	()
	{
		return				(m_visual_callback);
	}
	virtual void			create_physic_shell		();
	virtual void			activate_physic_shell	();
	virtual void			setup_physic_shell		();

private:
	CLuaGameObject			*m_lua_game_object;
public:
			CLuaGameObject	*lua_game_object();
protected:
			bool	frame_check			(u32 &frame);
};

#endif // !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)
