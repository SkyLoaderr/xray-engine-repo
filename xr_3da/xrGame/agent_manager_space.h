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
		eMotivationNoOrders,
		eMotivationOrders,

		eMotivationScript,
		eMotivationDummy		= u32(-1)
	};

	enum EProperties {
		ePropertyOrders			= u32(0),
		ePropertyItem,
		ePropertyEnemy,

		ePropertyScript,
		ePropertyDummy			= u32(-1),
	};

	enum EOperators {
		eOperatorNoOrders		= u32(0),
		eOperatorGatherItem,
		eOperatorKillEnemy,

		eOperatorScript,
		eOperatorDummy			= u32(-1),
	};

	enum EOrderType {
		eOrderTypeNoOrder		= u32(0),
		eOrderTypeAction,
		eOrderTypeGoal,
		eOrderTypeDummy			= u32(-1),
	};
};