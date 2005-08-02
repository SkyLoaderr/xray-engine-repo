// Actor.cpp: implementation of the CActor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Actor_Flags.h"

#include "hudmanager.h"
#ifdef DEBUG
#include "ode_include.h"
#include "../StatGraph.h"
#include "PHDebug.h"
#endif
#include "hit.h"
#include "PHDestroyable.h"
#include "Car.h"
#include "xrserver_objects_alife_monsters.h"

#include "CameraLook.h"
#include "CameraFirstEye.h"


#include "../effectorfall.h"
#include "EffectorBobbing.h"
#include "clsid_game.h"

#include "ShootingHitEffector.h"
#include "ActorEffector.h"
#include "EffectorZoomInertion.h"

#include "SleepEffector.h"

#include "character_info.h"
#include "CustomOutfit.h"

#include "actorcondition.h"


// breakpoints
#include "../xr_input.h"

//
#include "Actor.h"
#include "ActorAnimation.h"
#include "actor_anim_defs.h"

#include "HudItem.h"
#include "WeaponMagazined.h"

#include "ai_sounds.h"
#include "ai_space.h"

#include "trade.h"
#include "LevelFogOfWar.h"

#include "inventory.h"

#include "Physics.h"
#include "level.h"
#include "GamePersistent.h"
#include "game_cl_base.h"
#include "xrmessages.h"

#include "string_table.h"
#include "usablescriptobject.h"
#include "../cl_intersect.h"
#include "ExtendedGeom.h"
#include "alife_registry_wrappers.h"
#include "../skeletonanimated.h"

#include "artifact.h"
#include "CharacterPhysicsSupport.h"

#include "material_manager.h"
#include "IColisiondamageInfo.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIArtefactPanel.h"

#include "map_manager.h"
#include "GameTaskManager.h"

const u32		patch_frames	= 50;
const float		respawn_delay	= 1.f;
const float		respawn_auto	= 7.f;

static float IReceived = 0;
static float ICoincidenced = 0;


//skeleton



static Fbox		bbStandBox;
static Fbox		bbCrouchBox;
static Fvector	vFootCenter;
static Fvector	vFootExt;

Flags32			psActorFlags={0};



//--------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CActor::CActor() : CEntityAlive()
{
	//.
	dbgmp_light				= ::Render->light_create				();
	dbgmp_light->set_type	(IRender_Light::POINT);
	dbgmp_light->set_range	(10.f);
	dbgmp_light->set_color	(.5f,1.f,.5f);
	//.

	contacts_registry		= xr_new<CKnownContactsRegistryWrapper	>();
	encyclopedia_registry	= xr_new<CEncyclopediaRegistryWrapper	>();
//.	game_task_registry		= xr_new<CGameTaskRegistryWrapper		>();
	game_news_registry		= xr_new<CGameNewsRegistryWrapper		>();
	// Cameras
	cameras[eacFirstEye]	= xr_new<CCameraFirstEye>	(this, pSettings, "actor_firsteye_cam", 0);
	cameras[eacLookAt]		= xr_new<CCameraLook>		(this, pSettings, "actor_look_cam",		0);
	cameras[eacFreeLook]	= xr_new<CCameraLook>		(this, pSettings, "actor_free_cam",		0);
	cam_active				= eacFirstEye;
	fPrevCamPos				= 0.0f;
	vPrevCamDir.set			(0.f,0.f,1.f);
	fCurAVelocity			= 0.0f;
	// эффекторы
	pCamBobbing				= 0;
	m_pShootingEffector		= NULL;
	m_pSleepEffector		= NULL;
	m_pSleepEffectorPP		= NULL;

	// 
	//Weapons					= 0;

	r_torso.yaw				= 0;
	r_torso.pitch			= 0;
	r_torso.roll			= 0;
	r_torso_tgt_roll		= 0;
	r_model_yaw				= 0;
	r_model_yaw_delta		= 0;
	r_model_yaw_dest		= 0;

	b_DropActivated			= 0;
	f_DropPower				= 0.f;

	m_fRunFactor			= 2.f;
	m_fCrouchFactor			= 0.2f;
	m_fClimbFactor			= 1.f;
	m_fCamHeightFactor		= 0.87f;

	m_fFallTime				=	s_fFallTime;
	m_bAnimTorsoPlayed		=	false;

//	self_gmtl_id			=	GAMEMTL_NONE;
//	last_gmtl_id			=	GAMEMTL_NONE;
	m_pPhysicsShell			=	NULL;



	m_holder				=	NULL;
	m_holderID				=	u16(-1);
	m_pPhysics_support		=	xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etActor,this);

#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW);
#endif

	//разрешить использование по€са в inventory
	inventory().SetBeltUseful(true);

	m_pPersonWeLookingAt	= NULL;
	m_pVehicleWeLookingAt	= NULL;
	m_pObjectWeLookingAt	= NULL;
	m_bPickupMode			= false;

	////////////////////////////////////
	pStatGraph = NULL;
//	dDesyncVec.set(0, 0, 0);

	m_pActorEffector = NULL;

	m_bZoomAimingMode = false;

	m_sDefaultObjAction = NULL;

	m_bHeavyBreathSndPlaying = false;

	m_controlled_mouse_scale_factor	 = 10.f;
	m_fSprintFactor					 = 4.f;
	SetControlled					(false);
	//-----------------------------------------------------------------------------------
	hFriendlyIndicator.create				(FVF::F_LIT,RCache.Vertex.Buffer(),RCache.QuadIB);

	m_pUsableObject=NULL;


	m_anims = xr_new<SActorMotions>();
	m_vehicle_anims = xr_new<SActorVehicleAnims>();
	m_entity_condition		= NULL;
	m_pLastHitter			= NULL;
	m_pLastHittingWeapon	= NULL;
	m_game_task_manager		= NULL;
}


CActor::~CActor()
{
	dbgmp_light.destroy		();	//.

	xr_delete				(contacts_registry);
	xr_delete				(encyclopedia_registry);
//	xr_delete				(game_task_registry);
	xr_delete				(game_news_registry);
#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif
	//xr_delete(Weapons);
	for (int i=0; i<eacMaxCam; ++i) xr_delete(cameras[i]);

	// sounds 2D
	m_HeavyBreathSnd.destroy();

	// sounds 3D
	// for (i=0; i<SND_HIT_COUNT; ++i) ::Sound->destroy(sndHit[i]);
	// for (i=0; i<SND_DIE_COUNT; ++i) ::Sound->destroy(sndDie[i]);

	xr_delete(m_pActorEffector);

	xr_delete(m_pShootingEffector);
	xr_delete(m_pSleepEffector);
	//-----------------------------------------------------------
	hFriendlyIndicator.destroy();

	xr_delete						(m_pPhysics_support);

	xr_delete(m_anims);
	xr_delete(m_vehicle_anims);
}

void CActor::reinit	()
{
	m_PhysicMovementControl->CreateCharacter();
	m_PhysicMovementControl->SetPhysicsRefObject(this);
	CEntityAlive::reinit	();
	CInventoryOwner::reinit	();
	material().reinit		();
	m_pPhysics_support->in_Init		();
	m_pUsableObject=NULL;
}

void CActor::reload	(LPCSTR section)
{
	CEntityAlive::reload	(section);
	CInventoryOwner::reload	(section);
	material().reload		(section);
	CStepManager::reload	(section);
}

