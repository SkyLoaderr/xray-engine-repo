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

	enum EPathType {
		ePathTypeStraight = 0,
		ePathTypeDodge,
		ePathTypeCriteria,
		ePathTypeStraightDodge,
		ePathTypeDodgeCriteria,
	};

	enum EObjectAction {
		eObjectActionIdle = u32(0),
		eObjectActionPrimaryFire,
		eObjectActionSecondaryFire,
		eObjectActionReload,
		eObjectActionActivate,
		eObjectActionDeactivate,
		eObjectActionUse,
		eObjectActionTake,
		eObjectActionDrop,
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
};
