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
#include "../../motivation_action_manager_stalker.h"
#include "../../script_game_object.h"
#include "../../detail_path_manager.h"

extern int g_AI_inactive_time;

CAI_Stalker::CAI_Stalker			()
{
	init							();
	InitTrade						();
	m_pPhysics_support				= xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etStalker,this);
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::ai);

	m_actor_relation_flags.zero		();

	m_animation_manager				= xr_new<CStalkerAnimationManager>();
	CStepManager::init_external		(this);
	m_brain							= xr_new<CMotivationActionManagerStalker>();
}

CAI_Stalker::~CAI_Stalker			()
{
	xr_delete						(m_pPhysics_support);
	xr_delete						(m_animation_manager);
	xr_delete						(m_brain);
}

void CAI_Stalker::init()
{
	shedule.t_min					= 1;
	shedule.t_max					= 200;
	m_demo_mode						= false;
}

void CAI_Stalker::reinit			()
{
	CSSetupManager::reinit			(this);
	CObjectHandler::reinit			(this);
	CSightManager::reinit			(this);
	CCustomMonster::reinit			();
	animation_manager().reinit		();
	CStalkerMovementManager::reinit	();
	CStepManager::reinit			();

	m_pPhysics_support->in_Init		();
	
	m_last_best_item_frame			= 0;
	m_best_item_to_kill				= 0;
	m_best_ammo						= 0;
	m_best_found_item_to_kill		= 0;
	m_best_found_ammo				= 0;
	m_item_actuality				= false;
	m_ce_close						= xr_new<CCoverEvaluatorCloseToEnemy>(this);
	m_ce_far						= xr_new<CCoverEvaluatorFarFromEnemy>(this);
	m_ce_best						= xr_new<CCoverEvaluatorBest>(this);
	m_ce_angle						= xr_new<CCoverEvaluatorAngle>(this);
	m_ce_safe						= xr_new<CCoverEvaluatorSafe>(this);
	m_ce_random_game				= xr_new<CCoverEvaluatorRandomGame>(this);
	m_ce_ambush						= xr_new<CCoverEvaluatorAmbush>(this);
	m_ce_close->set_inertia			(3000);
	m_ce_far->set_inertia			(3000);
	m_ce_best->set_inertia			(1000);
	m_ce_angle->set_inertia			(0);
	m_ce_safe->set_inertia			(1000);
	m_ce_random_game->set_inertia	(3000);
	m_ce_ambush->set_inertia		(3000);

	m_not_enough_food				= false;
	m_can_buy_food					= false;
	m_not_enough_medikits			= false;
	m_can_buy_medikits				= false;
	m_no_or_bad_weapon				= false;
	m_can_buy_weapon				= false;
	m_not_enough_ammo				= false;
	m_can_buy_ammo					= false;
	m_last_alife_motivations_update	= 0;
}

void CAI_Stalker::LoadSounds		(LPCSTR section)
{
	LPCSTR							head_bone_name = pSettings->r_string(section,"bone_head");
	CSoundPlayer::add				(pSettings->r_string(section,"sound_death"),		100, SOUND_TYPE_MONSTER_DYING,		0, u32(eStalkerSoundMaskDie),				eStalkerSoundDie,				head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_anomaly_death"),100, SOUND_TYPE_MONSTER_DYING,		0, u32(eStalkerSoundMaskDieInAnomaly),		eStalkerSoundDieInAnomaly,		head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_hit"),			100, SOUND_TYPE_MONSTER_INJURING,	1, u32(eStalkerSoundMaskInjuring),			eStalkerSoundInjuring,			head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_humming"),		100, SOUND_TYPE_MONSTER_TALKING,	4, u32(eStalkerSoundMaskHumming),			eStalkerSoundHumming,			head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_alarm"),		100, SOUND_TYPE_MONSTER_TALKING,	3, u32(eStalkerSoundMaskAlarm),				eStalkerSoundAlarm,				head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_attack"),		100, SOUND_TYPE_MONSTER_TALKING,	3, u32(eStalkerSoundMaskAttack),			eStalkerSoundAttack,			head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_backup"),		100, SOUND_TYPE_MONSTER_TALKING,	3, u32(eStalkerSoundMaskBackup),			eStalkerSoundBackup,			head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_detour"),		100, SOUND_TYPE_MONSTER_TALKING,	3, u32(eStalkerSoundMaskDetour),			eStalkerSoundDetour,			head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_search"),		100, SOUND_TYPE_MONSTER_TALKING,	3, u32(eStalkerSoundMaskSearch),			eStalkerSoundSearch,			head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_friendly_fire"),100, SOUND_TYPE_MONSTER_INJURING,	1, u32(eStalkerSoundMaskInjuringByFriend),	eStalkerSoundInjuringByFriend,	head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_panic_human"),	100, SOUND_TYPE_MONSTER_TALKING,	2, u32(eStalkerSoundMaskPanicHuman),		eStalkerSoundPanicHuman,		head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_panic_monster"),100, SOUND_TYPE_MONSTER_TALKING,	2, u32(eStalkerSoundMaskPanicMonster),		eStalkerSoundPanicMonster,		head_bone_name);
}