void CActor::Load	(LPCSTR section )
{
	// Msg					("Loading actor: %s",section);
	inherited::Load			(section);
	material().Load			(section);
	CInventoryOwner::Load	(section);

	//////////////////////////////////////////////////////////////////////////
	ISpatial*		self			=	smart_cast<ISpatial*> (this);
	if (self)	{
		self->spatial.type	|=	STYPE_VISIBLEFORAI;
		self->spatial.type	&= ~STYPE_REACTTOSOUND;
	}
	//////////////////////////////////////////////////////////////////////////

	// m_PhysicMovementControl: General
	//m_PhysicMovementControl->SetParent		(this);
	Fbox	bb;Fvector	vBOX_center,vBOX_size;
	// m_PhysicMovementControl: BOX
	vBOX_center= pSettings->r_fvector3	(section,"ph_box2_center"	);
	vBOX_size	= pSettings->r_fvector3	(section,"ph_box2_size"		);
	bb.set	(vBOX_center,vBOX_center); bb.grow(vBOX_size);
	m_PhysicMovementControl->SetBox		(2,bb);

	// m_PhysicMovementControl: BOX
	vBOX_center= pSettings->r_fvector3	(section,"ph_box1_center"	);
	vBOX_size	= pSettings->r_fvector3	(section,"ph_box1_size"		);
	bb.set	(vBOX_center,vBOX_center); bb.grow(vBOX_size);
	m_PhysicMovementControl->SetBox		(1,bb);

	// m_PhysicMovementControl: BOX
	vBOX_center= pSettings->r_fvector3	(section,"ph_box0_center"	);
	vBOX_size	= pSettings->r_fvector3	(section,"ph_box0_size"		);
	bb.set	(vBOX_center,vBOX_center); bb.grow(vBOX_size);
	m_PhysicMovementControl->SetBox		(0,bb);

	//// m_PhysicMovementControl: Foots
	//Fvector	vFOOT_center= pSettings->r_fvector3	(section,"ph_foot_center"	);
	//Fvector	vFOOT_size	= pSettings->r_fvector3	(section,"ph_foot_size"		);
	//bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	////m_PhysicMovementControl->SetFoots	(vFOOT_center,vFOOT_size);

	// m_PhysicMovementControl: Crash speed and mass
	float	cs_min		= pSettings->r_float	(section,"ph_crash_speed_min"	);
	float	cs_max		= pSettings->r_float	(section,"ph_crash_speed_max"	);
	float	mass		= pSettings->r_float	(section,"ph_mass"				);
	m_PhysicMovementControl->SetCrashSpeeds	(cs_min,cs_max);
	m_PhysicMovementControl->SetMass		(mass);
	if(pSettings->line_exist(section,"stalker_restrictor_radius"))
		m_PhysicMovementControl->SetActorRestrictorRadius(CPHCharacter::rtStalker,pSettings->r_float(section,"stalker_restrictor_radius"));
	if(pSettings->line_exist(section,"medium_monster_restrictor_radius"))
		m_PhysicMovementControl->SetActorRestrictorRadius(CPHCharacter::rtMonsterMedium,pSettings->r_float(section,"medium_monster_restrictor_radius"));
	m_PhysicMovementControl->Load(section);

	m_PhysicMovementControl->SetParent(this);

	m_fWalkAccel				= pSettings->r_float(section,"walk_accel");	
	m_fJumpSpeed				= pSettings->r_float(section,"jump_speed");
	m_fRunFactor				= pSettings->r_float(section,"run_coef");
	m_fRunBackFactor			= pSettings->r_float(section,"run_back_coef");
	m_fWalkBackFactor			= pSettings->r_float(section,"walk_back_coef");
	m_fCrouchFactor				= pSettings->r_float(section,"crouch_coef");
	m_fClimbFactor				= pSettings->r_float(section,"climb_coef");
	m_fSprintFactor				= pSettings->r_float(section,"sprint_koef");


	m_fCamHeightFactor			= pSettings->r_float(section,"camera_height_factor");
	m_PhysicMovementControl		->SetJumpUpVelocity(m_fJumpSpeed);
	float AirControlParam		= pSettings->r_float	(section,"air_control_param"	);
	m_PhysicMovementControl		->SetAirControlParam(AirControlParam);

	m_fPickupInfoRadius	= pSettings->r_float(section,"pickup_info_radius");
	m_fSleepTimeFactor	= pSettings->r_float(section,"sleep_time_factor");

	m_pPhysics_support->in_Load		(section);
	
	//загрузить параметры эффектора
	LoadShootingEffector	("shooting_effector");
	LoadSleepEffector		("sleep_effector");

	//загрузить параметры смещени€ firepoint
	m_vMissileOffset	= pSettings->r_fvector3(section,"missile_throw_offset");

	//Weapons				= xr_new<CWeaponList> (this);

	LPCSTR hit_snd_sect = pSettings->r_string(section,"hit_sounds");
	for(int hit_type=0;hit_type<(int)ALife::eHitTypeMax;++hit_type){
		LPCSTR hit_name = ALife::g_cafHitType2String((ALife::EHitType)hit_type);
		LPCSTR hit_snds = pSettings->r_string(hit_snd_sect, hit_name);
		int cnt = _GetItemCount(hit_snds);
		string128 tmp;
		for(int i=0; i<cnt;++i){
			sndHit[hit_type].push_back		(ref_sound());
			sndHit[hit_type].back().create	(TRUE,_GetItem(hit_snds,i,tmp));
		}
	}




	// sounds
	char buf[256];

/*	::Sound->create		(sndHit[0],			TRUE,	strconcat(buf,*cName(),"\\hurt1"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[1],			TRUE,	strconcat(buf,*cName(),"\\hurt2"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[2],			TRUE,	strconcat(buf,*cName(),"\\hurt3"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[3],			TRUE,	strconcat(buf,*cName(),"\\hurt4"),SOUND_TYPE_MONSTER_INJURING);
*/	
	::Sound->create		(sndDie[0],			TRUE,	strconcat(buf,*cName(),"\\die0"), SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[1],			TRUE,	strconcat(buf,*cName(),"\\die1"), SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[2],			TRUE,	strconcat(buf,*cName(),"\\die2"), SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[3],			TRUE,	strconcat(buf,*cName(),"\\die3"), SOUND_TYPE_MONSTER_DYING);

	m_HeavyBreathSnd.create(TRUE, pSettings->r_string(section,"heavy_breath_snd"), SOUND_TYPE_MONSTER_INJURING);
	m_bHeavyBreathSndPlaying = false;

	cam_Set					(eacFirstEye);

	// sheduler
	shedule.t_min				= shedule.t_max = 1;

	// настройки дисперсии стрельбы
	m_fDispBase					= pSettings->r_float		(section,"disp_base"		 );
	m_fDispBase					= deg2rad(m_fDispBase);

	m_fDispAim					= pSettings->r_float		(section,"disp_aim"		 );
	m_fDispAim					= deg2rad(m_fDispAim);

	m_fDispVelFactor			= pSettings->r_float		(section,"disp_vel_factor"	 );
	m_fDispAccelFactor			= pSettings->r_float		(section,"disp_accel_factor" );
	m_fDispCrouchFactor			= pSettings->r_float		(section,"disp_crouch_factor");
	m_fDispCrouchNoAccelFactor	= pSettings->r_float		(section,"disp_crouch_no_acc_factor");

	LPCSTR							default_outfit = READ_IF_EXISTS(pSettings,r_string,section,"default_outfit",0);
	SetDefaultVisualOutfit			(default_outfit);

	invincibility_fire_shield_1st	= READ_IF_EXISTS(pSettings,r_string,section,"Invincibility_Shield_1st",0);
	invincibility_fire_shield_3rd	= READ_IF_EXISTS(pSettings,r_string,section,"Invincibility_Shield_3rd",0);
//-----------------------------------------
	m_AutoPickUp_AABB				= READ_IF_EXISTS(pSettings,r_fvector3,section,"AutoPickUp_AABB",Fvector().set(0.02f, 0.02f, 0.02f));
	m_AutoPickUp_AABB_Offset		= READ_IF_EXISTS(pSettings,r_fvector3,section,"AutoPickUp_AABB_offs",Fvector().set(0, 0, 0));

	CStringTable string_table;
	m_sCharacterUseAction			= string_table("character_use");
	m_sDeadCharacterUseAction		= string_table("dead_character_use");
	m_sCarCharacterUseAction		= string_table("car_character_use");
	m_sInventoryItemUseAction		= string_table("inventory_item_use");

}

