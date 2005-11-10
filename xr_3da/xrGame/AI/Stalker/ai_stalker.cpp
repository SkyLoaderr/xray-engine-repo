////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../ai_monsters_misc.h"
#include "../../weapon.h"
#include "../../script_space.h"
#include "../../hit.h"
#include "../../phdestroyable.h"
#include "../../CharacterPhysicsSupport.h"
#include "../../script_entity_action.h"
#include "../../game_level_cross_table.h"
#include "../../game_graph.h"
#include "../../inventory.h"
#include "../../artifact.h"
#include "../../phmovementcontrol.h"
#include "../../xrserver_objects_alife_monsters.h"
#include "../../cover_evaluators.h"
#include "../../xrserver.h"
#include "../../xr_level_controller.h"
#include "../../hudmanager.h"
#include "../../clsid_game.h"
#include "../../../skeletoncustom.h"
#include "../../character_info.h"
#include "../../actor.h"
#include "../../relation_registry.h"
#include "../../stalker_animation_manager.h"
#include "../../stalker_planner.h"
#include "../../script_game_object.h"
#include "../../detail_path_manager.h"
#include "../../agent_manager.h"
#include "../../agent_corpse_manager.h"
#include "../../object_handler_planner.h"
#include "../../object_handler_space.h"
#include "../../memory_manager.h"
#include "../../sight_manager.h"
#include "../../ai_object_location.h"
#include "../../stalker_movement_manager.h"
#include "../../entitycondition.h"
#include "../../script_engine.h"
#include "ai_stalker_impl.h"
#include "../../sound_player.h"
#include "../../stalker_sound_data.h"
#include "../../stalker_sound_data_visitor.h"
#include "ai_stalker_space.h"
#include "../../mt_config.h"
#include "../../effectorshot.h"
#include "../../visual_memory_manager.h"
#include "../../enemy_manager.h"
#include "../../BoneProtections.h"
#include "../../alife_human_brain.h"
#include "../../profiler.h"

#ifdef DEBUG
#	include "../../alife_simulator.h"
#	include "../../alife_object_registry.h"
#	include "../../level.h"
#	include "../../map_location.h"
#	include "../../map_manager.h"
#endif

using namespace StalkerSpace;

#ifdef DEBUG
XRCORE_API	BOOL	g_bMEMO;
#endif

extern int g_AI_inactive_time;

CAI_Stalker::CAI_Stalker			()
{
	m_sound_user_data_visitor		= 0;
	m_movement_manager				= 0;
	m_group_behaviour				= true;
	m_boneHitProtection				= NULL;
}

CAI_Stalker::~CAI_Stalker			()
{
	xr_delete						(m_pPhysics_support);
	xr_delete						(m_animation_manager);
	xr_delete						(m_brain);
	xr_delete						(m_sight_manager);
	xr_delete						(m_weapon_shot_effector);
	xr_delete						(m_sound_user_data_visitor);
}

void CAI_Stalker::reinit			()
{
	CObjectHandler::reinit			(this);
	sight().reinit					();
	CCustomMonster::reinit			();
	animation().reinit				();
	movement().reinit				();

	//�������� ������������� �������� ����� ��� �������� �������� m_SpecificCharacter
	sound().sound_prefix			(SpecificCharacter().sound_voice_prefix());
#ifdef DEBUG
	u32									start = 0;
	if (g_bMEMO)
		start							= Memory.mem_usage();
#endif
	LoadSounds						(*cNameSect());
#ifdef DEBUG
	if (g_bMEMO)
		Msg					("CAI_Stalker::LoadSounds() : %d",Memory.mem_usage() - start);
#endif

	m_pPhysics_support->in_Init		();
	
	m_last_best_item_frame			= 0;
	m_best_item_to_kill				= 0;
	m_best_ammo						= 0;
	m_best_found_item_to_kill		= 0;
	m_best_found_ammo				= 0;
	m_item_actuality				= false;
	m_sell_info_actuality			= false;

	m_ce_close						= xr_new<CCoverEvaluatorCloseToEnemy>(&movement().restrictions());
	m_ce_far						= xr_new<CCoverEvaluatorFarFromEnemy>(&movement().restrictions());
	m_ce_best						= xr_new<CCoverEvaluatorBest>(&movement().restrictions());
	m_ce_angle						= xr_new<CCoverEvaluatorAngle>(&movement().restrictions());
	m_ce_safe						= xr_new<CCoverEvaluatorSafe>(&movement().restrictions());
	m_ce_random_game				= xr_new<CCoverEvaluatorRandomGame>(&movement().restrictions());
	m_ce_ambush						= xr_new<CCoverEvaluatorAmbush>(&movement().restrictions());
	m_ce_best_by_time				= xr_new<CCoverEvaluatorBestByTime>(&movement().restrictions());
	
	m_ce_close->set_inertia			(3000);
	m_ce_far->set_inertia			(3000);
	m_ce_best->set_inertia			(1000);
	m_ce_angle->set_inertia			(5000);
	m_ce_safe->set_inertia			(1000);
	m_ce_random_game->set_inertia	(3000);
	m_ce_ambush->set_inertia		(3000);
	m_ce_best_by_time->set_inertia	(1000);

	m_can_kill_enemy				= false;
	m_can_kill_member				= false;
	m_pick_distance					= 0.f;
	m_pick_frame_id					= 0;

	m_weapon_shot_random_seed		= s32(Level().timeServer_Async());
}

