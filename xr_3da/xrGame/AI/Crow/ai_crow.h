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
		sound			sndHit[SND_HIT_COUNT];
		sound			sndDie[SND_DIE_COUNT];
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

		float			m_fActorChange;
		float			m_fOptRadius;
		float			m_fRadiusDeviation;
		float			m_fXDeviation;
		float			m_fYDeviation;
		float			m_fZDeviation;
		float			m_fOptHeight;
		DWORD			m_dwChangeInterval;
		
		Fvector			m_tSavedActorPosition;
		DWORD			m_dwLastTrajectoryChange;
		float			m_fRadius;
		Fvector			m_tCenter;

		// finite state machine
		stack<ECrowStates>	tStateStack;
		void Die();
		void FreeHunting();
		void SetDirectionLook();
		bool bfChangeFlyTrajectory(Fvector &tActorPosition, DWORD dwTime);

		// parameters block
		Fvector			m_vGoalDir;
		Fvector			m_vCurrentDir;
		Fvector			m_vHPB;
		float			m_fDHeading;
	public:
					   CAI_Crow();
		virtual		  ~CAI_Crow();
		virtual BOOL  ShadowGenerate()			{ return FALSE;	}
		virtual BOOL  ShadowReceive	()			{ return FALSE;	}
		virtual void  Update(DWORD DT);
		virtual void  HitSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  SenseSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  Death();
		virtual void  Load( LPCSTR section );
		virtual void  Think();
		virtual void  SelectAnimation( const Fvector& _view, const Fvector& _move, float speed );
		virtual void  net_Export(NET_Packet* P);
		virtual void  net_Import(NET_Packet* P);
};
		
#endif