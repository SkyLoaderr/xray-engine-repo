// CustomMonster.h: interface for the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_)
#define AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_
#pragma once

#include "../feel_touch.h"
#include "entity_alive.h"

#include "ai_space.h"
#include "AI/ai_monster_state.h"
#include "AI/script/ai_script_monster.h"
#include "ai_monster_space.h"

#include "damage_manager.h"
#include "event_memory_manager.h"
#include "location_manager.h"
#include "material_manager.h"
#include "memory_manager.h"
#include "movement_manager.h"
#include "selector_manager.h"
#include "sound_player.h"

using namespace MonsterSpace;

class CAI_Rat;

class CCustomMonster : 
			public CEntityAlive, 
			public CScriptMonster,
			public Feel::Touch,
			public CDamageManager,
			public CEventMemoryManager,
			public CLocationManager,
			public CMaterialManager,
			public CMemoryManager,
	virtual	public CMovementManager,
			public CSelectorManager,
			public CSoundPlayer
{
	typedef	CEntityAlive	inherited;
protected:
	
	struct				SAnimState
	{
		CMotionDef		*fwd;
		CMotionDef		*back;
		CMotionDef		*ls;
		CMotionDef		*rs;

		void			Create(CSkeletonAnimated* K, LPCSTR base);
	};
private:
	bool				m_client_update_activated;

public:
	// Eyes
	Fmatrix				eye_matrix;
	int					eye_bone;
	float				eye_fov;
	float				eye_range;

	float				m_fCurSpeed;

	u32					eye_pp_stage;
	u32					eye_pp_timestamp;
	Fvector				m_tEyeShift;
	float				m_fEyeShiftYaw;
	BOOL				NET_WasExtrapolating;

	Fvector				tWatchDirection;

	virtual void		Think() = 0;

	float				m_fTimeUpdateDelta;
	u32					m_dwLastUpdateTime;
	u32					m_current_update;
//	Fmatrix				m_tServerTransform;
	
	u32					m_dwCurrentTime;		// time updated in UpdateCL 

	struct net_update	{
		u32				dwTimeStamp;			// server(game) timestamp
		float			o_model;				// model yaw
		SRotation		o_torso;				// torso in world coords
		Fvector			p_pos;					// in world coords
		float			fHealth;

		// non-exported (temporal)

		net_update()	{
			dwTimeStamp		= 0;
			o_model			= 0;
			o_torso.yaw		= 0;
			o_torso.pitch	= 0;
			p_pos.set		(0,0,0);
			fHealth			= 0.f;
		}
		void	lerp	(net_update& A,net_update& B, float f);
	};
	xr_deque<net_update>	NET;
	net_update				NET_Last;
	BOOL					NET_WasInterpolating;	// previous update was by interpolation or by extrapolation
	u32						NET_Time;				// server time of last update
//------------------------------

	// utils
	void				mk_orientation			( Fvector& dir, Fmatrix& mR );
	void				mk_rotation				( Fvector& dir, SRotation &R);

	// stream executors
	virtual void		Exec_Action				( float dt );
	virtual void		Exec_Look				( float dt );
	void				Exec_Physics			( float dt );
	void				Exec_Visibility			( );
	void				eye_pp_s0				( );
	void				eye_pp_s1				( );
	void				eye_pp_s2				( );

	void				BuildCamera				( );
public:
						CCustomMonster			( );
	virtual				~CCustomMonster			( );

public:
	virtual CEntityAlive*				cast_entity_alive		()						{return CEntityAlive::cast_entity_alive();}
	virtual const CEntityAlive*			cast_entity_alive		() const				{return CEntityAlive::cast_entity_alive();}
	virtual CEntity*					cast_entity				()						{return CEntity::cast_entity();}
	virtual const CEntity*				cast_entity				() const				{return CEntity::cast_entity();}

public:

	virtual BOOL		net_Spawn				( LPVOID DC);
	virtual void		Die						( );

	virtual void		HitSignal				( float P,	Fvector& vLocalDir, CObject* who);
	virtual void		g_WeaponBones			(int &/**L/**/, int &/**R1/**/, int &/**R2/**/) {};
	virtual void		shedule_Update					( u32		DT		);
	virtual void		UpdateCL				( );

	// Network
	virtual void		net_Export				(NET_Packet& P);				// export to server
	virtual void		net_Import				(NET_Packet& P);				// import from server

	virtual void		SelectAnimation			( const Fvector& _view, const Fvector& _move, float speed ) = 0;

	// debug
#ifdef DEBUG
	virtual void		OnRender				( );
	virtual void		OnHUDDraw				(CCustomHUD* hud);
#endif

	virtual bool		bfExecMovement			(){return(false);};
	virtual CPHMovementControl*	movement_control(){return inherited::movement_control();}

	IC	bool					angle_lerp_bounds		(float &a, float b, float c, float d);
	IC	void					vfNormalizeSafe			(Fvector& Vector);

public:
	virtual	float				ffGetFov				()	const								{return eye_fov;}	
	virtual	float				ffGetRange				()	const								{return eye_range;}
//	virtual	void				feel_touch_new			(CObject	*O);
	virtual BOOL				feel_visible_isRelevant	(CObject		*O);
	virtual	Feel::Sound*		dcast_FeelSound			()			{ return this;	}
	virtual void				renderable_Render		();
	virtual	void				Hit						(float P, Fvector &dir,			CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void				OnEvent					( NET_Packet& P, u16 type		);
	virtual void				net_Destroy				();
	virtual BOOL				UsedAI_Locations		();
	///////////////////////////////////////////////////////////////////////
	virtual u16					PHGetSyncItemsNumber	()			{return inherited ::PHGetSyncItemsNumber();}
	virtual CPHSynchronize*		PHGetSyncItem			(u16 item)	{return inherited ::PHGetSyncItem(item);}
	virtual void				PHUnFreeze				()			{return inherited ::PHUnFreeze();}
	virtual void				PHFreeze				()			{return inherited ::PHFreeze();}
	///////////////////////////////////////////////////////////////////////
private:
			void				init					();
public:
	virtual void				Load					(LPCSTR	section);				
	virtual	void				reinit					();
	virtual void				reload					(LPCSTR	section);				
	virtual const SRotation		Orientation				() const
	{
		return					(m_body.current);
	};
	virtual bool				use_model_pitch			() const;
	virtual float				get_custom_pitch_speed	(float def_speed) {return def_speed;}
	virtual bool				human_being				() const
	{
		return					(false);
	}
	
	virtual void				ChangeTeam				(int team, int squad, int group);
	virtual	void				PitchCorrection			();

	virtual void				save					(NET_Packet &output_packet) {inherited::save(output_packet);}
	virtual void				load					(IReader &input_packet)		{inherited::load(input_packet);}
	virtual BOOL				net_SaveRelevant		()							{return inherited::net_SaveRelevant();}
	virtual CAI_Rat				*dcast_Rat				() {return 0;}
};

#include "custommonster_inline.h"

#endif // !defined(AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_)