void CAI_Stalker::LoadSounds		(LPCSTR section)
{
	LPCSTR							head_bone_name = pSettings->r_string(section,"bone_head");
	sound().add						(pSettings->r_string(section,"sound_death"),					100, SOUND_TYPE_MONSTER_DYING,		0, u32(eStalkerSoundMaskDie),					eStalkerSoundDie,					head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_anomaly_death"),			100, SOUND_TYPE_MONSTER_DYING,		0, u32(eStalkerSoundMaskDieInAnomaly),			eStalkerSoundDieInAnomaly,			head_bone_name, 0);
	sound().add						(pSettings->r_string(section,"sound_hit"),						100, SOUND_TYPE_MONSTER_INJURING,	1, u32(eStalkerSoundMaskInjuring),				eStalkerSoundInjuring,				head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_friendly_fire"),			100, SOUND_TYPE_MONSTER_INJURING,	1, u32(eStalkerSoundMaskInjuringByFriend),		eStalkerSoundInjuringByFriend,		head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_panic_human"),				100, SOUND_TYPE_MONSTER_TALKING,	2, u32(eStalkerSoundMaskPanicHuman),			eStalkerSoundPanicHuman,			head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_panic_monster"),			100, SOUND_TYPE_MONSTER_TALKING,	2, u32(eStalkerSoundMaskPanicMonster),			eStalkerSoundPanicMonster,			head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_grenade_alarm"),			100, SOUND_TYPE_MONSTER_TALKING,	3, u32(eStalkerSoundMaskGrenadeAlarm),			eStalkerSoundGrenadeAlarm,			head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_friendly_grenade_alarm"),	100, SOUND_TYPE_MONSTER_TALKING,	3, u32(eStalkerSoundMaskFriendlyGrenadeAlarm),	eStalkerSoundFriendlyGrenadeAlarm,	head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_tolls"),					100, SOUND_TYPE_MONSTER_TALKING,	4, u32(eStalkerSoundMaskTolls),					eStalkerSoundTolls,					head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_alarm"),					100, SOUND_TYPE_MONSTER_TALKING,	5, u32(eStalkerSoundMaskAlarm),					eStalkerSoundAlarm,					head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_attack"),					100, SOUND_TYPE_MONSTER_TALKING,	5, u32(eStalkerSoundMaskAttack),				eStalkerSoundAttack,				head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_backup"),					100, SOUND_TYPE_MONSTER_TALKING,	5, u32(eStalkerSoundMaskBackup),				eStalkerSoundBackup,				head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_detour"),					100, SOUND_TYPE_MONSTER_TALKING,	5, u32(eStalkerSoundMaskDetour),				eStalkerSoundDetour,				head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_search"),					100, SOUND_TYPE_MONSTER_TALKING,	5, u32(eStalkerSoundMaskSearch),				eStalkerSoundSearch,				head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_humming"),					100, SOUND_TYPE_MONSTER_TALKING,	6, u32(eStalkerSoundMaskHumming),				eStalkerSoundHumming,				head_bone_name, 0);
	sound().add						(pSettings->r_string(section,"sound_need_backup"),				100, SOUND_TYPE_MONSTER_TALKING,	4, u32(eStalkerSoundMaskNeedBackup),			eStalkerSoundNeedBackup,			head_bone_name, xr_new<CStalkerSoundData>(this));
	sound().add						(pSettings->r_string(section,"sound_running_in_danger"),		100, SOUND_TYPE_MONSTER_TALKING,	6, u32(eStalkerSoundMaskMovingInDanger),		eStalkerSoundRunningInDanger,		head_bone_name, xr_new<CStalkerSoundData>(this));
