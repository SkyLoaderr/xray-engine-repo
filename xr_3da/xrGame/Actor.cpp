// Actor.cpp: implementation of the CActor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Actor_Flags.h"

#include "customitem.h"
#include "hudmanager.h"

#include "UI.h"
#include "PDA.h"
#include "Car.h"
#include "UIGameSP.h"
#include "xrserver_objects_alife_monsters.h"

#include "CameraLook.h"
#include "CameraFirstEye.h"


#include "../effectorfall.h"
#include "EffectorBobbing.h"
#include "EffectorPPHit.h"
#include "EffectorHit.h"
#include "ShootingHitEffector.h"
#include "SleepEffector.h"
#include "ActorEffector.h"
#include "EffectorZoomInertion.h"



// breakpoints
#include "../xr_input.h"

//
#include "Actor.h"
#include "ActorAnimation.h"

#include "HudItem.h"

#include "targetassault.h"
#include "targetcs.h"

#include "ai_sounds.h"
#include "ai_space.h"

#include "trade.h"
#include "LevelFogOfWar.h"

#include "inventory.h"

#include "Physics.h"

const u32		patch_frames	= 50;
const float		respawn_delay	= 1.f;
const float		respawn_auto	= 7.f;

static float IReceived = 0;
static float ICoincidenced = 0;


//skeleton

float	CActor::skel_airr_lin_factor;
float	CActor::skel_airr_ang_factor;
float	CActor::hinge_force_factor1;
float	CActor::skel_fatal_impulse_factor;

static const float	s_fLandingTime1		= 0.1f;// через сколько снять флаг Landing1 (т.е. включить следующую анимацию)
static const float	s_fLandingTime2		= 0.3f;// через сколько снять флаг Landing2 (т.е. включить следующую анимацию)
static const float	s_fFallTime			= 0.2f;
static const float	s_fJumpTime			= 0.3f;
static const float	s_fJumpGroundTime	= 0.1f;	// для снятия флажка Jump если на земле

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
	r_model_yaw				= 0;
	r_model_yaw_delta		= 0;
	r_model_yaw_dest		= 0;

	bStep					= FALSE;

	b_DropActivated			= 0;
	f_DropPower				= 0.f;

	m_fRunFactor			= 2.f;
	m_fCrouchFactor			= 0.2f;

	m_fCamHeightFactor		= 0.87f;

	m_fFallTime				=	s_fFallTime;
	m_bAnimTorsoPlayed		=	false;

//	self_gmtl_id			=	GAMEMTL_NONE;
//	last_gmtl_id			=	GAMEMTL_NONE;
	m_pPhysicsShell			=	NULL;
	bDeathInit				=	false;
	m_saved_dir.set(0,0,0);
	m_saved_impulse=0.f;
	m_vehicle				=	NULL;
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::actor);
#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW);
#endif

	InitTrade();

	//разрешить использование пояса в inventory
	inventory().SetBeltUseful(true);

	m_pPersonWeLookingAt	= NULL;
	m_pVehicleWeLookingAt	= NULL;
	m_bPickupMode			= false;

	////////////////////////////////////
	pStatGraph = NULL;
	dDesyncVec.set(0, 0, 0);

	m_pActorEffector = NULL;

	m_bZoomAimingMode = false;
}


CActor::~CActor()
{
#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif
	//xr_delete(Weapons);
	for (int i=0; i<eacMaxCam; ++i) xr_delete(cameras[i]);

	// sounds 2D
	::Sound->destroy(sndZoneHeart);
	::Sound->destroy(sndZoneDetector);

	// sounds 3D
	for (i=0; i<SND_HIT_COUNT; ++i) ::Sound->destroy(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; ++i) ::Sound->destroy(sndDie[i]);

	if(m_pPhysicsShell) {
		m_pPhysicsShell->Deactivate();
		xr_delete<CPhysicsShell>(m_pPhysicsShell);
	}

	xr_delete(m_pActorEffector);
}

void CActor::reinit	()
{
	m_PhysicMovementControl->CreateCharacter();
	m_PhysicMovementControl->SetPhysicsRefObject(this);
	CEntityAlive::reinit	();
	CInventoryOwner::reinit	();
	CDamageManager::reinit	();
	CMaterialManager::reinit();
	m_r_hand				= PKinematics(Visual())->LL_BoneID("bip01_r_hand");
	m_l_finger1				= PKinematics(Visual())->LL_BoneID("bip01_l_finger1");
	m_r_finger2				= PKinematics(Visual())->LL_BoneID("bip01_r_finger2");
}

void CActor::reload	(LPCSTR section)
{
	CEntityAlive::reload	(section);
	CInventoryOwner::reload	(section);
	CDamageManager::reload	(section);
	CMaterialManager::reload(section);
}

