////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.h
//	Created 	: 23.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_RAT__
#define __XRAY_AI_RAT__

#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"
#include "ai_rat_selectors.h"
#include "..\\..\\..\\bodyinstance.h"

typedef struct tagSSubNode {
	Fvector tLeftDown;
	Fvector tRightUp;
	bool	bEmpty;
} SSubNode;

class CAI_Rat : public CCustomMonster  
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};

	enum ERatStates 	{
		aiRatDie = 0,
		aiRatUnderFire,
		aiRatSenseSomething,
		aiRatGoInThisDirection,
		aiRatGoToThisPosition,
		aiRatWaitOnPosition,
		aiRatHoldThisPosition,
		aiRatHoldPositionUnderFire,
		aiRatFreeHunting,
		aiRatFollowMe,
		aiRatAttack,
		aiRatDefend,
		aiRatPursuit,
		aiRatRetreat,
		aiRatCover,
	};
	
	typedef	CCustomMonster inherited;

	protected:
		// media
		sound3D			sndHit[SND_HIT_COUNT];
		sound3D			sndDie[SND_DIE_COUNT];
		// ai
		ERatStates		eCurrentState;
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
		
		CMotionDef* 	m_tpaAttackAnimations[3];

		CBlend*			m_tpCurrentBlend;

		CEntity*		m_tpEnemyBeingAttacked;
		float			m_fHitPower;
		DWORD			m_dwHitInterval;

		bool			m_bStartAttack;
		vector<SSubNode> tpSubNodes;
		DWORD			m_dwLastUpdate;
		CBoneInstance*	m_tpBoneSpine;

		// finite state machine
		stack<ERatStates>	tStateStack;
			void Die();
			void UnderFire();
			void SenseSomething();
			void GoInThisDirection();
			void GoToThisPosition();
			void WaitOnPosition();
			void HoldThisPosition();
			void HoldPositionUnderFire();
			void FreeHunting();
			void FollowMe();
			void Attack();
			void Defend();
			void Pursuit();
			void Retreat();
			void Cover();
			bool bfCheckPath(AI::Path &Path);
		IC  void SetDirectionLook();
			int	 ifDivideNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes);
			int	 ifDivideNearestNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes);
		IC	bool bfInsideSubNode(const Fvector &tCenter, const SSubNode &tpSubNode);
		IC	bool bfInsideSubNode(const Fvector &tCenter, const float fRadius, const SSubNode &tpSubNode);
		IC	bool bfNeighbourNode(const SSubNode &tCurrentSubNode, const SSubNode &tMySubNode);
		IC float ffComputeCost(Fvector tLeaderPosition,SSubNode &tCurrentNeighbour);
		IC	bool bfInsideNode(const Fvector &tCenter, const NodeCompressed *tpNode);
		IC float ffGetY(NodeCompressed &tNode, float X, float Z);
	
		CRatSelectorAttack		SelectorAttack;
		CRatSelectorFreeHunting SelectorFreeHunting;
		CRatSelectorFollow		SelectorFollow;
		CRatSelectorPursuit		SelectorPursuit;
		CRatSelectorUnderFire	SelectorUnderFire;

	public:
					   CAI_Rat();
		virtual		  ~CAI_Rat();
		virtual void  Update(DWORD DT);
		virtual void  HitSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  SenseSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  Death();
		virtual void  Load( CInifile* ini, const char* section );
		virtual void  Think();
		virtual float EnemyHeuristics(CEntity* E);
		virtual void  SelectEnemy(SEnemySelected& S);
		void		  FollowLeader(Fvector &tLeaderPosition);

		virtual void  net_Export				(NET_Packet* P);				// export to server
		virtual void  net_Import				(NET_Packet* P);				// import from server
		virtual void  SelectAnimation			( const Fvector& _view, const Fvector& _move, float speed );
		virtual void  Exec_Action				( float dt );
		virtual void  Exec_Movement				( float dt );

		bool			m_bAttackStart;
		DWORD			m_dwAttackStartTime;
};
		
#endif