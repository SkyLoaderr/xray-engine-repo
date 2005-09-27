#pragma once


#define PropertyGP(a,b) __declspec( property( get=a, put=b ) )

#include "physicsshellholder.h"
#include "damage_manager.h"

// refs
class	ENGINE_API CCameraBase;
class	ENGINE_API C3DSound;
class	ENGINE_API CMotionDef;
class	ENGINE_API CKinematics;
class	ENGINE_API CBoneInstance;
class	CWeaponList;
class   CPHMovementControl;
class	CHudItem;
 
class CEntity : 
		public CPhysicsShellHolder,
		public CDamageManager
{
private:
	typedef	CPhysicsShellHolder	inherited;			

	float				fHealth;
protected:
	// health & shield

	virtual float			GetfHealth			() const			{ return fHealth; }
	virtual float			SetfHealth			(float value)		{fHealth = value; return value;}
	
	float				fMAX_Health;
	float				m_fMaxHealthValue;


	//время через которое мертвое тело убирется с уровня
	ALife::_TIME_ID			m_dwBodyRemoveTime;	

public:
	float				m_fMorale;
	// Team params
	int					id_Team;
	int					id_Squad;
	int					id_Group;
	
	virtual void		ChangeTeam				(int team, int squad, int group);

	struct SEntityState
	{
		u32		bJump	:1;
		u32		bCrouch	:1;
		u32		bFall	:1;
		u32		bSprint	:1;
		float	fVelocity;
		float	fAVelocity;
	};
	
	float					m_fFood;

public:
	// General
	CEntity					();
	virtual ~CEntity		();
public:
	virtual CEntity*		cast_entity			()						{return this;}
public:

	// Core events
	virtual DLL_Pure		*_construct			();
	virtual void			Load				(LPCSTR section);
	virtual void			reinit				();
	virtual void			reload				(LPCSTR section);
	virtual BOOL			net_Spawn			(CSE_Abstract* DC);
	virtual void			net_Destroy			();
	
	virtual void			shedule_Update		(u32 dt);

	virtual void			renderable_Render	();

	bool					IsFocused			()const;
	bool					IsMyCamera			()const;

	virtual float			g_Health			()const	{ return GetfHealth();}
	virtual float			g_MaxHealth			()const	{ return m_fMaxHealthValue;	}

	/*virtual*/ BOOL		g_Alive				()const	{ return GetfHealth()>0; }
	virtual BOOL			g_State				(SEntityState&) const	{return FALSE;}
	
			bool			AlreadyDie			()			{return  0!=GetLevelDeathTime()?true:false;}
			ALife::_TIME_ID	GetGameDeathTime	()const		{return m_game_death_time;}
			u32				GetLevelDeathTime	()const		{return m_level_death_time;}
	
	virtual float			CalcCondition		(float hit);

	int						g_Team				()const	{ return id_Team;	}
	int						g_Squad				()const	{ return id_Squad;	}
	int						g_Group				()const	{ return id_Group;	}

	// Health calculations
	virtual	void			Hit					(float P, Fvector &dir,			CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void			HitSignal			(float P, Fvector &local_dir,	CObject* who, s16 element)		= 0;
	virtual void			HitImpulse			(float P, Fvector &vWorldDir, 	Fvector& vLocalDir)	= 0;

	virtual void			Die					(CObject* who);
			void			KillEntity			(CObject* who);
			void			KillEntity			(u16 whoID);
		
	// Events
	virtual void			OnEvent				( NET_Packet& P, u16 type		);

	virtual BOOL			IsVisibleForHUD		()	{return g_Alive();	}
	virtual void			g_fireParams		(const CHudItem*, Fvector &, Fvector &){}; 

	//time of entity death
	u32						m_level_death_time;
	ALife::_TIME_ID			m_game_death_time;

			void			set_death_time		();
	virtual	void			set_ready_to_save	();

private:
	ALife::_OBJECT_ID		m_killer_id;

public:
	IC		u16				killer_id			() const {return m_killer_id;};
};