void CAI_Stalker::load_killer_clsids(LPCSTR section)
{
	m_killer_clsids.clear			();
	LPCSTR							killers = pSettings->r_string(section,"killer_clsids");
	string16						temp;
	for (u32 i=0, n=_GetItemCount(killers); i<n; ++i)
		m_killer_clsids.push_back	(TEXT2CLSID(_GetItem(killers,i,temp)));
}

void CAI_Stalker::reload			(LPCSTR section)
{
	brain().setup					(this);
	CCustomMonster::reload			(section);
	
	CObjectHandler::reload			(section);
	inventory().m_slots[OUTFIT_SLOT].m_bUsable = false;

//	CSightManager::reload			(section);
	CStalkerMovementManager::reload	(section);

	m_disp_walk_stand				= pSettings->r_float(section,"disp_walk_stand");
	m_disp_walk_crouch				= pSettings->r_float(section,"disp_walk_crouch");
	m_disp_run_stand				= pSettings->r_float(section,"disp_run_stand");
	m_disp_run_crouch				= pSettings->r_float(section,"disp_run_crouch");
	m_disp_stand_stand				= pSettings->r_float(section,"disp_stand_stand");
	m_disp_stand_crouch				= pSettings->r_float(section,"disp_stand_crouch");
	
	m_r_hand						= smart_cast<CKinematics*>(Visual())->LL_BoneID(pSettings->r_string(section,"weapon_bone0"));
	m_l_finger1						= smart_cast<CKinematics*>(Visual())->LL_BoneID(pSettings->r_string(section,"weapon_bone1"));
	m_r_finger2						= smart_cast<CKinematics*>(Visual())->LL_BoneID(pSettings->r_string(section,"weapon_bone2"));

	m_panic_threshold				= pSettings->r_float(section,"panic_threshold");

	load_killer_clsids				(section);
}

void CAI_Stalker::Die				(CObject* who)
{
	SelectAnimation					(XFORM().k,detail_path_manager().direction(),speed());

	set_sound_mask					(0);
	if (!who || (std::find(m_killer_clsids.begin(),m_killer_clsids.end(),who->SUB_CLS_ID) == m_killer_clsids.end()))
		play						(eStalkerSoundDie);
	else
		play						(eStalkerSoundDieInAnomaly);
	
	inherited::Die					(who);
	m_hammer_is_clutched			= !::Random.randI(0,2);

	//запретить использование слотов в инвенторе
	inventory().SetSlotsUseful		(false);
}

void CAI_Stalker::Load				(LPCSTR section)
{ 
	setEnabled						(false);
	
	CCustomMonster::Load			(section);
	CObjectHandler::Load			(section);
	CSightManager::Load				(section);
	CStalkerMovementManager::Load	(section);
	CStepManager::load				(section);
	
	// skeleton physics
	m_pPhysics_support->in_Load		(section);
	m_demo_mode						= false;
}

