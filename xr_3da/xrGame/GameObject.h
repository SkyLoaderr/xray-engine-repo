// GameObject.h: interface for the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)
#define AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_
#pragma once

#include "level_graph.h"
#include "ai_object_location.h"
#include "prefetch_manager.h"
#include "ParticlesPlayer.h"
#include "xrServer_Space.h"
#include "alife_space.h"
#include "UsableScriptObject.h"
#include "script_binder.h"

class CPhysicsShell;
class CSE_Abstract;
class CPHSynchronize;
class CScriptGameObject;

class CGameObject : 
	public CObject, 
	virtual public CAI_ObjectLocation,
	public CPrefetchManager,
	public CParticlesPlayer,
	public CUsableScriptObject,
	public CScriptBinder
{
	typedef CObject inherited;
	u32						m_dwFrameLoad;
	u32						m_dwFrameReload;
	u32						m_dwFrameReinit;
	u32						m_dwFrameSpawn;
	u32						m_dwFrameDestroy;
	u32						m_dwFrameClient;
	u32						m_dwFrameSchedule;
	u32						m_dwFrameBeforeChild;
	u32						m_dwFrameBeforeIndependent;
	bool					m_spawned;

protected:
	//время удаления объекта
	bool					m_bObjectRemoved;

public:
	CInifile				*m_ini_file;

	// Utilities
	static void				u_EventGen			(NET_Packet& P, u32 type, u32 dest	);
	static void				u_EventSend			(NET_Packet& P, BOOL sync=TRUE		);
	
	// Methods
	virtual void			Load				(LPCSTR section);
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	
	//object serialization
	virtual void			net_Save			(NET_Packet &net_packet);
	virtual BOOL			net_SaveRelevant	();
	virtual void			save				(NET_Packet &output_packet);
	virtual void			load				(IReader &input_packet);

	virtual BOOL			net_Relevant		()	{ return getLocal();	}	// send messages only if active and local
	virtual void			spatial_move		();
	virtual BOOL			Ready				()	{ return getReady();	}	// update only if active and fully initialized by/for network
	virtual float			renderable_Ambient	();
	//virtual f32				GetMass				(){return 0.f;};
	virtual void			shedule_Update		(u32 dt);	
	virtual void			renderable_Render	();
	virtual void			OnEvent				(NET_Packet& P, u16 type);
	virtual	void			Hit					(float P, Fvector &dir,	CObject* who, 
		s16 element,Fvector p_in_object_space, 
		float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound){};
	virtual void			UpdateCL			();
	//virtual CPhysicsShell	*&PPhysicsShell		()																										
	/*											{
												R_ASSERT2(false,"Must never be colled. This object does not support physics shell!");
												static CPhysicsShell* no_shell=0; 
												return no_shell;
												}*/
	
	//игровое имя объекта
	virtual LPCSTR			Name                () const;
	
	//для наследования CParticlesPlayer
	virtual IRender_Sector*	Sector				()		{return inherited::Sector();}
	
	//virtual void			OnH_A_Independent	();
	virtual void			OnH_B_Chield		();
	virtual void			OnH_B_Independent	();

	virtual bool			IsVisibleForZones	() { return true; }
///////////////////////////////////////////////////////////////////////
	virtual bool			NeedToDestroyObject	() const;
	virtual void			DestroyObject		();
///////////////////////////////////////////////////////////////////////

	// Position stack
	virtual	SavedPosition	ps_Element			(u32 ID) const;

			void			setup_parent_ai_locations(bool assign_position = true);
			void			validate_ai_locations(bool decrement_reference = true);

	// Game-specific events
	CGameObject();
	virtual ~CGameObject();

	virtual f32 ExplosionEffect(const Fvector &expl_centre, const f32 expl_radius, xr_list<s16> &elements, xr_list<Fvector> &bs_positions);

	virtual BOOL			UsedAI_Locations	();

#ifdef DEBUG
	virtual void			OnRender			();
#endif

			void			init				();
	virtual	void			reinit				();
	virtual	void			reload				(LPCSTR section);
	///////////////////// network /////////////////////////////////////////
private:
	bool					m_bCrPr_Activated;
	u32						m_dwCrPr_ActivationStep;

public:
	virtual void			make_Interpolation	() {}; // interpolation from last visible to corrected position/rotation
	virtual void			PH_B_CrPr			() {}; // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr			() {}; // actions & operations after correction before prediction steps
#ifdef DEBUG
	virtual void			PH_Ch_CrPr			() {}; // 
#endif
	virtual void			PH_A_CrPr			() {}; // actions & operations after phisic correction-prediction steps
	virtual void			CrPr_SetActivationStep	(u32 Step)	{m_dwCrPr_ActivationStep = Step; };
	virtual u32				CrPr_GetActivationStep	()	{ return m_dwCrPr_ActivationStep; };
	virtual void			CrPr_SetActivated		(bool Activate)	{ m_bCrPr_Activated = Activate; };
	virtual bool			CrPr_IsActivated		()				{ return m_bCrPr_Activated; };
	///////////////////////////////////////////////////////////////////////
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
			void			SetKinematicsCallback	(bool set);

	IC		CALLBACK_VECTOR &visual_callbacks	()
	{
		return				(m_visual_callback);
	}


private:
	mutable CScriptGameObject	*m_lua_game_object;
	int						m_script_clsid;
public:
			CScriptGameObject	*lua_game_object() const;
			int				clsid			() const
	{
		VERIFY				(m_script_clsid >= 0);
		return				(m_script_clsid);
	}
public:
	IC		CInifile		*spawn_ini			()
	{
		return				(m_ini_file);
	}
protected:
			bool			frame_check			(u32 &frame)
			{
				if (Device.dwFrame == frame)	return		(false);
				frame			= Device.dwFrame;
				return			(true);
			}
	virtual	void			spawn_supplies		();
};

#endif // !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)
