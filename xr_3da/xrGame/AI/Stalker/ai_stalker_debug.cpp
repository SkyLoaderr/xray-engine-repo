////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_debug.cpp
//	Created 	: 05.07.2005
//  Modified 	: 05.07.2005
//	Author		: Dmitriy Iassenev
//	Description : Debug functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef DEBUG
#include "ai_stalker.h"
#include "../../hudmanager.h"
#include "../../memory_manager.h"
#include "../../visual_memory_manager.h"
#include "../../sound_memory_manager.h"
#include "../../hit_memory_manager.h"
#include "../../enemy_manager.h"
#include "../../danger_manager.h"
#include "../../item_manager.h"
#include "../../actor.h"
#include "../../motivation_action_manager_stalker.h"
#include "../../script_game_object.h"
#include "../../stalker_animation_manager.h"
#include "../../weapon.h"
#include "../../sound_player.h"
#include "../../inventory.h"
#include "../../object_handler_planner.h"
#include "../../stalker_movement_manager.h"
#include "../../movement_manager_space.h"
#include "../../patrol_path_manager.h"
#include "../../level_path_manager.h"
#include "../../game_path_manager.h"
#include "../../detail_path_manager.h"
#include "../../sight_manager.h"
#include "../../ai_object_location.h"
#include "../../entitycondition.h"
#include "../ai_monsters_misc.h"
#include "../../agent_manager.h"
#include "../../agent_member_manager.h"
#include "../../agent_enemy_manager.h"
#include "../../agent_corpse_manager.h"
#include "../../agent_location_manager.h"
#include "../../cover_point.h"

template <typename planner_type>
void draw_planner						(const planner_type &brain, LPCSTR start_indent, LPCSTR indent, LPCSTR planner_id)
{
	planner_type				&_brain = const_cast<planner_type&>(brain);
	if (brain.solution().empty())
		return;

	CScriptActionPlannerAction			*planner = smart_cast<CScriptActionPlannerAction*>(&_brain.action(brain.solution().front()));
	if (planner)
		draw_planner					(*planner,start_indent,indent,_brain.action2string(brain.solution().front()));

	HUD().Font().pFontSmall->OutNext	("%s ",start_indent);
	HUD().Font().pFontSmall->OutNext	("%splanner %s",start_indent,planner_id);
	HUD().Font().pFontSmall->OutNext	("%s%sevaluators  : %d",start_indent,indent,brain.evaluators().size());
	HUD().Font().pFontSmall->OutNext	("%s%soperators   : %d",start_indent,indent,brain.operators().size());
	HUD().Font().pFontSmall->OutNext	("%s%sselected    : %s",start_indent,indent,_brain.action2string(brain.solution().front()));
	// solution
	HUD().Font().pFontSmall->OutNext	("%s%ssolution",start_indent,indent);
	for (int i=0; i<(int)brain.solution().size(); ++i)
		HUD().Font().pFontSmall->OutNext("%s%s%s%s",start_indent,indent,indent,_brain.action2string(brain.solution()[i]));
	// current
	HUD().Font().pFontSmall->OutNext	("%s%scurrent world state",start_indent,indent);
	planner_type::EVALUATOR_MAP::const_iterator	I = brain.evaluators().begin();
	planner_type::EVALUATOR_MAP::const_iterator	E = brain.evaluators().end();
	for ( ; I != E; ++I) {
		xr_vector<planner_type::COperatorCondition>::const_iterator J = std::lower_bound(brain.current_state().conditions().begin(),brain.current_state().conditions().end(),planner_type::CWorldProperty((*I).first,false));
		char				temp = '?';
		if ((J != brain.current_state().conditions().end()) && ((*J).condition() == (*I).first)) {
			temp			= (*J).value() ? '+' : '-';
			HUD().Font().pFontSmall->OutNext	("%s%s%s    %5c : [%d][%s]",start_indent,indent,indent,temp,(*I).first,_brain.property2string((*I).first));
		}
	}
	// goal
	HUD().Font().pFontSmall->OutNext	("%s%starget world state",start_indent,indent);
	I = brain.evaluators().begin();
	for ( ; I != E; ++I) {
		xr_vector<planner_type::COperatorCondition>::const_iterator J = std::lower_bound(brain.target_state().conditions().begin(),brain.target_state().conditions().end(),planner_type::CWorldProperty((*I).first,false));
		char				temp = '?';
		if ((J != brain.target_state().conditions().end()) && ((*J).condition() == (*I).first)) {
			temp			= (*J).value() ? '+' : '-';
			HUD().Font().pFontSmall->OutNext	("%s%s%s    %5c : [%d][%s]",start_indent,indent,indent,temp,(*I).first,_brain.property2string((*I).first));
		}
	}
}