BOOL CAI_Stalker::net_Spawn			(LPVOID DC)
{
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeHumanStalker			*tpHuman = smart_cast<CSE_ALifeHumanStalker*>(e);
	R_ASSERT						(tpHuman);
	m_demo_mode						= !!tpHuman->m_demo_mode;

	if (!inherited::net_Spawn(DC) || 
		!CObjectHandler::net_Spawn(DC) ||
		!CStalkerMovementManager::net_Spawn(DC)
		)
		return						(FALSE);

	m_pPhysics_support->in_NetSpawn	(e);
	
	m_dwMoney						= tpHuman->m_dwMoney;

	animation_manager().reload		(this);

	m_head.current.yaw = m_head.target.yaw = m_body.current.yaw = m_body.target.yaw	= angle_normalize_signed(-tpHuman->o_Angle.y);
	m_body.current.pitch			= m_body.target.pitch	= 0;

	if (ai().game_graph().valid_vertex_id(tpHuman->m_tGraphID))
		set_game_vertex				(tpHuman->m_tGraphID);
	if (ai().game_graph().valid_vertex_id(tpHuman->m_tNextGraphID) && accessible(ai().game_graph().vertex(tpHuman->m_tNextGraphID)->level_point()))
		set_game_dest_vertex		(tpHuman->m_tNextGraphID);

	m_current_alife_task			= 0;

	if (!m_demo_mode) {
		R_ASSERT2					(
			ai().get_game_graph() && 
			ai().get_level_graph() && 
			ai().get_cross_table() && 
			(ai().level_graph().level_id() != u32(-1)),
			"There is no AI-Map, level graph, cross table, or graph is not compiled into the game graph!"
		);
	}

	setEnabled						(true);

	m_PhysicMovementControl->SetPosition	(Position());
	m_PhysicMovementControl->SetVelocity	(0,0,0);

	if (!Level().CurrentViewEntity())
		Level().SetEntity(this);

	// load damage params

	m_tpKnownCustomers				= tpHuman->m_tpKnownCustomers;

	if (!g_Alive())
		CSoundPlayer::set_sound_mask(u32(eStalkerSoundMaskDie));

	//загрузить иммунитеты из модельки сталкера
	CKinematics* pKinematics = smart_cast<CKinematics*>(Visual()); VERIFY(pKinematics);
	CInifile* ini = pKinematics->LL_UserData();
	if(ini)
	{
		if(ini->section_exist("immunities"))
		{
			LPCSTR imm_sect = ini->r_string("immunities", "immunities_sect");
			InitImmunities(imm_sect,pSettings);
		}
	}

	//вычислить иммунета в зависимости от ранга
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
	m_fRankDisperison = novice_rank_immunity + (expirienced_rank_dispersion - novice_rank_dispersion) * rank_k;

	//загрузка спецевической звуковой схемы для сталкера согласно m_SpecificCharacter
	LPCSTR snd_sound_sect = SpecificCharacter().SndConfigSect();
	if(snd_sound_sect && pSettings->section_exist(snd_sound_sect))
		LoadSounds(snd_sound_sect);
	else
		LoadSounds(*cNameSect());

	return							(TRUE);
}

void CAI_Stalker::net_Destroy()
{
	inherited::net_Destroy				();
	CInventoryOwner::net_Destroy		();
	CStalkerMovementManager::net_Destroy();
	m_pPhysics_support->in_NetDestroy	();

	xr_delete							(m_ce_close);
	xr_delete							(m_ce_far);
	xr_delete							(m_ce_best);
	xr_delete							(m_ce_angle);
	xr_delete							(m_ce_safe);
	xr_delete							(m_ce_ambush);
}


void CAI_Stalker::net_Save			(NET_Packet& P)
{
	inherited::net_Save(P);
	m_pPhysics_support->in_NetSave(P);
}

BOOL CAI_Stalker::net_SaveRelevant	()
{
	return BOOL(PPhysicsShell()!=NULL);
}
void CAI_Stalker::net_Export		(NET_Packet& P)
{
	R_ASSERT						(Local());

	// export last known packet
	R_ASSERT						(!NET.empty());
	net_update& N					= NET.back();
	P.w_float						(inventory().TotalWeight());
	P.w_u32							(m_dwMoney);

	P.w_float_q16					(fEntityHealth,-500,1000);

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
	ALife::_GRAPH_ID		l_game_vertex_id = game_vertex_id();
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

	ALife::ETaskState				task_state = ALife::eTaskStateChooseTask;
	P.w								(&task_state,sizeof(task_state));
	P.w_u32							(0);
	P.w_u32							(0);
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
	fEntityHealth = health;

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


	ALife::_GRAPH_ID				graph_vertex_id = game_dest_vertex_id();
	P.r								(&graph_vertex_id,		sizeof(ALife::_GRAPH_ID));
	graph_vertex_id					= game_vertex_id();
	P.r								(&graph_vertex_id,		sizeof(ALife::_GRAPH_ID));

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back				(N);
		NET_WasInterpolating		= TRUE;
	}

	P.r_float						();
	P.r_float						();

	ALife::ETaskState				task_state;
	P.r								(&task_state,sizeof(task_state));
	P.r_u32							();
	P.r_u32							();

	setVisible						(TRUE);
	setEnabled						(TRUE);
}

