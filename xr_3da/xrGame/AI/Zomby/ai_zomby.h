////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zomby.h
//	Created 	: 07.05.2002
//  Modified 	: 07.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Zomby"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_ZOMBY__
#define __XRAY_AI_ZOMBY__

#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"
#include "ai_zomby_selectors.h"
#include "..\\..\\..\\bodyinstance.h"

class CAI_Zomby : public CCustomMonster  
{
	typedef struct tagSSubNode {
		Fvector tLeftDown;
		Fvector tRightUp;
		bool	bEmpty;
	} SSubNode;

	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};

	enum EZombyStates 	{
		aiZombyDie = 0,
		aiZombyUnderFire,
		aiZombySenseSomething,
		aiZombyFreeHunting,
		aiZombyAttack
	};
	
	typedef	CCustomMonster inherited;

	protected:
		// media
		sound3D			sndHit[SND_HIT_COUNT];
		sound3D			sndDie[SND_DIE_COUNT];
		// ai
		EZombyStates		eCurrentState;
		bool			bStopThinking;
		// hit data
		DWORD			dwHitTime;
		Fvector			tHitDir;
		// sense data
		DWORD			dwSenseTime;
		Fvector			tSenseDir;
		// saved enemy
		CEntity*		tSavedEnemy;
		Fvector			tSavedEnemyPosition;
		DWORD			dwLostEnemyTime;
		NodeCompressed* tpSavedEnemyNode;
		DWORD			dwSavedEnemyNodeID;
		bool			bBuildPathToLostEnemy;
		
		CMotionDef* 	m_tpaDeathAnimations[3];
		CMotionDef* 	m_tpaAttackAnimations[2];

		bool			m_bAttackStart;
		CBlend*			m_tpCurrentBlend;

		CEntity*		m_tpEnemyBeingAttacked;
		float			m_fHitPower;
		DWORD			m_dwHitInterval;
		DWORD			m_dwAttackStartTime;
		
		vector<SSubNode> tpFreeNeighbourNodes;
		vector<SSubNode> tpSubNodes;

		// finite state machine
		stack<EZombyStates>	tStateStack;
		void Die();
		void UnderFire();
		void SenseSomething();
		void FreeHunting();
		void Attack();
		void Retreat();
		IC bool bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint);
		bool bfCheckPath(AI::Path &Path);
			int	 ifDivideNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes);
			int	 ifDivideNearestNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes);
		IC	bool bfInsideSubNode(const Fvector &tCenter, const SSubNode &tpSubNode);
		IC	bool bfInsideSubNode(const Fvector &tCenter, const float fRadius, const SSubNode &tpSubNode);
		IC	bool bfNeighbourNode(const SSubNode &tCurrentSubNode, const SSubNode &tMySubNode);
		IC float ffComputeCost(Fvector tLeaderPosition,SSubNode &tCurrentNeighbour);
		IC	bool bfInsideNode(const Fvector &tCenter, const NodeCompressed *tpNode);
		IC float ffGetY(NodeCompressed &tNode, float X, float Z);
			void FollowLeader(CSquad &Squad, CEntity* Leader);
	
		CZombySelectorAttack		SelectorAttack;
		CZombySelectorFreeHunting SelectorFreeHunting;
		CZombySelectorUnderFire	SelectorUnderFire;

		void SetDirectionLook(NodeCompressed *tNode);
		void SetLessCoverLook(NodeCompressed *tNode);
	public:
					   CAI_Zomby();
		virtual		  ~CAI_Zomby();
		virtual void  Update(DWORD DT);
		virtual void  HitSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  SenseSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  Death();
		virtual void  Load( CInifile* ini, const char* section );
		virtual void  Think();
		virtual float EnemyHeuristics(CEntity* E);
		virtual void  SelectEnemy(SEnemySelected& S);
		
		virtual void  net_Export				(NET_Packet* P);				// export to server
		virtual void  net_Import				(NET_Packet* P);				// import from server
		virtual void  SelectAnimation			( const Fvector& _view, const Fvector& _move, float speed );
		virtual void  Exec_Action				( float dt );
};
		
#endif