void CActor::Load	(LPCSTR section )
{
	Msg						("Loading actor: %s",section);
	inherited::Load			(section);
	CMaterialManager::Load	(section);
	CInventoryOwner::Load	(section);

	//////////////////////////////////////////////////////////////////////////
	ISpatial*		self			=	dynamic_cast<ISpatial*> (this);
	if (self)	{
		self->spatial.type	|=	STYPE_VISIBLEFORAI;
		self->spatial.type	&= ~STYPE_REACTTOSOUND;
	}
	//////////////////////////////////////////////////////////////////////////

	// m_PhysicMovementControl: General
	//m_PhysicMovementControl->SetParent		(this);
	Fbox	bb;

	// m_PhysicMovementControl: BOX
	Fvector	vBOX0_center= pSettings->r_fvector3	(section,"ph_box0_center"	);
	Fvector	vBOX0_size	= pSettings->r_fvector3	(section,"ph_box0_size"		);
	bb.set	(vBOX0_center,vBOX0_center); bb.grow(vBOX0_size);
	m_PhysicMovementControl->SetBox		(0,bb);

	// m_PhysicMovementControl: BOX
	Fvector	vBOX1_center= pSettings->r_fvector3	(section,"ph_box1_center"	);
	Fvector	vBOX1_size	= pSettings->r_fvector3	(section,"ph_box1_size"		);
	bb.set	(vBOX1_center,vBOX1_center); bb.grow(vBOX1_size);
	m_PhysicMovementControl->SetBox		(1,bb);

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


	// m_PhysicMovementControl: Frictions

	//float af, gf, wf;
	//af					= pSettings->r_float	(section,"ph_friction_air"	);
	//gf					= pSettings->r_float	(section,"ph_friction_ground");
	//wf					= pSettings->r_float	(section,"ph_friction_wall"	);
	//m_PhysicMovementControl->SetFriction	(af,wf,gf);

	// BOX activate
	m_PhysicMovementControl->ActivateBox	(0);

	m_PhysicMovementControl->Load(section);
	m_PhysicMovementControl->SetParent(this);

	m_fWalkAccel				= pSettings->r_float(section,"walk_accel");	
	m_fJumpSpeed				= pSettings->r_float(section,"jump_speed");
	m_fRunFactor				= pSettings->r_float(section,"run_coef");
	m_fCrouchFactor				= pSettings->r_float(section,"crouch_coef");
	skel_airr_lin_factor		= pSettings->r_float(section,"ph_skeleton_airr_lin_factor");
	skel_airr_ang_factor		= pSettings->r_float(section,"ph_skeleton_airr_ang_factor");
	hinge_force_factor1 		= pSettings->r_float(section,"ph_skeleton_hinger_factor1");
	skel_ddelay					= pSettings->r_s32	(section,"ph_skeleton_ddelay");
	skel_fatal_impulse_factor	= pSettings->r_float(section,"ph_skel_fatal_impulse_factor");
	m_fCamHeightFactor			= pSettings->r_float(section,"camera_height_factor");
	m_PhysicMovementControl->SetJumpUpVelocity(m_fJumpSpeed);

	m_fPickupInfoRadius	= pSettings->r_float(section,"pickup_info_radius");
	m_fSleepTimeFactor	= pSettings->r_float(section,"sleep_time_factor");

	//actor condition variables
	CActorCondition::Load(section);

	//загрузить параметры эффектора
	LoadShootingEffector	("shooting_effector");
	LoadSleepEffector		("sleep_effector");
	

	//Weapons				= xr_new<CWeaponList> (this);

	// sounds
	char buf[256];

	sndLanding.g_type	= SOUND_TYPE_WORLD_OBJECT_COLLIDING;
	::Sound->create		(sndZoneHeart,		TRUE,	"heart\\4",						SOUND_TYPE_MONSTER_STEP);
	::Sound->create		(sndZoneDetector,	TRUE,	"detectors\\geiger",			SOUND_TYPE_MONSTER_STEP);
	::Sound->create		(sndHit[0],			TRUE,	strconcat(buf,*cName(),"\\hurt1"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[1],			TRUE,	strconcat(buf,*cName(),"\\hurt2"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[2],			TRUE,	strconcat(buf,*cName(),"\\hurt3"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[3],			TRUE,	strconcat(buf,*cName(),"\\hurt4"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndDie[0],			TRUE,	strconcat(buf,*cName(),"\\die0"),SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[1],			TRUE,	strconcat(buf,*cName(),"\\die1"),SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[2],			TRUE,	strconcat(buf,*cName(),"\\die2"),SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[3],			TRUE,	strconcat(buf,*cName(),"\\die3"),SOUND_TYPE_MONSTER_DYING);

//	m_PhysicMovementControl->ActivateBox	(0);
	//m_PhysicMovementControl->ActivateBox	(0);
	cam_Set					(eacFirstEye);

	// motions
	m_current_legs_blend= 0;
	m_current_jump_blend= 0;
	m_current_legs		= 0;
	m_current_torso		= 0;

	// sheduler
	shedule.t_min		= shedule.t_max = 1;

	// настройки дисперсии стрельбы
	m_fDispBase				= pSettings->r_float		(section,"disp_base"		 );
	m_fDispBase				= deg2rad(m_fDispBase);
	m_fDispVelFactor		= pSettings->r_float		(section,"disp_vel_factor"	 );
	m_fDispAccelFactor		= pSettings->r_float		(section,"disp_accel_factor" );
	m_fDispCrouchFactor		= pSettings->r_float		(section,"disp_crouch_factor");

}


void CActor::Hit		(float iLost, Fvector &dir, CObject* who, s16 element,Fvector position_in_bone_space, float impulse, ALife::EHitType hit_type)
{
	if (g_Alive() && (hit_type == ALife::eHitTypeWound || hit_type == ALife::eHitTypeStrike))
	{
		m_PhysicMovementControl->ApplyImpulse(dir,impulse);
		m_saved_dir.set(dir);
		m_saved_position.set(position_in_bone_space);
		m_saved_impulse=impulse*skel_fatal_impulse_factor;
		m_saved_element=element;
	}
	else if(m_pPhysicsShell) 
		m_pPhysicsShell->applyImpulseTrace(position_in_bone_space,dir,impulse,element);
	//m_phSkeleton->applyImpulseTrace(position_in_bone_space,dir,impulse);
	else
	{
		m_saved_dir.set(dir);
		m_saved_impulse=impulse*skel_fatal_impulse_factor;
		m_saved_element=element;
		m_saved_position.set(position_in_bone_space);
	}


	if (!g_Alive()) return;

#ifndef _DEBUG
	if(Level().CurrentEntity() == this) {
		Level().Cameras.AddEffector(xr_new<CShootingHitEffectorPP>(	m_pShootingEffector->ppi,		m_pShootingEffector->time,		m_pShootingEffector->time_attack,		m_pShootingEffector->time_release));
		Level().Cameras.AddEffector(xr_new<CShootingHitEffector>(	m_pShootingEffector->ce_time,	m_pShootingEffector->ce_amplitude,m_pShootingEffector->ce_period_number,m_pShootingEffector->ce_power));
	}
#endif

	
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
			if (psActorFlags.test(AF_GODMODE))
			{
				//by Dandy for debug reasons
				//fEntityHealth += iLost;
			//	inherited::Hit(iLost,dir,who,element,position_in_bone_space, impulse, hit_type);
				return;
			}
			else inherited::Hit		(iLost,dir,who,element,position_in_bone_space, impulse, hit_type);
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
			float x		= _abs(vLocalDir.x);
			float z		= _abs(vLocalDir.z);
			if (z>x)	id = (vLocalDir.z<0)?2:0;
			else		id = (vLocalDir.x<0)?3:1;
			HUD().Hit	(id);
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
		CSkeletonAnimated *tpKinematics = PSkeletonAnimated(Visual());
#pragma todo("Dima to Dima : forward-back bone impulse direction has been determined incorrectly!")
		CMotionDef *tpMotionDef = m_anims.m_normal.m_damage[iFloor(tpKinematics->LL_GetBoneInstance(element).get_param(1) + (angle_difference(r_model_yaw + r_model_yaw_delta,yaw) <= PI_DIV_2 ? 0 : 1))];
		float power_factor = perc/100.f; clamp(power_factor,0.f,1.f);
		tpKinematics->PlayFX(tpMotionDef,power_factor);
	}
}

void CActor::Die	( )
{
	inherited::Die();

	//// Dima
	//b_DropActivated			= TRUE;
	//g_PerformDrop			();
	// @@@ WT
//	inventory().DropAll();
	//-------------------------------------
	xr_vector<CInventorySlot>::iterator I = inventory().m_slots.begin(), B = I;
	xr_vector<CInventorySlot>::iterator E = inventory().m_slots.end();
	for ( ; I != E; ++I)
		if ((I - B) == (int)inventory().GetActiveSlot()) 
			(*I).m_pIItem->Drop();
		else
			if((*I).m_pIItem) inventory().Ruck((*I).m_pIItem);
	///!!!
	TIItemList &l_list = inventory().m_ruck;
	for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
	{
		CArtifact* pArtifact = dynamic_cast<CArtifact*>(*l_it);
		if(pArtifact)
			pArtifact->Drop();
	}

	if (OnServer() && Game().type != GAME_SINGLE)
	{
		//if we are on server and actor has PDA - destroy PDA
		TIItemList &l_list = inventory().m_ruck;
		for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
		{
			if ((*l_it)->SUB_CLS_ID == CLSID_DEVICE_PDA)
			{
				NET_Packet P;
				u_EventGen(P,GE_DESTROY,(*l_it)->ID());
				u_EventSend(P, TRUE);
			}
		};
	};
	//-------------------------------------
	// Play ref_sound
	::Sound->play_at_pos		(sndDie[Random.randI(SND_DIE_COUNT)],this,Position());
	cam_Set					(eacFreeLook);

	mstate_wishful	&=		~mcAnyMove;
	mstate_real		&=		~mcAnyMove;
	create_Skeleton();
}

void CActor::g_Physics			(Fvector& _accel, float jump, float dt)
{
	if (!g_Alive())	{

		if(m_pPhysicsShell)
			if(m_pPhysicsShell->bActive && !fsimilar(0.f,m_saved_impulse))
			{
				m_pPhysicsShell->applyImpulseTrace(m_saved_position,m_saved_dir,m_saved_impulse*1.5f,m_saved_element);
				m_saved_impulse=0.f;
			}


			if(m_pPhysicsShell)
			{
				XFORM().set	(m_pPhysicsShell->mXFORM);
				if(skel_ddelay==0)
				{
					m_pPhysicsShell->set_JointResistance(5.f*hinge_force_factor1);
					m_pPhysicsShell->Enable();
				}
				--skel_ddelay;
			}
			else
			{if(bDeathInit)
			{
				//create_Skeleton();
				bDeathInit=false;
				return;
			}
			bDeathInit=true;
			}
			return;
	}

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

	m_PhysicMovementControl->Calculate			(_accel,0,jump,dt,false);
	m_PhysicMovementControl->GetPosition		(Position());
	m_PhysicMovementControl->bSleep=false;
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
	//	m_PhysicMovementControl->SetActualVelocity(velocity.magnitude());

	//m_PhysicMovementControl->gcontact_HealthLost=m_PhysicMovementControl->gcontact_HealthLost;
	//m_PhysicMovementControl->gcontact_Power=m_PhysicMovementControl->gcontact_Power;

	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/*
	if (m_PhysicMovementControl->gcontact_Was) 
	{
	Fvector correctV					= m_PhysicMovementControl->GetVelocity	();
	correctV.x							*= 0.1f;
	correctV.z							*= 0.1f;
	m_PhysicMovementControl->SetVelocity				(correctV);

	if (Local()) {
	g_pGameLevel->Cameras.AddEffector		(xr_new<CEffectorFall> (m_PhysicMovementControl->gcontact_Power));
	Fvector D; D.set					(0,1,0);
	if (m_PhysicMovementControl->gcontact_HealthLost)	Hit	(1.5f * m_PhysicMovementControl->gcontact_HealthLost,D,this,-1);
	}
	}
	*/	


	{
		//		SGameMtlPair* pair	= GMLib.GetMaterialPair(0,1); R_ASSERT(pair);
		//		::Sound->play_at_pos	(pair->HitSounds[0],this,Position());
		//		::Sound->play_at_pos						(sndLanding,this,Position());

		if (Local()) {

			if (m_PhysicMovementControl->gcontact_Was) 
				g_pGameLevel->Cameras.AddEffector		(xr_new<CEffectorFall> (m_PhysicMovementControl->gcontact_Power));
			Fvector D; D.set					(0,1,0);
			Fvector positionInBoneSpace; positionInBoneSpace.set(0,0,0);
			if (m_PhysicMovementControl->gcontact_HealthLost)	{
				Hit	(m_PhysicMovementControl->gcontact_HealthLost,D,this,m_PhysicMovementControl->ContactBone(),positionInBoneSpace,0,ALife::eHitTypeStrike);//s16(6 + 2*::Random.randI(0,2))
				if(g_Alive()<=0)
					m_PhysicMovementControl->GetDeathPosition(Position());
			}
		}
	}	
}

void CActor::UpdateCL()
{
	inherited::UpdateCL();

	if (!g_Alive())	
	{
		if(m_pPhysicsShell)
		{
			XFORM().set(m_pPhysicsShell->mXFORM);
		}
	}
	else
	{
		//update the fog of war
		Level().FogOfWar().UpdateFog(Position(), CFogOfWar::ACTOR_FOG_REMOVE_RADIUS);
	};

	if (g_Alive()) 
	{
		//if(m_PhysicMovementControl->CharacterExist())
		//			m_PhysicMovementControl->InterpolatePosition(Position());
		//обновить информацию о предметах лежащих рядом с актером
		PickupModeUpdate();	

		float				k =	(mstate_real&mcCrouch)?0.75f:1.f;
		float				tm = isAccelerated(mstate_real)?(PI/(k*10.f)):(PI/(k*7.f));
		float				s_k	= ((mstate_real&mcCrouch) ? CROUCH_SOUND_FACTOR : 1.f);
		float				s_vol = s_k * (isAccelerated(mstate_real) ? 1.f : ACCELERATED_SOUND_FACTOR);
		SGameMtlPair		*mtl_pair = GMLib.GetMaterialPair(self_material_idx(),last_material_idx());
		CMaterialManager::update		(
			Device.fTimeDelta,
			s_vol,
			tm,
			!(mtl_pair && (mstate_real & mcAnyMove) && (!(mstate_real & (mcJump|mcFall))))
		);

		// landing sounds
		if (mtl_pair && !sndLanding.feedback && (mstate_real & (mcLanding | mcLanding2))){
			if (!mtl_pair->CollideSounds.empty()){
				Fvector	s_pos		=	Position	();
				s_pos.y				+=	.15f;
				::Sound->play_at_pos(mtl_pair->CollideSounds[0],this,s_pos);
			}
		}
	}
}



void CActor::shedule_Update	(u32 DT)
{
	setSVU(OnServer());

	if(m_vehicle)
	{
		inherited::shedule_Update		(DT);
		return;
	}
	if (!getEnabled())	return;
	if (!Ready())		return;

	cam_shift				= 0.f;
	cam_gray				= 0.f;

	// 
	clamp					(DT,0u,100u);
	float	dt	 	=  float(DT)/1000.f;

	// Check controls, create accel, prelimitary setup "mstate_real"
	float	Jump	= 0;
	if (Local())	
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
			g_sv_Orientate				(NET_Last.mstate,dt			);
			g_Orientate					(NET_Last.mstate,dt			);
			g_Physics					(NET_Last.p_accel,Jump,dt	);
			g_cl_ValidateMState			(dt,mstate_wishful);
			g_SetAnimation				(NET_Last.mstate);
		};
	}
	make_Interpolation();

	//------------------------------------------------	


	// generic stuff
	inherited::shedule_Update	(DT);

	//эффектор включаемый при ходьбе
	if (!pCamBobbing)
	{
		pCamBobbing = xr_new<CEffectorBobbing>	();
		EffectorManager().AddEffector			(pCamBobbing);
	}
	pCamBobbing->SetState						(mstate_real);


	
	//обновить положение камеры
	if (eacFirstEye == cam_active)
	{
		CWeapon *pWeapon = dynamic_cast<CWeapon*>(inventory().GetActiveSlot() != NO_ACTIVE_SLOT ? 
					inventory().m_slots[inventory().GetActiveSlot()].m_pIItem : NULL);
		
		cam_Update(dt,pWeapon?pWeapon->GetZoomFactor():DEFAULT_FOV);
	}
	else 
	{
		cam_Update(dt, DEFAULT_FOV);
	}

	//если в режиме HUD, то сама модель актера не рисуется
	setVisible				(!HUDview	());

	//установить режим показа HUD для текущего активного слота
	CHudItem* pHudItem = dynamic_cast<CHudItem*>(inventory().ActiveItem());	
	if(pHudItem && !pHudItem->getDestroy()) 
		pHudItem->SetHUDmode(HUDview());


	///////////////////////////////////////////////////////////////
	// для приблеженного режима приближения

	CWeapon* pWeapon = dynamic_cast<CWeapon*>(inventory().ActiveItem());	
	m_bZoomAimingMode = false;

	if(pWeapon)
	{
		if(pWeapon->IsZoomed())
		{
			float full_fire_disp = pWeapon->GetFireDispersion();

			CEffectorZoomInertion* S = dynamic_cast<CEffectorZoomInertion*>	(EffectorManager().GetEffector(eCEZoom));
			if(S) S->SetParams(full_fire_disp);

			//помнить, что если m_bZoomAimingMode = true
			//pWeapon->GetFireDispersion() вернет значение дисперсии без
			//учета положения стрелка, так как он спрашивает у нас GetWeaponAccuracy
			m_bZoomAimingMode = true;
		}

		if(eacFirstEye == cam_active)
		{
			float only_weapon_fire_disp = pWeapon->GetFireDispersion();
			HUD().SetCrosshairDisp(only_weapon_fire_disp);
			HUD().ShowCrosshair(true);
		}
	}
	else
	{
		if(eacFirstEye == cam_active)
		{
			HUD().SetCrosshairDisp(0.f);
			HUD().ShowCrosshair(false);
		}
	}
	
	//что актер видит перед собой
	Collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	
	if(RQ.O &&  RQ.range<inventory().GetTakeDist()) 
	{
		inventory().m_pTarget	= dynamic_cast<PIItem>(RQ.O);
		m_pPersonWeLookingAt	= dynamic_cast<CInventoryOwner*>(RQ.O);
		m_pVehicleWeLookingAt	= dynamic_cast<CVehicleCustom*>(RQ.O);
	}
	else 
	{
		inventory().m_pTarget	= NULL;
		m_pPersonWeLookingAt	= NULL;
	}


	//обновление инвентаря и торговли
	UpdateInventoryOwner(DT);

	//для режима сна
	UpdateSleep();
}

BOOL CActor::g_State (SEntityState& state) const
{
	state.bJump			= !!(mstate_real&mcJump);
	state.bCrouch		= !!(mstate_real&mcCrouch);
	state.bFall			= !!(mstate_real&mcFall);
	state.fVelocity		= m_PhysicMovementControl->GetVelocityActual();
	return TRUE;
}

#define VEL_MAX 10.f

//возвращает текуший разброс стрельбы (в радианах)с учетом движения
float CActor::GetWeaponAccuracy() const
{
	if(m_bZoomAimingMode)
		return 0.f;

	float dispersion = m_fDispBase;

	CEntity::SEntityState state;
	if (g_State(state))
	{
		
		
		if(isAccelerated(mstate_real))
			dispersion *= (1.f + (state.fVelocity/VEL_MAX)*
							m_fDispVelFactor*(1.f + m_fDispAccelFactor));
		else
			dispersion *= (1.f + (state.fVelocity/VEL_MAX)*m_fDispVelFactor);
		
		if (state.bCrouch)	
			dispersion *= (1.f + m_fDispCrouchFactor);
	}

	return dispersion;
}


void CActor::renderable_Render	()
{
	inherited::renderable_Render			();

	if (!m_vehicle)
		CInventoryOwner::renderable_Render	();
}

BOOL CActor::renderable_ShadowGenerate	() 
{
	if(m_vehicle)
		return FALSE;
	
	return inherited::renderable_ShadowGenerate();
}


void CActor::g_cl_ValidateMState(float dt, u32 mstate_wf)
{
	// изменилось состояние
	if (mstate_wf != mstate_real)
	{
		// могу ли я встать
		if ((mstate_real&mcCrouch)&&(0==(mstate_wf&mcCrouch)))
		{
			// can we change size to "bbStandBox"
			Fvector				start_pos;
			//bbStandBox.getcenter(start_pos);
			start_pos.add		(Position());
			//if (!g_pGameLevel->ObjectSpace.EllipsoidCollide(CFORM(),XFORM(),start_pos,bbStandBox))
			Fbox stand_box=m_PhysicMovementControl->Boxes()[0];
			//stand_box.y1+=m_PhysicMovementControl->FootExtent().y;
			stand_box.y1+=m_PhysicMovementControl->FootRadius();
			m_PhysicMovementControl->GetPosition(start_pos);
			start_pos.y+=(
				//-(m_PhysicMovementControl->Box().y2-m_PhysicMovementControl->Box().y1)+
				(m_PhysicMovementControl->Boxes()[0].y2-m_PhysicMovementControl->Boxes()[0].y1)
				)/2.f;
			//start_pos.y+=m_PhysicMovementControl->FootExtent().y/2.f;
			start_pos.y+=m_PhysicMovementControl->FootRadius();
			if (!g_pGameLevel->ObjectSpace.EllipsoidCollide(CFORM(),XFORM(),start_pos,stand_box))
			{
				mstate_real &= ~mcCrouch;
				m_PhysicMovementControl->ActivateBox	(0);
			}
		}
	}
	// закончить приземление
	if (mstate_real&(mcLanding|mcLanding2)){
		m_fLandingTime		-= dt;
		if (m_fLandingTime<=0.f){
			mstate_real		&=~	(mcLanding|mcLanding2);
			mstate_real		&=~	(mcFall|mcJump);
		}
	}
	// закончить падение
	if (m_PhysicMovementControl->gcontact_Was){
		if (mstate_real&mcFall){
			if (m_PhysicMovementControl->GetContactSpeed()>4.f){
				if (fis_zero(m_PhysicMovementControl->gcontact_HealthLost)){	
					m_fLandingTime	= s_fLandingTime1;
					mstate_real		|= mcLanding;
				}else{
					m_fLandingTime	= s_fLandingTime2;
					mstate_real		|= mcLanding2;
				}
			}
		}
		m_bJumpKeyPressed	=	TRUE;
		m_fJumpTime			=	s_fJumpTime;
		mstate_real			&=~	(mcFall|mcJump);
	}
	if ((mstate_wf&mcJump)==0)	
		m_bJumpKeyPressed	=	FALSE;

	// Зажало-ли меня/уперся - не двигаюсь
	if (((m_PhysicMovementControl->GetVelocityActual()<0.2f)&&(!(mstate_real&(mcFall|mcJump)))) || m_PhysicMovementControl->bSleep) 
	{
		mstate_real				&=~ mcAnyMove;
	}
	if (m_PhysicMovementControl->Environment()==CPHMovementControl::peOnGround || m_PhysicMovementControl->Environment()==CPHMovementControl::peAtWall)
	{
		// если на земле гарантированно снимать флажок Jump
		if (((s_fJumpTime-m_fJumpTime)>s_fJumpGroundTime)&&(mstate_real&mcJump))
		{
			mstate_real			&=~	mcJump;
			m_fJumpTime			= s_fJumpTime;
		}
	}
	if(m_PhysicMovementControl->Environment()==CPHMovementControl::peAtWall)

		mstate_real				|=mcClimb;

	else
		mstate_real				&=~mcClimb;

	if(m_PhysicMovementControl->PHCapture()&&isAccelerated(mstate_real))
		
		mstate_real				^=mcAccel;
}

void CActor::g_cl_CheckControls(u32 mstate_wf, Fvector &vControlAccel, float &Jump, float dt)
{
	// ****************************** Check keyboard input and control acceleration
	vControlAccel.set	(0,0,0);

	if (!(mstate_real&mcFall) && (m_PhysicMovementControl->Environment()==CPHMovementControl::peInAir)) 
	{
		m_fFallTime				-=	dt;
		if (m_fFallTime<=0.f){
			m_fFallTime			=	s_fFallTime;
			mstate_real			|=	mcFall;
			mstate_real			&=~	mcJump;
		}
	}

	if (m_PhysicMovementControl->Environment()==CPHMovementControl::peOnGround || m_PhysicMovementControl->Environment()==CPHMovementControl::peAtWall )
	{
		// jump
		m_fJumpTime				-=	dt;

		if (((mstate_real&mcJump)==0) && (mstate_wf&mcJump) && (m_fJumpTime<=0.f) && !m_bJumpKeyPressed)
		{
			mstate_real			|=	mcJump;
			m_bJumpKeyPressed	=	TRUE;
			Jump				= m_fJumpSpeed;
			m_fJumpTime			= s_fJumpTime;

			
			//уменьшить силу игрока из-за выполненого прыжка
			if (!psActorFlags.test(AF_GODMODE))	
					ConditionJump((inventory().TotalWeight()+GetMass())/
								  (inventory().GetMaxWeight() + GetMass()));
		}

		/*
		if(m_bJumpKeyPressed)
		Jump				= m_fJumpSpeed;
		*/
		// crouch
		if ((0==(mstate_real&mcCrouch))&&(mstate_wf&mcCrouch))
		{
			mstate_real			|=	mcCrouch;
			m_PhysicMovementControl->EnableCharacter();
			m_PhysicMovementControl->ActivateBox(1);
			//m_PhysicMovementControl->ActivateBox(1);
		}

		// mask input into "real" state
		u32 move	= mcAnyMove|mcAccel;
		mstate_real &= (~move);
		mstate_real |= (mstate_wf & move);

		// check player move state
		if (mstate_real&mcAnyMove)
		{
			BOOL	bAccelerated		= isAccelerated(mstate_real)&&!m_PhysicMovementControl->PHCapture();

			// update player accel
			if (mstate_real&mcFwd)		vControlAccel.z +=  1;
			if (mstate_real&mcBack)		vControlAccel.z += -1;
			if (mstate_real&mcLStrafe)	vControlAccel.x += -1;
			if (mstate_real&mcRStrafe)	vControlAccel.x +=  1;

			// correct "mstate_real" if opposite keys pressed
			if (_abs(vControlAccel.z)<EPS)	mstate_real &= ~(mcFwd+mcBack		);
			if (_abs(vControlAccel.x)<EPS)	mstate_real &= ~(mcLStrafe+mcRStrafe);

			// normalize and analyze crouch and run
			float	scale				= vControlAccel.magnitude();
			if (scale>EPS)	{
				scale	=	m_fWalkAccel/scale;
				if (bAccelerated)			scale *= m_fRunFactor;
				if (mstate_real&mcCrouch)	scale *= m_fCrouchFactor;
				vControlAccel.mul			(scale);
			}else{
				//				mstate_real	&= ~mcAnyMove;
			}
		}
	}else{
		//		mstate_real			&=~ mcAnyMove;
	}
	
	// transform local dir to world dir
	Fmatrix				mOrient;
	mOrient.rotateY		(-r_model_yaw);
	mOrient.transform_dir(vControlAccel);
}

void CActor::g_Orientate	(u32 mstate_rl, float dt)
{
	// visual effect of "fwd+strafe" like motion
	float calc_yaw = 0;
	switch(mstate_rl&mcAnyMove)
	{
	case mcFwd+mcLStrafe:
	case mcBack+mcRStrafe:
		calc_yaw = +PI_DIV_4; 
		break;
	case mcFwd+mcRStrafe:
	case mcBack+mcLStrafe: 
		calc_yaw = -PI_DIV_4; 
		break;
	case mcLStrafe:
		calc_yaw = +PI_DIV_3-EPS_L; 
		break;
	case mcRStrafe:
		calc_yaw = -PI_DIV_2+EPS_L; 
		break;
	}

	// lerp angle for "effect" and capture torso data from camera
	angle_lerp		(r_model_yaw_delta,calc_yaw,PI_MUL_4,dt);

	// build matrix
	Fmatrix mXFORM;
	mXFORM.rotateY	(-(r_model_yaw + r_model_yaw_delta));
	mXFORM.c.set	(Position());
	XFORM().set		(mXFORM);
}

// ****************************** Update actor orientation according to camera orientation
void CActor::g_cl_Orientate	(u32 mstate_rl, float dt)
{
	// capture camera into torso (only for FirstEye & LookAt cameras)
	if (eacFreeLook!=cam_active){

		r_torso.yaw		=	cam_Active()->GetWorldYaw	();
		r_torso.pitch	=	cam_Active()->GetWorldPitch	();

		unaffected_r_torso_yaw	 = r_torso.yaw;
		unaffected_r_torso_pitch = r_torso.pitch;
	}

	CWeapon *pWeapon = dynamic_cast<CWeapon*>(inventory().GetActiveSlot() != NO_ACTIVE_SLOT ? 
		inventory().m_slots[inventory().GetActiveSlot()].m_pIItem : NULL);

	if(pWeapon) 
	{
		Fvector dangle;
		dangle = pWeapon->GetRecoilDeltaAngle();
		r_torso.yaw		=	unaffected_r_torso_yaw + dangle.y;
		r_torso.pitch	=	unaffected_r_torso_pitch + dangle.x;
	}


	// если есть движение - выровнять модель по камере
	if (mstate_rl&mcAnyMove)	{
		r_model_yaw		= angle_normalize(r_torso.yaw);
		mstate_real		&=~mcTurn;
	} else {
		// if camera rotated more than 45 degrees - align model with it
		float ty = angle_normalize(r_torso.yaw);
		if (_abs(r_model_yaw-ty)>PI_DIV_4)	{
			r_model_yaw_dest = ty;
			// 
			mstate_real	|= mcTurn;
		}
		if (_abs(r_model_yaw-r_model_yaw_dest)<EPS_L){
			mstate_real	&=~mcTurn;
		}
		if (mstate_rl&mcTurn){
			angle_lerp	(r_model_yaw,r_model_yaw_dest,PI_MUL_2,dt);
		}
	}
}

void CActor::g_sv_Orientate(u32 /**mstate_rl/**/, float /**dt/**/)
{
	// rotation
	r_model_yaw		= NET_Last.o_model;
	r_torso.yaw		= NET_Last.o_torso.yaw;
	r_torso.pitch	= NET_Last.o_torso.pitch;

	CWeapon *pWeapon = dynamic_cast<CWeapon*>(inventory().GetActiveSlot() != NO_ACTIVE_SLOT ? 
		inventory().m_slots[inventory().GetActiveSlot()].m_pIItem : NULL);

	if(pWeapon) 
	{
		Fvector dangle;
		dangle = pWeapon->GetRecoilDeltaAngle();
		r_torso.yaw		=	unaffected_r_torso_yaw + dangle.y;
		r_torso.pitch	=	unaffected_r_torso_pitch + dangle.x;
	}

}

void CActor::g_fireParams	(Fvector &fire_pos, Fvector &fire_dir)
{
	fire_pos = EffectorManager().vPosition;
	fire_dir = EffectorManager().vDirection;


/*
	if(Level().game.type == GAME_SINGLE)
	{
		if (HUDview()) 
		{
			fire_pos = Device.vCameraPosition;
			fire_dir = Device.vCameraDirection;
		}
	}
	else
	{
		Fvector		fire_normal;
		if (cam_Active()->style != eacFreeLook)
			cam_Active()->Get(fire_pos, fire_dir, fire_normal);
		else
		{
			fire_pos = Device.vCameraPosition;
			fire_dir = Device.vCameraDirection;
		}
	}
*/
}


void CActor::g_PerformDrop	( )
{
	VERIFY					(b_DropActivated);
	b_DropActivated			= FALSE;

	if (m_pArtifact)
	{
		NET_Packet				P;
		u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16					(u16(m_pArtifact->ID()));
		u_EventSend				(P);

		m_pArtifact				= 0;
	} else {
		//
		
		PIItem pItem = inventory().ActiveItem();
		if (0==pItem) return;
		pItem->Drop();

		/*NET_Packet				P;
		u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16					(u16(O->ID()));
		u_EventSend				(P);*/
	}
}

void CActor::g_WeaponBones	(int &L, int &R1, int &R2)
{
	R1				= m_r_hand;
	R2				= m_r_finger2;
	L				= m_l_finger1;
}

// HUD
void CActor::OnHUDDraw	(CCustomHUD* /**hud/**/)
{
	//CWeapon* W			= Weapons->ActiveWeapon();
	//if (W)				W->renderable_Render		();
	//CWeapon *W = dynamic_cast<CWeapon*>(inventory().ActiveItem()); if(W) W->renderable_Render();

	if(inventory().ActiveItem()&&!m_vehicle) {
		inventory().ActiveItem()->renderable_Render();
	}


#ifndef NDEBUG
	string128 buf;
	HUD().pFontSmall->SetColor	(0xffffffff);
	HUD().pFontSmall->OutSet	(170,530);
	HUD().pFontSmall->OutNext	("Position:      [%3.2f, %3.2f, %3.2f]",VPUSH(Position()));
	HUD().pFontSmall->OutNext	("Velocity:      [%3.2f, %3.2f, %3.2f]",VPUSH(m_PhysicMovementControl->GetVelocity()));
	HUD().pFontSmall->OutNext	("Vel Magnitude: [%3.2f]",m_PhysicMovementControl->GetVelocityMagnitude());
	HUD().pFontSmall->OutNext	("Vel Actual:    [%3.2f]",m_PhysicMovementControl->GetVelocityActual());
	switch (m_PhysicMovementControl->Environment())
	{
	case CPHMovementControl::peOnGround:	strcpy(buf,"ground");			break;
	case CPHMovementControl::peInAir:		strcpy(buf,"air");				break;
	case CPHMovementControl::peAtWall:		strcpy(buf,"wall");				break;
	}
	HUD().pFontSmall->OutNext	(buf);

	if (IReceived != 0)
	{
		float Size = 0;
		Size = HUD().pFontSmall->GetSize();
		HUD().pFontSmall->SetSize(Size*2);
		HUD().pFontSmall->SetColor	(0xffff0000);
		HUD().pFontSmall->OutNext ("Input :		[%3.2f]", ICoincidenced/IReceived * 100.0f);
		HUD().pFontSmall->SetSize(Size);
	};
#endif
}

float CActor::HitScale	(int element)
{
	CKinematics* V		= PKinematics(Visual());			VERIFY(V);
	float scale			= fis_zero(V->LL_GetBoneInstance(u16(element)).get_param(0))?1.f:V->LL_GetBoneInstance(u16(element)).get_param(0);
	return hit_factor*scale;
}

void CActor::SetPhPosition(const Fmatrix &pos)
{
	if(!m_pPhysicsShell) m_PhysicMovementControl->SetPosition(pos.c);
	//else m_phSkeleton->S
}

void CActor::ForceTransform(const Fmatrix& m)
{
	if(g_Alive()<=0)			return;
	XFORM().set					(m);
	if(m_PhysicMovementControl->CharacterExist()) m_PhysicMovementControl->EnableCharacter	();
	m_PhysicMovementControl->SetPosition		(m.c);
}

#ifdef DEBUG
void dbg_draw_frustum (float FOV, float _FAR, float A, Fvector &P, Fvector &D, Fvector &U);
void CActor::OnRender	()
{
	if (Local())// && Level().CurrentEntity() != this)
	{
	};
	if (Remote())
	{
		if (NET_A.size())
		{
/*			
			float size = 0.3f;
			Fvector Pos;

			Pos = IEndPos;
			Pos.y += size;
			RCache.dbg_DrawAABB	(Pos, size, size, size, 0xff0000ff);

			Pos = NET_A.back().State.position;
			Pos.y += size;
			RCache.dbg_DrawAABB	(Pos, size, size, size, 0xffff0000);

			Pos = Position();
			Pos.y += size;
			RCache.dbg_DrawAABB	(Pos, size, size, size, 0x80ffffff);
*/
		};

		////////////////////////////////////
		CPHSynchronize* pSyncObj = NULL;
		pSyncObj = PHGetSyncItem(0);
		if (!pSyncObj) return;
		////////////////////////////////////
		SPHNetState		CurState;
		pSyncObj->get_State(CurState);

		HUD().pFontSmall->OutSet	(170,630);
		HUD().pFontSmall->SetSize	(16.0f);
		HUD().pFontSmall->SetColor	(0xffff0000);
		HUD().pFontSmall->OutNext	("NumUpdates:      [%.2f]",g_fNumUpdates);
		if (!Level().InterpolationDisabled())
			HUD().pFontSmall->OutNext	("Intepolation:      [%d]",Level().GetInterpolationSteps());
		else
			HUD().pFontSmall->OutNext	("Intepolation:      [Disabled]");
		

//		HUD().pFontSmall->OutNext	("Accel:    [%3.2f, %3.2f, %3.2f]",VPUSH(CurState.accel));

		HUD().pFontSmall->SetSize	(8.0f);
	};

	if (!bDebug)				return;

	m_PhysicMovementControl->dbg_Draw	();
	//if(g_Alive()>0)
	m_PhysicMovementControl->dbg_Draw	();
	//CCameraBase* C				= cameras	[cam_active];
	//dbg_draw_frustum			(C->f_fov, 230.f, 1.f, C->Position(), C->vDirection, C->vNormal);
}
#endif

ENGINE_API extern float		psHUD_FOV;
float CActor::Radius()const
{ 
	float R		= inherited::Radius();
	CWeapon* W	= dynamic_cast<CWeapon*>(inventory().ActiveItem());
	if (W) R	+= W->Radius();
	//	if (HUDview()) R *= 1.f/psHUD_FOV;
	return R;
}

CWound* CActor::ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element)
{
	if (psActorFlags.test(AF_GODMODE)) return NULL;

	return CActorCondition::ConditionHit(who, hit_power, hit_type, element);
}

