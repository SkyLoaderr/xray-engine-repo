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

//
class CEntity : public CGameObject
{
private:
	typedef	CGameObject	inherited;			
protected:
	// health & shield
	float				fHealth,	fMAX_Health;
	float				fArmor,		fMAX_Armor;
	float				fAccuracy;
protected:	
	// EVENT: health lost 
	EVENT				eHealthLost_Begin;
	EVENT				eHealthLost_End;
	BOOL				eHealthLost_Enabled;
	float				eHealthLost_speed;
	float				eHealthLost_granularity;
	float				eHealthLost_cumulative;
public:
	// Team params
	int					id_Team;
	int					id_Squad;
	int					id_Group;

	// Frags
	int					iFrags;
	
	// Utilities
	static IC bool		u_lerp_angle	(float& c, float t, float s, float dt)
	{
		float diff	= t - c;
		if (diff>0) {
			if (diff>PI)	
				diff	-= PI_MUL_2;
		} else {
			if (diff<-PI)	
				diff	+= PI_MUL_2;
		}
		float diff_a	= fabsf(diff);
		
		if (diff_a<EPS_S)	
			return true;
		
		float mot		= s*dt;
		if (mot>diff_a) mot=diff_a;
		c				+= (diff/diff_a)*mot;
	
		if (c<0)				
			c+=PI_MUL_2;
		else if (c>PI_MUL_2)	
			c-=PI_MUL_2;
		
		return false;
	}
	static IC float		u_lerp_angle	(float A, float B, float f)
	{
		float diff		= B - A;
		if (diff>PI)		diff	-= PI_MUL_2;
		else if (diff<-PI)	diff	+= PI_MUL_2;
		
		return			A + diff*f;
	}

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
	virtual BOOL			net_Spawn			(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags);
	virtual void			net_Destroy			();
	virtual void			Update				(DWORD dt);	
	virtual void			OnVisible			();

	bool					IsFocused			()	{ return (pCreator->CurrentEntity()==this);		}
	bool					IsMyCamera			()	{ return (pCreator->CurrentViewEntity()==this);	}

	float					g_Armor				()	{ return fArmor;	}
	float					g_Health			()	{ return fHealth;	}
	float					g_Accuracy			()	{ return fAccuracy;	}
	virtual BOOL			g_State				(SEntityState& state)	{return FALSE;}

	int						g_Team				()	{ return id_Team;	}
	int						g_Squad				()	{ return id_Squad;	}
	int						g_Group				()	{ return id_Group;	}

	// Health calculations
	virtual	BOOL			Hit					(float P, Fvector &dir,			CObject* who);		// TRUE if died
	virtual void			HitSignal			(float P, Fvector &local_dir,	CObject* who)		= 0;
	virtual void			HitImpulse			(float P, Fvector &vWorldDir, 	Fvector& vLocalDir)	= 0;
	virtual void			Die					()													= 0;

	// Fire control
	virtual void			g_fireParams		(Fvector& P, Fvector& D)			= 0;
	virtual void			g_fireStart			( )					{;}
	virtual void			g_fireEnd			( )					{;}

	// Events
	virtual void			OnEvent				( EVENT E, DWORD P1, DWORD P2	);
	virtual void			OnEvent				( NET_Packet& P, u16 type		);

	virtual BOOL			IsVisibleForAI		()	{return TRUE;}
	virtual BOOL			IsVisibleForHUD		()	{return TRUE;}
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
	virtual BOOL			net_Spawn				(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags);
	virtual void			HitImpulse				(float amount, Fvector& vWorldDir, Fvector& vLocalDir);

	virtual void			g_WeaponBones			(int& L, int& R)										= 0;

	// Visibility related
	virtual void			GetVisible				(objVisible& R)	{};
	virtual	float			ffGetFov				()				= 0;	
	virtual	float			ffGetRange				()				= 0;	
};

#endif // !defined(AFX_ENTITY_H__A2C7300B_20F0_4521_90D3_E883BEF837FE__INCLUDED_)
