////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_space.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace AgentManager {
	enum EMotivations {
		eMotivationGlobal		= u32(0),
		eMotivationIdle,

		eMotivationScript,
		eMotivationDummy		= u32(-1)
	};

	enum EProperties {
		ePropertyIdle			= u32(0),

		ePropertyScript,
		ePropertyDummy			= u32(-1),
	};

	enum EOperators {
		eOperatorIdle			= u32(0),

		eOperatorScript,
		eOperatorDummy			= u32(-1),
	};

	enum EOrderType {
		eOrderTypeNoOrder		= u32(0),
		eOrderTypeAction,
		eOrderTypeGoal,
		eOrderTypeNoDummy		= u32(-1),
	};
};