void CActor::UpdateCondition()
{
	if (psActorFlags.test(AF_GODMODE)) return;

	if(isAccelerated(mstate_real) && (mstate_real&mcAnyMove))
	{
	   ConditionAccel((inventory().TotalWeight()+GetMass())/
						(inventory().GetMaxWeight()+GetMass()));
	}
	
	CActorCondition::UpdateCondition();
}

//information receive
void CActor::OnReceiveInfo(int info_index)
{
	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TalkMenu.IsShown())
	{
		if(pGameSP->TalkMenu.IsShown())
		{
			pGameSP->TalkMenu.UpdateQuestions();
		}
		//если пришло сообщение по PDA когда сам PDA не был запущен
		else if(!pGameSP->m_pUserMenu->IsShown())
		{
		}
	}

    CInventoryOwner::OnReceiveInfo(info_index);
}

void CActor::ReceivePdaMessage(u16 who, EPdaMsg msg, int info_index)
{
	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;
	//визуализация в интерфейсе
	
	CObject* pPdaObject =  Level().Objects.net_Find(who);
	R_ASSERT(pPdaObject);
	CPda* pPda = dynamic_cast<CPda*>(pPdaObject);
	R_ASSERT(pPda);
	HUD().GetUI()->UIMainIngameWnd.ReceivePdaMessage(pPda->GetOriginalOwner(), msg, info_index);

    CInventoryOwner::ReceivePdaMessage(who, msg, info_index);
}

