////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\weapon.h"
#include "..\\..\\CharacterPhysicsSupport.h"
CAI_Stalker::CAI_Stalker			()
{
	m_pPhysics_support=xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etStalker,this);
	Init();
	Movement.AllocateCharacterObject(CPHMovementControl::CharacterType::ai_stalker);
	shedule.t_min	= 200;
	shedule.t_max	= 1;
}

CAI_Stalker::~CAI_Stalker			()
{
xr_delete(m_pPhysics_support);
}

void CAI_Stalker::Init()
{
	//	m_tStateList.clear				();
	//	while (m_tStateStack.size())
	//		m_tStateStack.pop			();
	m_pPhysics_support				->in_Init();
	m_tMovementDirection			= eMovementDirectionForward;
	m_tDesirableDirection			= eMovementDirectionForward;
	m_tLookType						= eLookTypeDirection;
	m_tBodyState					= eBodyStateStand;
	m_dwDirectionStartTime			= 0;
	m_dwAnimationSwitchInterval		= 500;
	r_torso_speed					= PI_MUL_2;
	r_head_speed					= 3*PI_DIV_2;

	m_dwTimeToChange				= 0;
	m_dwHitTime						= 0;

	m_dwActionRefreshRate			= 1000;
	m_fAttackSuccessProbability		= .7f;
	m_dwNoFireTime					= 0;

	m_tSavedEnemy					= 0;
	m_tSavedEnemyPosition.set		(0,0,0);
	m_dwLostEnemyTime				= 0;
	m_tpSavedEnemyNode				= 0;
	m_dwSavedEnemyNodeID			= u32(-1);
	m_tpItemsToTake.clear			();
	m_dwItemToTakeIndex				= u32(-1);
	m_bActionStarted				= false;
	m_iSoundIndex					= -1;
	m_dwSoundTime					= 0;

	m_dwLastRangeSearch				= 0;
	m_dwRandomFactor				= 100;
	m_dwInertion					= 20000;
	m_dwParticularState				= u32(-1);

	m_tMovementType					= eMovementTypeStand;
	m_tPathState					= ePathStateSearchNode;
	m_tPathType						= ePathTypeDodge;
	m_tStateType					= eStateTypeNormal;

	m_dwActionStartTime				= 0;
	m_dwActionEndTime				= 0;
	m_bHammerIsClutched				= false;

	m_dwDeathTime					= 0;

	AI_Path.TravelStart				= 0;
	AI_Path.DestNode				= u32(-1);

	m_bIfSearchFailed				= false;
	m_bStateChanged					= true;

	_A=_B=_C=_D=_E=_F=_G=_H=_I=_J=_K=_L=_M=false;

	m_fAttackSuccessProbability0	= .8f;
	m_fAttackSuccessProbability1	= .6f;
	m_fAttackSuccessProbability2	= .4f;
	m_fAttackSuccessProbability3	= .2f;
	m_dwRandomState					= 2+0*::Random.randI(5);

	//	m_fAccuracy						= 0.f;
	//	m_fIntelligence					= 0.f;

	m_fTimeToStep					= 0;
	
	m_dwMyMaterialID				= GMLib.GetMaterialIdx("actor");
	m_dwLastMaterialID				= GMLib.GetMaterialIdx("default");


	m_dwLookChangedTime				= 0;

	m_fHitFactor					= 1.f;

	m_dwLastEnemySearch				= 0;
	m_dwLastSoundUpdate				= 0;

	m_tpCurrentSound				= 0;
	m_bPlayHumming					= false;

	m_dwLastUpdate					= 0;

	AI_Path.TravelPath.clear		();
	AI_Path.Nodes.clear				();

	m_dwStartFireTime				= 0;
}

// when soldier is dead
void CAI_Stalker::Die				()
{
	//	vfAddStateToList				(m_eCurrentState = eStalkerStateDie);

//#ifdef DEBUG
//	Msg								("Death position : [%f][%f][%f]",VPUSH(Position()));
//#endif
	Fvector	dir;
	AI_Path.Direction				(dir);
	SelectAnimation					(XFORM().k,dir,AI_Path.fSpeed);
	m_dwDeathTime					= Level().timeServer();

	ref_sound						&S  = m_tpSoundDie[::Random.randI((u32)m_tpSoundDie.size())];
	S.play_at_pos					(this,Position());
	S.feedback->set_volume			(1.f);
	inherited::Die					();
	m_bHammerIsClutched				= !::Random.randI(0,2);
//#ifdef DEBUG
//	Msg								("Death position : [%f][%f][%f]",VPUSH(Position()));
//#endif
}