void CActor::PHHit(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /* = ALife::eHitTypeWound */)
{
	m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse,hit_type,!g_Alive());
}

void CActor::Hit		(float iLost, Fvector &dir, CObject* who, s16 element,Fvector position_in_bone_space, float impulse, ALife::EHitType hit_type)
{
#ifndef _DEBUG
	if(Level().CurrentEntity() == this) {
		Level().Cameras.AddEffector(xr_new<CShootingHitEffectorPP>(	m_pShootingEffector->ppi,		m_pShootingEffector->time,		m_pShootingEffector->time_attack,		m_pShootingEffector->time_release));
		Level().Cameras.AddEffector(xr_new<CShootingHitEffector>(	m_pShootingEffector->ce_time,	m_pShootingEffector->ce_amplitude,m_pShootingEffector->ce_period_number,m_pShootingEffector->ce_power));
	}
#endif

	if( !sndHit[hit_type].empty() && (ALife::eHitTypeTelepatic != hit_type)){
		ref_sound& S = sndHit[hit_type][Random.randI(sndHit[hit_type].size())];
		bool bPlaySound = true;
		
		if(ALife::eHitTypeExplosion == hit_type)
		{
			if (this == Level().CurrentControlEntity())
			{
				S.set_volume(10.0f);
				if(!m_sndShockEffector)
					m_sndShockEffector = xr_new<SndShockEffector>();

				m_sndShockEffector->Start( S._handle()->length_ms() );
			}
			else
				bPlaySound = false;
		}
		if (bPlaySound) S.play_at_pos(this, Position());
	}


	if (GameID() != GAME_SINGLE)
	{
		game_PlayerState* ps = Game().GetPlayerByGameID(ID());
		if (ps && ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE) && Device.dwFrame != last_hit_frame &&
			element != -1)
		{		
			// вычислить позицию и направленность партикла
			Fmatrix pos; 
			
			CParticlesPlayer::MakeXFORM(this,element,dir,position_in_bone_space,pos);

			// установить particles
			CParticlesObject* ps = NULL;

			if (eacFirstEye == cam_active && this == Level().CurrentEntity())
				ps = xr_new<CParticlesObject>(invincibility_fire_shield_1st,TRUE);
			else
				ps = xr_new<CParticlesObject>(invincibility_fire_shield_3rd,TRUE);

			ps->UpdateParent(pos,Fvector().set(0.f,0.f,0.f));
			GamePersistent().ps_needtoplay.push_back(ps);
		};

		last_hit_frame = Device.dwFrame;
	};

	
	//slow actor, only when he gets hit
	if(hit_type == ALife::eHitTypeWound || hit_type == ALife::eHitTypeStrike)
	{
		hit_slowmo				= iLost/100.f;
		if (hit_slowmo>1.f)		hit_slowmo = 1.f;
	}
	else
		hit_slowmo = 0.f;
	//---------------------------------------------------------------
	if (Level().CurrentViewEntity() == this){
		HitSector(m_pLastHitter, m_pLastHittingWeapon);
	};

	if (mstate_real & mcSprint && Level().CurrentControlEntity() == this)
	{
//		mstate_real	&=~mcSprint;
		mstate_wishful	&=~mcSprint;

//		psHUD_Flags.set(HUD_WEAPON_RT,TRUE);
//		psHUD_Flags.set(HUD_CROSSHAIR_RT,TRUE);
	};
	//---------------------------------------------------------------

	switch (GameID())
	{
	case GAME_SINGLE:		
		{
			float hit_power	= HitArtefactsOnBelt(iLost, hit_type);

			if (GodMode())//psActorFlags.test(AF_GODMODE))
			{
				inherited::Hit(0.f,dir,who,element,position_in_bone_space,impulse, hit_type);
				return;
			}
			else inherited::Hit		(hit_power,dir,who,element,position_in_bone_space, impulse, hit_type);
		}
		break;
	default:
		{
			m_bWasBackStabbed = false;
			if (hit_type == ALife::eHitTypeWound_2 && Check_for_BackStab_Bone(element))
			{
				// convert impulse into local coordinate system
				Fmatrix					mInvXForm;
				mInvXForm.invert		(XFORM());
				Fvector					vLocalDir;
				mInvXForm.transform_dir	(vLocalDir,dir);
				vLocalDir.invert		();

				Fvector a	= {0,0,1};
				float res = a.dotproduct(vLocalDir);
				if (res < -0.707)
				{
					game_PlayerState* ps = Game().GetPlayerByGameID(ID());
					if (!ps || !ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE))						
						m_bWasBackStabbed = true;
				}
			};
			
			float hit_power = 0;

			if (m_bWasBackStabbed) hit_power = 100000;
			else hit_power	= HitArtefactsOnBelt(iLost, hit_type);

			inherited::Hit	(hit_power,dir,who,element,position_in_bone_space, impulse, hit_type);
		}		
		break;
	}
}

void CActor::HitSignal(float perc, Fvector& vLocalDir, CObject* who, s16 element)
{
	if (g_Alive()) 
	{
//		ref_sound& S = sndHit[Random.randI(SND_HIT_COUNT)];
//			if (S.feedback) return;
//
//			Play hit-ref_sound
//			S.play_at_pos(this, Position());
		// hit marker
		if (Local() && (this!=who) && Level().CurrentEntity() == this)	
		{
			int id		= -1;
			Fvector a	= {0,0,1};
			//---------------------------------------------------------
			Fvector b;//	= {vLocalDir.x,0,vLocalDir.z};
			if (who && GameID() != GAME_SINGLE)
			{
//				b.set(who->Position().x - Position().x, 0,who->Position().z - Position().z);
				Fmatrix m_inv;
				Fvector tmp_d = {who->Position().x, 0,who->Position().z};
				m_inv.invert(XFORM());
				m_inv.transform_tiny(b, tmp_d);
			}
			else
				b.set(vLocalDir.x,0,vLocalDir.z);
			//---------------------------------------------------------
			float mb	= b.magnitude();		
			if (!fis_zero(mb)){
				b.mul	(1.f/mb);
				bool FB	= _abs(a.dotproduct(b))>0.7071f;
//				float x	= _abs(vLocalDir.x);
//				float z	= _abs(vLocalDir.z);
			//---------------------------------------------------------
				if (FB)	id = (b.z<0)?2:0;
				else	id = (b.x<0)?3:1;
			//---------------------------------------------------------
				HUD().Hit(id);
			}
		}

		// stop-motion
		if (m_PhysicMovementControl->Environment()==CPHMovementControl::peOnGround || m_PhysicMovementControl->Environment()==CPHMovementControl::peAtWall)
		{
			Fvector zeroV;
			zeroV.set			(0,0,0);
			m_PhysicMovementControl->SetVelocity(zeroV);
		}
		
		//slow actor, only when wound hit
		/*if(hit_type == ALife::eHitTypeWound)
		{
			hit_slowmo				= perc/100.f;
			if (hit_slowmo>1.f)		hit_slowmo = 1.f;
		}
		hit_slowmo = 0.f;*/

		// check damage bone
		Fvector D;
		XFORM().transform_dir(D,vLocalDir);

		float	yaw, pitch;
		D.getHP(yaw,pitch);
		CSkeletonAnimated *tpKinematics = smart_cast<CSkeletonAnimated*>(Visual());
		VERIFY(tpKinematics);
#pragma todo("Dima to Dima : forward-back bone impulse direction has been determined incorrectly!")
		MotionID motion_ID = m_anims->m_normal.m_damage[iFloor(tpKinematics->LL_GetBoneInstance(element).get_param(1) + (angle_difference(r_model_yaw + r_model_yaw_delta,yaw) <= PI_DIV_2 ? 0 : 1))];
		float power_factor = perc/100.f; clamp(power_factor,0.f,1.f);
		VERIFY(motion_ID.valid());
		tpKinematics->PlayFX(motion_ID,power_factor);
	}
}

