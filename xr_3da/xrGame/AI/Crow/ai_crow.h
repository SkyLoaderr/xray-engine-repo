////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.h
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_CROW__
#define __XRAY_AI_CROW__

#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"
#include "ai_crow_selectors.h"
#include "..\\..\\..\\bodyinstance.h"

class CAI_Crow : public CCustomMonster  
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};

	enum ECrowStates 	{
		aiCrowDie = 0,
		aiCrowFreeHunting,
	};
	
	typedef	CCustomMonster inherited;

	protected:
		// media
		sound3D			sndHit[SND_HIT_COUNT];
		sound3D			sndDie[SND_DIE_COUNT];
		// ai
		ECrowStates		eCurrentState;
		bool			bStopThinking;
		// hit data
		DWORD			dwHitTime;
		Fvector			tHitDir;
		// sense data
		DWORD			dwSenseTime;
		Fvector			tSenseDir;
		// saved enemy
		CMotionDef*		m_fly;
		CMotionDef*		m_death_idle;

		CBlend*			m_tpCurrentBlend;

		// finite state machine
		stack<ECrowStates>	tStateStack;
		void Die();
		void FreeHunting();
		void SetDirectionLook(NodeCompressed *tNode);
	
		CCrowSelectorFreeHunting SelectorFreeHunting;

	public:
					   CAI_Crow();
		virtual		  ~CAI_Crow();
		virtual void  Update(DWORD DT);
		virtual void  HitSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  SenseSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  Death();
		virtual void  Load( CInifile* ini, const char* section );
		virtual void  Think();
		
		virtual void  net_Export				(NET_Packet* P);				// export to server
		virtual void  net_Import				(NET_Packet* P);				// import from server
		virtual void  SelectAnimation			( const Fvector& _view, const Fvector& _move, float speed );
};
		
#endif