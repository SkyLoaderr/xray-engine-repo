// Entity.h: interface for the CEntity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_)
#define AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_
#pragma once


#define PropertyGP(a,b) __declspec( property( get=a, put=b ) )

#include "physicsshellholder.h"
#include "entitycondition.h"
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
 
class CEntity : virtual public CPhysicsShellHolder,
						public CDamageManager
{
private:
	typedef	CPhysicsShellHolder	inherited;			

	float				fHealth;
protected:
	// health & shield

	virtual float GetfHealth() const { return fHealth; }
	virtual float SetfHealth(float value) {fHealth = value; return value;}
	PropertyGP(GetfHealth,SetfHealth) float fEntityHealth;

//	virtual float& GetEntityHealth() { return fHealth; }
	
	float				fMAX_Health;
	float				fArmor,		fMAX_Armor;
	float				m_fMaxHealthValue;

	//icon position (используется при торговле и обыске трупов)
	int m_iTradeIconX;
	int m_iTradeIconY;
	//иконки для карты
	int m_iMapIconX;
	int m_iMapIconY;

	//время через которое мертвое тело убирется с уровня
	ALife::_TIME_ID			m_dwBodyRemoveTime;	

public:
	float				m_fMorale;
	// Team params
	int					id_Team;
	int					id_Squad;
	int					id_Group;

	struct SEntityState
	{
		u32		bJump	:1;
		u32		bCrouch	:1;
		u32		bFall	:1;
		float	fVelocity;
	};
	
	float					m_fFood;

public:
	// General
	CEntity					();
	virtual ~CEntity		();
public:
	virtual CEntity*					cast_entity				()						{return this;}
	virtual const CEntity*				cast_entity				() const				{return this;}
public:

	// Core events
	virtual void			Load				(LPCSTR section);
	virtual void			reinit				();
	virtual void			reload				(LPCSTR section);
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();

	virtual void			renderable_Render	();

	bool					IsFocused			()const	{ return (smart_cast<const CEntity*>(g_pGameLevel->CurrentEntity())==this);		}
	bool					IsMyCamera			()const	{ return (smart_cast<const CEntity*>(g_pGameLevel->CurrentViewEntity())==this);	}

	float					g_Armor				()const	{ return fArmor;	}
	virtual float			g_Health			()const	{ return fEntityHealth;}
	virtual float			g_MaxHealth			()const	{ return m_fMaxHealthValue;	}

	virtual BOOL			g_Alive				()const	{ return fEntityHealth>0; }
	virtual BOOL			g_State				(SEntityState&) const	{return FALSE;}
	
	virtual bool			AlreadyDie()			{return  0!=GetLevelDeathTime()?true:false;}
	virtual ALife::_TIME_ID	GetGameDeathTime()const	{return m_game_death_time;}
	virtual u32				GetLevelDeathTime()const{return m_level_death_time;}
	
	virtual float			CalcCondition		(float hit);

	int						g_Team				()const	{ return id_Team;	}
	int						g_Squad				()const	{ return id_Squad;	}
	int						g_Group				()const	{ return id_Group;	}

	// misc
	virtual CWeaponList*	GetItemList			()	{ return 0;			}

	// Health calculations
	virtual	void			Hit					(float P, Fvector &dir,			CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void			HitSignal			(float P, Fvector &local_dir,	CObject* who, s16 element)		= 0;
	virtual void			HitImpulse			(float P, Fvector &vWorldDir, 	Fvector& vLocalDir)	= 0;
	virtual	void			HitScale			(const int bone_num, float hit_scale, float wound_scale);
	virtual void			Die					();
	virtual void			KillEntity			(CObject* who);
		
	// Events
	virtual void			OnEvent				( NET_Packet& P, u16 type		);

	virtual BOOL			IsVisibleForHUD		()	{return g_Alive();	}
	virtual void			g_fireParams		(const CHudItem*, Fvector &, Fvector &){}; 

	//icon
	virtual int GetTradeIconX() const {return m_iTradeIconX;}
	virtual int GetTradeIconY() const {return m_iTradeIconY;}
	virtual int GetMapIconX()   const {return m_iMapIconX;}
	virtual int GetMapIconY()	const {return m_iMapIconY;}

	//time of entity death
	u32						m_level_death_time;
	ALife::_TIME_ID			m_game_death_time;

			void			set_death_time		();
};

#endif // AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_