// CustomMonster.h: interface for the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../feel_vision.h"
#include "../feel_sound.h"
#include "../feel_touch.h"

#include "ai_space.h"
#include "ai_monster_space.h"

#include "entity_alive.h"
#include "script_entity.h"

using namespace MonsterSpace;

class CAI_Rat;
class CMotionDef;
class CSkeletonAnimated;
class CMemoryManager;
class CMovementManager;
class CSoundPlayer;
class CMaterialManager;

class CCustomMonster : 
	public CEntityAlive, 
	public CScriptEntity,
	public Feel::Vision,
	public Feel::Sound,
	public Feel::Touch
{
private:
	typedef	CEntityAlive	inherited;

private:
	CMemoryManager		*m_memory_manager;
	CMovementManager	*m_movement_manager;
	CSoundPlayer		*m_sound_player;
	CMaterialManager	*m_material_manager;

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
	xr_vector<CLASS_ID>	m_killer_clsids;

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

	virtual BOOL		feel_touch_on_contact	(CObject *);
	virtual BOOL		feel_touch_contact		(CObject *);
	// utils
	void				mk_orientation			( Fvector& dir, Fmatrix& mR );
	void				mk_rotation				( Fvector& dir, SRotation &R);

	// stream executors
	virtual void		Exec_Action				( float dt );
	virtual void		Exec_Look				( float dt );
	void				Exec_Visibility			( );
	void				eye_pp_s0				( );
	void				eye_pp_s1				( );
	void				eye_pp_s2				( );

	void				BuildCamera				( );
public:
						CCustomMonster			( );
	virtual				~CCustomMonster			( );

public:
	virtual CEntityAlive*				cast_entity_alive		()						{return this;}
	virtual CEntity*					cast_entity				()						{return this;}

public:

	virtual DLL_Pure	*_construct				();
	virtual BOOL		net_Spawn				( CSE_Abstract* DC);
	virtual void		Die						( CObject* who);

	virtual void		HitSignal				( float P,	Fvector& vLocalDir, CObject* who);
	virtual void		g_WeaponBones			(int &/**L/**/, int &/**R1/**/, int &/**R2/**/) {};
	virtual void		shedule_Update					( u32		DT		);
	virtual void		UpdateCL				( );

	// Network
	virtual void		net_Export				(NET_Packet& P);				// export to server
	virtual void		net_Import				(NET_Packet& P);				// import from server
	virtual void		net_Relcase				(CObject*	 O);

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
public:
	virtual void				Load					(LPCSTR	section);				
	virtual	void				reinit					();
	virtual void				reload					(LPCSTR	section);				
	virtual const SRotation		Orientation				() const;
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
	
	virtual	const MonsterSpace::SBoneRotation &head_orientation	() const;
	
	virtual CAI_Rat				*dcast_Rat				() {return 0;}
	
	virtual void				UpdatePositionAnimation	();
	virtual void				set_ready_to_save		();
	virtual CPhysicsShellHolder*cast_physics_shell_holder	()	{return this;}
	virtual CParticlesPlayer*	cast_particles_player	()	{return this;}
	virtual CCustomMonster*		cast_custom_monster		()	{return this;}
	virtual CScriptEntity*		cast_script_entity		()	{return this;}

			void				load_killer_clsids		(LPCSTR section);
			bool				is_special_killer		(CObject *obj);

	IC		CMemoryManager		&memory					() const;
	virtual float				feel_vision_mtl_transp	(u32 element);
	virtual	void				feel_sound_new			(CObject* who, int type, CSoundUserDataPtr user_data, const Fvector &Position, float power);
	virtual bool				useful					(const CGameObject *object) const;
	virtual float				evaluate				(const CGameObject *object) const;
	virtual bool				useful					(const CEntityAlive *object) const;
	virtual float				evaluate				(const CEntityAlive *object) const;

private:
	CSoundUserDataVisitor			*m_sound_user_data_visitor;

protected:
	virtual CSoundUserDataVisitor	*create_sound_visitor	();
	virtual CMemoryManager			*create_memory_manager	();
	virtual CMovementManager		*create_movement_manager();

public:
	IC		CMovementManager		&movement				() const;
	IC		CSoundPlayer			&sound					() const;
	IC		CMaterialManager		&material				() const;
	IC		CSoundUserDataVisitor	*sound_user_data_visitor() const;
};

#include "custommonster_inline.h"