//	sound().add						(pSettings->r_string(section,"sound_walking_in_danger"),		100, SOUND_TYPE_MONSTER_TALKING,	6, u32(eStalkerSoundMaskMovingInDanger),		eStalkerSoundWalkingInDanger,		head_bone_name, xr_new<CStalkerSoundData>(this));
}

void CAI_Stalker::reload			(LPCSTR section)
{
#ifdef DEBUG
	u32									start = 0;
	if (g_bMEMO)
		start							= Memory.mem_usage();
#endif
	brain().setup					(this);
#ifdef DEBUG
	if (g_bMEMO)
		Msg					("brain().setup() : %d",Memory.mem_usage() - start);
#endif

	CCustomMonster::reload			(section);
	CStepManager::reload			(section);
	CObjectHandler::reload			(section);
	inventory().m_slots[OUTFIT_SLOT].m_bUsable = false;

	sight().reload					(section);
	movement().reload				(section);

	m_disp_walk_stand				= pSettings->r_float(section,"disp_walk_stand");
	m_disp_walk_crouch				= pSettings->r_float(section,"disp_walk_crouch");
	m_disp_run_stand				= pSettings->r_float(section,"disp_run_stand");
	m_disp_run_crouch				= pSettings->r_float(section,"disp_run_crouch");
	m_disp_stand_stand				= pSettings->r_float(section,"disp_stand_stand");
	m_disp_stand_crouch				= pSettings->r_float(section,"disp_stand_crouch");
	m_disp_stand_stand_zoom			= pSettings->r_float(section,"disp_stand_stand_zoom");
	m_disp_stand_crouch_zoom		= pSettings->r_float(section,"disp_stand_crouch_zoom");
}

void CAI_Stalker::Die				(CObject* who)
{
	SelectAnimation					(XFORM().k,movement().detail().direction(),movement().speed());

	sound().set_sound_mask			(0);
	if (is_special_killer(who))
		sound().play				(eStalkerSoundDieInAnomaly);
	else
		sound().play				(eStalkerSoundDie);
	
	agent_manager().corpse().register_corpse	(this);

	inherited::Die					(who);
	m_hammer_is_clutched			= !!memory().enemy().selected() && !CObjectHandler::planner().m_storage.property(ObjectHandlerSpace::eWorldPropertyStrapped) && !::Random.randI(0,2);

	//��������� ������������� ������ � ���������
	inventory().SetSlotsUseful		(false);
}

void CAI_Stalker::Load				(LPCSTR section)
{ 
	CCustomMonster::Load			(section);
	CObjectHandler::Load			(section);
	sight().Load					(section);
	movement().Load	(section);
	
	// skeleton physics
	m_pPhysics_support->in_Load		(section);
}

