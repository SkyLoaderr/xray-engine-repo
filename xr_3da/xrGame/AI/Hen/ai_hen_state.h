////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_state.h
//	Created 	: 05.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : State class for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_STATE__
#define __XRAY_AI_HEN_STATE__

class CCustomMonster;

namespace AI {

	//#define WRITE_LOG

	enum EHenStates 	{
		aiHenDie = 0,
		aiHenUnderFire,
		aiSenseSomething,
		aiHenSenseSomething,
		aiHenGoInThisDirection,
		aiHenGoToThisPosition,
		aiHenWaitOnPosition,
		aiHenHoldThisPosition,
		aiHenHoldPositionUnderFire,
		aiHenFreeHunting,
		aiHenFollowMe,
		aiHenAttack,
		aiHenDefend,
		aiHenPursuit,
		aiHenRetreat,
		aiHenCover,
		aiLAST,
		aiFORCEDWORD = DWORD(-1)
	};
	
	struct EnemySelected
	{
		CEntity*	E;
		BOOL		Visible;
		float		Cost;
	};

	class CHenState
	{
		// hit data
		DWORD	hitTime;
		Fvector	hitDir;
		// sense data
		DWORD	senseTime;
		Fvector	senseDir;

		BOOL	bBuildPathToLostEnemy;
		public:
			CEntity*	EnemySaved;			// жертва
			EHenStates	Type;
			DWORD		svTime_Create;
			CEntity*	victim;				// жертва
			Fvector		PositionPredicted;
			Fvector		savedPosition;
			DWORD		savedTime;
			DWORD		savedNode;
			BOOL		bDirectPathBuilded;
			
			CHenState(EHenStates T) : Type(T)
			{ 
				svTime_Create = Level().timeServer(); 
			};			
			static  CHenState*	Create(DWORD type);
			virtual BOOL		Parse(CCustomMonster* Me) = 0;
			virtual void		Hit(Fvector &dir);
			virtual void		Sense(Fvector &dir);
		IC	virtual float		EnemyHeuristics(CCustomMonster* Me, CEntity* E);
			virtual void		SelectEnemy(EnemySelected& S, CCustomMonster* Me);
	};
};
