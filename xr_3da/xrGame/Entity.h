// Entity.h: interface for the CEntity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_)
#define AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_
#pragma once


#define PropertyGP(a,b) __declspec( property( get=a, put=b ) )

#include "gameobject.h"
#include "phmovementcontrol.h"
#include "entitycondition.h"



// refs
class	ENGINE_API CCameraBase;
class	ENGINE_API C3DSound;
class	ENGINE_API CMotionDef;
class	ENGINE_API CKinematics;
class	ENGINE_API CBoneInstance;
class	CWeaponList;
class   CPHMovementControl;
 
class CEntity : virtual public CGameObject
{
private:
	typedef	CGameObject	inherited;			

	float				fHealth;
protected:
	// health & shield

	virtual float GetfHealth() const { return fHealth; }
	virtual float SetfHealth(float value) {fHealth = value; return value;}
	PropertyGP(GetfHealth,SetfHealth) float fEntityHealth;

//	virtual float& GetEntityHealth() { return fHealth; }
	
	float				fMAX_Health;
	float				fArmor,		fMAX_Armor;
	float				fAccuracy;
	float				m_fMaxHealthValue;

	//icon position (������������ ��� �������� � ������ ������)
	int m_iTradeIconX;
	int m_iTradeIconY;
	//������ ��� �����
	int m_iMapIconX;
	int m_iMapIconY;

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
		float	fVelocity;
	};
	
	float					m_fFood;

public:
	// General
	CEntity					();
	virtual ~CEntity		();

	// Core events
	virtual void			Load				(LPCSTR section);
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual void			shedule_Update		(u32 dt);	
	virtual void			renderable_Render	();

	bool					IsFocused			()const	{ return (dynamic_cast<const CEntity*>(g_pGameLevel->CurrentEntity())==this);		}
	bool					IsMyCamera			()const	{ return (dynamic_cast<const CEntity*>(g_pGameLevel->CurrentViewEntity())==this);	}

	float					g_Armor				()const	{ return fArmor;	}
	virtual float			g_Health			()const	{ return fEntityHealth;}
	virtual float			g_MaxHealth			()const	{ return m_fMaxHealthValue;	}
	float					g_Accuracy			()const	{ return fAccuracy;	}
	virtual BOOL			g_Alive				()const	{ return fEntityHealth>0; }
	virtual BOOL			g_State				(SEntityState &/**state/**/)	{return FALSE;}
	virtual bool			AlreadyDie()		{return  0!=m_dwDeathTime?true:false;}

	virtual float			CalcCondition		(float hit);

	int						g_Team				()const	{ return id_Team;	}
	int						g_Squad				()const	{ return id_Squad;	}
	int						g_Group				()const	{ return id_Group;	}

	// misc
	virtual CWeaponList*	GetItemList			()	{ return 0;			}

	// Health calculations
	virtual	void			Hit					(float P, Fvector &dir,			CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type = eHitTypeWound);
	virtual void			HitSignal			(float P, Fvector &local_dir,	CObject* who, s16 element)		= 0;
	virtual void			HitImpulse			(float P, Fvector &vWorldDir, 	Fvector& vLocalDir)	= 0;
	virtual	float			HitScale			(int /**element/**/){return 1.f;}
	virtual void			Die					() = 0;
	virtual void			KillEntity			(CObject* who);
		
	// Events
	virtual void			OnEvent				( NET_Packet& P, u16 type		);

	virtual BOOL			IsVisibleForHUD		()	{return g_Alive();	}
	virtual void			g_fireParams			(Fvector &, Fvector &){}; 


	//icon
	virtual int GetTradeIconX() {return m_iTradeIconX;}
	virtual int GetTradeIconY() {return m_iTradeIconY;}
	virtual int GetMapIconX() {return m_iMapIconX;}
	virtual int GetMapIconY() {return m_iMapIconY;}


	//time of entity death
	_TIME_ID				m_dwDeathTime;
};

#endif // !defined(AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_)
NT					m_tpEventSay;
	bool					m_bMobility;
	float					m_fAccuracy;
	float					m_fIntelligence;
	//m_PhysicMovementControl
	CPHMovementControl		m_PhysicMovementControl;
public:
	// General
	CEntityAlive			();
	virtual ~CEntityAlive	();

	// Core events
	virtual void			Load					(LPCSTR section);
	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual void			net_Destroy				();

	virtual void			shedule_Update			(u32 dt);

	virtual void			HitImpulse				(float amount, Fvector& vWorldDir, Fvector& vLocalDir);
	virtual	void			Hit						(float P, Fvector &dir,			CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type = eHitTypeWound);
	virtual void			g_WeaponBones			(int &L, int &R1, int &R2)										= 0;

//	virtual float&			GetEntityHealth()		{ return m_fHealth; }
	
	virtual float GetfHealth() { return m_fHealth*100.f; }
	virtual float SetfHealth(float value) {m_fHealth = value/100.f; return value;}
	PropertyGP(GetfHealth,SetfHealth) float fEntityHealth;

	virtual float			g_Health			()	{ return GetHealth()*100.f; }
	virtual float			g_MaxHealth			()	{ return GetMaxHealth()*100.f; }

	virtual float			CalcCondition		(float hit);


	// Visibility related
	virtual void			GetVisible				(objVisible& /**R/**/)	{};
	virtual	float			ffGetFov				()				= 0;	
	virtual	float			ffGetRange				()				= 0;	
	virtual	ERelationType	tfGetRelationType		(CEntityAlive *tpEntityAlive)
	{
		if (tpEntityAlive->g_Team() != g_Team())
			return(eRelationTypeEnemy);
		else
			return(eRelationTypeFriend);
	};

	virtual void			BuyItem					(LPCSTR buf);
public:
IC	CPHMovementControl* PMovement()
	{
		return &m_PhysicMovementControl;
	}

};

#endif // !defined(AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_)
