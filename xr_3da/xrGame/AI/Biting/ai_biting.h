////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.h
//	Created 	: 21.05.2003
//  Modified 	: 21.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all the monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\..\\CustomMonster.h"
#include "..\\ai_monsters_misc.h"
#include "ai_biting_anim.h"

class CAI_Biting : public CCustomMonster, public CBitingAnimations
{
public:
	typedef	CCustomMonster inherited;

							CAI_Biting		();
	virtual					~CAI_Biting		();
	virtual	BOOL			ShadowReceive	()			{ return FALSE;	}
	virtual void			Die				();
	virtual void			HitSignal		(float amount, Fvector& vLocalDir, CObject* who, s16 element);
	virtual void			SelectAnimation	(const Fvector& _view, const Fvector& _move, float speed );

	virtual void			Load			(LPCSTR section);

	// network routines
	virtual BOOL			net_Spawn		(LPVOID DC);
	virtual void			net_Destroy		();
	virtual void			net_Export		(NET_Packet& P);
	virtual void			net_Import		(NET_Packet& P);

	virtual void			Think			();


//	virtual float EnemyHeuristics(CEntity* E);
//	virtual void  SelectEnemy(SEnemySelected& S);
//	virtual objQualifier* GetQualifier();
//	virtual	void  feel_sound_new(CObject* who, int type, const Fvector &Position, float power);
//	virtual void  Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse);

private:			
			void			Init			();

// members
public:

private:
	SOUND_VECTOR			m_tpSoundDie;
	SOUND_VECTOR			m_tpSoundHit;

	// LifeSimulation
	_GRAPH_ID				m_tCurGP;
	_GRAPH_ID				m_tNextGP;
	float					m_fGoingSpeed;			

	// Fields
	bool					bPeaceful;
	
	// properties
	u32						m_dwHealth;
	u32						m_dwMoraleValue;
	
	// BEHAVIOUR
	Fvector					m_tHPB;
	float yaw;
};