void CAI_Stalker::UpdateCL()
{
	inherited::UpdateCL				();
	m_pPhysics_support->in_UpdateCL	();

	if (g_Alive()) {
		VERIFY						(!m_pPhysicsShell);
//		float						s_k		= ((eBodyStateCrouch == body_state()) ? CROUCH_SOUND_FACTOR : 1.f);
//		float						s_vol	= s_k*((eMovementTypeRun == movement_type()) ? 1.f : ACCELERATED_SOUND_FACTOR);
//		float						step_time = !fis_zero(CMovementManager::speed()) ? .725f/CMovementManager::speed() : 1.f;
//		CMaterialManager::update	(Device.fTimeDelta,1.f+0*s_vol,step_time,!!fis_zero(speed()));
		CSightManager::update		();
		CObjectHandler::update		();
		Exec_Look					(Device.fTimeDelta);
		CStepManager::update		();
	}

#ifdef DEBUG
	if (this->ID() == Level().CurrentViewEntity()->ID())
		Exec_Visibility				();
#endif
}

void CAI_Stalker::Hit(float P, Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	//хит может меняться в зависимости от ранга (новички получают больше хита, чем ветераны)
	P *= m_fRankImmunity;
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
//	inherited::Hit(0.05f,dir,who,element,p_in_object_space,impulse,hit_type);
}

void CAI_Stalker ::PHHit				(float P,Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /*ALife::eHitTypeWound*/)
{
	m_pPhysics_support->in_Hit(P,dir,element,p_in_object_space,impulse,hit_type,!g_Alive());
}

CPHDestroyable*		CAI_Stalker::		ph_destroyable	()						
{
	return smart_cast<CPHDestroyable*>(character_physics_support());
}

void CAI_Stalker::shedule_Update		( u32 DT )
{
	// Queue shrink
	VERIFY				(_valid(Position()));
	u32	dwTimeCL		= Level().timeServer()-NET_Latency;
	VERIFY				(!NET.empty());
	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();

	Fvector				vNewPosition = Position();
	VERIFY				(_valid(Position()));
	// *** general stuff
	float dt			= float(DT)/1000.f;

	if (g_Alive())
		CMemoryManager::update			(dt);
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
			if (!m_demo_mode && (Device.dwFrame > spawn_time() + g_AI_inactive_time))
				Think					();
		m_dwLastUpdateTime				= Level().timeServer();
		Device.Statistic.AI_Think.End	();
		Engine.Sheduler.Slice			();
		VERIFY				(_valid(Position()));

		// Look and action streams
		if (fEntityHealth>0) {
			VERIFY				(_valid(Position()));
//			Exec_Look				(dt);
			VERIFY				(_valid(Position()));
			Exec_Visibility			();

			//////////////////////////////////////
			Fvector C; float R;
			//////////////////////////////////////
			// С Олеся - ПИВО!!!! (Диме :-))))
			// m_PhysicMovementControl->GetBoundingSphere	(C,R);
			//////////////////////////////////////
			Center(C);
			R = Radius();
			//////////////////////////////////////
			feel_touch_update		(C,R);

			net_update				uNext;
			uNext.dwTimeStamp		= Level().timeServer();
			uNext.o_model			= m_body.current.yaw;
			uNext.o_torso			= m_head.current;
			uNext.p_pos				= vNewPosition;
			uNext.fHealth			= fEntityHealth;
			NET.push_back			(uNext);
		}
		else 
		{
			net_update			uNext;
			uNext.dwTimeStamp	= Level().timeServer();
			uNext.o_model		= m_body.current.yaw;
			uNext.o_torso		= m_head.current;
			uNext.p_pos			= vNewPosition;
			uNext.fHealth		= fEntityHealth;
			NET.push_back		(uNext);
		}
	}
	VERIFY				(_valid(Position()));

	// inventory update
	if (GetLevelDeathTime() && (inventory().TotalWeight() > 0)) {
		CWeapon *tpWeapon = smart_cast<CWeapon*>(inventory().ActiveItem());
		if (!tpWeapon || !tpWeapon->GetAmmoElapsed() || !hammer_is_clutched() || (Level().timeServer() - GetLevelDeathTime() > 500)) {
			xr_vector<CInventorySlot>::iterator I = inventory().m_slots.begin(), B = I;
			xr_vector<CInventorySlot>::iterator E = inventory().m_slots.end();
			for ( ; I != E; ++I)
				if ((I - B) == (int)inventory().GetActiveSlot()) {
					if ((*I).m_pIItem && (*I).m_pIItem->SUB_CLS_ID != CLSID_IITEM_BOLT)
						(*I).m_pIItem->Drop();
				}
				else
					if((*I).m_pIItem)
						if ((*I).m_pIItem->SUB_CLS_ID != CLSID_IITEM_BOLT)
							inventory().Ruck((*I).m_pIItem);

//			///!!!
//			TIItemList &l_list = inventory().m_ruck;
//			for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
//			{
//				CArtefact* pArtefact = smart_cast<CArtefact*>(*l_it);
//				if(pArtefact)
//					pArtefact->Drop();
//
//				//if ((*l_it)->Useful())
//					//(*l_it)->Drop();
//			}
		}
		else {
			inventory().Action(kWPN_FIRE,	CMD_START);
			xr_vector<CInventorySlot>::iterator I = inventory().m_slots.begin(), B = I;
			xr_vector<CInventorySlot>::iterator E = inventory().m_slots.end();
			for ( ; I != E; ++I)
				if ((I - B) != (int)inventory().GetActiveSlot())
					if ((*I).m_pIItem && ((*I).m_pIItem->SUB_CLS_ID != CLSID_IITEM_BOLT))//(*I).m_pIItem &&  added!!??
						inventory().Ruck((*I).m_pIItem);
			//		(*I).m_pIItem->Drop();
			
			/*TIItemList &l_list = inventory().m_ruck;
			for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
				if ((*l_it)->Useful())
					(**l_it).Drop();*/
		}
	}
	UpdateInventoryOwner(DT);
	
	VERIFY				(_valid(Position()));
	m_pPhysics_support->in_shedule_Update(DT);
	VERIFY				(_valid(Position()));
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

