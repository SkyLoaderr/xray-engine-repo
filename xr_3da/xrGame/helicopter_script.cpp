#include "stdafx.h"
#include "helicopter.h"
#include "script_space.h"

using namespace luabind;
void CHelicopter::script_register(lua_State *L)
{
	module(L)
		[
			class_<CHelicopter>("CHelicopter")
			.enum_("events")
			[
				value("on_point",			int(CHelicopter::EV_ON_POINT))
			]
			.enum_("state")
				[
					value("eIdleState",			int(CHelicopter::eIdleState)),
					value("eInitiatePatrolZone",			int(CHelicopter::eInitiatePatrolZone)),
					value("eMovingByPatrolZonePath",			int(CHelicopter::eMovingByPatrolZonePath)),
					value("eInitiateHunt",			int(CHelicopter::eInitiateHunt)),
					value("eInitiateHunt2",			int(CHelicopter::eInitiateHunt2)),
					value("eMovingByAttackTraj",			int(CHelicopter::eMovingByAttackTraj)),
					value("eWaitForStart",			int(CHelicopter::eWaitForStart)),
					value("eWaitBetweenPatrol",			int(CHelicopter::eWaitBetweenPatrol)),
					value("eInitiateWaitBetweenPatrol",			int(CHelicopter::eInitiateWaitBetweenPatrol)),
					value("eMovingToWaitPoint",			int(CHelicopter::eMovingToWaitPoint)),
					value("eInitiateGoToPoint",			int(CHelicopter::eInitiateGoToPoint)),
					value("eMovingToPoint",			int(CHelicopter::eMovingToPoint)),
					value("eInitiatePatrolByPath",			int(CHelicopter::eInitiatePatrolByPath)),
					value("eMovingByPatrolPath",			int(CHelicopter::eMovingByPatrolPath)),
					value("eDead",			int(CHelicopter::eDead)),
					value("eForce",			int(CHelicopter::eForce))
				]
				.def_readwrite("on_point_dist", &CHelicopter::m_on_point_range_dist)

				.def("GetState", &CHelicopter::state)
				.def("SetState", &CHelicopter::setState)
				.def("SetDestPosition", &CHelicopter::SetDestPosition)
				.def("GetCurrAltitude", &CHelicopter::GetCurrAltitude)
				.def("GetCurrVelocity", &CHelicopter::GetCurrVelocity)
				.def("SetCurrVelocity", &CHelicopter::SetCurrVelocity)
		];
}