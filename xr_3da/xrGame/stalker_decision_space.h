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
		eMotivationDummy			= u32(-1)
	};

	enum EWorldProperties {
		eWorldPropertyAlive			= u32(0),
		eWorldPropertyDead,
		eWorldPropertyAlreadyDead,
		
		eWorldPropertyALife,
		eWorldPropertyPuzzleSolved,

		eWorldPropertyItems,
		eWorldPropertyEnemies,

		eWorldPropertyScript,
		eWorldPropertyDummy			= u32(-1),
	};

	enum EWorldOperators {
		eWorldOperatorAlreadyDead	= u32(0),
		eWorldOperatorDead,
		
		eWorldOperatorFreeNoALife,
		eWorldOperatorFreeALife,
		
		eWorldOperatorGatherItems,
		eWorldOperatorKillEnemy,

		eWorldOperatorScript,
		eWorldOperatorDummy			= u32(-1),
	};
};