void CAI_Stalker::Load				(LPCSTR section)
{ 
	setEnabled						(false);
	inherited::Load					(section);
	//m_tSelectorAttack.Load			(section,"selector_attack");
	m_tSelectorFreeHunting.Load		(section,"selector_free_hunting");
	m_tSelectorReload.Load			(section,"selector_reload");
	m_tSelectorRetreat.Load			(section,"selector_retreat");
	m_tSelectorCover.Load			(section,"selector_cover");
	// visibility
	m_dwMovementIdleTime			= pSettings->r_s32(section,"MovementIdleTime");
	m_fMaxInvisibleSpeed			= pSettings->r_float(section,"MaxInvisibleSpeed");
	m_fMaxViewableSpeed				= pSettings->r_float(section,"MaxViewableSpeed");
	m_fMovementSpeedWeight			= pSettings->r_float(section,"MovementSpeedWeight");
	m_fDistanceWeight				= pSettings->r_float(section,"DistanceWeight");
	m_fSpeedWeight					= pSettings->r_float(section,"SpeedWeight");
	m_fVisibilityThreshold			= pSettings->r_float(section,"VisibilityThreshold");
	m_fLateralMultiplier			= pSettings->r_float(section,"LateralMultiplier");
	m_fShadowWeight					= pSettings->r_float(section,"ShadowWeight");

	m_fCrouchFactor					= pSettings->r_float(section,"CrouchFactor");
	m_fWalkFactor					= pSettings->r_float(section,"WalkFactor");
	m_fRunFactor					= pSettings->r_float(section,"RunFactor");
	m_fWalkFreeFactor				= pSettings->r_float(section,"WalkFreeFactor");
	m_fRunFreeFactor				= pSettings->r_float(section,"RunFreeFactor");
	m_fPanicFactor					= pSettings->r_float(section,"PanicFactor");

	//fire
	m_dwFireRandomMin  				= pSettings->r_s32(section,"FireRandomMin");
	m_dwFireRandomMax  				= pSettings->r_s32(section,"FireRandomMax");
	m_dwNoFireTimeMin  				= pSettings->r_s32(section,"NoFireTimeMin");
	m_dwNoFireTimeMax  				= pSettings->r_s32(section,"NoFireTimeMax");
	m_fMinMissDistance 				= pSettings->r_float(section,"MinMissDistance");
	m_fMinMissFactor   				= pSettings->r_float(section,"MinMissFactor");
	m_fMaxMissDistance 				= pSettings->r_float(section,"MaxMissDistance");
	m_fMaxMissFactor				= pSettings->r_float(section,"MaxMissFactor");
	if (pSettings->line_exist(section,"State"))
		m_dwParticularState			= pSettings->r_u32(section,"State");

	eye_fov							= pSettings->r_float(section,"eye_fov");
	eye_range						= pSettings->r_float(section,"eye_range");

	m_tpaTerrain.clear				();
	LPCSTR							S = pSettings->r_string(section,"terrain");
	u32								N = _GetItemCount(S);
	R_ASSERT						(((N % (LOCATION_TYPE_COUNT + 2)) == 0) && (N));
	STerrainPlace					tTerrainPlace;
	tTerrainPlace.tMask.resize		(LOCATION_TYPE_COUNT);
	m_tpaTerrain.reserve			(32);
	string16						I;
	for (u32 i=0; i<N;) {
		for (u32 j=0; j<LOCATION_TYPE_COUNT; j++, i++)
			tTerrainPlace.tMask[j]	= _LOCATION_ID(atoi(_GetItem(S,i,I)));
		tTerrainPlace.dwMinTime		= atoi(_GetItem(S,i++,I))*1000;
		tTerrainPlace.dwMaxTime		= atoi(_GetItem(S,i++,I))*1000;
		m_tpaTerrain.push_back		(tTerrainPlace);
	}
	m_fGoingSpeed					= pSettings->r_float	(section, "going_speed");

	m_dwMaxDynamicObjectsCount		= _min(pSettings->r_s32(section,"DynamicObjectsCount"),MAX_DYNAMIC_OBJECTS);
	m_dwMaxDynamicSoundsCount		= _min(pSettings->r_s32(section,"DynamicSoundsCount"),MAX_DYNAMIC_SOUNDS);

	// skeleton physics
	m_pPhysics_support				->in_Load(section);

	::Sound->create					(m_tpSoundStep[0],	TRUE,	"Actor\\StepL",						SOUND_TYPE_MONSTER_WALKING_HUMAN);
	::Sound->create					(m_tpSoundStep[1],	TRUE,	"Actor\\StepR",						SOUND_TYPE_MONSTER_WALKING_HUMAN);

	g_vfLoadSounds					(m_tpSoundDie,pSettings->r_string(section,"sound_death"),100);
	g_vfLoadSounds					(m_tpSoundHit,pSettings->r_string(section,"sound_hit"),100);
	g_vfLoadSounds					(m_tpSoundHumming,pSettings->r_string(section,"sound_humming"),100);
	g_vfLoadSounds					(m_tpSoundAlarm,pSettings->r_string(section,"sound_alarm"),100);
	g_vfLoadSounds					(m_tpSoundSurrender,pSettings->r_string(section,"sound_surrender"),100);

	// prefetching
	cNameVisual_set					("actors\\different_stalkers\\stalker_black_mask");
	cNameVisual_set					("actors\\different_stalkers\\stalker_ecolog");
	cNameVisual_set					("actors\\different_stalkers\\stalker_hood_multiplayer");
	cNameVisual_set					("actors\\different_stalkers\\stalker_no_hood_singleplayer");
	cNameVisual_set					("actors\\different_stalkers\\stalker_scientist");
	cNameVisual_set					("actors\\different_stalkers\\stalker_svoboda");
	cNameVisual_set					("actors\\different_stalkers\\stalker_svoboda2");
	cNameVisual_set					("actors\\different_stalkers\\stalker_svoboda3");
	cNameVisual_set					("actors\\different_stalkers\\stalker_svoboda4");
	cNameVisual_set					("actors\\different_stalkers\\stalker_ik_test");
	cNameVisual_set					("actors\\different_stalkers\\stalker_ik_test_koan");
}

