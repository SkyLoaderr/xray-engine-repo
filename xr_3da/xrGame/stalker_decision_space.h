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
		eMotivationScript,
		eMotivationDummy			= u32(-1)
	};

	enum EWorldProperties {
		eWorldPropertyAlive			= u32(0),
		eWorldPropertyDead,
		eWorldPropertyAlreadyDead,
		
		eWorldPropertyALife,
		eWorldPropertyPuzzleSolved,

		eWorldPropertyItems,
		eWorldPropertyEnemy,
		
		eWorldPropertySeeEnemy,
		eWorldPropertyItemToKill,
		eWorldPropertyFoundItemToKill,
		eWorldPropertyItemCanKill,
		eWorldPropertyFoundAmmo,
		eWorldPropertyReadyToKill,
		eWorldPropertyKillDistance,
		eWorldPropertyEnemyAimed,
		eWorldPropertyEnemyLost,

		eWorldPropertyScript,
		eWorldPropertyDummy			= u32(-1),
	};

	enum EWorldOperators {
		eWorldOperatorAlreadyDead	= u32(0),
		eWorldOperatorDead,
		
		eWorldOperatorFreeNoALife,
		eWorldOperatorFreeALife,
		
		eWorldOperatorGatherItems,
		eWorldOperatorGetKillDistance,
		eWorldOperatorGetEnemy,
		eWorldOperatorGetEnemySeen,
		eWorldOperatorGetItemToKill,
		eWorldOperatorFindItemToKill,
		eWorldOperatorMakeItemKilling,
		eWorldOperatorFindAmmo,

		eWorldOperatorGetReadyToKillVeryAggressive,
		eWorldOperatorKillEnemyVeryAggressive,
		
		eWorldOperatorGetReadyToKillAggressive,
		eWorldOperatorKillEnemyAggressive,
		eWorldOperatorAimEnemy,

		eWorldOperatorGetReadyToKillAvoid,
		eWorldOperatorKillEnemyAvoid,
		
		eWorldOperatorRetreatFromEnemy,

		eWorldOperatorGetReadyToKillModerate,
		eWorldOperatorKillEnemyModerate,
		eWorldOperatorCamping,
		eWorldOperatorGetEnemySeenModerate,
		eWorldOperatorKillEnemyLostModerate,

		eWorldOperatorScript,
		eWorldOperatorDummy			= u32(-1),
	};

	enum ESightActionType {
		eSightActionTypeWatchItem	= u32(0),
		eSightActionTypeWatchEnemy,
		eSightActionTypeDummy		= u32(-1),
	};
};