LPCSTR animation_name(CAI_Stalker *self, const MotionID &animation)
{
	if (!animation)
		return			("");
	CSkeletonAnimated	*skeleton_animated = smart_cast<CSkeletonAnimated*>(self->Visual());
	VERIFY				(skeleton_animated);
	LPCSTR				name = skeleton_animated->LL_MotionDefName_dbg(animation);
	return				(name);
}

void draw_restrictions(const shared_str &restrictions, LPCSTR start_indent, LPCSTR indent, LPCSTR header)
{
	HUD().Font().pFontSmall->OutNext	("%s%s%s",start_indent,indent,header);
	string256	temp;
	for (u32 i=0, n=_GetItemCount(*restrictions); i<n; ++i)
		HUD().Font().pFontSmall->OutNext("%s%s%s",start_indent,indent,indent,_GetItem(*restrictions,i,temp));
}

void CAI_Stalker::OnHUDDraw				(CCustomHUD *hud)
{
	inherited::OnHUDDraw				(hud);

	if (!g_Alive())
		return;

	CActor								*actor = smart_cast<CActor*>(Level().Objects.net_Find(0));
	if (!actor)
		return;

	float								up_indent = 40.f;
	LPCSTR								indent = "  ";

	HUD().Font().pFontSmall->SetColor	(D3DCOLOR_XRGB(0,255,0));
	HUD().Font().pFontSmall->OutSet		(0,up_indent);
	// memory
	HUD().Font().pFontSmall->OutNext	("memory");
	HUD().Font().pFontSmall->OutNext	("%sname          : %s",indent,*cName());
	HUD().Font().pFontSmall->OutNext	("%sid            : %d",indent,ID());
	HUD().Font().pFontSmall->OutNext	("%shealth        : %f",indent,conditions().health());
	// visual
	HUD().Font().pFontSmall->OutNext	("%svisual",indent);
	HUD().Font().pFontSmall->OutNext	("%s%sobjects     : %d",indent,indent,memory().visual().objects().size());
	HUD().Font().pFontSmall->OutNext	("%s%snot yet     : %d",indent,indent,memory().visual().not_yet_visible_objects().size());
	HUD().Font().pFontSmall->OutNext	("%s%sin frustum  : %d",indent,indent,memory().visual().raw_objects().size());
	if (memory().visual().visible_now(actor))
		HUD().Font().pFontSmall->OutNext("%s%sactor       : visible",indent,indent);
	else {
		MemorySpace::CNotYetVisibleObject	*object = memory().visual().not_yet_visible_object(actor);
		if (object && !fis_zero(object->m_value))
			HUD().Font().pFontSmall->OutNext("%s%sactor       : not yet visible : %f",indent,indent,object->m_value);
		else
			HUD().Font().pFontSmall->OutNext("%s%sactor       : not visible",indent,indent);
	}
	// sound
	HUD().Font().pFontSmall->OutNext	("%ssound",indent);
	HUD().Font().pFontSmall->OutNext	("%s%sobjects     : %d",indent,indent,memory().sound().objects().size());
	if (memory().sound().sound()) {
		HUD().Font().pFontSmall->OutNext	("%s%sselected",indent,indent);
//		switch (memory().sound().sound()->m_sound_type)
		HUD().Font().pFontSmall->OutNext	("%s%s%stype",indent,indent,indent);
		HUD().Font().pFontSmall->OutNext	("%s%s%spower     : %f",indent,indent,indent,memory().sound().sound()->m_power);
		HUD().Font().pFontSmall->OutNext	("%s%s%sobject    : %s",indent,indent,indent,memory().sound().sound()->m_object ? *memory().sound().sound()->m_object->cName() : "unknown");
	}
	// hit
	HUD().Font().pFontSmall->OutNext	("%shit",indent);
	HUD().Font().pFontSmall->OutNext	("%s%sobjects     : %d",indent,indent,memory().hit().objects().size());
	if (memory().hit().hit()) {
		HUD().Font().pFontSmall->OutNext	("%s%sselected",indent,indent);
		HUD().Font().pFontSmall->OutNext	("%s%s%spower     : %f",indent,indent,indent,memory().hit().hit()->m_amount);
		HUD().Font().pFontSmall->OutNext	("%s%s%sobject    : %s",indent,indent,indent,memory().hit().hit()->m_object ? *memory().hit().hit()->m_object->cName() : "unknown");
	}
	// enemy
	HUD().Font().pFontSmall->OutNext	("%senemy",indent);
	HUD().Font().pFontSmall->OutNext	("%s%sobjects     : %d",indent,indent,memory().enemy().objects().size());
	if (memory().enemy().selected()) {
		HUD().Font().pFontSmall->OutNext	("%s%sselected",indent,indent);
		HUD().Font().pFontSmall->OutNext	("%s%s%sobject    : %s",indent,indent,indent,*memory().enemy().selected()->cName());
		float								interval = (1.f - panic_threshold())*.25f, left = -1.f, right = -1.f;
		LPCSTR								description = "invalid";
		u32									result = dwfChooseAction(
			2000,
			1.f - interval,
			1.f - 2*interval,
			1.f - 3*interval,
			panic_threshold(),
			g_Team(),
			g_Squad(),
			g_Group(),
			0,
			1,
			2,
			3,
			4,
			this,
			300.f
		);
		switch (result) {
			case 0 : {
				description					= "attack";
				left						= 1.f;
				right						= 1.f - 1.f*interval;
				break;
			}
			case 1 : {
				description					= "careful attack";
				left						= 1.f - 1.f*interval;
				right						= 1.f - 2.f*interval;
				break;
			}
			case 2 : {
				description					= "defend";
				left						= 1.f - 2.f*interval;
				right						= 1.f - 3.f*interval;
				break;
			}
			case 3 : {
				description					= "retreat";
				left						= 1.f - 3*interval;
				right						= panic_threshold();
				break;
			}
			case 4 : {
				description					= "panic";
				left						= panic_threshold();
				right						= 0.f;
				break;
			}
			default : NODEFAULT;
		}
		HUD().Font().pFontSmall->OutNext	("%s%s%svictory   : [%f%%,%f%%] -> %s",indent,indent,indent,right,left,description);
	}
	// danger
	HUD().Font().pFontSmall->OutNext	("%sdanger",indent);
	HUD().Font().pFontSmall->OutNext	("%s%sobjects     : %d",indent,indent,memory().danger().objects().size());
	if (memory().danger().selected() && memory().danger().selected()->object()) {
		HUD().Font().pFontSmall->OutNext	("%s%sselected",indent,indent);
		HUD().Font().pFontSmall->OutNext	("%s%s%sinitiator : %s",indent,indent,indent,*memory().danger().selected()->object()->cName());
		if (memory().danger().selected()->dependent_object())
			HUD().Font().pFontSmall->OutNext("%s%s%sdependent : %s",indent,indent,indent,*memory().danger().selected()->dependent_object()->cName());
	}
	// item
	HUD().Font().pFontSmall->OutNext	("%sdanger",indent);
	HUD().Font().pFontSmall->OutNext	("%s%sobjects     : %d",indent,indent,memory().danger().objects().size());
	if (memory().item().selected()) {
		HUD().Font().pFontSmall->OutNext	("%s%sselected",indent,indent);
		HUD().Font().pFontSmall->OutNext	("%s%s%sobject   : %s",indent,indent,indent,*memory().item().selected()->cName());
	}

	// agent manager
	HUD().Font().pFontSmall->OutNext	(" ");
	HUD().Font().pFontSmall->OutNext	("agent manager");
	HUD().Font().pFontSmall->OutNext	("%smembers           : %d",indent,agent_manager().member().members().size());
	HUD().Font().pFontSmall->OutNext	("%senemies           : %d",indent,agent_manager().enemy().enemies().size());
	HUD().Font().pFontSmall->OutNext	("%scorpses           : %d",indent,agent_manager().corpse().corpses().size());
	HUD().Font().pFontSmall->OutNext	("%sdanger locations  : %d",indent,agent_manager().location().locations().size());
	HUD().Font().pFontSmall->OutNext	("%smembers in combat : %d",indent,agent_manager().member().combat_members().size());
	HUD().Font().pFontSmall->OutNext	("%sI am in combat    : %s",indent,agent_manager().member().registered_in_combat(this) ? "+" : "-");

	if (agent_manager().member().member(this).cover())
		HUD().Font().pFontSmall->OutNext("%scover         : [%f][%f][%f]",indent,VPUSH(agent_manager().member().member(this).cover()->position()));

	if (agent_manager().member().member(this).member_death_reaction().m_processing)
		HUD().Font().pFontSmall->OutNext("%react on death : %s",indent,*agent_manager().member().member(this).member_death_reaction().m_member->cName());

	if (agent_manager().member().member(this).grenade_reaction().m_processing)
		HUD().Font().pFontSmall->OutNext("%react on grenade : %s",indent,agent_manager().member().member(this).grenade_reaction().m_game_object ? *agent_manager().member().member(this).grenade_reaction().m_game_object->cName() : "unknown");

	HUD().Font().pFontSmall->OutSet		(320,up_indent);
	// brain
	HUD().Font().pFontSmall->OutNext	("brain");
	// motivations
	HUD().Font().pFontSmall->OutNext	("%smotivations",indent);

	const CMotivationActionManagerStalker &brain = this->brain();
	HUD().Font().pFontSmall->OutNext	("%s%sobjects     : %d",indent,indent,brain.graph().vertices().size());
	HUD().Font().pFontSmall->OutNext	("%s%selected_id  : %d",indent,indent,brain.selected_id());
	// actions
	draw_planner						(this->brain(),indent,indent,"root");
	
	HUD().Font().pFontSmall->OutSet		(640,up_indent);
	// brain
	HUD().Font().pFontSmall->OutNext	("controls");
	// animations
	HUD().Font().pFontSmall->OutNext	("%sanimations",indent);

	HUD().Font().pFontSmall->OutNext	("%s%shead        : [%s][%f]",indent,indent,
		animation_name(this,animation().head().animation()),
		animation().head().blend() ? animation().head().blend()->timeCurrent : 0.f
	);
	HUD().Font().pFontSmall->OutNext	("%s%storso       : [%s][%f]",indent,indent,
		animation_name(this,animation().torso().animation()),
		animation().torso().blend() ? animation().torso().blend()->timeCurrent : 0.f
	);
	HUD().Font().pFontSmall->OutNext	("%s%slegs        : [%s][%f]",indent,indent,
		animation_name(this,animation().legs().animation()),
		animation().legs().blend() ? animation().legs().blend()->timeCurrent : 0.f
	);
	HUD().Font().pFontSmall->OutNext	("%s%sglobal      : [%s][%f]",indent,indent,
		animation_name(this,animation().global().animation()),
		animation().global().blend() ? animation().global().blend()->timeCurrent : 0.f
	);
	HUD().Font().pFontSmall->OutNext	("%s%sscript      : [%s][%f]",indent,indent,
		animation_name(this,animation().script().animation()),
		animation().script().blend() ? animation().script().blend()->timeCurrent : 0.f
	);

	// sounds
	HUD().Font().pFontSmall->OutNext	(" ");
	HUD().Font().pFontSmall->OutNext	("%ssounds",indent);
	HUD().Font().pFontSmall->OutNext	("%s%scollections : %d",indent,indent,sound().objects().size());
	
	{
		u32			object_count = 0;
		CSoundPlayer::SOUND_COLLECTIONS::const_iterator	I = sound().objects().begin();
		CSoundPlayer::SOUND_COLLECTIONS::const_iterator	E = sound().objects().end();
		for ( ; I != E; ++I)
			object_count	+= (*I).second.m_sounds.size();
		HUD().Font().pFontSmall->OutNext("%s%sobjects     : %d",indent,indent,object_count);
	}
	{
		xr_vector<CSoundPlayer::CSoundSingle>::const_iterator	I = sound().playing_sounds().begin();
		xr_vector<CSoundPlayer::CSoundSingle>::const_iterator	E = sound().playing_sounds().end();
		for ( ; I != E; ++I)
			HUD().Font().pFontSmall->OutNext("%s%s%sactive    : %s",indent,indent,indent,(*I).m_sound->handle ? (*I).m_sound->handle->file_name() : "no source");
	}

	// sight
	HUD().Font().pFontSmall->OutNext	(" ");
	HUD().Font().pFontSmall->OutNext	("%ssight",indent);

	LPCSTR								sight_type = "invalid";
	switch (sight().current_action().sight_type()) {
		case SightManager::eSightTypeCurrentDirection : {
			sight_type					= "current direction";
			break;
		}
		case SightManager::eSightTypePathDirection : {
			sight_type					= "path direction";
			break;
		}
		case SightManager::eSightTypeDirection : {
			sight_type					= "direction";
			break;
		}
		case SightManager::eSightTypePosition : {
			sight_type					= "position";
			break;
		}
		case SightManager::eSightTypeObject : {
			sight_type					= "object";
			break;
		}
		case SightManager::eSightTypeCover : {
			sight_type					= "cover";
			break;
		}
		case SightManager::eSightTypeSearch : {
			sight_type					= "search";
			break;
		}
		case SightManager::eSightTypeLookOver : {
			sight_type					= "look over";
			break;
		}
		case SightManager::eSightTypeCoverLookOver : {
			sight_type					= "cover look over";
			break;
		}
		default : NODEFAULT;
	}

	HUD().Font().pFontSmall->OutNext	("%s%stype            : %s",indent,indent,sight_type);
	HUD().Font().pFontSmall->OutNext	("%s%suse torso       : %s",indent,indent,sight().current_action().use_torso_look() ? "+" : "-");
	
	switch (sight().current_action().sight_type()) {
		case SightManager::eSightTypeCurrentDirection : {
			break;
		}
		case SightManager::eSightTypePathDirection : {
			break;
		}
		case SightManager::eSightTypeDirection : {
			HUD().Font().pFontSmall->OutNext	("%s%sdirection       : [%f][%f][%f]",indent,indent,VPUSH(sight().current_action().vector3d()));
			break;
		}
		case SightManager::eSightTypePosition : {
			HUD().Font().pFontSmall->OutNext	("%s%sposition        : [%f][%f][%f]",indent,indent,VPUSH(sight().current_action().vector3d()));
			break;
		}
		case SightManager::eSightTypeObject : {
			HUD().Font().pFontSmall->OutNext	("%s%sobject          : %s",indent,indent,*sight().current_action().object().cName());
			HUD().Font().pFontSmall->OutNext	("%s%sposition        : [%f][%f][%f]",indent,indent,VPUSH(sight().current_action().object().Position()));
			break;
		}
		case SightManager::eSightTypeCover : {
			sight_type					= "cover";
			break;
		}
		case SightManager::eSightTypeSearch : {
			sight_type					= "search";
			break;
		}
		case SightManager::eSightTypeLookOver : {
			sight_type					= "look over";
			break;
		}
		case SightManager::eSightTypeCoverLookOver : {
			sight_type					= "cover look over";
			break;
		}
		default : NODEFAULT;
	}

	// movement
	HUD().Font().pFontSmall->OutNext	(" ");
	HUD().Font().pFontSmall->OutNext	("%smovement",indent);

	LPCSTR								mental_state = "invalid";
	switch (movement().mental_state()) {
		case MonsterSpace::eMentalStateFree : {
			mental_state				= "free";
			break;
		}
		case MonsterSpace::eMentalStateDanger : {
			mental_state				= "danger";
			break;
		}
		case MonsterSpace::eMentalStatePanic : {
			mental_state				= "panic";
			break;
		}
		default : NODEFAULT;
	}
	HUD().Font().pFontSmall->OutNext	("%s%smental state    : %s",indent,indent,mental_state);

	LPCSTR								body_state = "invalid";
	switch (movement().body_state()) {
		case MonsterSpace::eBodyStateStand : {
			body_state					= "stand";
			break;
		}
		case MonsterSpace::eBodyStateCrouch : {
			body_state					= "crouch";
			break;
		}
		case MonsterSpace::eBodyStateStandDamaged : {
			body_state					= "stand damaged";
			break;
		}
		default : NODEFAULT;
	}
	HUD().Font().pFontSmall->OutNext	("%s%sbody state      : %s",indent,indent,body_state);

	LPCSTR								movement_type = "invalid";
	switch (movement().movement_type()) {
		case MonsterSpace::eMovementTypeStand : {
			movement_type				= "stand";
			break;
		}
		case MonsterSpace::eMovementTypeWalk : {
			movement_type				= "walk";
			break;
		}
		case MonsterSpace::eMovementTypeRun : {
			movement_type				= "run";
			break;
		}
		default : NODEFAULT;
	}
	HUD().Font().pFontSmall->OutNext	("%s%smovement type   : %s",indent,indent,movement_type);

	LPCSTR						path_type = "invalid";
	switch (movement().path_type()) {
		case MovementManager::ePathTypeGamePath : {
			path_type			= "game path";
			break;
		}
		case MovementManager::ePathTypeLevelPath : {
			path_type			= "level path";
			break;
		}
		case MovementManager::ePathTypePatrolPath : {
			path_type			= "patrol path";
			break;
		}
		case MovementManager::ePathTypeNoPath : {
			path_type			= "no path";
			break;
		}
		default : NODEFAULT;
	}
	HUD().Font().pFontSmall->OutNext	("%s%spath type       : %s",indent,indent,path_type);
	HUD().Font().pFontSmall->OutNext	("%s%sposition        : [%f][%f][%f]",indent,indent,VPUSH(Position()));
	HUD().Font().pFontSmall->OutNext	("%s%slevel vertex id : %d",indent,indent,ai_location().level_vertex_id());
	HUD().Font().pFontSmall->OutNext	("%s%sgame vertex id  : %d",indent,indent,ai_location().game_vertex_id());
	HUD().Font().pFontSmall->OutNext	("%s%shead current    : [%f][%f]",indent,indent,movement().head_orientation().current.yaw,movement().head_orientation().current.pitch);
	HUD().Font().pFontSmall->OutNext	("%s%shead target     : [%f][%f]",indent,indent,movement().head_orientation().target.yaw,movement().head_orientation().target.pitch);
	HUD().Font().pFontSmall->OutNext	("%s%sbody current    : [%f][%f]",indent,indent,movement().body_orientation().current.yaw,movement().body_orientation().current.pitch);
	HUD().Font().pFontSmall->OutNext	("%s%sbody target     : [%f][%f]",indent,indent,movement().body_orientation().target.yaw,movement().body_orientation().target.pitch);
	
	if (movement().path_type() == MovementManager::ePathTypePatrolPath) {
		HUD().Font().pFontSmall->OutNext("%s%spatrol");
		HUD().Font().pFontSmall->OutNext("%s%s%spath          : %s",indent,indent,indent,*movement().patrol().path_name());
		HUD().Font().pFontSmall->OutNext("%s%s%scompleted     : %s",indent,indent,indent,movement().patrol().completed() ? "+" : "-");
		HUD().Font().pFontSmall->OutNext("%s%s%scurrent point : %d",indent,indent,indent,movement().patrol().get_current_point_index());
		HUD().Font().pFontSmall->OutNext("%s%s%sextrapolate   : %s",indent,indent,indent,movement().patrol().extrapolate_path() ? "+" : "-");
	}

	if (movement().path_type() == MovementManager::ePathTypeGamePath) {
		HUD().Font().pFontSmall->OutNext("%s%sgame",indent,indent);
		HUD().Font().pFontSmall->OutNext("%s%s%scompleted     : %s",indent,indent,indent,movement().game_path().completed() ? "+" : "-");
		HUD().Font().pFontSmall->OutNext("%s%s%spath size     : %d",indent,indent,indent,movement().game_path().path().size());
		HUD().Font().pFontSmall->OutNext("%s%s%scurrent point : %d",indent,indent,indent,movement().game_path().intermediate_index());
	}
	
	HUD().Font().pFontSmall->OutNext	("%s%slevel",indent,indent);
	HUD().Font().pFontSmall->OutNext	("%s%s%spath size     : %d",indent,indent,indent,movement().level_path().path().size());
	HUD().Font().pFontSmall->OutNext	("%s%s%sstart vertex  : %d",indent,indent,indent,movement().level_path().path().empty() ? -1 : movement().level_path().path().front());
	HUD().Font().pFontSmall->OutNext	("%s%s%sdest vertex   : %d",indent,indent,indent,movement().level_path().path().empty() ? -1 : movement().level_path().path().back());

	HUD().Font().pFontSmall->OutNext	("%s%sdetail",indent,indent);
	HUD().Font().pFontSmall->OutNext	("%s%s%svelocities    : %d",indent,indent,indent,movement().detail().velocities().size());
	HUD().Font().pFontSmall->OutNext	("%s%s%sextrapolate   : %f",indent,indent,indent,movement().detail().extrapolate_length());
	HUD().Font().pFontSmall->OutNext	("%s%s%spath size     : %d",indent,indent,indent,movement().detail().path().size());
	HUD().Font().pFontSmall->OutNext	("%s%s%sstart point   : [%f][%f][%f]",indent,indent,indent,movement().detail().path().empty() ? VPUSH(Fvector().set(0.f,0.f,0.f)) : VPUSH(movement().detail().path().front().position));
	HUD().Font().pFontSmall->OutNext	("%s%s%sdest point    : [%f][%f][%f]",indent,indent,indent,movement().detail().path().empty() ? VPUSH(Fvector().set(0.f,0.f,0.f)) : VPUSH(movement().detail().path().back().position));

	string256							temp;
	if	(
			movement().restrictions().out_restrictions().size() ||
			movement().restrictions().in_restrictions().size() ||
			movement().restrictions().base_out_restrictions().size() ||
			movement().restrictions().base_in_restrictions().size()
		) {
		HUD().Font().pFontSmall->OutNext	("%s%srestrictions",indent,indent);
		strconcat							(temp,indent,indent,indent);
		draw_restrictions					(movement().restrictions().out_restrictions(),temp,indent,"out");
		draw_restrictions					(movement().restrictions().in_restrictions(),temp,indent,"in");
		draw_restrictions					(movement().restrictions().base_out_restrictions(),temp,indent,"base out");
		draw_restrictions					(movement().restrictions().base_in_restrictions(),temp,indent,"base in");
	}
	// objects
	HUD().Font().pFontSmall->OutNext	(" ");
	HUD().Font().pFontSmall->OutNext	("%sobjects",indent);
	HUD().Font().pFontSmall->OutNext	("%s%sobjects             : %d",indent,indent,inventory().m_all.size());
	HUD().Font().pFontSmall->OutNext	("%s%sbest weapon         : %s",indent,indent,best_weapon() ? *best_weapon()->object().cName() : "");
	HUD().Font().pFontSmall->OutNext	("%s%sitem to kill        : %s",indent,indent,item_to_kill() ? *m_best_item_to_kill->object().cName() : "");
	HUD().Font().pFontSmall->OutNext	("%s%sitem can kill       : %s",indent,indent,item_can_kill() ? "+" : "-");
	HUD().Font().pFontSmall->OutNext	("%s%smemory item to kill : %s",indent,indent,remember_item_to_kill() ? *m_best_found_item_to_kill->object().cName() : "");
	HUD().Font().pFontSmall->OutNext	("%s%smemory ammo         : %s",indent,indent,remember_ammo() ? *m_best_found_ammo->object().cName() : "");
	if (inventory().ActiveItem()) {
		HUD().Font().pFontSmall->OutNext	("%s%sactive item",indent,indent);
		HUD().Font().pFontSmall->OutNext	("%s%s%sobject         : %s",indent,indent,indent,inventory().ActiveItem() ? *inventory().ActiveItem()->object().cName() : "");
		CWeapon	*weapon = smart_cast<CWeapon*>(inventory().ActiveItem());
		if (weapon) {
			HUD().Font().pFontSmall->OutNext("%s%s%sstrapped       : %s",indent,indent,indent,weapon_strapped(weapon) ? "+" : "-");
			HUD().Font().pFontSmall->OutNext("%s%s%sunstrapped     : %s",indent,indent,indent,weapon_unstrapped(weapon) ? "+" : "-");
		}
	}
	const CObjectHandlerPlanner	&objects = planner();
	strconcat					(temp,indent,indent);
	draw_planner				(objects,temp,indent,"root");
}
#endif