BOOL CAI_Stalker::net_Spawn			(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return						(FALSE);
	Movement.SetPLastMaterial		(&m_dwLastMaterialID);

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeHumanAbstract			*tpHuman = dynamic_cast<CSE_ALifeHumanAbstract*>(e);
	R_ASSERT						(tpHuman);
	cNameVisual_set					(tpHuman->get_visual());

	r_current.yaw = r_target.yaw = r_torso_current.yaw = r_torso_target.yaw	= angle_normalize_signed(-tpHuman->o_Angle.y);
	r_torso_current.pitch			= r_torso_target.pitch	= 0;

	fHealth							= tpHuman->fHealth;
	m_tCurGP						= tpHuman->m_tGraphID;
	m_tNextGP						= tpHuman->m_tNextGraphID;
	m_dwBornTime					= Level().timeServer();

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (m_dwParticularState == u32(-1))
		m_tNextGP					= m_tCurGP = getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//	m_tStateStack.push				(m_eCurrentState = eStalkerStateAccomplishingTask);
	//	vfAddStateToList				(m_eCurrentState);

	CStalkerAnimations::Load		(PKinematics(Visual()));
	vfAssignBones					(pSettings,cNameSect());

	setEnabled						(true);

	m_pPhysics_support->in_NetSpawn();
	Movement.SetPosition	(Position());
	Movement.SetVelocity	(0,0,0);

	if (!Level().CurrentViewEntity())
		Level().SetEntity(this);

	// load damage params
	if (pSettings->line_exist(cNameSect(),"damage")) {
		string32 buf;
		CInifile::Sect& dam_sect	= pSettings->r_section(pSettings->r_string(cNameSect(),"damage"));
		for (CInifile::SectIt it=dam_sect.begin(); it!=dam_sect.end(); it++)
		{
			if (0==strcmp(it->first,"default")){
				m_fHitFactor	= (float)atof(it->second);
			}else{
				int bone	= PKinematics(Visual())->LL_BoneID(it->first); 
				R_ASSERT2(bone!=BONE_NONE,it->first);
				CBoneInstance& B = PKinematics(Visual())->LL_GetInstance(bone);
				B.set_param(0,(float)atof(_GetItem(it->second,0,buf)));
				B.set_param(1,(float)atoi(_GetItem(it->second,1,buf)));
			}
		}
	}

	return							(TRUE);
}