bool		CActor::use_bolts				() const
{
	if (Game().type != GAME_SINGLE) return false;
	return CInventoryOwner::use_bolts();
};


bool  CActor::NeedToDestroyObject() const
{
	if(Level().game.type == GAME_SINGLE)
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
		return Level().GetGameTime() - m_dwDeathTime;
	else
		return 0;
}


void CActor::GoSleep(u32 sleep_time)
{
	CActorCondition::GoSleep();

	//поставить будильник
	m_dwWakeUpTime = Level().GetGameTime() + sleep_time;

	m_fOldTimeFactor = Level().GetGameTimeFactor();
	Level().SetGameTimeFactor(m_fSleepTimeFactor*m_fOldTimeFactor);


	if(m_pSleepEffectorPP)
	{
		m_pSleepEffectorPP->m_bSleeping = false;
		m_pSleepEffectorPP = NULL;
	}
	m_pSleepEffectorPP = xr_new<CSleepEffectorPP>(m_pSleepEffector->ppi, m_pSleepEffector->time,		m_pSleepEffector->time_attack, m_pSleepEffector->time_release);
	//m_pSleepEffectorPP->m_bSleeping = true;
		
	Level().Cameras.AddEffector(m_pSleepEffectorPP);
}
void CActor::Awoke()
{
	if(!IsSleeping()) return;

	Level().SetGameTimeFactor(m_fOldTimeFactor);

	if(m_pSleepEffectorPP)
	{
		//m_pSleepEffectorPP->m_bSleeping = false;
		m_pSleepEffectorPP = NULL;
	}

	CActorCondition::Awoke();
}

