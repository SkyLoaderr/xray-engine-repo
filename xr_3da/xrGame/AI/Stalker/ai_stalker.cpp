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
#include "../../ai_script_actions.h"
#include "../../game_level_cross_table.h"
#include "../../game_graph.h"
#include "../../inventory.h"
#include "../../artifact.h"
#include "../../phmovementcontrol.h"
#include "../../xrserver_objects_alife_monsters.h"
#include "../../cover_evaluators.h"

#ifdef OLD_DECISION_BLOCK
CAI_Stalker::CAI_Stalker			() : CStateManagerStalker("StalkerManager")
#else
CAI_Stalker::CAI_Stalker			()
#endif
{
	Init							();
	InitTrade						();
	m_pPhysics_support				= xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etStalker,this);
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
}

CAI_Stalker::~CAI_Stalker			()
{
	xr_delete						(m_pPhysics_support);
}

void CAI_Stalker::Init()
{
	shedule.t_min					= 200;
	shedule.t_max					= 1;
	m_dwParticularState				= u32(-1);
}

void CAI_Stalker::reinit			()
{
	m_pPhysics_support->in_NetSpawn	();
	CCustomMonster::reinit			();
#ifdef OLD_OBJECT_HANDLER
	CObjectHandler::reinit			(this);
#else
	CObjectHandlerGOAP::reinit		(this);
#endif
	CSightManager::reinit			(this);
	CStalkerAnimations::reinit		();
	CStalkerMovementManager::reinit	();
#ifdef OLD_DECISION_BLOCK
	CStateManagerStalker::reinit	(this);
#else
	CMotivationActionManagerStalker::reinit(this,false);
#endif
	CScriptBinder::reinit			();
	CSSetupManager::reinit			(this);

	m_pPhysics_support->in_Init		();
	
	m_tTaskState					= ALife::eTaskStateChooseTask;

	m_r_hand						= PKinematics(Visual())->LL_BoneID("bip01_r_hand");
	m_l_finger1						= PKinematics(Visual())->LL_BoneID("bip01_l_finger1");
	m_r_finger2						= PKinematics(Visual())->LL_BoneID("bip01_r_finger2");

	m_last_best_item_frame			= 0;
	m_best_item_to_kill				= 0;
	m_best_ammo						= 0;
	m_best_found_item_to_kill		= 0;
	m_best_found_ammo				= 0;
	m_item_actuality				= false;
	m_ce_close						= xr_new<CCoverEvaluatorCloseToEnemy>();
	m_ce_far						= xr_new<CCoverEvaluatorFarFromEnemy>();
	m_ce_best						= xr_new<CCoverEvaluatorBest>();
	m_ce_angle						= xr_new<CCoverEvaluatorAngle>();
	m_ce_close->set_inertia			(3000);
	m_ce_far->set_inertia			(3000);
	m_ce_best->set_inertia			(1000);
	m_ce_angle->set_inertia			(5000);
}

void CAI_Stalker::reload			(LPCSTR section)
{
	CCustomMonster::reload			(section);
	CInventoryOwner::reload			(section);
#ifndef OLD_OBJECT_HANDLER
	CObjectHandlerGOAP::reload		(section);
#endif
//	CSightManager::reload			(section);
//	CStalkerAnimations::reload		(section);
	CStalkerMovementManager::reload	(section);
#ifdef OLD_DECISION_BLOCK
	CStateManagerStalker::reload	(section);
#else
	CMotivationActionManagerStalker::reload(section);
#endif
	CScriptBinder::reload			(section);
}

void CAI_Stalker::Die				()
{
	SelectAnimation					(XFORM().k,direction(),speed());

	play							(eStalkerSoundDie);
	inherited::Die					();
	m_bHammerIsClutched				= !::Random.randI(0,2);

	//запретить использование слотов в инвенторе
	inventory().SetSlotsUseful		(false);
}

