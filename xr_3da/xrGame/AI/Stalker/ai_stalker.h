////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\..\\CustomMonster.h"
#include "..\\ai_monsters.h"
#include "ai_stalker_animations.h"

#define MAX_STATE_LIST_SIZE 256

class CAI_Stalker : public CCustomMonster, public CStalkerAnimations {
private:
	typedef CCustomMonster inherited;
	enum EStalkerStates {
		aiStalkerDie = 0,
		aiStalkerTurnOver,
		aiStalkerWaitForAnimation,
		aiStalkerWaitForTime,
		aiStalkerRecharge,
		aiStalkerLookingOver,
	};

	typedef struct tagSStalkerStates {
		EStalkerStates		eState;
		u32					dwTime;
	} SStalkerStates;

	DEFINE_SVECTOR			(SStalkerStates, MAX_STATE_LIST_SIZE, STATE_VECTOR, STATE_IT)

	STATE_VECTOR			m_tStateList;
	u32						m_dwUpdateCount;
	u32						m_dwCurrentUpdate;

	IC		void			vfAddStateToList		(EStalkerStates eState)
	{
		if ((m_tStateList.size()) && (m_tStateList[m_tStateList.size() - 1].eState == eState)) {
			m_tStateList[m_tStateList.size() - 1].dwTime = m_dwCurrentUpdate;
			return;
		}
		if (m_tStateList.size() >= MAX_STATE_LIST_SIZE)
			m_tStateList.erase(u32(0));
		SStalkerStates tStalkerStates;
		tStalkerStates.dwTime = m_dwCurrentUpdate;
		tStalkerStates.eState = eState;
		m_tStateList.push_back(tStalkerStates);
	};
			void			vfLoadAnimations		();

public:
	typedef CCustomMonster inherited;
							CAI_Stalker				();
	virtual					~CAI_Stalker			();
	virtual objQualifier*	GetQualifier			();
	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual void			Death					();
	virtual void			OnDeviceCreate			();
	virtual void			Load					(LPCSTR	section );				
//	virtual void			HitSignal				(float P,	Fvector& vLocalDir, CObject* who);
//	virtual void			g_WeaponBones			(int& L,	int& R	);
//	virtual void			Think					();
//
//	// Fire control
//	virtual void			g_fireParams			(Fvector& P, Fvector& D);
//
//	// Network
//	virtual void			net_Export				(NET_Packet& P);				// export to server
//	virtual void			net_Import				(NET_Packet& P);				// import from server
//
//	virtual void			SelectAnimation			(const Fvector& _view, const Fvector& _move, float speed );
};
