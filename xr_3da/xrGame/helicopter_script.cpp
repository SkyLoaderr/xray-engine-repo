#include "stdafx.h"
#include "helicopter.h"
#include "script_space.h"
#include "script_game_object.h"

using namespace luabind;
void CHelicopter::script_register(lua_State *L)
{
	module(L)
		[
			class_<CHelicopter>("CHelicopter")
			.enum_("events")
			[
				value("on_point",			int(CHelicopter::EV_ON_POINT)),
				value("on_hit",				int(CHelicopter::EV_ON_HIT))
			]
			.enum_("state")
				[
					value("eIdleState",						int(CHelicopter::eIdleState)),
//					value("eInitiatePatrolZone",			int(CHelicopter::eInitiatePatrolZone)),
//					value("eMovingByPatrolZonePath",		int(CHelicopter::eMovingByPatrolZonePath)),
//					value("eInitiateHunt",					int(CHelicopter::eInitiateHunt)),
//					value("eInitiateHunt2",					int(CHelicopter::eInitiateHunt2)),
					value("eMovingByAttackTraj",			int(CHelicopter::eMovingByAttackTraj)),
//					value("eWaitForStart",					int(CHelicopter::eWaitForStart)),
//					value("eWaitBetweenPatrol",				int(CHelicopter::eWaitBetweenPatrol)),
//					value("eInitiateWaitBetweenPatrol",		int(CHelicopter::eInitiateWaitBetweenPatrol)),
//					value("eMovingToWaitPoint",				int(CHelicopter::eMovingToWaitPoint)),
//					value("eInitiateGoToPoint",				int(CHelicopter::eInitiateGoToPoint)),
					value("eMovingToPoint",					int(CHelicopter::eMovingToPoint)),
//					value("eInitiatePatrolByPath",			int(CHelicopter::eInitiatePatrolByPath)),
					value("eMovingByPatrolPath",			int(CHelicopter::eMovingByPatrolPath)),
					value("eDead",							int(CHelicopter::eDead))
//					value("eForce",							int(CHelicopter::eForce))
				]

				.def("GetState",					&CHelicopter::state_script)
				.def("SetState",					&CHelicopter::setState_script)
				.def("SetDestPosition",				&CHelicopter::SetDestPosition)
				.def("GetCurrAltitude",				&CHelicopter::GetCurrAltitude)
				.def("GetCurrVelocity",				&CHelicopter::GetCurrVelocity)
				.def("SetCurrVelocity",				&CHelicopter::SetCurrVelocity)
				.def("GetfHealth",					&CHelicopter::GetfHealth)
				.def("SetfHealth",					&CHelicopter::SetfHealth)
				
				.def("GetDistanceToDestPosition",	&CHelicopter::GetDistanceToDestPosition)

				.def("SetEnemy",					&CHelicopter::SetEnemy)
				.def("GoPatrolByPatrolPath",		&CHelicopter::goPatrolByPatrolPath)
				.def("Die",							&CHelicopter::Die)
				.def("PrepareDie",					&CHelicopter::PrepareDie)
				.def("Explode",						&CHelicopter::Explode)

				.def("GetRealAltitude",				&CHelicopter::GetRealAltitude)

				.def_readwrite("m_use_rocket_on_attack", &CHelicopter::m_use_rocket_on_attack)
				.def_readwrite("m_use_mgun_on_attack", &CHelicopter::m_use_mgun_on_attack)
				.def_readwrite("m_min_rocket_dist", &CHelicopter::m_min_rocket_dist)
				.def_readwrite("m_max_rocket_dist", &CHelicopter::m_max_rocket_dist)
				.def_readwrite("m_min_mgun_dist", &CHelicopter::m_min_mgun_dist)
				.def_readwrite("m_max_mgun_dist", &CHelicopter::m_max_mgun_dist)
				.def_readwrite("m_time_between_rocket_attack", &CHelicopter::m_time_between_rocket_attack)
				.def_readwrite("m_syncronize_rocket", &CHelicopter::m_syncronize_rocket)
				.def_readwrite("m_on_point_range_dist", &CHelicopter::m_on_point_range_dist)
//				.def_readwrite("", &CHelicopter::)

		];
}