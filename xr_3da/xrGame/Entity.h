// Entity.h: interface for the CEntity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_)
#define AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_
#pragma once

#include "movementcontrol.h"
#include "ai_goals.h"
#include "gameobject.h"

// refs
class ENGINE_API CCameraBase;
class ENGINE_API C3DSound;
class CWeaponList;

class	ENGINE_API CMotionDef;
class	ENGINE_API CKinematics;
class	ENGINE_API CBoneInstance;
class	CWeaponList;
 
class CEntity : public CGameObject
{
private:
	typedef	CGameObject	inherited;			
protected:
	// health & shield
	float				fHealth,	fMAX_Health;
	float				fArmor,		fMAX_Armor;
	float				fAccuracy;
	float				m_fMaxHealthValue;
public:
	// Team params
	int					id_Team;
	int					id_Squad;
	int					id_Group;

	struct SEntityState
	{
		DWORD	bJump	:1;
		DWORD	bCrouch	:1;
		float	fVelocity;
	};
public:
	// General
	CEntity					();
	virtual ~CEntity		();

	// Core events
	virtual void			Load				(LPCSTR section);
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual void			Update				(DWORD dt);	
	virtual void			OnVisible			();

	bool					IsFocused			()	{ return (pCreator->CurrentEntity()==this);		}
	bool					IsMyCamera			()	{ return (pCreator->CurrentViewEntity()==this);	}

	float					g_Armor				()	{ return fArmor;	}
	float					g_Health			()	{ return fHealth;	}
	float					g_MaxHealth			()	{ return m_fMaxHealthValue;	}
	float					g_Accuracy			()	{ return fAccuracy;	}
	BOOL					g_Alive				()	{ return g_Health()>0; }
	virtual BOOL			g_State				(SEntityState& state)	{return FALSE;}

	int						g_Team				()	{ return id_Team;	}
	int						g_Squad				()	{ return id_Squad;	}
	int						g_Group				()	{ return id_Group;	}

	// misc
	virtual CWeaponList*	GetItemList			()	{ return 0;			}

	// Health calculations
	virtual	void			Hit					(float P, Fvector &dir,			CObject* who);
	virtual void			HitSignal			(float P, Fvector &local_dir,	CObject* who)		= 0;
	virtual void			HitImpulse			(float P, Fvector &vWorldDir, 	Fvector& vLocalDir)	= 0;
	virtual void			Die					()													= 0;

	// Fire control
	virtual void			g_fireParams		(Fvector& P, Fvector& D)			= 0;
	virtual void			g_fireStart			( )					{;}
	virtual void			g_fireEnd			( )					{;}

	// Events
	virtual void			OnEvent				( NET_Packet& P, u16 type		);

	virtual BOOL			IsVisibleForAI		()	{return g_Alive();	}
	virtual BOOL			IsVisibleForHUD		()	{return g_Alive();	}
};

class CEntityAlive			: public CEntity
{
private:
	typedef	CEntity			inherited;			
public:
	// movement
	CMovementControl		Movement;
	EVENT					m_tpEventSay;
	bool					m_bMobility;
public:
	// General
	CEntityAlive			();
	virtual ~CEntityAlive	();

	// Core events
	virtual void			Load					(LPCSTR section);
	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual void			HitImpulse				(float amount, Fvector& vWorldDir, Fvector& vLocalDir);

	virtual void			g_WeaponBones			(int& L, int& R)										= 0;

	// Visibility related
	virtual void			GetVisible				(objVisible& R)	{};
	virtual	float			ffGetFov				()				= 0;	
	virtual	float			ffGetRange				()				= 0;	
};

#endif // !defined(AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_)
