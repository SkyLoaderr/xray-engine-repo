////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_stalker_script.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivation action manager class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "motivation_action_manager_stalker.h"
#include "script_space.h"
#include "stalker_decision_space.h"
#include "ai/stalker/ai_stalker_space.h"
#include "ai/stalker/ai_stalker.h"

using namespace luabind;

void CMotivationActionManagerStalker::script_register(lua_State *L)
{
	module(L)
	[
		class_<CMotivationActionManagerStalker>("stalker_ids")
			.enum_("motivations")
			[
				luabind::value("motivation_global",							StalkerDecisionSpace::eMotivationGlobal),
				luabind::value("motivation_alive",							StalkerDecisionSpace::eMotivationAlive),
				luabind::value("motivation_dead",							StalkerDecisionSpace::eMotivationDead),
				luabind::value("motivation_solve_zone_puzzle",				StalkerDecisionSpace::eMotivationSolveZonePuzzle),
				luabind::value("motivation_squad_command",					StalkerDecisionSpace::eMotivationSquadCommand),
				luabind::value("motivation_squad_goal",						StalkerDecisionSpace::eMotivationSquadGoal),
				luabind::value("motivation_squad_action",					StalkerDecisionSpace::eMotivationSquadAction),
				luabind::value("motivation_script",							StalkerDecisionSpace::eMotivationScript)
			]
			
			.enum_("properties")
			[
				luabind::value("property_alive",								StalkerDecisionSpace::eWorldPropertyAlive),
				luabind::value("property_dead",								StalkerDecisionSpace::eWorldPropertyDead),
				luabind::value("property_already_dead",						StalkerDecisionSpace::eWorldPropertyAlreadyDead),
				luabind::value("property_alife",								StalkerDecisionSpace::eWorldPropertyALife),
				luabind::value("property_puzzle_solved",						StalkerDecisionSpace::eWorldPropertyPuzzleSolved),
				luabind::value("property_items",								StalkerDecisionSpace::eWorldPropertyItems),
				luabind::value("property_enemy",								StalkerDecisionSpace::eWorldPropertyEnemy),
				luabind::value("property_see_enemy",							StalkerDecisionSpace::eWorldPropertySeeEnemy),
				luabind::value("property_item_to_kill",						StalkerDecisionSpace::eWorldPropertyItemToKill),
				luabind::value("property_found_item_to_kill",				StalkerDecisionSpace::eWorldPropertyFoundItemToKill),
				luabind::value("property_item_can_kill",						StalkerDecisionSpace::eWorldPropertyItemCanKill),
				luabind::value("property_found_ammo",						StalkerDecisionSpace::eWorldPropertyFoundAmmo),
				luabind::value("property_ready_to_kill",						StalkerDecisionSpace::eWorldPropertyReadyToKill),
				luabind::value("property_kill_distance",						StalkerDecisionSpace::eWorldPropertyKillDistance),
				luabind::value("property_enemy_aimed",						StalkerDecisionSpace::eWorldPropertyEnemyAimed),
				luabind::value("property_safe_to_kill",						StalkerDecisionSpace::eWorldPropertySafeToKill),
				luabind::value("property_fire_enough",						StalkerDecisionSpace::eWorldPropertyFireEnough),
				luabind::value("property_squad_action",						StalkerDecisionSpace::eWorldPropertySquadAction),
				luabind::value("property_anomaly",							StalkerDecisionSpace::eWorldPropertyAnomaly),
				luabind::value("property_inside_anomaly",					StalkerDecisionSpace::eWorldPropertyInsideAnomaly),
				luabind::value("property_script",							StalkerDecisionSpace::eWorldPropertyScript)
			]
			
			.enum_("action")
			[
				luabind::value("action_already_dead",						StalkerDecisionSpace::eWorldOperatorAlreadyDead),
				luabind::value("action_dead",								StalkerDecisionSpace::eWorldOperatorDead),
				luabind::value("action_free_no_alife",						StalkerDecisionSpace::eWorldOperatorFreeNoALife),
				luabind::value("action_free",								StalkerDecisionSpace::eWorldOperatorFreeALife),
				luabind::value("action_gather_items",						StalkerDecisionSpace::eWorldOperatorGatherItems),
				luabind::value("action_get_kill_distance",					StalkerDecisionSpace::eWorldOperatorGetKillDistance),
				luabind::value("action_get_enemy",							StalkerDecisionSpace::eWorldOperatorGetEnemy),
				luabind::value("action_get_enemy_seen",						StalkerDecisionSpace::eWorldOperatorGetEnemySeen),
				luabind::value("action_get_item_to_kill",					StalkerDecisionSpace::eWorldOperatorGetItemToKill),
				luabind::value("action_find_item_to_kill",					StalkerDecisionSpace::eWorldOperatorFindItemToKill),
				luabind::value("action_make_item_killing",					StalkerDecisionSpace::eWorldOperatorMakeItemKilling),
				luabind::value("action_find_ammo",							StalkerDecisionSpace::eWorldOperatorFindAmmo),
				luabind::value("get_out_of_anomaly",							StalkerDecisionSpace::eWorldOperatorGetOutOfAnomaly),
				luabind::value("detect_anomaly",								StalkerDecisionSpace::eWorldOperatorDetectAnomaly),
				luabind::value("action_script",								StalkerDecisionSpace::eWorldOperatorScript),
				luabind::value("action_get_ready_to_kill_very_aggressive",	StalkerDecisionSpace::eWorldOperatorGetReadyToKillVeryAggressive),
				luabind::value("action_kill_enemy_very_aggressive",			StalkerDecisionSpace::eWorldOperatorKillEnemyVeryAggressive),
				luabind::value("action_get_ready_to_kill_aggressive",		StalkerDecisionSpace::eWorldOperatorGetReadyToKillAggressive),
				luabind::value("action_kill_enemy_aggressive",				StalkerDecisionSpace::eWorldOperatorKillEnemyAggressive),
				luabind::value("action_aim_enemy",							StalkerDecisionSpace::eWorldOperatorAimEnemy),
				luabind::value("action_get_ready_to_kill_moderate",			StalkerDecisionSpace::eWorldOperatorGetReadyToKillModerate),
				luabind::value("action_get_enemy_seen_moderate",				StalkerDecisionSpace::eWorldOperatorGetEnemySeenModerate),
				luabind::value("action_kill_enemy_moderate",					StalkerDecisionSpace::eWorldOperatorKillEnemyModerate),
				luabind::value("action_camping",								StalkerDecisionSpace::eWorldOperatorCamping),
				luabind::value("action_get_ready_to_kill_avoid",				StalkerDecisionSpace::eWorldOperatorGetReadyToKillAvoid),
				luabind::value("action_kill_enemy_avoid",					StalkerDecisionSpace::eWorldOperatorKillEnemyAvoid),
				luabind::value("action_retreat_from_enemy",					StalkerDecisionSpace::eWorldOperatorRetreatFromEnemy),
				luabind::value("action_squad_action",						StalkerDecisionSpace::eWorldOperatorSquadAction),
				luabind::value("action_death_planner",						StalkerDecisionSpace::eWorldOperatorDeathPlanner),
				luabind::value("action_puzzle_solver",						StalkerDecisionSpace::eWorldOperatorALifePlanner),
				luabind::value("action_combat_planner",						StalkerDecisionSpace::eWorldOperatorCombatPlanner),
				luabind::value("action_anomaly_planner",						StalkerDecisionSpace::eWorldOperatorCombatPlanner)
			]

			.enum_("sounds")
			[
				luabind::value("sound_die",									StalkerSpace::eStalkerSoundDie),
				luabind::value("sound_injuring",								StalkerSpace::eStalkerSoundInjuring),
				luabind::value("sound_humming",								StalkerSpace::eStalkerSoundHumming),
				luabind::value("sound_alarm",								StalkerSpace::eStalkerSoundAlarm),
				luabind::value("sound_surrender",							StalkerSpace::eStalkerSoundSurrender),
				luabind::value("sound_backup",								StalkerSpace::eStalkerSoundBackup),
				luabind::value("sound_attack",								StalkerSpace::eStalkerSoundAttack),
				luabind::value("sound_injuring_by_friend",					StalkerSpace::eStalkerSoundInjuringByFriend),
				luabind::value("sound_script",								StalkerSpace::eStalkerSoundScript)
			]
	];
}