void CActor::UpdateSleep()
{
	if(!IsSleeping()) return;
	
	if(m_pSleepEffectorPP)
	{
//		if(m_pSleepEffectorPP->m_bFallAsleep = true;
	}

	//разбудить актера, если он проспал столько сколько задумал
	if(Level().GetGameTime()>m_dwWakeUpTime)
		Awoke();
}

#include "WeaponMagazined.h"

void CActor::OnItemTake			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemTake(inventory_item);
	if (OnClient()) return;
	
	switch (Game().type)
	{
	case GAME_DEATHMATCH:
	case GAME_TEAMDEATHMATCH:
		{
			SpawnAmmoForWeapon(inventory_item);
		}break;
	};	
}

void CActor::OnItemDrop			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemDrop(inventory_item);
	if (OnClient()) return;

	switch (Game().type)
	{
	case GAME_DEATHMATCH:
	case GAME_TEAMDEATHMATCH:
		{
			RemoveAmmoForWeapon(inventory_item);
		}break;
	};
}

void	CActor::SpawnAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (!pIItem) return;

	CWeaponMagazined* pWM = dynamic_cast<CWeaponMagazined*> (pIItem);
	if (pWM)
	{
		bool UsableAmmoExist = false;
		for (u32 I = 0; I<pWM->m_ammoTypes.size(); I++)
		{
			CWeaponAmmo* pAmmo = dynamic_cast<CWeaponAmmo*>(inventory().Get(*(pWM->m_ammoTypes[I]), false));
			if (!pAmmo) continue;
					
			UsableAmmoExist = true;
			break;
		};
		if (!UsableAmmoExist) pWM->SpawnAmmo(0xffffffff, NULL, ID());
	};
};

void	CActor::RemoveAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (!pIItem) return;

	CWeaponMagazined* pWM = dynamic_cast<CWeaponMagazined*> (pIItem);
	if (pWM)
	{
		NET_Packet			P;
		bool UsableAmmoExist = false;

		for (u32 I = 0; I<pWM->m_ammoTypes.size(); I++)
		{
			CWeaponAmmo* pAmmo = dynamic_cast<CWeaponAmmo*>(inventory().Get(*(pWM->m_ammoTypes[I]), false));
			if (!pAmmo) break;

			UsableAmmoExist = true;
			
			u_EventGen			(P,GE_DESTROY,pAmmo->ID());
			Msg					("ge_destroy: [%d] - %s",pAmmo->ID(),*(pAmmo->cName()));
		};
		if (UsableAmmoExist) u_EventSend			(P);
	};
};
