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

CAI_Stalker::CAI_Stalker			()
{
	Init							();
	InitTrade						();
	m_pPhysics_support				= xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etStalker,this);
	m_PhysicMovementControl.AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
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
	CCustomMonster::reinit			();
	CObjectHandler::reinit			(this);
	CSightManager::reinit			();
	CStalkerAnimations::reinit		();
	CStalkerMovementManager::reinit	();
	CStateManagerStalker::reinit	(this);
	CSStateInternal::reinit			(this);

	m_pPhysics_support->in_Init		();
	m_dwRandomFactor				= 100;
	m_dwInertion					= 20000;
	m_dwActionStartTime				= 0;
	m_bStateChanged					= true;
	_A=_B=_C=_D=_E=_F=_G=_H=_I=_J=_K=_L=_M=false;
	m_dwLastSoundUpdate				= 0;
	m_dwLastUpdate					= 0;
	m_tTaskState					= eTaskStateChooseTask;
}

void CAI_Stalker::reload			(LPCSTR section)
{
	CCustomMonster::reload			(section);
	CInventoryOwner::reload			(section);
//	CObjectHandler::reload			(section);
//	CSightManager::reload			(section);
//	CStalkerAnimations::reload		(section);
	CStalkerMovementManager::reload	(section);
	CStateManagerStalker::reload	(section);
	CSStateInternal::reload			(section);
}

void CAI_Stalker::Die				()
{
	SelectAnimation					(XFORM().k,direction(),speed());

	play							(eStalkerSoundDie);
	inherited::Die					();
	m_bHammerIsClutched				= !::Random.randI(0,2);

	//запретить использование слотов в инвенторе
	m_inventory.SetSlotsUseful		(false);
}

void CAI_Stalker::Load				(LPCSTR section)
{ 
	setEnabled						(false);
	
	CCustomMonster::Load			(section);
	CObjectHandler::Load			(section);
	CSightManager::Load				(section);
	CStalkerMovementManager::Load	(section);
	CStateManagerStalker::Load		(section);
	CSStateInternal::Load			(section);

	CSelectorManager::add<
		PathManagers::CVertexEvaluator<
			aiSearchRange | aiEnemyDistance
		>
	>								(section,"selector_free_hunting");
	CSelectorManager::add<
		PathManagers::CVertexEvaluator<
			aiSearchRange | aiCoverFromEnemyWeight
		>
	>								(section,"selector_reload");
	CSelectorManager::add<
		PathManagers::CVertexEvaluator<
			aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance
		>
	>								(section,"selector_retreat");
	CSelectorManager::add<
		PathManagers::CVertexEvaluator<
			aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance | aiEnemyViewDeviationWeight
		>
	>								(section,"selector_cover");

	LPCSTR							head_bone_name = pSettings->r_string(section,"bone_head");
	CSoundPlayer::add				(pSettings->r_string(section,"sound_death"),	100, SOUND_TYPE_MONSTER_DYING,		0, u32(-1),				eStalkerSoundDie,		head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_hit"),		100, SOUND_TYPE_MONSTER_INJURING,	1, u32(-1),				eStalkerSoundInjuring,	head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_humming"),	100, SOUND_TYPE_MONSTER_TALKING,	4, u32(1 << 31) | 0,	eStalkerSoundHumming,	head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_alarm"),	100, SOUND_TYPE_MONSTER_TALKING,	2, u32(1 << 31) | 1,	eStalkerSoundAlarm,		head_bone_name);
	CSoundPlayer::add				(pSettings->r_string(section,"sound_surrender"),100, SOUND_TYPE_MONSTER_TALKING,	3, u32(1 << 31) | 2,	eStalkerSoundSurrender,	head_bone_name);

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

	CStalkerAnimations::reload		(Visual(),pSettings,cNameSect());

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

	m_pPhysics_support->in_NetSpawn();
	m_PhysicMovementControl.SetPosition	(Position());
	m_PhysicMovementControl.SetVelocity	(0,0,0);

	if (!Level().CurrentViewEntity())
		Level().SetEntity(this);

	// load damage params

	m_tpKnownCustomers				= tpHuman->m_tpKnownCustomers;

	return							(TRUE);
}

