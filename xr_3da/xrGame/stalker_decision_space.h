////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_decision_space.h
//	Created 	: 30.03.2004
//  Modified 	: 30.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker decision space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace StalkerDecisionSpace {
	enum EMotivations {
		eMotivationGlobal			= u32(0),
		eMotivationAlive,
		eMotivationDead,
		eMotivationSolveZonePuzzle,
		eMotivationSquadCommand,
		eMotivationSquadGoal,
		eMotivationSquadAction,
		eMotivationScript,
		eMotivationDummy			= u32(-1)
	};

	enum EWorldProperties {
		eWorldPropertyAlive			= u32(0),
		eWorldPropertyDead,
		eWorldPropertyAlreadyDead,
		
		eWorldPropertyALife,
		eWorldPropertyPuzzleSolved,
		eWorldPropertyReachedTaskLocation,
		eWorldPropertyTaskCompleted,
		eWorldPropertyReachedCustomerLocation,
		eWorldPropertyCustomerSatisfied,

		eWorldPropertyNotEnoughFood,
		eWorldPropertyCanBuyFood,

		eWorldPropertyNotEnoughMedikits,
		eWorldPropertyCanBuyMedikit,

		eWorldPropertyNoOrBadWeapon,
		eWorldPropertyCanBuyWeapon,

		eWorldPropertyNotEnoughAmmo,
		eWorldPropertyCanBuyAmmo,

		eWorldPropertyItems,
		eWorldPropertyEnemy,
		
		eWorldPropertyItemToKill,
		eWorldPropertyFoundItemToKill,
		eWorldPropertyItemCanKill,
		eWorldPropertyFoundAmmo,
		eWorldPropertyReadyToKill,
		eWorldPropertySeeEnemy,
		eWorldPropertyPanic,
		eWorldPropertyInCover,
		eWorldPropertyLookedOut,
		eWorldPropertyPositionHolded,
		eWorldPropertyEnemyDetoured,

		eWorldPropertySquadAction,
		eWorldPropertySquadGoal,

		eWorldPropertyAnomaly,
		eWorldPropertyInsideAnomaly,

		eWorldPropertyScript,
		eWorldPropertyDummy			= u32(-1),
	};

	enum EWorldOperators {
// death
		eWorldOperatorAlreadyDead	= u32(0),
		eWorldOperatorDead,
		
// alfie
		eWorldOperatorGatherItems,

// alife : tasks
		eWorldOperatorFreeNoALife,
		eWorldOperatorFreeALife,
		eWorldOperatorReachTaskLocation,
		eWorldOperatorAccomplishTask,
		eWorldOperatorReachCustomerLocation,
		eWorldOperatorCommunicateWithCustomer,

// anomaly
		eWorldOperatorGetOutOfAnomaly,
		eWorldOperatorDetectAnomaly,

// combat
		eWorldOperatorGetItemToKill,
		eWorldOperatorFindItemToKill,
		eWorldOperatorMakeItemKilling,
		eWorldOperatorFindAmmo,

		eWorldOperatorAimEnemy,
		eWorldOperatorGetReadyToKill,
		eWorldOperatorKillEnemy,
		eWorldOperatorRetreatFromEnemy,
		eWorldOperatorTakeCover,
		eWorldOperatorLookOut,
		eWorldOperatorHoldPosition,
		eWorldOperatorDetourEnemy,
		eWorldOperatorSearchEnemy,

// global
		eWorldOperatorSquadAction,
		eWorldOperatorDeathPlanner,
		eWorldOperatorALifePlanner,
		eWorldOperatorCombatPlanner,
		eWorldOperatorAnomalyPlanner,

// script
		eWorldOperatorScript,
		eWorldOperatorDummy			= u32(-1),
	};

	enum ESightActionType {
		eSightActionTypeWatchItem	= u32(0),
		eSightActionTypeWatchEnemy,
		eSightActionTypeDummy		= u32(-1),
	};
};