BOOL CAI_Stalker::net_Spawn			(CSE_Abstract* DC)
{
#ifdef DEBUG
	u32									start = 0;
	if (g_bMEMO)
		start							= Memory.mem_usage();
#endif
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeHumanStalker			*tpHuman = smart_cast<CSE_ALifeHumanStalker*>(e);
	R_ASSERT						(tpHuman);
	m_group_behaviour				= !!tpHuman->m_flags.test(CSE_ALifeObject::flGroupBehaviour);

	if (!CObjectHandler::net_Spawn(DC) || !inherited::net_Spawn(DC))
		return						(FALSE);

	m_pPhysics_support->in_NetSpawn	(e);
	
	m_dwMoney						= tpHuman->m_dwMoney;

#ifdef DEBUG
	u32									_start = 0;
	if (g_bMEMO)
		_start							= Memory.mem_usage();
#endif
	animation().reload				(this);
#ifdef DEBUG
	if (g_bMEMO)
		Msg					("CStalkerAnimationManager::reload() : %d",Memory.mem_usage() - _start);
#endif

	movement().m_head.current.yaw	= movement().m_head.target.yaw = movement().m_body.current.yaw = movement().m_body.target.yaw	= angle_normalize_signed(-tpHuman->o_torso.yaw);
	movement().m_body.current.pitch	= movement().m_body.target.pitch	= 0;

	if (ai().game_graph().valid_vertex_id(tpHuman->m_tGraphID))
		ai_location().game_vertex		(tpHuman->m_tGraphID);

	if (ai().game_graph().valid_vertex_id(tpHuman->m_tNextGraphID) && movement().restrictions().accessible(ai().game_graph().vertex(tpHuman->m_tNextGraphID)->level_point()))
		movement().set_game_dest_vertex	(tpHuman->m_tNextGraphID);

	m_current_alife_task			= 0;

	R_ASSERT2					(
		ai().get_game_graph() && 
		ai().get_level_graph() && 
		ai().get_cross_table() && 
		(ai().level_graph().level_id() != u32(-1)),
		"There is no AI-Map, level graph, cross table, or graph is not compiled into the game graph!"
	);

	setEnabled						(TRUE);

	m_PhysicMovementControl->SetPosition	(Position());
	m_PhysicMovementControl->SetVelocity	(0,0,0);

	if (!Level().CurrentViewEntity())
		Level().SetEntity(this);

	// load damage params

//	m_tpKnownCustomers				= tpHuman->m_tpKnownCustomers;
	m_tpKnownCustomers				= tpHuman->brain().m_tpKnownCustomers;

	if (!g_Alive())
		sound().set_sound_mask(u32(eStalkerSoundMaskDie));

	//��������� ���������� �� �������� ��������
	CKinematics* pKinematics = smart_cast<CKinematics*>(Visual()); VERIFY(pKinematics);
	CInifile* ini = pKinematics->LL_UserData();
	if(ini)
	{
		if(ini->section_exist("immunities"))
		{
			LPCSTR imm_sect = ini->r_string("immunities", "immunities_sect");
			conditions().LoadImmunities(imm_sect,pSettings);
		}

		if(ini->line_exist("bone_protection","bones_protection_sect")){
			m_boneHitProtection			= xr_new<SBoneProtections>();
			m_boneHitProtection->reload	(ini->r_string("bone_protection","bones_protection_sect"), pKinematics );
		}
	}

	//��������� �������� � ����������� �� �����
	static float novice_rank_immunity			= pSettings->r_float("ranks_properties", "immunities_novice_k");
	static float expirienced_rank_immunity		= pSettings->r_float("ranks_properties", "immunities_experienced_k");

	static float novice_rank_visibility			= pSettings->r_float("ranks_properties", "visibility_novice_k");
	static float expirienced_rank_visibility	= pSettings->r_float("ranks_properties", "visibility_experienced_k");

	static float novice_rank_dispersion			= pSettings->r_float("ranks_properties", "dispersion_novice_k");
	static float expirienced_rank_dispersion	= pSettings->r_float("ranks_properties", "dispersion_experienced_k");

	
	CHARACTER_RANK_VALUE rank = Rank();
	clamp(rank, 0, 100);
	float rank_k = float(rank)/100.f;
	m_fRankImmunity = novice_rank_immunity + (expirienced_rank_immunity - novice_rank_immunity) * rank_k;
	m_fRankVisibility = novice_rank_visibility + (expirienced_rank_visibility - novice_rank_visibility) * rank_k;
	m_fRankDisperison = expirienced_rank_dispersion + (expirienced_rank_dispersion - novice_rank_dispersion) * (1-rank_k);

	if (!fis_zero(SpecificCharacter().panic_threshold()))
		m_panic_threshold						= SpecificCharacter().panic_threshold();

	sight().setup					(CSightAction(SightManager::eSightTypeCurrentDirection));

#ifdef _DEBUG
	if (!Level().MapManager().HasMapLocation("debug_stalker",ID())) {
		CMapLocation				*map_location = 
			Level().MapManager().AddMapLocation(
				"debug_stalker",
				ID()
			);

		map_location->SetHint		(cName());
	}
#endif

#ifdef DEBUG
	if (g_bMEMO) {
		Msg							("CAI_Stalker::net_Spawn() : %d",Memory.mem_usage() - start);
	}
#endif

	return							(TRUE);
}

