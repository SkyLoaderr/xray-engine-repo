// Entity.h: interface for the CEntity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_)
#define AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_
#pragma once


#include "gameobject.h"
#include "phmovementcontrol.h"
// refs
class	ENGINE_API CCameraBase;
class	ENGINE_API C3DSound;
class	ENGINE_API CMotionDef;
class	ENGINE_API CKinematics;
class	ENGINE_API CBoneInstance;
class	CWeaponList;
class   CPHMovementControl;
 
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

	bool					IsFocused			()const	{ return ((const CEntity*)g_pGameLevel->CurrentEntity()==this);		}
	bool					IsMyCamera			()const	{ return ((const CEntity*)g_pGameLevel->CurrentViewEntity()==this);	}

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
	virtual	void			Hit					(float P, Fvector &dir,			CObject* who, s16 element,Fvector position_in_object_space, float impulse);
	virtual void			HitSignal			(float P, Fvector &local_dir,	CObject* who, s16 element)		= 0;
	virtual void			HitImpulse			(float P, Fvector &vWorldDir, 	Fvector& vLocalDir)	= 0;
	virtual	float			HitScale			(int element){return 1.f;}
	virtual void			Die					()													= 0;
	
	// Events
	virtual void			OnEvent				( NET_Packet& P, u16 type		);

	virtual BOOL			IsVisibleForHUD		()	{return g_Alive();	}
	virtual void			g_fireParams			(Fvector &, Fvector &){}; 
};


enum ERelationType {
	eRelationTypeFriend = 0,
	eRelationTypeNeutral,
	eRelationTypeEnemy,
};

class CEntityAlive			: public CEntity
{
private:
	typedef	CEntity			inherited;			
public:
	EVENT					m_tpEventSay;
	bool					m_bMobility;
	u32						m_dwDeathTime;
	float					m_fAccuracy;
	float					m_fIntelligence;
	//Movement
	CPHMovementControl		Movement;
public:
	// General
	CEntityAlive			();
	virtual ~CEntityAlive	();

	// Core events
	virtual void			Load					(LPCSTR section);
	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual void			net_Destroy				();
	virtual void			HitImpulse				(float amount, Fvector& vWorldDir, Fvector& vLocalDir);

	virtual void			g_WeaponBones			(int &L, int &R1, int &R2)										= 0;

	// Visibility related
	virtual void			GetVisible				(objVisible& R)	{};
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
		return &Movement;
	}

};

#endif // !defined(AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_)
