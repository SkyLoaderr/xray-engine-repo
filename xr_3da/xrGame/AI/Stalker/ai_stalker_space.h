////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_space.h
//	Created 	: 28.03.2003
//  Modified 	: 28.03.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker types and structures
////////////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_STATE_LIST_SIZE				256
#define MAX_DYNAMIC_OBJECTS 			32
#define MAX_DYNAMIC_SOUNDS  			32
#define MAX_HURT_COUNT					32
#define MAX_SEARCH_COUNT				32

#define MAX_STATE_LIST_SIZE				256
#define MAX_TIME_RANGE_SEARCH			5000.f
#define	MAX_HEAD_TURN_ANGLE				(2.f*PI_DIV_6)

#define R2D(x)							(angle_normalize(x)*180.f/PI)

enum EBodyState {
	eBodyStateCrouch = 0,
	eBodyStateStand,
};

enum EMovementType {
	eMovementTypeWalk = 0,
	eMovementTypeRun,
	eMovementTypeStand,
};

enum EStateType {
	eStateTypeDanger = 0,
	eStateTypeNormal,
	eStateTypePanic,
};

enum EMovementDirection {
	eMovementDirectionForward = 0,
	eMovementDirectionBack,
	eMovementDirectionLeft,
	eMovementDirectionRight,
};

enum ELookType {
	eLookTypeDirection = 0,
	eLookTypeSearch,
	eLookTypeDanger,
	eLookTypePoint,
	eLookTypeFirePoint,
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
};

enum EWeaponState {
	eWeaponStateIdle = 0,
	eWeaponStatePrimaryFire,
	eWeaponStateSecondaryFire,
};

enum EActionState {
	eActionStateRun = 0,
	eActionStateStand,
};