void CAI_Stalker::net_Destroy()
{
	inherited::net_Destroy				();
	CInventoryOwner::net_Destroy		();
	m_pPhysics_support->in_NetDestroy	();

	Device.remove_from_seq_parallel	(
		fastdelegate::FastDelegate0<>(
			this,
			&CAI_Stalker::update_object_handler
		)
	);

#ifdef DEBUG
	fastdelegate::FastDelegate0<>	f = fastdelegate::FastDelegate0<>(this,&CAI_Stalker::update_object_handler);
	xr_vector<fastdelegate::FastDelegate0<> >::const_iterator	I;
	I	= std::find(Device.seqParallel.begin(),Device.seqParallel.end(),f);
	VERIFY							(I == Device.seqParallel.end());
#endif

	xr_delete							(m_ce_close);
	xr_delete							(m_ce_far);
	xr_delete							(m_ce_best);
	xr_delete							(m_ce_angle);
	xr_delete							(m_ce_safe);
	xr_delete							(m_ce_random_game);
	xr_delete							(m_ce_ambush);
	xr_delete							(m_ce_best_by_time);
	xr_delete							(m_boneHitProtection);

}

void CAI_Stalker::net_Save			(NET_Packet& P)
{
	inherited::net_Save(P);
	m_pPhysics_support->in_NetSave(P);
}

BOOL CAI_Stalker::net_SaveRelevant	()
{
	return (inherited::net_SaveRelevant() || BOOL(PPhysicsShell()!=NULL));
}

void CAI_Stalker::net_Export		(NET_Packet& P)
{
	R_ASSERT						(Local());

	// export last known packet
	R_ASSERT						(!NET.empty());
	net_update& N					= NET.back();
//	P.w_float						(inventory().TotalWeight());
//	P.w_u32							(m_dwMoney);

	P.w_float_q16					(GetfHealth(),-500,1000);

	P.w_u32							(N.dwTimeStamp);
	P.w_u8							(0);
	P.w_vec3						(N.p_pos);
	P.w_angle8						(N.o_model);
	P.w_angle8						(N.o_torso.yaw);
	P.w_angle8						(N.o_torso.pitch);
	P.w_angle8						(N.o_torso.roll);
	P.w_u8							(u8(g_Team()));
	P.w_u8							(u8(g_Squad()));
	P.w_u8							(u8(g_Group()));
	

	float					f1 = 0;
	GameGraph::_GRAPH_ID		l_game_vertex_id = ai_location().game_vertex_id();
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
//	P.w						(&f1,						sizeof(f1));
//	P.w						(&f1,						sizeof(f1));
	if (ai().game_graph().valid_vertex_id(l_game_vertex_id)) {
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.w					(&f1,						sizeof(f1));
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.w					(&f1,						sizeof(f1));
	}
	else {
		P.w					(&f1,						sizeof(f1));
		P.w					(&f1,						sizeof(f1));
	}

//.
//	ALife::ETaskState				task_state = ALife::eTaskStateChooseTask;
//	P.w								(&task_state,sizeof(task_state));
//	P.w_u32							(0);
//	P.w_u32							(0);

	P.w_stringZ						(m_sStartDialog);
}

void CAI_Stalker::net_Import		(NET_Packet& P)
{
	R_ASSERT						(Remote());
	net_update						N;

	u8 flags;

	P.r_float						();
	m_dwMoney						= P.r_u32();

	float health;
	P.r_float_q16		(health,-500,1000);
	SetfHealth			(health);
//	fEntityHealth = health;

	P.r_u32							(N.dwTimeStamp);
	P.r_u8							(flags);
	P.r_vec3						(N.p_pos);
	P.r_angle8						(N.o_model);
	P.r_angle8						(N.o_torso.yaw);
	P.r_angle8						(N.o_torso.pitch);
	P.r_angle8						(N.o_torso.roll	);
	id_Team							= P.r_u8();
	id_Squad						= P.r_u8();
	id_Group						= P.r_u8();


	GameGraph::_GRAPH_ID				graph_vertex_id = movement().game_dest_vertex_id();
	P.r								(&graph_vertex_id,		sizeof(GameGraph::_GRAPH_ID));
	graph_vertex_id					= ai_location().game_vertex_id();
	P.r								(&graph_vertex_id,		sizeof(GameGraph::_GRAPH_ID));

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back				(N);
		NET_WasInterpolating		= TRUE;
	}

	P.r_float						();
	P.r_float						();
