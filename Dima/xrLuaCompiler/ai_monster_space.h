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
		eObjectActionSwitch1,
		eObjectActionSwitch2,
		eObjectActionReload1,
		eObjectActionReload2,
		eObjectActionAim1,
		eObjectActionAim2,
		eObjectActionFire1,
		eObjectActionFire2,
		eObjectActionIdle,
		eObjectActionStrapped,
		eObjectActionDrop,
		eObjectActionAimReady1,
		eObjectActionAimReady2,
		// for scripts only
		eObjectActionActivate,
		eObjectActionDeactivate,
		eObjectActionUse,
		eObjectActionTurnOn,
		eObjectActionTurnOff,
		// for old object handler only
		eObjectActionShow,
		eObjectActionHide,
		eObjectActionTake,
		eObjectActionMisfire1,
		eObjectActionEmpty1,
		eObjectActionNoItems		= eObjectActionIdle | u16(-1),
		// 
		eObjectActionDummy			= u32(-1),
	};

	enum EMentalState {
		eMentalStateDanger = u32(0),
		eMentalStateFree,
		eMentalStatePanic,
		eMentalStateAsleep,
		eMentalStateZombied,
		eMentalStateDummy = u32(-1),
	};

	enum EScriptMonsterMoveAction {
		eMA_WalkFwd,
		eMA_WalkBkwd,
		eMA_Run,
		eMA_Drag,
		eMA_Jump,
		eMA_Steal
	};
	

	enum EScriptMonsterSpeedParam {
		eSP_Default				= u32(0),
		eSP_ForceSpeed,	
		eSP_None				= u32(-1),
	};

	enum EScriptMonsterAnimAction {
		eAA_StandIdle, 
		eAA_SitIdle,			
		eAA_LieIdle,			
		eAA_Eat,				
		eAA_Sleep,				
		eAA_Rest,				
		eAA_Attack,				
		eAA_LookAround,
		eAA_Turn,
		eAA_NoAction			= u32(-1)
	};

	enum EScriptMonsterGlobalAction {
		eGA_Rest				= u32(0),
		eGA_Eat,
		eGA_Attack,
		eGA_Panic,
		eGA_None				= u32(-1)
	};

	enum EScriptSoundAnim {
		eAnimSoundCustom		= u32(0),
		eAnimSoundDefault,
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
		eMonsterSoundPanic,
		eMonsterSoundGrowling,
		eMonsterSoundDummy		= u32(-1),
	};

	struct SBoneRotation {
		SRotation		current;
		SRotation		target;
		float			speed;
	};
};
