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

		eWorldPropertyHumanToDialog,
		eWorldPropertyReadyToDialog,
		eWorldPropertyHelloCompleted,

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
		eWorldPropertyDanger,
		
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
		eWorldPropertyEnemyCanBeSeen,
		eWorldPropertyEnemyDetoured,
		eWorldPropertyPureEnemy,

		eWorldPropertyDangerRicochet,
		eWorldPropertyDangerShot,
		eWorldPropertyDangerHit,
		eWorldPropertyDangerDeath,
		eWorldPropertyDangerAttack,
		eWorldPropertyDangerCorpse,

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
		
// alife
		eWorldOperatorGatherItems,
		eWorldOperatorALifeEmulation,

// alife : tasks
		eWorldOperatorSolveZonePuzzle,
		eWorldOperatorReachTaskLocation,
		eWorldOperatorAccomplishTask,
		eWorldOperatorReachCustomerLocation,
		eWorldOperatorCommunicateWithCustomer,

// alife : dialogs
		eWorldOperatorGetReadyToDialog,
		eWorldOperatorHello,
		eWorldOperatorDialog,
		
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
		eWorldOperatorGetDistance,
		eWorldOperatorDetourEnemy,
		eWorldOperatorSearchEnemy,

// guard
		eWorldOperatorDangerRicochet,
		eWorldOperatorDangerShot,
		eWorldOperatorDangerHit,
		eWorldOperatorDangerDeath,
		eWorldOperatorDangerAttack,
		eWorldOperatorDangerCorpse,

// global
		eWorldOperatorPostCombatWait,
		eWorldOperatorSquadAction,
		eWorldOperatorDeathPlanner,
		eWorldOperatorALifePlanner,
		eWorldOperatorALifeDialogPlanner,
		eWorldOperatorCombatPlanner,
		eWorldOperatorAnomalyPlanner,
		eWorldOperatorGuardPlanner,

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