//.
//	ALife::ETaskState				task_state;
//	P.r								(&task_state,sizeof(task_state));
//	P.r_u32							();
//	P.r_u32							();

	P.r_stringZ						(m_sStartDialog);

	setVisible						(TRUE);
	setEnabled						(TRUE);
}

void CAI_Stalker::update_object_handler	()
{
	if (!g_Alive())
		return;

	try {
		try {
			CObjectHandler::update	();
		}
#ifdef DEBUG
		catch (luabind::cast_failed &message) {
			Msg						("! Expression \"%s\" from luabind::object to %s",message.what(),message.info()->name());
			throw;
		}
#endif
		catch (std::exception &message) {
			Msg						("! Expression \"%s\"",message.what());
			throw;
		}
		catch(...) {
			throw;
		}
	}
	catch(...) {
		CObjectHandler::set_goal(eObjectActionIdle);
		CObjectHandler::update	();
	}
}

struct bug_tracker {
	CCustomMonster		*m_object;

	bug_tracker		(CCustomMonster *object)
	{
		VERIFY					(object);
		m_object				= object;
	}

	~bug_tracker	()
	{
		VERIFY2					(m_object->PPhysicsShell()||m_object->getEnabled(), *m_object->cName());
	}
};

void CAI_Stalker::UpdateCL()
{
	START_PROFILE("AI/Stalker/client_update")
	VERIFY2						(PPhysicsShell()||getEnabled(), *cName());
	bug_tracker					bug_tracker(this);

//	if (Position().distance_to(Level().CurrentEntity()->Position()) <= 50.f)
//		Msg				("[%6d][CL][%s]",Device.dwTimeGlobal,*cName());
	if (g_Alive()) {
		if (g_mt_config.test(mtObjectHandler) && CObjectHandler::planner().initialized()) {
			fastdelegate::FastDelegate0<>								f = fastdelegate::FastDelegate0<>(this,&CAI_Stalker::update_object_handler);
#ifdef DEBUG
			xr_vector<fastdelegate::FastDelegate0<> >::const_iterator	I;
			I	= std::find(Device.seqParallel.begin(),Device.seqParallel.end(),f);
			VERIFY							(I == Device.seqParallel.end());
#endif
			Device.seqParallel.push_back	(fastdelegate::FastDelegate0<>(this,&CAI_Stalker::update_object_handler));
		}
		else
			update_object_handler			();

		if	(
				(movement().speed(m_PhysicMovementControl) > EPS_L)
				&& 
				(eMovementTypeStand != movement().movement_type())
				&&
				(eMentalStateDanger == movement().mental_state())
			) {
			if	(
					(eBodyStateStand == movement().body_state())
					&&
					(eMovementTypeRun == movement().movement_type())
				) {
				sound().play	(eStalkerSoundRunningInDanger);
			}
			else {
//				sound().play	(eStalkerSoundWalkingInDanger);
			}
		}
	}

	inherited::UpdateCL				();
	
	m_pPhysics_support->in_UpdateCL	();

	if (g_Alive()) {
		VERIFY						(!m_pPhysicsShell);
		try {
			sight().update			();
		}
		catch(...) {
			sight().setup			(CSightAction(SightManager::eSightTypeCurrentDirection));
			sight().update			();
		}

		Exec_Look					(Device.fTimeDelta);

		CStepManager::update		();

		if (weapon_shot_effector().IsActive())
			weapon_shot_effector().Update	();
	}
	STOP_PROFILE
}

void CAI_Stalker::Hit(float P, Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	//��� ����� �������� � ����������� �� ����� (������� �������� ������ ����, ��� ��������)
	P *= m_fRankImmunity;
	if(m_boneHitProtection && hit_type == ALife::eHitTypeFireWound){
		float BoneArmour = m_boneHitProtection->getBoneArmour(element);	
		float NewHitPower = P - BoneArmour;
		if (NewHitPower < P*m_boneHitProtection->m_fHitFrac) P = P*m_boneHitProtection->m_fHitFrac;
		else
			P = NewHitPower;
	}
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
//	inherited::Hit(0.05f,dir,who,element,p_in_object_space,impulse,hit_type);
}

void CAI_Stalker ::PHHit				(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /*ALife::eHitTypeWound*/)
{
	m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse,hit_type,!g_Alive());
}

CPHDestroyable*		CAI_Stalker::		ph_destroyable	()						
{
	return smart_cast<CPHDestroyable*>(character_physics_support());
}

#include "../../enemy_manager.h"