void CActor::Die	(CObject* who)
{
	inherited::Die(who);
	//-------------------------------------
	xr_vector<CInventorySlot>::iterator I = inventory().m_slots.begin(), B = I;
	xr_vector<CInventorySlot>::iterator E = inventory().m_slots.end();
	for ( ; I != E; ++I)
	{
		if ((I - B) == (int)inventory().GetActiveSlot()) 
		{
			if((*I).m_pIItem)
				(*I).m_pIItem->Drop();
		}
		else
		{
			CCustomOutfit *pOutfit = smart_cast<CCustomOutfit *> ((*I).m_pIItem);
			if (pOutfit) continue;

			if((*I).m_pIItem) inventory().Ruck((*I).m_pIItem);
		};
	};


	///!!! чистка по€са
	TIItemContainer &l_blist = inventory().m_belt;
	while (!l_blist.empty())	inventory().Ruck(l_blist.front());

	if (OnServer() && GameID() != GAME_SINGLE)
	{
		bool MedKitDropped = false;
		//if we are on server and actor has PDA - destroy PDA
		TIItemContainer &l_rlist = inventory().m_ruck;
		for(TIItemContainer::iterator l_it = l_rlist.begin(); l_rlist.end() != l_it; ++l_it)
		{
			if (GameID() == GAME_ARTEFACTHUNT)
			{
				CArtefact* pArtefact = smart_cast<CArtefact*> (*l_it);
				if (pArtefact)
				{
					(*l_it)->Drop();
					continue;
				};
			};

			if ((*l_it)->object().CLS_ID == CLSID_IITEM_MEDKIT && !MedKitDropped)
			{
				MedKitDropped = true;
				(*l_it)->Drop();
				continue;
			};

			if ((*l_it)->object().CLS_ID == CLSID_DEVICE_PDA)
			{
				(*l_it)->Drop();
				continue;
			};

			CCustomOutfit *pOutfit = smart_cast<CCustomOutfit *> (*l_it);
			if (pOutfit) continue;

			//пока у нас нельз€ обыскивать трупы, удал€ем все объекты из инвентар€
			(*l_it)->object().DestroyObject();
		};
	};
	//-------------------------------------
	// Play ref_sound
	::Sound->play_at_pos		(sndDie[Random.randI(SND_DIE_COUNT)],this,Position());
	cam_Set					(eacFreeLook);

	mstate_wishful	&=		~mcAnyMove;
	mstate_real		&=		~mcAnyMove;


	//остановить звук т€желого дыхани€
	m_HeavyBreathSnd.stop();
	m_bHeavyBreathSndPlaying = false;
}

void CActor::g_Physics			(Fvector& _accel, float jump, float dt)
{

	
	// Correct accel
	Fvector		accel;
	accel.set					(_accel);
	hit_slowmo					-=	dt;
	if (hit_slowmo<0)			hit_slowmo = 0.f;

	accel.mul					(1.f-hit_slowmo);
	
	// Calculate physics

	//m_PhysicMovementControl->SetPosition		(Position());
	//m_PhysicMovementControl->Calculate		(accel,0,jump,dt,false);
	//m_PhysicMovementControl->GetPosition		(Position());
	//Fvector vAccel;
	//m_PhysicMovementControl->vExternalImpulse.div(dt);

	//m_PhysicMovementControl->SetPosition		(Position());
	if(g_Alive())
	{
	if(mstate_real&mcClimb&&!cameras[eacFirstEye]->bClampYaw)accel.set(0.f,0.f,0.f);
	m_PhysicMovementControl->Calculate			(accel,cameras[cam_active]->vDirection,0,jump,dt,false);
	m_PhysicMovementControl->GetPosition		(Position());
	m_PhysicMovementControl->bSleep				=false;
	}
	///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////Update m_PhysicMovementControl///////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	//m_PhysicMovementControl->SetEnvironment(m_PhysicMovementControl->Environment(),m_PhysicMovementControl->OldEnvironment());//peOnGround,peAtWall,peInAir
	//m_PhysicMovementControl->SetPosition		(Position());
	//Fvector velocity=m_PhysicMovementControl->GetVelocity();
	//m_PhysicMovementControl->SetVelocity(velocity);
	//m_PhysicMovementControl->gcontact_Was=m_PhysicMovementControl->gcontact_Was;
	//m_PhysicMovementControl->SetContactSpeed(m_PhysicMovementControl->GetContactSpeed());
	//velocity.y=0.f;
	//m_PhysicMovementControl->SetActualVelocity(velocity.magnitude());
	//m_PhysicMovementControl->gcontact_HealthLost=m_PhysicMovementControl->gcontact_HealthLost;
	//m_PhysicMovementControl->gcontact_Power=m_PhysicMovementControl->gcontact_Power;

	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	
	//if (m_PhysicMovementControl->gcontact_Was) 
	//{
	//	Fvector correctV					= m_PhysicMovementControl->GetVelocity	();
	//	correctV.x							*= 0.1f;
	//	correctV.z							*= 0.1f;
	//	m_PhysicMovementControl->SetVelocity				(correctV);

	//	if (Local()) {
	//	g_pGameLevel->Cameras.AddEffector		(xr_new<CEffectorFall> (m_PhysicMovementControl->gcontact_Power));
	//	/*SGameMtlPair* pair	= GMLib.GetMaterialPair(0,1); R_ASSERT(pair);
	//	
	//	::Sound->play_at_pos						(pair->,this,Position());*/
	//	}
	//}
	

	{


		if (Local() && g_Alive()) {
			if (m_PhysicMovementControl->gcontact_Was) 
				g_pGameLevel->Cameras.AddEffector		(xr_new<CEffectorFall> (m_PhysicMovementControl->gcontact_Power));
			if (!fis_zero(m_PhysicMovementControl->gcontact_HealthLost))	{
				const ICollisionDamageInfo* di=m_PhysicMovementControl->CollisionDamageInfo();
				Fvector hdir;di->HitDir(hdir);
				SetHitInfo(this, NULL, 0);
				Hit	(m_PhysicMovementControl->gcontact_HealthLost,hdir,di->DamageInitiator(),m_PhysicMovementControl->ContactBone(),di->HitPos(),0.f,ALife::eHitTypeStrike);//s16(6 + 2*::Random.randI(0,2))
				if(!g_Alive())
					m_PhysicMovementControl->GetDeathPosition(Position());
			}
		}
	}	
}

