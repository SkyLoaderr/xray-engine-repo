////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_export.h
//	Created 	: 01.04.2004
//  Modified 	: 22.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine export
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_fvector.h"
#include "script_fmatrix.h"
#include "script_engine.h"
#include "script_token_list.h"
#include "script_ini_file.h"
#include "object_factory.h"
#include "script_sound_type.h"
#include "script_net_packet.h"
#include "xrServer_Objects_ALife_All.h"

#ifndef XRGAME_EXPORTS
#	include "script_properties_list_helper.h"
#else
#	include "alife_simulator.h"
#	include "script_hit.h"
#	include "script_monster_hit_info.h"
#	include "script_sound_info.h"
#	include "script_sound.h"
#	include "script_binder_object.h"
#	include "motivation_action_manager_stalker.h"
#	include "motivation_action_manager.h"
#	include "motivation_manager.h"
#	include "motivation_action.h"
#	include "motivation.h"
#	include "action_planner_action.h"
#	include "action_planner.h"
#	include "action_base.h"
#	include "property_storage.h"
#	include "property_evaluator.h"
#	include "script_world_state.h"
#	include "script_world_property.h"
#	include "ai/stalker/ai_stalker.h"
#	include "script_effector.h"
#	include "particlesobject.h"
#	include "artifactmerger.h"
#	include "level.h"
#	include "memory_space.h"
#	include "script_render_device.h"
#	include "game_sv_base.h"
#	include "script_game_object.h"
#	include "cover_point.h"
#	include "path_manager_params_level_evaluator.h"
#	include "script_movement_action.h"
#	include "script_watch_action.h"
#	include "script_animation_action.h"
#	include "script_sound_action.h"
#	include "script_particle_action.h"
#	include "script_object_action.h"
#	include "script_action_condition.h"
#	include "script_monster_action.h"
#	include "script_entity_action.h"
#	include "game_sv_Deathmatch.h"

#endif