void CAI_Stalker::shedule_Update		( u32 DT )
{
	START_PROFILE("AI/Stalker/schedule_update")
	VERIFY2				(getEnabled()||PPhysicsShell(), *cName());
	bug_tracker			bug_tracker(this);
//	if (Position().distance_to(Level().CurrentEntity()->Position()) <= 50.f)
//		Msg				("[%6d][SH][%s]",Device.dwTimeGlobal,*cName());
	// Queue shrink
	VERIFY				(_valid(Position()));
	u32	dwTimeCL		= Level().timeServer()-NET_Latency;
	VERIFY				(!NET.empty());
	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();

	Fvector				vNewPosition = Position();
	VERIFY				(_valid(Position()));
	// *** general stuff
	float dt			= float(DT)/1000.f;

	if (g_Alive()) {
		animation().play_delayed_callbacks	();

//		bool			check = !!memory().enemy().selected();
#if 0//def DEBUG
		memory().visual().check_visibles();
#endif
		if (g_mt_config.test(mtAiVision))
			Device.seqParallel.push_back(fastdelegate::FastDelegate0<>(this,&CCustomMonster::Exec_Visibility));
		else
			Exec_Visibility				();

		START_PROFILE("AI/Stalker/schedule_update/memory")
		process_enemies					();
		memory().update					(dt);
		STOP_PROFILE

//		if (memory().enemy().selected() && !check)
//			Msg			("Stalker %s found new enemy %s",*cName(),*memory().enemy().selected()->cName());
//		if (!memory().enemy().selected() && check)
//			Msg			("Stalker %s lost enemy",*cName());
	}
	inherited::inherited::shedule_Update(DT);
	
	if (Remote())		{
	} else {
		// here is monster AI call
		VERIFY				(_valid(Position()));
		m_fTimeUpdateDelta				= dt;
		Device.Statistic.AI_Think.Begin	();
		if (GetScriptControl())
			ProcessScripts				();
		else
#ifdef DEBUG
			if (Device.dwFrame > (spawn_time() + g_AI_inactive_time))
#endif
				Think					();
		m_dwLastUpdateTime				= Device.dwTimeGlobal;
		Device.Statistic.AI_Think.End	();
		VERIFY							(_valid(Position()));

		// Look and action streams
		float							temp = conditions().health();
		if (temp > 0) {
			VERIFY				(_valid(Position()));
//			Exec_Look				(dt);
			VERIFY				(_valid(Position()));

			//////////////////////////////////////
			Fvector C; float R;
			//////////////////////////////////////
			// � ����� - ����!!!! (���� :-))))
			// m_PhysicMovementControl->GetBoundingSphere	(C,R);
			//////////////////////////////////////
			Center(C);
			R = Radius();
			//////////////////////////////////////
			feel_touch_update		(C,R);

			net_update				uNext;
			uNext.dwTimeStamp		= Level().timeServer();
			uNext.o_model			= movement().m_body.current.yaw;
			uNext.o_torso			= movement().m_head.current;
			uNext.p_pos				= vNewPosition;
			uNext.fHealth			= GetfHealth();
			NET.push_back			(uNext);
		}
		else 
		{
			net_update			uNext;
			uNext.dwTimeStamp	= Level().timeServer();
			uNext.o_model		= movement().m_body.current.yaw;
			uNext.o_torso		= movement().m_head.current;
			uNext.p_pos			= vNewPosition;
			uNext.fHealth		= GetfHealth();
			NET.push_back		(uNext);
		}
	}
	VERIFY				(_valid(Position()));

	UpdateInventoryOwner(DT);

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		smart_cast<CSE_ALifeHumanStalker*>(ai().alife().objects().object(ID()))->check_inventory_consistency();
	}
#endif
	
	VERIFY				(_valid(Position()));
	m_pPhysics_support->in_shedule_Update(DT);
	VERIFY				(_valid(Position()));
	STOP_PROFILE
}

float CAI_Stalker::Radius() const
{ 
	float R		= inherited::Radius();
	CWeapon* W	= smart_cast<CWeapon*>(inventory().ActiveItem());
	if (W) R	+= W->Radius();
	return R;
}

bool CAI_Stalker::use_model_pitch	() const
{
	return					(false);
}

void CAI_Stalker::spawn_supplies	()
{
	inherited::spawn_supplies			();
	CObjectHandler::spawn_supplies	();
}