float CActor::currentFOV()
{
	CWeapon* pWeapon = smart_cast<CWeapon*>(inventory().ActiveItem());	

	if (eacFirstEye == cam_active && pWeapon &&
		pWeapon->IsZoomed() && (!pWeapon->ZoomTexture() ||
		(!pWeapon->IsRotatingToZoom() && pWeapon->ZoomTexture())))
		return pWeapon->GetZoomFactor();
	else
		return DEFAULT_FOV;
}

BOOL	g_bEnableMPL	= FALSE;	//.
void CActor::UpdateCL	()
{
	if(m_holder)
		m_holder->UpdateEx( currentFOV() );

	m_snd_noise -= 0.3f*Device.fTimeDelta;
	//clamp(m_snd_noise,0.0f,4.f);

	VERIFY2								(_valid(renderable.xform),*cName());
	inherited::UpdateCL();
	VERIFY2								(_valid(renderable.xform),*cName());
	m_pPhysics_support->in_UpdateCL	();
	VERIFY2								(_valid(renderable.xform),*cName());

	//. *** dbgmp
	u32					_C				= 0;
	if (g_Alive() && Level().CurrentEntity() && (this->ID()!=Level().CurrentEntity()->ID()))	{
		if (g_bEnableMPL)	{
			// red for enemy
			if (GameID() == GAME_DEATHMATCH)								_C = color_xrgb(255,127,127);	// red
			else	{
				CEntity*	_me			= smart_cast<CEntity*> (this);
				CEntity*	_viewer		= smart_cast<CEntity*> (Level().CurrentEntity());
				if (_me && _viewer && _me->g_Team()!=_viewer->g_Team())		_C = color_xrgb(255,127,127);	// red
			};
		}
		string256		name;	strcpy		(name,cName().c_str());
		strlwr			(name)	;
		if (strstr(name,"anton"))			{
			// blue
			_C								= color_xrgb(127,127,255);
		}
	}
	if (_C)	{
		Fvector			C;
		Center			(C);
		dbgmp_light->set_rotation	(XFORM().k,XFORM().i);
		dbgmp_light->set_position	(C);
		dbgmp_light->set_active		(true);
		dbgmp_light->set_color		(2.f* color_get_R(_C)/255.f, 2.f* color_get_G(_C)/255.f, 2.f* color_get_B(_C)/255.f);
	} else {
		dbgmp_light->set_active		(false);
	}
	//. *** dbgmp end

	if (g_Alive()) 
	{
		//update the fog of war
//		Level().FogOfWar().UpdateFog(Position(), CFogOfWar::ACTOR_FOG_REMOVE_RADIUS);
		//-------------------------------------------------------------------------------

		//if(m_PhysicMovementControl->CharacterExist())
		//			m_PhysicMovementControl->InterpolatePosition(Position());
		//обновить информацию о предметах лежащих р€дом с актером
		PickupModeUpdate	();	
	}
	PickupModeUpdate_COD();
	//-------------------------------------------------------------------
//*
	m_bZoomAimingMode = false;
	CWeapon* pWeapon = smart_cast<CWeapon*>(inventory().ActiveItem());	
/*
	//обновить положение камеры и FOV 
	float dt = float(Device.dwTimeDelta)/1000.0f;

	if (eacFirstEye == cam_active && pWeapon &&
		pWeapon->IsZoomed() && (!pWeapon->ZoomTexture() ||
		(!pWeapon->IsRotatingToZoom() && pWeapon->ZoomTexture())))
		cam_Update(dt, pWeapon->GetZoomFactor());
	
	else 
		cam_Update(dt, DEFAULT_FOV);
*/
	cam_Update(float(Device.dwTimeDelta)/1000.0f, currentFOV());

	if(pWeapon)
	{
		if(pWeapon->IsZoomed()/* && !pWeapon->IsRotatingToZoom()*/)
		{
			float full_fire_disp = pWeapon->GetFireDispersion(false);

			CEffectorZoomInertion* S = smart_cast<CEffectorZoomInertion*>	(EffectorManager().GetEffector(eCEZoom));
			if(S) S->SetParams(full_fire_disp);

			//помнить, что если m_bZoomAimingMode = true
			//pWeapon->GetFireDispersion() вернет значение дисперсии без
			//учета положени€ стрелка, так как он спрашивает у нас GetWeaponAccuracy
			m_bZoomAimingMode = true;
		}

		//if(eacFirstEye == cam_active)
//		if(this == smart_cast<CActor*>(Level().CurrentEntity()))
		if(Level().CurrentEntity() && this->ID()==Level().CurrentEntity()->ID() )
		{
			float fire_disp = pWeapon->GetFireDispersion(false);
			HUD().SetCrosshairDisp(fire_disp);
			HUD().ShowCrosshair(true);
		}
	}
	else
	{
//		if(this == smart_cast<CActor*>(Level().CurrentEntity()))
		if(Level().CurrentEntity() && this->ID()==Level().CurrentEntity()->ID() )
		{
			HUD().SetCrosshairDisp(0.f);
			HUD().ShowCrosshair(false);
		}
	}

	UpdateDefferedMessages();
//*/
	//-------------------------------------------------------------------

	if (g_Alive()) 
		CStepManager::update();

	spatial.type |=STYPE_REACTTOSOUND;

	if(m_sndShockEffector)
	{
		if (this == Level().CurrentViewEntity())
		{
			m_sndShockEffector->Update();

			if(!m_sndShockEffector->Active())
				xr_delete(m_sndShockEffector);
		}
		else
			xr_delete(m_sndShockEffector);
	}
}

