#pragma once

#include "entity.h"
#include "entitycondition.h"

class CEntityAlive			: public CEntity, 
							  virtual public CEntityCondition
{
private:
	typedef	CEntity			inherited;			
public:
	EVENT					m_tpEventSay;
	bool					m_bMobility;
	float					m_fAccuracy;
	float					m_fIntelligence;
	//m_PhysicMovementControl
	CPHMovementControl		m_PhysicMovementControl;
public:
	// General
							CEntityAlive			();
	virtual					~CEntityAlive			();

	// Core events
			void			Init					();
	virtual void			Load					(LPCSTR section);
	virtual void			reinit					();
	virtual void			reload					(LPCSTR section);

	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual void			net_Destroy				();

	virtual void			shedule_Update			(u32 dt);

	virtual void			HitImpulse				(float amount, Fvector& vWorldDir, Fvector& vLocalDir);
	virtual	void			Hit						(float P, Fvector &dir,			CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type = eHitTypeWound);
	virtual void			g_WeaponBones			(int &L, int &R1, int &R2)										= 0;

//	virtual float&			GetEntityHealth()		{ return m_fHealth; }
	
	virtual float GetfHealth() const { return m_fHealth*100.f; }
	virtual float SetfHealth(float value) {m_fHealth = value/100.f; return value;}
	PropertyGP(GetfHealth,SetfHealth) float fEntityHealth;

	virtual float			g_Health			()	const { return GetHealth()*100.f; }
	virtual float			g_MaxHealth			()	const { return GetMaxHealth()*100.f; }

	virtual float			CalcCondition		(float hit);


	// Visibility related
	virtual	float			ffGetFov				()	const			= 0;	
	virtual	float			ffGetRange				()	const			= 0;	
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
///////////////////////////////////////////////////////////////////////
	virtual u16				PHGetSyncItemsNumber();
	virtual CPHSynchronize*	PHGetSyncItem		(u16 item);
	virtual void			PHUnFreeze			();
	virtual void			PHFreeze			();
///////////////////////////////////////////////////////////////////////
};