void CAI_Stalker::Think			()
{
	START_PROFILE("AI/Stalker/schedule_update/think")
	u32							update_delta = Device.dwTimeGlobal - m_dwLastUpdateTime;
	
	START_PROFILE("AI/Stalker/schedule_update/think/brain")
	try {
		try {
			brain().update			(update_delta);
		}
#ifdef DEBUG
		catch (luabind::cast_failed &message) {
			Msg						("! Expression \"%s\" from luabind::object to %s",message.what(),message.info()->name());
			throw;
		}
#endif
		catch (std::exception &message) {
			Msg						("! Expression \"%s\"",message.what());
			throw;
		}
		catch (...) {
			throw;
		}
	}
	catch(...) {
#ifdef DEBUG
		Msg						("! Last action being executed : %s",brain().current_action().m_action_name);
#endif
		brain().setup			(this);
		brain().update			(update_delta);
	}
	STOP_PROFILE

	START_PROFILE("AI/Stalker/schedule_update/think/movement")
	if (!g_Alive())
		return;

	try {
		movement().update		(update_delta);
	}
	catch(...) {
#ifdef DEBUG
		Msg						("! Last action being executed : %s",brain().current_action().m_action_name);
#endif
		movement().initialize	();
		movement().update		(update_delta);
	}
	STOP_PROFILE
	STOP_PROFILE
}

void CAI_Stalker::save (NET_Packet &output_packet)
{
	inherited::save(output_packet);
	CInventoryOwner::save(output_packet);
}

void CAI_Stalker::load (IReader &input_packet)		
{
	inherited::load(input_packet);
	CInventoryOwner::load(input_packet);
}

void CAI_Stalker::SelectAnimation(const Fvector &view, const Fvector &move, float speed)
{
	if (!Device.Pause())
		animation().update();
}

const SRotation CAI_Stalker::Orientation	() const
{
	return		(movement().m_head.current);
};
const MonsterSpace::SBoneRotation &CAI_Stalker::head_orientation	() const
{
	return		(movement().head_orientation());
}

void CAI_Stalker::net_Relcase				(CObject*	 O)
{
	inherited::net_Relcase				(O);

	sight().remove_links				(O);

	if (!g_Alive())
		return;

	agent_manager().remove_links		(O);
}

CMovementManager *CAI_Stalker::create_movement_manager	()
{
	return	(m_movement_manager = xr_new<CStalkerMovementManager>(this));
}

CSound_UserDataVisitor *CAI_Stalker::create_sound_visitor		()
{
	return	(m_sound_user_data_visitor	= xr_new<CStalkerSoundDataVisitor>(this));
}

CMemoryManager *CAI_Stalker::create_memory_manager		()
{
	return	(xr_new<CMemoryManager>(this,create_sound_visitor()));
}

DLL_Pure *CAI_Stalker::_construct			()
{
#ifdef DEBUG
	u32									start = 0;
	if (g_bMEMO)
		start							= Memory.mem_usage();
#endif

	CCustomMonster::_construct			();
	CObjectHandler::_construct			();
	CStepManager::_construct			();

	m_pPhysics_support					= xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etStalker,this);
	m_actor_relation_flags.zero			();
	m_animation_manager					= xr_new<CStalkerAnimationManager>();
	m_brain								= xr_new<CStalkerPlanner>();
	m_sight_manager						= xr_new<CSightManager>(this);
	m_weapon_shot_effector				= xr_new<CWeaponShotEffector>();

#ifdef DEBUG
	if (g_bMEMO)
		Msg								("CAI_Stalker::_construct() : %d",Memory.mem_usage() - start);
#endif

	return								(this);
}

bool CAI_Stalker::use_center_to_aim		() const
{
	return								(movement().body_state() != eBodyStateCrouch);
}

void CAI_Stalker::UpdateCamera			()
{
	float								new_range = eye_range, new_fov = eye_fov;
	Fvector								temp = eye_matrix.k;
	if (g_Alive()) {
		update_range_fov				(new_range, new_fov, memory().visual().current_state().m_max_view_distance*eye_range, eye_fov);
		if (weapon_shot_effector().IsActive())
			temp						= weapon_shot_effector_direction(temp);
	}

	g_pGameLevel->Cameras.Update		(eye_matrix.c,temp,eye_matrix.j,new_fov,.75f,new_range);
}