void CAI_Stalker::net_Destroy()
{
	inherited::net_Destroy	();
	Init					();
	m_inventory.Clear		();
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

	P.w_float_q16					(fHealth,-1000,1000);

	P.w_u32							(N.dwTimeStamp);
	P.w_u8							(0);
	P.w_vec3						(N.p_pos);
	P.w_angle8						(N.o_model);
	P.w_angle8						(N.o_torso.yaw);
	P.w_angle8						(N.o_torso.pitch);

	P.w								(&m_tNextGP,				sizeof(m_tNextGP));
	P.w								(&m_tCurGP,					sizeof(m_tCurGP));
	P.w								(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	P.w								(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));

	float							f1;
	if (m_tCurGP != u16(-1)) {
		f1							= Position().distance_to		(getAI().m_tpaGraph[m_tCurGP].tLocalPoint);
		P.w							(&f1,						sizeof(f1));
		f1							= Position().distance_to		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
		P.w							(&f1,						sizeof(f1));
	}
	else {
		f1							= 0;
		P.w							(&f1,						sizeof(f1));
		P.w							(&f1,						sizeof(f1));
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
	P.r_float_q16					(fHealth,-1000,1000);

	P.r_u32							(N.dwTimeStamp);
	P.r_u8							(flags);
	P.r_vec3						(N.p_pos);
	P.r_angle8						(N.o_model);
	P.r_angle8						(N.o_torso.yaw);
	P.r_angle8						(N.o_torso.pitch);

	P.r								(&m_tNextGP,		sizeof(m_tNextGP));
	P.r								(&m_tCurGP,			sizeof(m_tCurGP));

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

void CAI_Stalker::Exec_Movement		(float dt)
{
	AI_Path.Calculate				(this,Position(),Position(),m_fCurSpeed,dt);
}

void CAI_Stalker::CreateSkeleton()
{

}

void CAI_Stalker::UpdateCL(){

	inherited::UpdateCL();
	m_pPhysics_support->in_UpdateCL();
		if (Level().CurrentViewEntity() == this) {
			Exec_Visibility();
		}
}

void CAI_Stalker::Hit(float P, Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse){

if(m_pPhysics_support->isAlive())inherited::Hit(P,dir,who,element,p_in_object_space,impulse);
m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse);
}

void CAI_Stalker::shedule_Update		( u32 DT )
{
	// Queue shrink
	VERIFY				(_valid(Position()));
	u32	dwTimeCL	= Level().timeServer()-NET_Latency;
	VERIFY				(!NET.empty());
	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();

	// Queue setup
	float dt			= float(DT)/1000.f;
	if (dt > 3)
		return;

	VERIFY				(_valid(Position()));
	if (!Remote()) {
		if ((fHealth>0) || bfExecMovement())
			// функция должна выполняться до inherited::shedule_Update, для smooth movement
			//Msg				("TIME DELTA : %d",DT);
			Exec_Movement	(dt);  
	}

	VERIFY				(_valid(Position()));
	// *** general stuff
	inherited::inherited::shedule_Update	(DT);
	
	if (Remote())		{
	} else {
		// here is monster AI call
		VERIFY				(_valid(Position()));
		m_fTimeUpdateDelta				= dt;
		Device.Statistic.AI_Think.Begin	();
		Think							();
		m_dwLastUpdateTime				= Level().timeServer();
		Device.Statistic.AI_Think.End	();
		Engine.Sheduler.Slice			();
		VERIFY				(_valid(Position()));

		// Look and action streams
		if (fHealth>0) {
			VERIFY				(_valid(Position()));
			Exec_Look				(dt);
			VERIFY				(_valid(Position()));
			Exec_Visibility			();

			//////////////////////////////////////
			Fvector C; float R;
			//////////////////////////////////////
			// С Олеся - ПИВО!!!! (Диме :-))))
			// Movement.GetBoundingSphere	(C,R);
			//////////////////////////////////////
			Center(C);
			R = Radius();
			//////////////////////////////////////
			feel_touch_update		(C,R);

			net_update				uNext;
			uNext.dwTimeStamp		= Level().timeServer();
			uNext.o_model			= r_torso_current.yaw;
			uNext.o_torso			= r_current;
			uNext.p_pos				= Position();
			uNext.fHealth			= fHealth;
			NET.push_back			(uNext);
		}
		else 
		{
			net_update			uNext;
			uNext.dwTimeStamp	= Level().timeServer();
			uNext.o_model		= r_torso_current.yaw;
			uNext.o_torso		= r_current;
			uNext.p_pos			= Position();
			uNext.fHealth		= fHealth;
			NET.push_back		(uNext);
		}
	}
	VERIFY				(_valid(Position()));

	// inventory update
	if (m_dwDeathTime && (m_inventory.TotalWeight() > 0)) {
		CWeapon *tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
		if (!tpWeapon || !tpWeapon->GetAmmoElapsed() || !m_bHammerIsClutched || (Level().timeServer() - m_dwDeathTime > 500)) {
			xr_vector<CInventorySlot>::iterator I = m_inventory.m_slots.begin();
			xr_vector<CInventorySlot>::iterator E = m_inventory.m_slots.end();
			for ( ; I != E; I++)
				m_inventory.Ruck((*I).m_pIItem);
			TIItemList &l_list = m_inventory.m_ruck;
			for(PPIItem l_it = l_list.begin(); l_it != l_list.end(); l_it++)
				if (*l_it == tpWeapon)
					(**l_it).Drop();
		}
		else {
			m_inventory.Action(kWPN_FIRE,	CMD_START);
			xr_vector<CInventorySlot>::iterator I = m_inventory.m_slots.begin(), B = I;
			xr_vector<CInventorySlot>::iterator E = m_inventory.m_slots.end();
			for ( ; I != E; I++)
				if ((I - B) != (int)m_inventory.m_activeSlot)
					m_inventory.Ruck((*I).m_pIItem);
			TIItemList &l_list = m_inventory.m_ruck;
			for(PPIItem l_it = l_list.begin(); l_it != l_list.end(); l_it++)
				(**l_it).Drop();
		}
	}
	VERIFY				(_valid(Position()));

	if (g_Alive()) {
		R_ASSERT					(m_dwLastMaterialID != GAMEMTL_NONE);
		SGameMtlPair				*mtl_pair = GMLib.GetMaterialPair(m_dwMyMaterialID,m_dwLastMaterialID);
		R_ASSERT3					(mtl_pair,"Undefined material pair: Actor # ", GMLib.GetMaterial(m_dwLastMaterialID)->name);
		// ref_sound step
		if (m_tMovementType != eMovementTypeStand) {
			if(m_fTimeToStep < 0) {
				m_bStep				= !m_bStep;
				float k				= (m_tBodyState == eBodyStateCrouch)?0.75f:1.f;
				float tm			= (m_tMovementType == eMovementTypeRun) ? (PI/(k*10.f)) : (PI/(k*7.f));
				m_fTimeToStep		= tm;
				m_tpSoundStep[m_bStep].clone		(mtl_pair->StepSounds[m_bStep]);
				m_tpSoundStep[m_bStep].play_at_pos	(this,Position());
			}
			m_fTimeToStep -= dt;
		}
		float	s_k			= ffGetStartVolume(SOUND_TYPE_MONSTER_WALKING)*((m_tBodyState == eBodyStateCrouch) ? CROUCH_SOUND_FACTOR : 1.f);
		float	s_vol		= s_k*((m_tMovementType == eMovementTypeRun) ? 1.f : ACCELERATED_SOUND_FACTOR);
		if (m_tpSoundStep[0].feedback)		{
			m_tpSoundStep[0].set_position	(Position());
			m_tpSoundStep[0].set_volume	(s_vol);
		}
		if (m_tpSoundStep[1].feedback)		{
			m_tpSoundStep[1].set_position	(Position());
			m_tpSoundStep[1].set_volume	(s_vol);
		}
	}
	VERIFY				(_valid(Position()));
	m_inventory.Update(DT);
	VERIFY				(_valid(Position()));

	m_pPhysics_support->in_shedule_Update(DT);
	VERIFY				(_valid(Position()));
}

float CAI_Stalker::Radius()const
{ 
	float R		= inherited::Radius();
	CWeapon* W	= dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	if (W) R	+= W->Radius();
	return R;
}
