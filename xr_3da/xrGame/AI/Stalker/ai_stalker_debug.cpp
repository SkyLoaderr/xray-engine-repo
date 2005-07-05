////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_debug.cpp
//	Created 	: 05.07.2005
//  Modified 	: 05.07.2005
//	Author		: Dmitriy Iassenev
//	Description : Debug functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../hudmanager.h"
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

#ifdef DEBUG
void draw_planner						(const CScriptActionPlanner &brain, LPCSTR indent, LPCSTR planner_id)
{
	CScriptActionPlanner				&_brain = const_cast<CScriptActionPlanner&>(brain);
	if (brain.solution().empty())
		return;

	CScriptActionPlannerAction			*planner = smart_cast<CScriptActionPlannerAction*>(&_brain.action(brain.solution().front()));
	if (planner)
		draw_planner					(*planner,indent,_brain.action2string(brain.solution().front()));

	HUD().Font().pFontSmall->OutNext	("%s ",indent);
	HUD().Font().pFontSmall->OutNext	("%splanner %s",indent,planner_id);
	HUD().Font().pFontSmall->OutNext	("%s%sevaluators  : %d",indent,indent,brain.evaluators().size());
	HUD().Font().pFontSmall->OutNext	("%s%soperators   : %d",indent,indent,brain.operators().size());
	HUD().Font().pFontSmall->OutNext	("%s%sselected    : %s",indent,indent,_brain.action2string(brain.solution().front()));
	// solution
	HUD().Font().pFontSmall->OutNext	("%s%ssolution",indent,indent);
	for (int i=0; i<(int)brain.solution().size(); ++i)
		HUD().Font().pFontSmall->OutNext("%s%s%s%s",indent,indent,indent,_brain.action2string(brain.solution()[i]));
	// current
	HUD().Font().pFontSmall->OutNext	("%s%scurrent world state",indent,indent);
	CMotivationActionManagerStalker::EVALUATOR_MAP::const_iterator	I = brain.evaluators().begin();
	CMotivationActionManagerStalker::EVALUATOR_MAP::const_iterator	E = brain.evaluators().end();
	for ( ; I != E; ++I) {
		xr_vector<CMotivationActionManagerStalker::COperatorCondition>::const_iterator J = std::lower_bound(brain.current_state().conditions().begin(),brain.current_state().conditions().end(),CMotivationActionManagerStalker::CWorldProperty((*I).first,false));
		char				temp = '?';
		if ((J != brain.current_state().conditions().end()) && ((*J).condition() == (*I).first)) {
			temp			= (*J).value() ? '+' : '-';
			HUD().Font().pFontSmall->OutNext	("%s%s%s    %5c : [%d][%s]",indent,indent,indent,temp,(*I).first,_brain.property2string((*I).first));
		}
	}
	// goal
	HUD().Font().pFontSmall->OutNext	("%s%starget world state",indent,indent);
	I = brain.evaluators().begin();
	for ( ; I != E; ++I) {
		xr_vector<CMotivationActionManagerStalker::COperatorCondition>::const_iterator J = std::lower_bound(brain.target_state().conditions().begin(),brain.target_state().conditions().end(),CMotivationActionManagerStalker::CWorldProperty((*I).first,false));
		char				temp = '?';
		if ((J != brain.target_state().conditions().end()) && ((*J).condition() == (*I).first)) {
			temp			= (*J).value() ? '+' : '-';
			HUD().Font().pFontSmall->OutNext	("%s%s%s    %5c : [%d][%s]",indent,indent,indent,temp,(*I).first,_brain.property2string((*I).first));
		}
	}
}

LPCSTR animation_name(CAI_Stalker *self, const MotionID &animation)
{
	if (!animation)
		return			("");
	CSkeletonAnimated	*skeleton_animated = smart_cast<CSkeletonAnimated*>(self->Visual());
	VERIFY				(skeleton_animated);
	CMotionDef			*motion = skeleton_animated->LL_GetMotionDef(animation);
	VERIFY				(motion);
	LPCSTR				name = skeleton_animated->LL_MotionDefName_dbg(motion);
	return				(name);
}

void CAI_Stalker::OnHUDDraw				(CCustomHUD *hud)
{
	inherited::OnHUDDraw				(hud);

	if (!g_Alive())
		return;

	CActor								*actor = smart_cast<CActor*>(Level().Objects.net_Find(0));
	if (!actor)
		return;

	LPCSTR								indent = "  ";

	HUD().Font().pFontSmall->SetColor	(D3DCOLOR_XRGB(0,255,0));
	HUD().Font().pFontSmall->OutSet		(0,210);
	HUD().Font().pFontSmall->OutNext	("STALKER : %s",*cName());
	HUD().Font().pFontSmall->OutNext	(" ");
	// memory
	HUD().Font().pFontSmall->OutNext	("memory");
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

	HUD().Font().pFontSmall->OutSet		(300,210);
	// brain
	HUD().Font().pFontSmall->OutNext	(" ");
	HUD().Font().pFontSmall->OutNext	(" ");
	HUD().Font().pFontSmall->OutNext	("brain");
	// motivations
	HUD().Font().pFontSmall->OutNext	("%smotivations",indent);

	const CMotivationActionManagerStalker &brain = this->brain();
	HUD().Font().pFontSmall->OutNext	("%s%sobjects     : %d",indent,indent,brain.graph().vertices().size());
	HUD().Font().pFontSmall->OutNext	("%s%selected_id  : %d",indent,indent,brain.selected_id());
	// actions
	draw_planner						(brain,indent,"root");
	
	HUD().Font().pFontSmall->OutSet		(640,210);
	// brain
	HUD().Font().pFontSmall->OutNext	(" ");
	HUD().Font().pFontSmall->OutNext	(" ");
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

	// objects
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
}
#endif