void CAI_Stalker::Load				(LPCSTR section)
{ 
	setEnabled						(false);
	
	CCustomMonster::Load			(section);
#ifdef OLD_OBJECT_HANDLER
	CObjectHandler::Load			(section);
#else
	CObjectHandlerGOAP::Load		(section);
#endif
	CSightManager::Load				(section);
	CStalkerMovementManager::Load	(section);
#ifdef OLD_DECISION_BLOCK
	CStateManagerStalker::Load		(section);
#else
	CMotivationActionManagerStalker::Load	(section);
#endif
	CScriptBinder::Load				(section);

	CSelectorManager::add<
		CVertexEvaluator<
			aiSearchRange | aiEnemyDistance
		>
	>								(section,"selector_free_hunting");
	CSelectorManager::add<
		CVertexEvaluator<
			aiSearchRange | aiCoverFromEnemyWeight
		>
	>								(section,"selector_reload");
	CSelectorManager::add<
		CVertexEvaluator<
			aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance
		>
	>								(section,"selector_retreat");
	CSelectorManager::add<
		CVertexEvaluator<
			aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance | aiEnemyViewDeviationWeight
		>
	>								(section,"selector_cover");

	LPCSTR							head_bone_name = pSettings->r_string(section,"bone_head");
	CSoundPlayer::add				(pSettings->r_string(section,"sound_death"),	100, SOUND_TYPE_MONSTER_DYING,		0, u32(eStalkerSoundMaskDie),		eStalkerSoundDie,		head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_hit"),		100, SOUND_TYPE_MONSTER_INJURING,	1, u32(eStalkerSoundMaskInjuring),	eStalkerSoundInjuring,	head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_humming"),	100, SOUND_TYPE_MONSTER_TALKING,	4, u32(eStalkerSoundMaskHumming),	eStalkerSoundHumming,	head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_alarm"),	100, SOUND_TYPE_MONSTER_TALKING,	3, u32(eStalkerSoundMaskAlarm),		eStalkerSoundAlarm,		head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_surrender"),100, SOUND_TYPE_MONSTER_TALKING,	3, u32(eStalkerSoundMaskSurrender),	eStalkerSoundSurrender,	head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_backup"),	100, SOUND_TYPE_MONSTER_TALKING,	3, u32(eStalkerSoundMaskBackup),	eStalkerSoundBackup,	head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_attack"),	100, SOUND_TYPE_MONSTER_TALKING,	3, u32(eStalkerSoundMaskAttack),	eStalkerSoundAttack,	head_bone_name);

	// skeleton physics
	m_pPhysics_support->in_Load		(section);
	
	if (pSettings->line_exist(section,"State"))
		m_dwParticularState			= pSettings->r_u32(section,"State");
}

BOOL CAI_Stalker::net_Spawn			(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return						(FALSE);
	if (!CInventoryOwner::net_Spawn(DC))
		return						(FALSE);

	if (!CScriptBinder::net_Spawn(DC))
		return						(FALSE);

	CStalkerAnimations::reload		(Visual(),pSettings,*cNameSect());

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeHumanAbstract			*tpHuman = dynamic_cast<CSE_ALifeHumanAbstract*>(e);
	R_ASSERT						(tpHuman);

	m_head.current.yaw = m_head.target.yaw = m_body.current.yaw = m_body.target.yaw	= angle_normalize_signed(-tpHuman->o_Angle.y);
	m_body.current.pitch			= m_body.target.pitch	= 0;

	if (ai().game_graph().valid_vertex_id(tpHuman->m_tGraphID))
		set_game_vertex				(tpHuman->m_tGraphID);
	if (ai().game_graph().valid_vertex_id(tpHuman->m_tNextGraphID))
		set_game_dest_vertex		(tpHuman->m_tNextGraphID);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (u32(-1) == m_dwParticularState) {
		R_ASSERT2					(
			ai().get_game_graph() && 
			ai().get_level_graph() && 
			ai().get_cross_table() && 
			(ai().level_graph().level_id() != u32(-1)),
			"There is no AI-Map, level graph, cross table, or graph is not compiled into the game graph!"
		);
		set_game_vertex				(ai().cross_table().vertex(level_vertex_id()).game_vertex_id());
		set_game_dest_vertex		(ai().cross_table().vertex(level_vertex_id()).game_vertex_id());
	}
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	setEnabled						(true);

	m_PhysicMovementControl->SetPosition	(Position());
	m_PhysicMovementControl->SetVelocity	(0,0,0);

	if (!Level().CurrentViewEntity())
		Level().SetEntity(this);

	// load damage params

	m_tpKnownCustomers				= tpHuman->m_tpKnownCustomers;

	return							(TRUE);
}

void CAI_Stalker::net_Destroy()
{
	inherited::net_Destroy				();
	CInventoryOwner::net_Destroy		();
	CScriptBinder::net_Destroy			();
	m_pPhysics_support->in_NetDestroy	();

	xr_delete							(m_ce_close);
	xr_delete							(m_ce_far);
	xr_delete							(m_ce_best);
	xr_delete							(m_ce_angle);
}

void CAI_Stalker::net_Export		(NET_Packet& P)
{
	R_ASSERT						(Local());

	// export last known packet
	R_ASSERT						(!NET.empty());
	net_update& N					= NET.back();
	P.w_float						(inventory().TotalWeight());
	P.w_u32							(0);
	P.w_u32							(0);

	P.w_float_q16					(fEntityHealth,-1000,1000);

	P.w_u32							(N.dwTimeStamp);
	P.w_u8							(0);
	P.w_vec3						(N.p_pos);
	P.w_angle8						(N.o_model);
	P.w_angle8						(N.o_torso.yaw);
	P.w_angle8						(N.o_torso.pitch);

	float					f1 = 0;
	ALife::_GRAPH_ID		l_game_vertex_id = game_vertex_id();
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&f1,						sizeof(f1));
	P.w						(&f1,						sizeof(f1));
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

	P.w_u32							(0);
	P.w_u32							(0);
	P.w_u32							(0);
//	P.w_u32							(0);
	CScriptBinder::net_Export		(P);
}

