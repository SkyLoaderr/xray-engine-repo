////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monster_space.h
//	Created 	: 06.10.2003
//  Modified 	: 06.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Monster types and structures
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace MonsterSpace {
	enum EBodyState {
		eBodyStateCrouch = 0,
		eBodyStateStand,
		eBodyStateStandDamaged,
		eBodyStateLie,
	};

	enum EMovementType {
		eMovementTypeWalk = 0,
		eMovementTypeRun,
		eMovementTypeStand,
	};

	enum EMovementDirection {
		eMovementDirectionForward = 0,
		eMovementDirectionBack,
		eMovementDirectionLeft,
		eMovementDirectionRight,
	};

	enum ELookType {
		eLookTypePathDirection = 0,
		eLookTypeSearch,
		eLookTypeDanger,
		eLookTypePoint,
		eLookTypeFirePoint,
		eLookTypeLookOver,
		eLookTypeLookFireOver,
		eLookTypeDirection,
		eLookTypeCurrentDirection
	};

	enum EDirectionType {
		eDirectionTypeForward = 0,
		eDirectionTypeForwardDodge,
		eDirectionTypeForwardCover,
		eDirectionTypeBack,
		eDirectionTypeBackDodge,
		eDirectionTypeBackCover,
	};

	enum EPathState {
		ePathStateSearchNode = 0,
		ePathStateBuildNodePath,
		ePathStateBuildTravelLine,
		ePathStateDodgeTravelLine,
	};

//	enum EPathType {
//		ePathTypeStraight = 0,
//		ePathTypeDodge,
//		ePathTypeCriteria,
//		ePathTypeStraightDodge,
//		ePathTypeDodgeCriteria,
//	};

	enum EObjectAction {
		eObjectActionIdle = u32(0),
		eObjectActionShow,		
		eObjectActionHide,		
		eObjectActionTake,		
		eObjectActionDrop,		
		eObjectActionStrap,		
		eObjectActionAim1,		
		eObjectActionAim2,		
		eObjectActionReload1,	
		eObjectActionReload2,	
		eObjectActionFire1,		
		eObjectActionFire2,		
		eObjectActionSwitch1,	
		eObjectActionSwitch2,	
		eObjectActionActivate,
		eObjectActionDeactivate,
		eObjectActionMisfire1,
		eObjectActionEmpty1,
		eObjectActionUse,
		eObjectActionTurnOn,
		eObjectActionTurnOff,
		eObjectActionNoItems = eObjectActionIdle | u16(-1),
		eObjectActionDummy = u32(-1)
	};

	enum EMentalState {
		eMentalStateDanger = u32(0),
		eMentalStateFree,
		eMentalStatePanic,
		eMentalStateAsleep,
		eMentalStateZombied,
		eMentalStateDummy = u32(-1),
	};

	enum EActState {
		eActStandIdle = u32(0), // movement parameter (body state)
		eActSitIdle,			// movement parameter (body state)
		eActLieIdle,			// movement parameter (body state)
		eActWalkFwd,			// movement parameter
		eActWalkBkwd,			// movement parameter
		eActRun,				// movement parameter
		eActEat,				// object parameter
		eActSleep,				// animation parameter (a-la mental state)
		eActRest,				// animation parameter (a-la mental state)
		eActDrag,				// object parameter
		eActAttack,				// object parameter
		eActSteal,				// animation parameter (a-la mental state)
		eActLookAround,			// watch parameter
		eActJump,				// movement parameter
		eActTurn,
		eActNone	 = u32(-1),	
	};
	
	enum EActTypeEx {
		eAT_Default				= u32(0),
		eAT_ForceMovementType,	
		eAT_None				= u32(-1),
	};

	enum EMonsterSounds {
		eMonsterSoundIdle		= u32(0),
		eMonsterSoundEat,
		eMonsterSoundAttack,
		eMonsterSoundAttackHit,
		eMonsterSoundTakeDamage,
		eMonsterSoundDie,
		eMonsterSoundThreaten,
		eMonsterSoundBkgnd,
		eMonsterSoundLanding,
		eMonsterSoundSteal,
		eMonsterSoundDummy		= u32(-1),
	};

};