void CActor::shedule_Update	(u32 DT)
{
	setSVU(OnServer());

	//установить режим показа HUD дл€ текущего активного слота
	CHudItem* pHudItem = smart_cast<CHudItem*>(inventory().ActiveItem());	
	if(pHudItem && !pHudItem->object().getDestroy()) 
		pHudItem->SetHUDmode(HUDview());

	//обновление инвентар€
	UpdateInventoryOwner			(DT);
	GameTaskManager().UpdateTasks	();

	if(m_holder || !getEnabled() || !Ready())
	{
		inherited::shedule_Update		(DT);
		return;
	}

	// 
	clamp					(DT,0u,100u);
	float	dt	 			=  float(DT)/1000.f;

	// Check controls, create accel, prelimitary setup "mstate_real"
	float	Jump	= 0;
	//----------- for E3 -----------------------------
//	if (Local() && (OnClient() || Level().CurrentEntity()==this))
	if (Level().CurrentControlEntity() == this)
	//------------------------------------------------
	{
		//-----------------------------------
//		NetInput_Save			( );

//		NetInput_Send			( );
		//-----------------------------------
		
		g_cl_CheckControls		(mstate_wishful,NET_SavedAccel,Jump,dt);
		g_cl_Orientate			(mstate_real,dt);
		g_Orientate				(mstate_real,dt);

		g_Physics				(NET_SavedAccel,Jump,dt);
		
		g_cl_ValidateMState		(dt,mstate_wishful);
		g_SetAnimation			(mstate_real);
		UpdateMotionIcon		(mstate_real);

		// Check for game-contacts
		Fvector C; float R;		
		//m_PhysicMovementControl->GetBoundingSphere	(C,R);
		
		Center(C);
		R=Radius();
		feel_touch_update		(C,R);

		// Dropping
		if (b_DropActivated)	{
			f_DropPower			+= dt*0.1f;
			clamp				(f_DropPower,0.f,1.f);
		} else {
			f_DropPower			= 0.f;
		}
	}
	else 
	{
		if (NET.size())
		{
			NET_SavedAccel = NET_Last.p_accel;
			mstate_real = mstate_wishful = NET_Last.mstate;

			g_sv_Orientate				(NET_Last.mstate,dt			);
			g_Orientate					(NET_Last.mstate,dt			);
			g_Physics					(NET_Last.p_accel,Jump,dt	);
			g_cl_ValidateMState			(dt,mstate_wishful);
			g_SetAnimation				(mstate_real);

			if (NET_Last.mstate & mcCrouch)
			{
				if (isActorAccelerated(mstate_real, IsZoomAimingMode()))
					m_PhysicMovementControl->ActivateBox(1, true);
				else
					m_PhysicMovementControl->ActivateBox(2, true);
			}
			else 
				m_PhysicMovementControl->ActivateBox(0, true);
		};
	}
	make_Interpolation();

	//------------------------------------------------	


	// generic stuff
	inherited::shedule_Update	(DT);
	//------------------------------------------------
	if (OnServer())
	{
		Check_Weapon_ShowHideState();
	};	
	//------------------------------------------------
	//эффектор включаемый при ходьбе
	if (!pCamBobbing)
	{
		pCamBobbing = xr_new<CEffectorBobbing>	();
		EffectorManager().AddEffector			(pCamBobbing);
	}
	pCamBobbing->SetState						(mstate_real, conditions().IsLimping(), IsZoomAimingMode());

	//звук т€желого дыхани€ при уталости и хромании
	if(conditions().IsLimping() && g_Alive()){
		if(!m_bHeavyBreathSndPlaying){
			Fvector pos;
			pos.set(0,ACTOR_HEIGHT,0);
			m_HeavyBreathSnd.play_at_pos(this, pos, sm_Looped | sm_2D);
			m_bHeavyBreathSndPlaying = true;
		}else{
			Fvector pos;
			pos.set(0,ACTOR_HEIGHT,0);
			m_HeavyBreathSnd.set_position(pos);
		}
	}else if(m_bHeavyBreathSndPlaying){
		m_bHeavyBreathSndPlaying = false;
		m_HeavyBreathSnd.stop();
	}

	//если в режиме HUD, то сама модель актера не рисуетс€
	if(!character_physics_support()->IsRemoved())
										setVisible				(!HUDview	());
/*
	//установить режим показа HUD дл€ текущего активного слота
	CHudItem* pHudItem = smart_cast<CHudItem*>(inventory().ActiveItem());	
	if(pHudItem && !pHudItem->object().getDestroy()) 
		pHudItem->SetHUDmode(HUDview());
*/


	//что актер видит перед собой
	collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	
	m_pObjectWeLookingAt = smart_cast<CGameObject*>(RQ.O);

	if(RQ.O &&  RQ.range<inventory().GetTakeDist()) 
	{
		CGameObject						*game_object = smart_cast<CGameObject*>(RQ.O);
		m_pUsableObject					= smart_cast<CUsableScriptObject*>(game_object);
		inventory().m_pTarget			= smart_cast<PIItem>(game_object);
		m_pPersonWeLookingAt			= smart_cast<CInventoryOwner*>(game_object);
		m_pVehicleWeLookingAt			= smart_cast<CHolderCustom*>(game_object);
		CEntityAlive* pEntityAlive		= smart_cast<CEntityAlive*>(game_object);
		
		if (GameID() == GAME_SINGLE )
		{
//			CStringTable string_table;
			// јнализируем какой объект мы видим, и назначаем соответсвующее
			// действие по умолчанию, которое будет определ€ть всплывающую 
			// подсказку
			if (m_pUsableObject && m_pUsableObject->tip_text())
			{
				m_sDefaultObjAction = m_pUsableObject->tip_text();
			}
			else
			{
				if (m_pPersonWeLookingAt && pEntityAlive->g_Alive())
					m_sDefaultObjAction = m_sCharacterUseAction;//string_table("character_use");

				else if (pEntityAlive && !pEntityAlive->g_Alive())
					m_sDefaultObjAction = m_sDeadCharacterUseAction;//string_table("dead_character_use");

				else if (m_pVehicleWeLookingAt)
					m_sDefaultObjAction = m_sCarCharacterUseAction;//string_table("car_character_use");

				else if (inventory().m_pTarget)
					m_sDefaultObjAction = m_sInventoryItemUseAction;//string_table("inventory_item_use");
				else 
					m_sDefaultObjAction = NULL;
			}
		}
	}
	else 
	{
		inventory().m_pTarget	= NULL;
		m_pPersonWeLookingAt	= NULL;
		m_sDefaultObjAction		= NULL;
		m_pUsableObject			= NULL;
	}

/*
	//обновление инвентар€ и торговли
	UpdateInventoryOwner(DT);
*/
	//дл€ режима сна
	UpdateSleep();

	//дл€ свойст артефактов, наход€щихс€ на по€се
	UpdateArtefactsOnBelt();
	m_pPhysics_support->in_shedule_Update(DT);
	///////////////////////////////////////////////
	Check_for_AutoPickUp();
	if (GameID() == GAME_SINGLE)
	{
		Fvector2 pos2d;
		pos2d.set(Position().x, Position().z);
		CLevelFogOfWar* F = Level().FogOfWarMngr().GetFogOfWar(Level().name());
		if(F)
			F->Open(pos2d);
	};
};

void CActor::renderable_Render	()
{
	inherited::renderable_Render			();
	if (!HUDview())
		CInventoryOwner::renderable_Render	();
}

BOOL CActor::renderable_ShadowGenerate	() 
{
	if(m_holder)
		return FALSE;
	
	return inherited::renderable_ShadowGenerate();
}



void CActor::g_PerformDrop	( )
{
//	VERIFY					(b_DropActivated);
	b_DropActivated			= FALSE;

	PIItem pItem = inventory().ActiveItem();
	if (0==pItem) return;
	pItem->Drop();

	/*NET_Packet				P;
	u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
	P.w_u16					(u16(O->ID()));
	u_EventSend				(P);*/
}


extern	BOOL	g_ShowAnimationInfo		;
// HUD
void CActor::OnHUDDraw	(CCustomHUD* /**hud/**/)
{
	//CWeapon* W			= Weapons->ActiveWeapon();
	//if (W)				W->renderable_Render		();
	//CWeapon *W = smart_cast<CWeapon*>(inventory().ActiveItem()); if(W) W->renderable_Render();

	if(inventory().ActiveItem()  ) {
		inventory().ActiveItem()->renderable_Render();
	}

#ifndef NDEBUG
	if (Level().CurrentControlEntity() == this && g_ShowAnimationInfo)
	{
		string128 buf;
		HUD().Font().pFontSmall->SetColor	(0xffffffff);
		HUD().Font().pFontSmall->OutSet	(170,530);
		HUD().Font().pFontSmall->OutNext	("Position:      [%3.2f, %3.2f, %3.2f]",VPUSH(Position()));
		HUD().Font().pFontSmall->OutNext	("Velocity:      [%3.2f, %3.2f, %3.2f]",VPUSH(m_PhysicMovementControl->GetVelocity()));
		HUD().Font().pFontSmall->OutNext	("Vel Magnitude: [%3.2f]",m_PhysicMovementControl->GetVelocityMagnitude());
		HUD().Font().pFontSmall->OutNext	("Vel Actual:    [%3.2f]",m_PhysicMovementControl->GetVelocityActual());
		switch (m_PhysicMovementControl->Environment())
		{
		case CPHMovementControl::peOnGround:	strcpy(buf,"ground");			break;
		case CPHMovementControl::peInAir:		strcpy(buf,"air");				break;
		case CPHMovementControl::peAtWall:		strcpy(buf,"wall");				break;
		}
		HUD().Font().pFontSmall->OutNext	(buf);

		if (IReceived != 0)
		{
			float Size = 0;
			Size = HUD().Font().pFontSmall->GetSize();
			HUD().Font().pFontSmall->SetSize(Size*2);
			HUD().Font().pFontSmall->SetColor	(0xffff0000);
			HUD().Font().pFontSmall->OutNext ("Input :		[%3.2f]", ICoincidenced/IReceived * 100.0f);
			HUD().Font().pFontSmall->SetSize(Size);
		};
	};
#endif
}

