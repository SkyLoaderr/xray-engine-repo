////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.h
//	Created 	: 05.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN__
#define __XRAY_AI_HEN__

#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"

namespace AI {
	class CAI_Hen : public CCustomMonster  
	{
		enum ESoundCcount {
			SND_HIT_COUNT=8,
			SND_DIE_COUNT=4
		};

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
		
		typedef	CCustomMonster inherited;

		protected:
			// media
			sound3D				sndHit[SND_HIT_COUNT];
			sound3D				sndDie[SND_DIE_COUNT];
			EHenStates			eCurrentState;

		public:
			// State machine
			virtual void		Update		(DWORD DT);
			virtual void		HitSignal	(int amount, Fvector& vLocalDir, CEntity* who);
			virtual void		SenseSignal	(int amount, Fvector& vLocalDir, CEntity* who);
			virtual void		Die			();
			virtual void		Load		( CInifile* ini, const char* section );
			virtual void		Think		();

			CAI_Hen			();
			virtual ~CAI_Hen	();
	};
	
	class CHenSelectorBase : public AI::NodeEstimator 
	{
		public:
			LPSTR		Name;
			float		fTravelWeight,
						fLightWeight,
						fCoverFromLeaderWeight,
						fCoverFromMembersWeight,
						fCoverFromEnemyWeight,
						fOptimalLeaderDistance,
						fLeaderDistanceWeight,
						fOptimalMemberDistance,
						fMemberDistanceWeight,
						fOptimalEnemyDistance,
						fEnemyDistanceWeight,
						fMaxHeightDistance,
						fMaxHeightDistanceWeight,
						fMemberViewDeviationWeight,
						fEnemyViewDeviationWeight,
						fTotalViewVectorWeight,
						fLaziness;

			CEntity		*tMe;
			EntityVec	taMembers;
			CEntity		*tEnemy;

			virtual	void Load(CInifile* ini, const char* section);
	};
};

const DWORD _FB_hit_RelevantTime	= 10;
const DWORD _FB_sense_RelevantTime	= 10;
const float _FB_look_speed			= PI;
const float _FB_invisible_hscale	= 2.f;

#endif