void CAI_Stalker::net_Import		(NET_Packet& P)
{
	R_ASSERT						(Remote());
	net_update						N;

	u8 flags;

	float health;
	P.r_float_q16		(health,-1000,1000);
	fEntityHealth = health;

	P.r_u32							(N.dwTimeStamp);
	P.r_u8							(flags);
	P.r_vec3						(N.p_pos);
	P.r_angle8						(N.o_model);
	P.r_angle8						(N.o_torso.yaw);
	P.r_angle8						(N.o_torso.pitch);

	ALife::_GRAPH_ID				graph_vertex_id = game_dest_vertex_id();
	P.r								(&graph_vertex_id,		sizeof(ALife::_GRAPH_ID));
	graph_vertex_id					= game_vertex_id();
	P.r								(&graph_vertex_id,		sizeof(ALife::_GRAPH_ID));

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back				(N);
		NET_WasInterpolating		= TRUE;
	}

	float							fDummy;
	u32								dwDummy;
	P.r_float						(fDummy);
	P.r_u32							(dwDummy);
	P.r_u32							(dwDummy);

	P.r_u32							(dwDummy);
	P.r_u32							(dwDummy);
	P.r_u32							(dwDummy);
//	P.r_u32							(dwDummy);

	setVisible						(TRUE);
	setEnabled						(TRUE);
	CScriptBinder::net_Import		(P);
}

void CAI_Stalker::UpdateCL(){

	inherited::UpdateCL();
	m_pPhysics_support->in_UpdateCL();

	if (g_Alive()) {
#ifdef OLD_OBJECT_HANDLER
		CObjectHandler::update			(Level().timeServer() - m_dwLastUpdateTime);
#else
		CObjectHandlerGOAP::update		(Level().timeServer() - m_dwLastUpdateTime);
#endif
		float							s_k		= ((eBodyStateCrouch == body_state()) ? CROUCH_SOUND_FACTOR : 1.f);
		float							s_vol	= s_k*((eMovementTypeRun == movement_type()) ? 1.f : ACCELERATED_SOUND_FACTOR);
		float							step_time = !fis_zero(CMovementManager::speed()) ? .725f/CMovementManager::speed() : 1.f;
		CMaterialManager::update		(Device.fTimeDelta,1.f+0*s_vol,step_time,!!fis_zero(speed()));
	}

	if (this->ID() == Level().CurrentViewEntity()->ID())
		Exec_Visibility();
}

void CAI_Stalker::Hit(float P, Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

void CAI_Stalker ::PHHit				(Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /*ALife::eHitTypeWound*/)
{
	m_pPhysics_support->in_Hit(dir,element,p_in_object_space,impulse,hit_type,!g_Alive());
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

	CMemoryManager::update				();
	inherited::inherited::shedule_Update(DT);
	
	// Queue setup
	float dt			= float(DT)/1000.f;
	if (dt > 3) {
		return;
	}

	if (Remote())		{
	} else {
		// here is monster AI call
		VERIFY				(_valid(Position()));
		m_fTimeUpdateDelta				= dt;
		Device.Statistic.AI_Think.Begin	();
		if (GetScriptControl())
			ProcessScripts				();
		else
			if ((-1 == m_dwParticularState))
				Think					();
		m_dwLastUpdateTime				= Level().timeServer();
		Device.Statistic.AI_Think.End	();
		Engine.Sheduler.Slice			();
		VERIFY				(_valid(Position()));

		// Look and action streams
		if (fEntityHealth>0) {
			VERIFY				(_valid(Position()));
			Exec_Look				(dt);
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
	if (m_dwDeathTime && (inventory().TotalWeight() > 0)) {
		CWeapon *tpWeapon = dynamic_cast<CWeapon*>(inventory().ActiveItem());
		if (!tpWeapon || !tpWeapon->GetAmmoElapsed() || !m_bHammerIsClutched || (Level().timeServer() - m_dwDeathTime > 500)) {
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

			///!!!
			TIItemList &l_list = inventory().m_ruck;
			for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
			{
				CArtifact* pArtifact = dynamic_cast<CArtifact*>(*l_it);
				if(pArtifact)
					pArtifact->Drop();

				//if ((*l_it)->Useful())
					//(*l_it)->Drop();
			}
		}
		else {
			inventory().Action(kWPN_FIRE,	CMD_START);
			xr_vector<CInventorySlot>::iterator I = inventory().m_slots.begin(), B = I;
			xr_vector<CInventorySlot>::iterator E = inventory().m_slots.end();
			for ( ; I != E; ++I)
				if ((I - B) != (int)inventory().GetActiveSlot())
					if ((*I).m_pIItem->SUB_CLS_ID != CLSID_IITEM_BOLT)
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
	CWeapon* W	= dynamic_cast<CWeapon*>(inventory().ActiveItem());
	if (W) R	+= W->Radius();
	return R;
}

bool CAI_Stalker::use_model_pitch	() const
{
	return					(false);
}

void CAI_Stalker::spawn_supplies	()
{
	inherited::spawn_supplies		();
#ifdef OLD_OBJECT_HANDLER
	CObjectHandler::spawn_supplies	();
#else
	CObjectHandlerGOAP::spawn_supplies	();
#endif
}