void CAI_Stalker::net_Destroy()
{
	inherited::net_Destroy	();
	m_pPhysics_support->in_NetDestroy();
}

void CAI_Stalker::net_Export		(NET_Packet& P)
{
	R_ASSERT						(Local());

	// export last known packet
	R_ASSERT						(!NET.empty());
	net_update& N					= NET.back();
	P.w_float						(m_inventory.TotalWeight());
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
}

void CAI_Stalker::UpdateCL(){

	inherited::UpdateCL();
	m_pPhysics_support->in_UpdateCL();

	if (g_Alive()) {
		CObjectHandler::update			(Level().timeServer() - m_dwLastUpdateTime);
		float							s_k		= ((eBodyStateCrouch == m_tBodyState) ? CROUCH_SOUND_FACTOR : 1.f);
		float							s_vol	= s_k*((eMovementTypeRun == m_tMovementType) ? 1.f : ACCELERATED_SOUND_FACTOR);
		float							k		= (eBodyStateCrouch == m_tBodyState) ? 0.75f : 1.f;
		float							tm		= (eMovementTypeRun == m_tMovementType) ? (PI/(k*10.f)) : (PI/(k*7.f));
		CMaterialManager::update		(Device.fTimeDelta,s_vol,tm,!!fis_zero(speed()));
	}

	if (this == Level().CurrentViewEntity())
		Exec_Visibility();
}

void CAI_Stalker::Hit(float P, Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type){

	bool kill_hit=g_Alive()&& (fEntityHealth<P);
	if (m_pPhysics_support->isAlive())
		inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
	m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse,kill_hit);
}

void CAI_Stalker::shedule_Update		( u32 DT )
{
	// Queue shrink
	VERIFY				(_valid(Position()));
	u32	dwTimeCL		= Level().timeServer()-NET_Latency;
	VERIFY				(!NET.empty());
	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();

	// Queue setup
	float dt			= float(DT)/1000.f;
	if (dt > 3)
		return;

	Fvector				vNewPosition = Position();
	VERIFY				(_valid(Position()));
	// *** general stuff

	CMemoryManager::update				();
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
			// m_PhysicMovementControl.GetBoundingSphere	(C,R);
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
	if (m_dwDeathTime && (m_inventory.TotalWeight() > 0)) {
		CWeapon *tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
		if (!tpWeapon || !tpWeapon->GetAmmoElapsed() || !m_bHammerIsClutched || (Level().timeServer() - m_dwDeathTime > 500)) {
			xr_vector<CInventorySlot>::iterator I = m_inventory.m_slots.begin(), B = I;
			xr_vector<CInventorySlot>::iterator E = m_inventory.m_slots.end();
			for ( ; I != E; ++I)
				if ((I - B) == (int)m_inventory.GetActiveSlot()) 
					(*I).m_pIItem->Drop();
				else
					if((*I).m_pIItem) m_inventory.Ruck((*I).m_pIItem);

			/*TIItemList &l_list = m_inventory.m_ruck;
			for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
				if ((*l_it)->Useful())
					(*l_it)->Drop();*/
		}
		else {
			m_inventory.Action(kWPN_FIRE,	CMD_START);
			xr_vector<CInventorySlot>::iterator I = m_inventory.m_slots.begin(), B = I;
			xr_vector<CInventorySlot>::iterator E = m_inventory.m_slots.end();
			for ( ; I != E; ++I)
				if ((I - B) != (int)m_inventory.GetActiveSlot())
					m_inventory.Ruck((*I).m_pIItem);
			//		(*I).m_pIItem->Drop();
			
			/*TIItemList &l_list = m_inventory.m_ruck;
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
	CWeapon* W	= dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	if (W) R	+= W->Radius();
	return R;
}