void CActor::RenderIndicator			(Fvector dpos, float r1, float r2, ref_shader IndShader)
{
	if (!g_Alive()) return;

	u32			dwOffset = 0,dwCount = 0;
	FVF::LIT* pv_start				= (FVF::LIT*)RCache.Vertex.Lock(4,hFriendlyIndicator->vb_stride,dwOffset);
	FVF::LIT* pv					= pv_start;
	// base rect

	CBoneInstance& BI = smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(u16(m_head));
	Fmatrix M;
	smart_cast<CKinematics*>(Visual())->CalculateBones	();
	M.mul						(XFORM(),BI.mTransform);

	Fvector pos = M.c; pos.add(dpos);
	const Fvector& T        = Device.vCameraTop;
	const Fvector& R        = Device.vCameraRight;
	Fvector Vr, Vt;
	Vr.x            = R.x*r1;
	Vr.y            = R.y*r1;
	Vr.z            = R.z*r1;
	Vt.x            = T.x*r2;
	Vt.y            = T.y*r2;
	Vt.z            = T.z*r2;

	Fvector         a,b,c,d;
	a.sub           (Vt,Vr);
	b.add           (Vt,Vr);
	c.invert        (a);
	d.invert        (b);
	pv->set         (d.x+pos.x,d.y+pos.y,d.z+pos.z, 0xffffffff, 0.f,1.f);        pv++;
	pv->set         (a.x+pos.x,a.y+pos.y,a.z+pos.z, 0xffffffff, 0.f,0.f);        pv++;
	pv->set         (c.x+pos.x,c.y+pos.y,c.z+pos.z, 0xffffffff, 1.f,1.f);        pv++;
	pv->set         (b.x+pos.x,b.y+pos.y,b.z+pos.z, 0xffffffff, 1.f,0.f);        pv++;
	// render	
	dwCount 				= u32(pv-pv_start);
	RCache.Vertex.Unlock	(dwCount,hFriendlyIndicator->vb_stride);

	RCache.set_xform_world		(Fidentity);
	RCache.set_Shader			(IndShader);
	RCache.set_Geometry			(hFriendlyIndicator);
	RCache.Render	   			(D3DPT_TRIANGLESTRIP,dwOffset,0, dwCount, 0, 2);
};

static float mid_size = 0.097f;
static float fontsize = 15.0f;
static float upsize	= 0.33f;
void CActor::RenderText				(LPCSTR Text, Fvector dpos, float* pdup, u32 color)
{
	if (!g_Alive()) return;
	
	CBoneInstance& BI = smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(u16(m_head));
	Fmatrix M;
	smart_cast<CKinematics*>(Visual())->CalculateBones	();
	M.mul						(XFORM(),BI.mTransform);
	//------------------------------------------------
	Fvector v0, v1;
	v0.set(M.c); v1.set(M.c);
	Fvector T        = Device.vCameraTop;
	v1.add(T);

	Fvector v0r, v1r;
	Device.mFullTransform.transform(v0r,v0);
	Device.mFullTransform.transform(v1r,v1);
	float size = v1r.distance_to(v0r);
	float OldFontSize = HUD().Font().pFontDI->GetSize		();	
	float delta_up = 0.0f;
	if (size < mid_size) delta_up = upsize;
	else delta_up = upsize*(mid_size/size);
	dpos.y += delta_up;
	if (size > mid_size) size = mid_size;
	float NewFontSize = size/mid_size * fontsize;
	//------------------------------------------------
	M.c.y += dpos.y;

	Fvector4 v_res;	
	Device.mFullTransform.transform(v_res,M.c);

	if (v_res.z < 0 || v_res.w < 0)	return;
	if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) return;

	float x = (1.f + v_res.x)/2.f * (Device.dwWidth);
	float y = (1.f - v_res.y)/2.f * (Device.dwHeight);

	HUD().Font().pFontDI->SetAligment	(CGameFont::alCenter);
	HUD().Font().pFontDI->SetColor		(color);
	HUD().Font().pFontDI->SetSize		(NewFontSize);
	HUD().Font().pFontDI->Out			(x,y,Text);
	//-------------------------------------------------
	HUD().Font().pFontDI->SetSize(OldFontSize);
	*pdup = delta_up;
};

void CActor::SetPhPosition(const Fmatrix &transform)
{
	if(!m_pPhysicsShell){ 
		m_PhysicMovementControl->SetPosition(transform.c);
	}
	//else m_phSkeleton->S
}

void CActor::ForceTransform(const Fmatrix& m)
{
	if(!g_Alive())				return;
	XFORM().set					(m);
	if(m_PhysicMovementControl->CharacterExist()) m_PhysicMovementControl->EnableCharacter	();
	m_PhysicMovementControl->SetPosition		(m.c);
}

ENGINE_API extern float		psHUD_FOV;
float CActor::Radius()const
{ 
	float R		= inherited::Radius();
	CWeapon* W	= smart_cast<CWeapon*>(inventory().ActiveItem());
	if (W) R	+= W->Radius();
	//	if (HUDview()) R *= 1.f/psHUD_FOV;
	return R;
}

bool		CActor::use_bolts				() const
{
	if (GameID() != GAME_SINGLE) return false;
	return CInventoryOwner::use_bolts();
};

INT		g_iCorpseRemove = 1;

bool  CActor::NeedToDestroyObject() const
{
	if(GameID() == GAME_SINGLE)
	{
		return false;
	}
	else 
	{
		if (g_Alive()) return false;
		if (g_iCorpseRemove == -1) return false;
		if (g_iCorpseRemove == 0 && m_bAllowDeathRemove) return true;
		if(TimePassedAfterDeath()>m_dwBodyRemoveTime && m_bAllowDeathRemove)
			return true;
		else
			return false;
	}
}

ALife::_TIME_ID	 CActor::TimePassedAfterDeath()	const
{
	if(!g_Alive())
		return Level().timeServer() - GetLevelDeathTime();
	else
		return 0;
}


void CActor::OnItemTake			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemTake(inventory_item);
	if (OnClient()) return;

//	if (GameID()!=GAME_SINGLE)
//			SpawnAmmoForWeapon(inventory_item);
}

void CActor::OnItemDrop			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemDrop(inventory_item);
//	if (OnClient()) return;

	CArtefact* artefact = smart_cast<CArtefact*>(inventory_item);
	if(artefact && artefact->m_eItemPlace == eItemPlaceBelt)
		MoveArtefactBelt(artefact, false);
/*
	switch (GameID())
	{
	case GAME_DEATHMATCH:
	case GAME_TEAMDEATHMATCH:
	case GAME_ARTEFACTHUNT:
		{
			RemoveAmmoForWeapon(inventory_item);
		}break;
	};
	*/
}