#ifdef DEBUG
void CAI_Stalker::OnRender			()
{
	inherited::OnRender		();

	{
		Fvector					c0 = Position(),c1,t0 = Position(),t1;
		c0.y					+= 2.f;
		c1.setHP				(-m_body.current.yaw,-m_body.current.pitch);
		c1.add					(c0);
		RCache.dbg_DrawLINE		(Fidentity,c0,c1,D3DCOLOR_XRGB(0,255,0));
		
		t0.y					+= 2.f;
		t1.setHP				(-m_body.target.yaw,-m_body.target.pitch);
		t1.add					(t0);
		RCache.dbg_DrawLINE		(Fidentity,t0,t1,D3DCOLOR_XRGB(255,0,0));
	}

	if (!psAI_Flags.is(aiVision))
		return;

	if (!smart_cast<CGameObject*>(Level().CurrentEntity()))
		return;

	Fvector						shift;
	shift.set					(0.f,2.5f,0.f);

	Fmatrix						res;
	res.mul						(Device.mFullTransform,XFORM());

	Fvector4					v_res;

	res.transform				(v_res,shift);

	if (v_res.z < 0 || v_res.w < 0)
		return;

	if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f)
		return;

	float						x = (1.f + v_res.x)/2.f * (Device.dwWidth);
	float						y = (1.f - v_res.y)/2.f * (Device.dwHeight);

	CNotYetVisibleObject		*object = not_yet_visible_object(smart_cast<CGameObject*>(Level().CurrentEntity()));
	string64					out_text;
	sprintf						(out_text,"%.2f",object ? object->m_value : 0.f);

	HUD().pFontMedium->SetColor	(D3DCOLOR_XRGB(255,0,0));
	HUD().pFontMedium->OutSet	(x,y);
	HUD().pFontMedium->OutNext	(out_text);
}
#endif

void CAI_Stalker::Think			()
{
	brain().update					(Level().timeServer() - m_dwLastUpdateTime);
	CStalkerMovementManager::update	(Level().timeServer() - m_dwLastUpdateTime);
	CSSetupManager::update			();
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
	animation_manager().update();
}
