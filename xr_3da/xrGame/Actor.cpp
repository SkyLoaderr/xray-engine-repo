// Actor.cpp: implementation of the CActor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Actor_Flags.h"

#include "hudmanager.h"

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
	contacts_registry		= xr_new<CKnownContactsRegistryWrapper	>();
	encyclopedia_registry	= xr_new<CEncyclopediaRegistryWrapper	>();
	game_task_registry		= xr_new<CGameTaskRegistryWrapper		>();
	game_news_registry		= xr_new<CGameNewsRegistryWrapper		>();
	// Cameras
	cameras[eacFirstEye]	= xr_new<CCameraFirstEye>	(this, pSettings, "actor_firsteye_cam", 0);
	cameras[eacLookAt]		= xr_new<CCameraLook>		(this, pSettings, "actor_look_cam",		0);
	cameras[eacFreeLook]	= xr_new<CCameraLook>		(this, pSettings, "actor_free_cam",		0);
	cam_active				= eacFirstEye;
	fPrevCamPos				= 0;
	
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
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::actor);
#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW);
#endif

	//разрешить использование пояса в inventory
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
	m_entity_condition = NULL;
	m_material_manager = 0;
}


CActor::~CActor()
{
	xr_delete				(contacts_registry);
	xr_delete				(encyclopedia_registry);
	xr_delete				(game_task_registry);
	xr_delete				(game_news_registry);
	xr_delete				(m_material_manager);
#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif
	//xr_delete(Weapons);
	for (int i=0; i<eacMaxCam; ++i) xr_delete(cameras[i]);

	// sounds 2D
	m_HeavyBreathSnd.destroy();
	// sounds 3D
	for (i=0; i<SND_HIT_COUNT; ++i) ::Sound->destroy(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; ++i) ::Sound->destroy(sndDie[i]);

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
}

