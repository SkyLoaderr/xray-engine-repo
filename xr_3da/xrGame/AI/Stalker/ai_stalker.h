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
#include "ai_stalker_selectors.h"

#define MAX_STATE_LIST_SIZE				256
#define MAX_TIME_RANGE_SEARCH			5000.f

class CAI_Stalker : public CCustomMonster, public CStalkerAnimations {
private:
	typedef CCustomMonster inherited;
	enum EStalkerStates {
		eStalkerStateDie = 0,
		eStalkerStateTurnOver,
		eStalkerStateWaitForAnimation,
		eStalkerStateWaitForTime,
		eStalkerStateRecharge,
		eStalkerStateDrop,
		eStalkerStateLookingOver,
		eStalkerStateSearching,
	};

	typedef struct tagSStalkerStates {
		EStalkerStates		eState;
		u32					dwTime;
	} SStalkerStates;

	stack<EStalkerStates>	m_tStateStack;
	svector<SStalkerStates,MAX_STATE_LIST_SIZE>	m_tStateList;
	u32						m_dwLastUpdate;
	u32						m_dwCurrentUpdate;
	u32						m_dwUpdateCount;
	bool					m_bStopThinking;
	EStalkerStates			m_eCurrentState;
	EStalkerStates			m_ePreviousState;
	bool					m_bStateChanged;
	CStalkerSelectorFreeHunting	m_tSelectorFreeHunting;


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

			void			TurnOver();
			void			WaitForAnimation();
			void			WaitForTime();
			void			Recharge();
			void			Drop();
			void			LookingOver();
			void			Searching();
			///
			void			DropItem();
			void			vfBuildPathToDestinationPoint(CAISelectorBase *S);
			void			vfSearchForBetterPosition(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
			void			vfInitSelector			(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
public:
	typedef CCustomMonster inherited;
							CAI_Stalker				();
	virtual					~CAI_Stalker			();
	virtual objQualifier*	GetQualifier			();
	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual void			Death					();
	virtual void			OnDeviceCreate			();
	virtual void			Load					(LPCSTR	section );				
	virtual void			HitSignal				(float P,	Fvector& vLocalDir, CObject* who, s16 element);
	virtual void			g_WeaponBones			(int& L,	int& R	);
	virtual void			Think					();
	virtual void			Die						();
	virtual void			g_fireParams			(Fvector& P, Fvector& D);
	virtual void			net_Export				(NET_Packet& P);
	virtual void			net_Import				(NET_Packet& P);
	virtual void			SelectAnimation			(const Fvector& _view, const Fvector& _move, float speed );
	virtual void			OnEvent					(NET_Packet& P, u16 type);
	virtual void			feel_touch_new			(CObject* O);
	virtual void			OnVisible				();
	virtual void			Exec_Movement			(float dt);
};