void CActor::OnItemDropUpdate ()
{
	CInventoryOwner::OnItemDropUpdate ();

	//на тот случай если была выкинута присоедин€ема€ вещь (типа фонарика)
	//а у нас есть еще одна
	TIItemContainer::iterator					I = inventory().m_all.begin();
	TIItemContainer::iterator					E = inventory().m_all.end();
	for ( ; I != E; ++I)
		if(!(*I)->m_drop && !attached(*I))
			attach(*I);
}

//////////////////////////////////////////////////////////////////////////


void CActor::OnItemRuck		(CInventoryItem *inventory_item, EItemPlace previous_place)
{
	CInventoryOwner::OnItemRuck(inventory_item, previous_place);

	CArtefact* artefact = smart_cast<CArtefact*>(inventory_item);
	if(artefact && previous_place == eItemPlaceBelt)
		MoveArtefactBelt(artefact, false);
}
void CActor::OnItemBelt		(CInventoryItem *inventory_item, EItemPlace previous_place)
{
	CInventoryOwner::OnItemBelt(inventory_item, previous_place);

	CArtefact* artefact = smart_cast<CArtefact*>(inventory_item);
	if(artefact)
		MoveArtefactBelt(artefact, true);
}


void CActor::MoveArtefactBelt(const CArtefact* artefact, bool on_belt)
{
	VERIFY(artefact);

	//повесить артефакт на по€с
	if(on_belt)
	{
		VERIFY(m_ArtefactsOnBelt.end() == std::find(m_ArtefactsOnBelt.begin(), m_ArtefactsOnBelt.end(), artefact));
		m_ArtefactsOnBelt.push_back(artefact);
	}
	else
	{
		xr_vector<const CArtefact*>::iterator it = std::remove(m_ArtefactsOnBelt.begin(), m_ArtefactsOnBelt.end(), artefact);
		VERIFY(it != m_ArtefactsOnBelt.end());
		m_ArtefactsOnBelt.erase(it);
	}
	if (Level().CurrentViewEntity() == this)
		HUD().GetUI()->UIMainIngameWnd->m_artefactPanel->InitIcons(m_ArtefactsOnBelt);
}
#define ARTEFACTS_UPDATE_TIME 100
//old==1000

void CActor::UpdateArtefactsOnBelt()
{
	static u64 update_time = 0;

	float f_update_time = 0;

	if(update_time<ARTEFACTS_UPDATE_TIME)
	{
		update_time += conditions().delta_time();
		return;
	}
	else
	{
		f_update_time = static_cast<float>(update_time)/1000.f;
		update_time = 0;
	}

	for(TIItemContainer::iterator it = inventory().m_belt.begin(); 
		inventory().m_belt.end() != it; ++it) 
	{
		CArtefact*	artefact = smart_cast<CArtefact*>(*it);
		if(artefact && artefact->m_bActorPropertiesEnabled)
		{
			conditions().ChangeBleeding			(artefact->m_fBleedingRestoreSpeed*f_update_time);
			conditions().ChangeHealth			(artefact->m_fHealthRestoreSpeed*f_update_time);
			conditions().ChangePower			(artefact->m_fPowerRestoreSpeed*f_update_time);
			conditions().ChangeSatiety			(artefact->m_fSatietyRestoreSpeed*f_update_time);
			conditions().ChangeRadiation		(artefact->m_fRadiationRestoreSpeed*f_update_time);
		}
	}
}

float	CActor::HitArtefactsOnBelt		(float hit_power, ALife::EHitType hit_type)
{
	for(TIItemContainer::iterator it = inventory().m_belt.begin(); 
		inventory().m_belt.end() != it; ++it) 
	{
		CArtefact*	artefact = smart_cast<CArtefact*>(*it);
		if(artefact && artefact->m_bActorPropertiesEnabled)
			hit_power = artefact->m_ArtefactHitImmunities.AffectHit(hit_power, hit_type);
	}

	return hit_power;
}


//////////////////////////////////////////////////////////////////////////

void	CActor::SpawnAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

	CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(inventory().GetAny( *(pWM->m_ammoTypes[0]) ));
	if (!pAmmo) 
		pWM->SpawnAmmo(0xffffffff, NULL, ID());
};

void	CActor::RemoveAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

	CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(inventory().GetAny(*(pWM->m_ammoTypes[0]) ));
	if (!pAmmo) return;
	//--- мы нашли патроны к текущему оружию	
	/*
	//--- провер€ем не подход€т ли они к чему-то еще
	bool CanRemove = true;
	TIItemContainer::const_iterator I = inventory().m_all.begin();//, B = I;
	TIItemContainer::const_iterator E = inventory().m_all.end();
	for ( ; I != E; ++I)
	{
		CInventoryItem* pItem = (*I);//->m_pIItem;
		CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pItem);
		if (!pWM || !pWM->AutoSpawnAmmo()) continue;
		if (pWM == pIItem) continue;
		if (pWM->m_ammoTypes[0] != pAmmo->CInventoryItem::object().cNameSect()) continue;
		CanRemove = false;
		break;
	};

	if (!CanRemove) return;
	*/
	pAmmo->DestroyObject();
//	NET_Packet			P;
//	u_EventGen			(P,GE_DESTROY,pAmmo->ID());
//	u_EventSend			(P);
};

void	CActor::SetZoomRndSeed		(s32 Seed)
{
	if (0 != Seed) m_ZoomRndSeed = Seed;
	else m_ZoomRndSeed = s32(Level().timeServer_Async());
};

void	CActor::SetShotRndSeed		(s32 Seed)
{
	if (0 != Seed) m_ShotRndSeed = Seed;
	else m_ShotRndSeed = s32(Level().timeServer_Async());
};

Fvector CActor::GetMissileOffset	() const
{
	return m_vMissileOffset;
}

void CActor::SetMissileOffset		(const Fvector &vNewOffset)
{
	m_vMissileOffset.set(vNewOffset);
}

void CActor::spawn_supplies			()
{
	inherited::spawn_supplies		();
	CInventoryOwner::spawn_supplies	();
}


void CActor::AnimTorsoPlayCallBack(CBlend* B)
{
	CActor* actor		= (CActor*)B->CallbackParam;
	actor->m_bAnimTorsoPlayed = FALSE;
}

void CActor::UpdateMotionIcon(u32 mstate_rl)
{
	CUIMotionIcon		&motion_icon=HUD().GetUI()->UIMainIngameWnd->MotionIcon();
	if(mstate_rl&mcClimb)
	{
		motion_icon.ShowState(CUIMotionIcon::stClimb);
	}
	else
	{
		if(mstate_rl&mcCrouch)
		{
			if (!isActorAccelerated(mstate_rl, IsZoomAimingMode()))
				motion_icon.ShowState(CUIMotionIcon::stCreep);
			else
				motion_icon.ShowState(CUIMotionIcon::stCrouch);
		}
		else 
			motion_icon.ShowState(CUIMotionIcon::stNormal);
	}
}


CPHDestroyable*	CActor::ph_destroyable	()
{
	return smart_cast<CPHDestroyable*>(character_physics_support());
}

CEntityCondition *CActor::create_entity_condition	()
{
	return		(m_entity_condition = xr_new<CActorCondition>(this));
}

DLL_Pure *CActor::_construct			()
{
	CEntityAlive::_construct		();
	CInventoryOwner::_construct		();
	CStepManager::_construct		();

	return							(this);
}

bool CActor::use_center_to_aim			() const
{
	return							(!(mstate_real&mcCrouch));
}