void CActor::Load	(LPCSTR section )
{
	Msg						("Loading actor: %s",section);
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
	if(pSettings->line_exist(section,"restrictor_radius"))
		m_PhysicMovementControl->SetActorRestrictorRadius(pSettings->r_float(section,"restrictor_radius"));
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

	//загрузить параметры смещения firepoint
	m_vMissileOffset	= pSettings->r_fvector3(section,"missile_throw_offset");

	//Weapons				= xr_new<CWeaponList> (this);

	// sounds
	char buf[256];

	::Sound->create		(sndHit[0],			TRUE,	strconcat(buf,*cName(),"\\hurt1"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[1],			TRUE,	strconcat(buf,*cName(),"\\hurt2"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[2],			TRUE,	strconcat(buf,*cName(),"\\hurt3"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[3],			TRUE,	strconcat(buf,*cName(),"\\hurt4"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndDie[0],			TRUE,	strconcat(buf,*cName(),"\\die0"), SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[1],			TRUE,	strconcat(buf,*cName(),"\\die1"), SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[2],			TRUE,	strconcat(buf,*cName(),"\\die2"), SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[3],			TRUE,	strconcat(buf,*cName(),"\\die3"), SOUND_TYPE_MONSTER_DYING);

	m_HeavyBreathSnd.create(TRUE, pSettings->r_string(section,"heavy_breath_snd"), SOUND_TYPE_MONSTER_INJURING);
	m_bHeavyBreathSndPlaying = false;

	cam_Set					(eacFirstEye);

	// motions
	m_current_legs_blend= 0;
	m_current_jump_blend= 0;
	m_current_legs		= 0;
	m_current_torso		= 0;
	m_current_head		= 0;
	// sheduler
	shedule.t_min		= shedule.t_max = 1;

	// настройки дисперсии стрельбы
	m_fDispBase				= pSettings->r_float		(section,"disp_base"		 );
	m_fDispBase				= deg2rad(m_fDispBase);
	m_fDispVelFactor		= pSettings->r_float		(section,"disp_vel_factor"	 );
	m_fDispAccelFactor		= pSettings->r_float		(section,"disp_accel_factor" );
	m_fDispCrouchFactor		= pSettings->r_float		(section,"disp_crouch_factor");

	if (pSettings->line_exist(section, "default_outfit"))
		SetDefaultVisualOutfit(pSettings->r_string(section, "default_outfit"));
	else
		SetDefaultVisualOutfit(NULL);

	invincibility_fire_shield_1st			= NULL;
	if (pSettings->line_exist(section,"Invincibility_Shield_1st"))
	{
		invincibility_fire_shield_1st			= pSettings->r_string(section,"Invincibility_Shield_1st");
	};	
	invincibility_fire_shield_3rd			= NULL;
	if (pSettings->line_exist(section,"Invincibility_Shield_3rd"))
	{
		invincibility_fire_shield_3rd			= pSettings->r_string(section,"Invincibility_Shield_3rd");
	};	

	//-----------------------------------------
	m_AutoPickUp_AABB.set(0.02f, 0.02f, 0.02f);
	if (pSettings->line_exist(section, "AutoPickUp_AABB"))
		m_AutoPickUp_AABB = pSettings->r_fvector3(section, "AutoPickUp_AABB");

	m_AutoPickUp_AABB_Offset.set(0, 0, 0);
	if (pSettings->line_exist(section, "AutoPickUp_AABB_offs"))
		m_AutoPickUp_AABB_Offset = pSettings->r_fvector3(section, "AutoPickUp_AABB_offs");
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
	
	if (GameID() != GAME_SINGLE)
	{
		game_PlayerState* ps = Game().GetPlayerByGameID(ID());
		if (ps && ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE) && Device.dwFrame != last_hit_frame)
		{		
			// вычислить позицию и направленность партикла
			Fmatrix pos; 
			
			CParticlesPlayer::MakeXFORM(this,element,dir,position_in_bone_space,pos);

			// установить particles
			CParticlesObject* ps = NULL;

			if (eacFirstEye == cam_active && this == Level().CurrentEntity())
				ps = xr_new<CParticlesObject>(invincibility_fire_shield_1st);
			else
				ps = xr_new<CParticlesObject>(invincibility_fire_shield_3rd);

			ps->UpdateParent(pos,Fvector().set(0.f,0.f,0.f));
			Level().ps_needtoplay.push_back(ps);
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
		inherited::Hit	(iLost,dir,who,element,position_in_bone_space, impulse, hit_type);
		break;
	}


}

void CActor::HitSignal(float perc, Fvector& vLocalDir, CObject* who, s16 element)
{
	if (g_Alive()) 
	{
		ref_sound& S = sndHit[Random.randI(SND_HIT_COUNT)];
		if (S.feedback) return;

		// Play hit-ref_sound
		::Sound->play_at_pos	(S,this,Position());

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
		CMotionDef *tpMotionDef = m_anims->m_normal.m_damage[iFloor(tpKinematics->LL_GetBoneInstance(element).get_param(1) + (angle_difference(r_model_yaw + r_model_yaw_delta,yaw) <= PI_DIV_2 ? 0 : 1))];
		float power_factor = perc/100.f; clamp(power_factor,0.f,1.f);
		VERIFY(tpMotionDef);
		tpKinematics->PlayFX(tpMotionDef,power_factor);
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


	///!!! чистка пояса
	TIItemList &l_blist = inventory().m_belt;
	while (!l_blist.empty())	inventory().Ruck(l_blist.front());

	if (OnServer() && GameID() != GAME_SINGLE)
	{
		bool MedKitDropped = false;
		//if we are on server and actor has PDA - destroy PDA
		TIItemList &l_rlist = inventory().m_ruck;
		for(PPIItem l_it = l_rlist.begin(); l_rlist.end() != l_it; ++l_it)
		{
			if (GameID() == GAME_ARTEFACTHUNT)
			{
				if ((*l_it)->object().CLS_ID == CLSID_ARTEFACT)
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

			CCustomOutfit *pOutfit = smart_cast<CCustomOutfit *> (*l_it);
			if (pOutfit) continue;

			//пока у нас нельзя обыскивать трупы, удаляем все объекты из инвентаря
//			if ((*l_it)->CLS_ID == CLSID_DEVICE_PDA)
			{
				NET_Packet P;
				u_EventGen(P,GE_DESTROY,(*l_it)->object().ID());
				u_EventSend(P, TRUE);
			}
		};
/*
		for (xr_vector<u16>::iterator itr = ItemsToRemove.begin(); ItemsToRemove.end() != itr; itr++ )
		{
			NET_Packet P;
			u_EventGen(P,GE_DESTROY,(*itr));
			u_EventSend(P, TRUE);
		};
*/
	};
	//-------------------------------------
	// Play ref_sound
	::Sound->play_at_pos		(sndDie[Random.randI(SND_DIE_COUNT)],this,Position());
	cam_Set					(eacFreeLook);

	mstate_wishful	&=		~mcAnyMove;
	mstate_real		&=		~mcAnyMove;


	//остановить звук тяжелого дыхания
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
			Fvector D; D.set					(0,1,0);
			Fvector positionInBoneSpace; positionInBoneSpace.set(0,0,0);
			if (m_PhysicMovementControl->gcontact_HealthLost)	{
				Hit	(m_PhysicMovementControl->gcontact_HealthLost,D,this,m_PhysicMovementControl->ContactBone(),positionInBoneSpace,0,ALife::eHitTypeStrike);//s16(6 + 2*::Random.randI(0,2))
				if(!g_Alive())
					m_PhysicMovementControl->GetDeathPosition(Position());
			}
		}
	}	
}

void CActor::UpdateCL()
{
	inherited::UpdateCL();
	m_pPhysics_support->in_UpdateCL	();
	if (g_Alive()) 
	{
		//update the fog of war
		Level().FogOfWar().UpdateFog(Position(), CFogOfWar::ACTOR_FOG_REMOVE_RADIUS);
		//-------------------------------------------------------------------------------

		//if(m_PhysicMovementControl->CharacterExist())
		//			m_PhysicMovementControl->InterpolatePosition(Position());
		//обновить информацию о предметах лежащих рядом с актером
		PickupModeUpdate	();	

		float				k =	(mstate_real&mcCrouch)?0.75f:1.f;
		float				tm = isAccelerated(mstate_real)?(PI/(k*10.f)):(PI/(k*7.f));
		float				s_k	= ((mstate_real&mcCrouch) ? CROUCH_SOUND_FACTOR : 1.f);
		float				s_vol = s_k * (isAccelerated(mstate_real) ? 1.f : ACCELERATED_SOUND_FACTOR);
		SGameMtlPair		*mtl_pair = GMLib.GetMaterialPair(material().self_material_idx(),material().last_material_idx());
		
		if(!m_holder)
		{
			material().set_run_mode(isAccelerated(mstate_real));
			material().update		(
				Device.fTimeDelta,
				s_vol,
				tm,
				!(mtl_pair && (mstate_real & mcAnyMove) && (!(mstate_real & (mcJump|mcFall))))
			);
		}

		// landing sounds
		//if (mtl_pair && (mstate_real & (mcLanding | mcLanding2))){
		//	if (!mtl_pair->CollideSounds.empty()){
		//		Fvector	s_pos		=	Position	();
		//		s_pos.y				+=	.15f;
		//		::Sound->play_at_pos(mtl_pair->CollideSounds[0],this,s_pos);
		//	}
		//}
	}
	//-------------------------------------------------------------------
//*
	CWeapon* pWeapon = smart_cast<CWeapon*>(inventory().ActiveItem());	
	m_bZoomAimingMode = false;

	//обновить положение камеры и FOV 
	float dt = float(Device.dwTimeDelta)/1000.0f;

	if (eacFirstEye == cam_active && pWeapon &&
		pWeapon->IsZoomed() && (!pWeapon->ZoomTexture() ||
		(!pWeapon->IsRotatingToZoom() && pWeapon->ZoomTexture())))
		cam_Update(dt, pWeapon->GetZoomFactor());
	
	else 
		cam_Update(dt, DEFAULT_FOV);

	if(pWeapon)
	{
		if(pWeapon->IsZoomed()/* && !pWeapon->IsRotatingToZoom()*/)
		{
			float full_fire_disp = pWeapon->GetFireDispersion(false);

			CEffectorZoomInertion* S = smart_cast<CEffectorZoomInertion*>	(EffectorManager().GetEffector(eCEZoom));
			if(S) S->SetParams(full_fire_disp);

			//помнить, что если m_bZoomAimingMode = true
			//pWeapon->GetFireDispersion() вернет значение дисперсии без
			//учета положения стрелка, так как он спрашивает у нас GetWeaponAccuracy
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
}

void CActor::shedule_Update	(u32 DT)
{
	setSVU(OnServer());

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
		NetInput_Save			( );

		NetInput_Send			( );
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
//			NET_Last = NET.back();
			mstate_real = mstate_wishful = NET_Last.mstate;
			g_sv_Orientate				(NET_Last.mstate,dt			);
			g_Orientate					(NET_Last.mstate,dt			);
			g_Physics					(NET_Last.p_accel,Jump,dt	);
			g_cl_ValidateMState			(dt,mstate_wishful);
			g_SetAnimation				(mstate_real);

			if (NET_Last.mstate & mcCrouch)
			{
				if (isAccelerated(mstate_real))
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
	pCamBobbing->SetState						(mstate_real, conditions().IsLimping());

	//звук тяжелого дыхания при уталости и хромании
	if(conditions().IsLimping() && g_Alive())
	{
		if(!m_bHeavyBreathSndPlaying)
		{
			Fvector pos;
			pos.set(Position());
			pos.y += ACTOR_HEIGHT;
			m_HeavyBreathSnd.play_at_pos(this, pos, sm_Looped | sm_2D);
			m_bHeavyBreathSndPlaying = true;
		}
		else
		{
			Fvector pos;
			pos.set(Position());
			pos.y += ACTOR_HEIGHT;
			m_HeavyBreathSnd.set_position(pos);
		}
	} 
	else if(m_bHeavyBreathSndPlaying)
	{
		m_bHeavyBreathSndPlaying = false;
		m_HeavyBreathSnd.stop();
	}

	//если в режиме HUD, то сама модель актера не рисуется
	if(!character_physics_support()->IsRemoved())
										setVisible				(!HUDview	());

	//установить режим показа HUD для текущего активного слота
	CHudItem* pHudItem = smart_cast<CHudItem*>(inventory().ActiveItem());	
	if(pHudItem && !pHudItem->object().getDestroy()) 
		pHudItem->SetHUDmode(HUDview());



	//что актер видит перед собой
	collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	
	m_pObjectWeLookingAt = smart_cast<CGameObject*>(RQ.O);

	if(RQ.O &&  RQ.range<inventory().GetTakeDist()) 
	{
		m_pUsableObject					= smart_cast<CUsableScriptObject*>(RQ.O);
		inventory().m_pTarget			= smart_cast<PIItem>(RQ.O);
		m_pPersonWeLookingAt			= smart_cast<CInventoryOwner*>(RQ.O);
		m_pVehicleWeLookingAt			= smart_cast<CHolderCustom*>(RQ.O);
		CEntityAlive* pEntityAlive		= smart_cast<CEntityAlive*>(RQ.O);
		
		if (GameID() == GAME_SINGLE )
		{
			CStringTable string_table;
			// Анализируем какой объект мы видим, и назначаем соответсвующее
			// действие по умолчанию, которое будет определять всплывающую 
			// подсказку
			if (m_pUsableObject && m_pUsableObject->tip_text())
			{
				m_sDefaultObjAction = m_pUsableObject->tip_text();
			}
			else
			{
				if (m_pPersonWeLookingAt && pEntityAlive->g_Alive())
					m_sDefaultObjAction = string_table("character_use");

				else if (pEntityAlive && !pEntityAlive->g_Alive())
					m_sDefaultObjAction = string_table("dead_character_use");

				else if (m_pVehicleWeLookingAt)
					m_sDefaultObjAction = string_table("car_character_use");

				else if (inventory().m_pTarget)
					m_sDefaultObjAction = string_table("inventory_item_use");
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


	//обновление инвентаря и торговли
	UpdateInventoryOwner(DT);

	//для режима сна
	UpdateSleep();

	//для свойст артефактов, находящихся на поясе
	UpdateArtefactsOnBelt();
	m_pPhysics_support->in_shedule_Update(DT);
	///////////////////////////////////////////////
	Check_for_AutoPickUp();
}

void CActor::renderable_Render	()
{
	inherited::renderable_Render			();

	if (!m_holder)	{
		CInventoryOwner::renderable_Render	();
	}
	//-------------------------------------------------------------
	if (g_Alive())
	{
	}
}

BOOL CActor::renderable_ShadowGenerate	() 
{
	if(m_holder)
		return FALSE;
	
	return inherited::renderable_ShadowGenerate();
}



void CActor::g_PerformDrop	( )
{
	VERIFY					(b_DropActivated);
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

	if(inventory().ActiveItem()&&!m_holder) {
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

bool  CActor::NeedToDestroyObject() const
{
	if(GameID() == GAME_SINGLE)
	{
		return false;
	}
	else //if(this != Level().CurrentEntity())
	{
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
	if (OnClient()) return;

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

	//на тот случай если была выкинута присоединяемая вещь (типа фонарика)
	//а у нас есть еще одна
	PSPIItem					I = inventory().m_all.begin();
	PSPIItem					E = inventory().m_all.end();
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

	//повесить артефакт на пояс
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
		HUD().GetUI()->UIMainIngameWnd.m_artefactPanel.InitIcons(m_ArtefactsOnBelt);
}
#define ARTEFACTS_UPDATE_TIME 1000

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

	for(PPIItem it = inventory().m_belt.begin(); 
		inventory().m_belt.end() != it; ++it) 
	{
		CArtefact*	artefact = smart_cast<CArtefact*>(*it);
		if(artefact && artefact->m_bActorPropertiesEnabled)
		{
			conditions().ChangeBleeding(artefact->m_fBleedingRestoreSpeed*f_update_time);
			conditions().ChangeHealth(artefact->m_fHealthRestoreSpeed*f_update_time);
			conditions().ChangePower(artefact->m_fPowerRestoreSpeed*f_update_time);
			conditions().ChangeSatiety(artefact->m_fSatietyRestoreSpeed*f_update_time);
			conditions().ChangeRadiation(artefact->m_fRadiationRestoreSpeed*f_update_time);
		}
	}
}

float	CActor::HitArtefactsOnBelt		(float hit_power, ALife::EHitType hit_type)
{
	for(PPIItem it = inventory().m_belt.begin(); 
		inventory().m_belt.end() != it; ++it) 
	{
		CArtefact*	artefact = smart_cast<CArtefact*>(*it);
		if(artefact && artefact->m_bActorPropertiesEnabled)
			hit_power *= artefact->m_ArtefactHitImmunities.AffectHit(hit_power, hit_type);
	}

	return hit_power;
}


//////////////////////////////////////////////////////////////////////////

void	CActor::SpawnAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

	CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(inventory().Get(*(pWM->m_ammoTypes[0]), false));
	if (!pAmmo) 
		pWM->SpawnAmmo(0xffffffff, NULL, ID());
};

void	CActor::RemoveAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

	CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(inventory().Get(*(pWM->m_ammoTypes[0]), false));
	if (!pAmmo || !pAmmo->m_bCanBeUnlimited) return;
	//--- мы нашли патроны к текущему оружию	
	/*
	//--- проверяем не подходят ли они к чему-то еще
	bool CanRemove = true;
	TIItemSet::const_iterator I = inventory().m_all.begin();//, B = I;
	TIItemSet::const_iterator E = inventory().m_all.end();
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
	NET_Packet			P;
	u_EventGen			(P,GE_DESTROY,pAmmo->ID());
	u_EventSend			(P);
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
	CUIMotionIcon		&motion_icon=HUD().GetUI()->UIMainIngameWnd.MotionIcon();
	if(mstate_rl&mcCrouch)
	{
//		if(mstate_rl&mcAccel)
		if (!isAccelerated(mstate_rl))
			motion_icon.ShowState(CUIMotionIcon::stCreep);
		else
			motion_icon.ShowState(CUIMotionIcon::stCrouch);
	}
	else if(mstate_rl&mcClimb)
		motion_icon.ShowState(CUIMotionIcon::stClimb);
	else
		motion_icon.ShowState(CUIMotionIcon::stNormal);
}


CPHDestroyable*	CActor::ph_destroyable	()
{
	return smart_cast<CPHDestroyable*>(character_physics_support());
}

CEntityCondition *CActor::create_entity_condition	()
{
	return		(m_entity_condition = xr_new<CActorCondition>(this));
}

DLL_Pure *CActor::_construct		()
{
	m_material_manager				= xr_new<CMaterialManager>(this,m_PhysicMovementControl);
	CEntityAlive::_construct		();
	CInventoryOwner::_construct		();
	return							(this);
}