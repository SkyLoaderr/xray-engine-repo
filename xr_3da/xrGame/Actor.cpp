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
//#include "EffectorPPHit.h"
//#include "EffectorHit.h"
#include "ShootingHitEffector.h"
#include "ActorEffector.h"
#include "EffectorZoomInertion.h"



// breakpoints
#include "../xr_input.h"

//
#include "Actor.h"
#include "ActorAnimation.h"

#include "HudItem.h"
#include "WeaponMagazined.h"

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
	
	// ���������
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
	m_fClimbFactor			= 1.f;
	m_fCamHeightFactor		= 0.87f;

	m_fFallTime				=	s_fFallTime;
	m_bAnimTorsoPlayed		=	false;

//	self_gmtl_id			=	GAMEMTL_NONE;
//	last_gmtl_id			=	GAMEMTL_NONE;
	m_pPhysicsShell			=	NULL;
	bDeathInit				=	false;
	m_saved_dir.set(0,0,0);
	m_saved_impulse=0.f;
	m_holder				=	NULL;
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::actor);
#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW);
#endif

	InitTrade();

	//��������� ������������� ����� � inventory
	inventory().SetBeltUseful(true);

	m_pPersonWeLookingAt	= NULL;
	m_pVehicleWeLookingAt	= NULL;
	m_pObjectWeLookingAt	= NULL;
	m_pWaitingTradePartner	= NULL;
	m_pWaitingTradePartnerInvOwner = NULL;
	m_bPickupMode			= false;

	////////////////////////////////////
	pStatGraph = NULL;
	dDesyncVec.set(0, 0, 0);

	m_pActorEffector = NULL;

	m_bZoomAimingMode = false;

	m_eDefaultObjAction = eaaNoAction;
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
	Fvector	vBOX1_center= pSettings->r_fvector3	(section,"ph_box1_center"	);
	Fvector	vBOX1_size	= pSettings->r_fvector3	(section,"ph_box1_size"		);
	bb.set	(vBOX1_center,vBOX1_center); bb.grow(vBOX1_size);
	m_PhysicMovementControl->SetBox		(1,bb);

	// m_PhysicMovementControl: BOX
	Fvector	vBOX0_center= pSettings->r_fvector3	(section,"ph_box0_center"	);
	Fvector	vBOX0_size	= pSettings->r_fvector3	(section,"ph_box0_size"		);
	bb.set	(vBOX0_center,vBOX0_center); bb.grow(vBOX0_size);
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
	m_fRunBackFactor			= pSettings->r_float(section,"run_back_coef");
	m_fWalkBackFactor			= pSettings->r_float(section,"walk_back_coef");
	m_fCrouchFactor				= pSettings->r_float(section,"crouch_coef");
	m_fClimbFactor				= pSettings->r_float(section,"climb_coef");
	skel_airr_lin_factor		= pSettings->r_float(section,"ph_skeleton_airr_lin_factor");
	skel_airr_ang_factor		= pSettings->r_float(section,"ph_skeleton_airr_ang_factor");
	hinge_force_factor1 		= pSettings->r_float(section,"ph_skeleton_hinger_factor1");
	skel_ddelay					= pSettings->r_s32	(section,"ph_skeleton_ddelay");
	skel_fatal_impulse_factor	= pSettings->r_float(section,"ph_skel_fatal_impulse_factor");
	m_fCamHeightFactor			= pSettings->r_float(section,"camera_height_factor");
	m_PhysicMovementControl		->SetJumpUpVelocity(m_fJumpSpeed);
	float AirControlParam		= pSettings->r_float	(section,"air_control_param"	);
	m_PhysicMovementControl		->SetAirControlParam(AirControlParam);

	m_fPickupInfoRadius	= pSettings->r_float(section,"pickup_info_radius");
	m_fSleepTimeFactor	= pSettings->r_float(section,"sleep_time_factor");

	//actor condition variables
	CActorCondition::Load(section);

	//��������� ��������� ���������
	LoadShootingEffector	("shooting_effector");
	LoadSleepEffector		("sleep_effector");

	//��������� ��������� �������� firepoint
	m_vMissileOffset	= pSettings->r_fvector3(section,"missile_throw_offset");	

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

	cam_Set					(eacFirstEye);

	// motions
	m_current_legs_blend= 0;
	m_current_jump_blend= 0;
	m_current_legs		= 0;
	m_current_torso		= 0;
	m_current_head		= 0;
	// sheduler
	shedule.t_min		= shedule.t_max = 1;

	// ��������� ��������� ��������
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
		m_saved_hit_type=hit_type;
	}
	else if(m_pPhysicsShell) 
		m_pPhysicsShell->applyHit(position_in_bone_space,dir,impulse,element,hit_type);
	//m_phSkeleton->applyImpulseTrace(position_in_bone_space,dir,impulse);
	else
	{
		m_saved_dir.set(dir);
		m_saved_impulse=impulse*skel_fatal_impulse_factor;
		m_saved_element=element;
		m_saved_position.set(position_in_bone_space);
	}


	if (!g_Alive())
	{
		inherited::Hit(iLost,dir,who,element,position_in_bone_space,impulse,hit_type);
		return;
	}

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
				inherited::Hit(0.f,dir,who,element,position_in_bone_space, 0.f, hit_type);
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
			Fvector a	= {0,0,1};
			Fvector b	= {vLocalDir.x,0,vLocalDir.z};
			float mb	= b.magnitude();		
			if (!fis_zero(mb)){
				b.mul	(1.f/mb);
				bool FB	= _abs(a.dotproduct(b))>0.7071f;
//				float x	= _abs(vLocalDir.x);
//				float z	= _abs(vLocalDir.z);
				if (FB)	id = (vLocalDir.z<0)?2:0;
				else	id = (vLocalDir.x<0)?3:1;
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
		CSkeletonAnimated *tpKinematics = PSkeletonAnimated(Visual());
		VERIFY(tpKinematics);
#pragma todo("Dima to Dima : forward-back bone impulse direction has been determined incorrectly!")
		CMotionDef *tpMotionDef = m_anims.m_normal.m_damage[iFloor(tpKinematics->LL_GetBoneInstance(element).get_param(1) + (angle_difference(r_model_yaw + r_model_yaw_delta,yaw) <= PI_DIV_2 ? 0 : 1))];
		float power_factor = perc/100.f; clamp(power_factor,0.f,1.f);
		VERIFY(tpMotionDef);
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
	{
		if ((I - B) == (int)inventory().GetActiveSlot()) 
			(*I).m_pIItem->Drop();
		else
			if((*I).m_pIItem) inventory().Ruck((*I).m_pIItem);
	};
	///!!! ����������� ���������
	/*
	TIItemList &l_list = inventory().m_ruck;
	for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
	{
		CArtifact* pArtifact = dynamic_cast<CArtifact*>(*l_it);
		if(pArtifact)
			pArtifact->Drop();
	}
	*/

	///!!! ������ �����
	TIItemList &l_blist = inventory().m_belt;
	while (!l_blist.empty())	inventory().Ruck(l_blist.front());

	if (OnServer() && Game().type != GAME_SINGLE)
	{
		//if we are on server and actor has PDA - destroy PDA
		TIItemList &l_rlist = inventory().m_ruck;
		for(PPIItem l_it = l_rlist.begin(); l_rlist.end() != l_it; ++l_it)
		{
			if (Game().type == GAME_ARTEFACTHUNT)
			{
				if ((*l_it)->SUB_CLS_ID == CLSID_ARTEFACT)
				{
					(*l_it)->Drop();
					continue;
				};
			};

			
			//���� � ��� ������ ���������� �����, ������� ��� ������� �� ���������
//			if ((*l_it)->SUB_CLS_ID == CLSID_DEVICE_PDA)
			{
				NET_Packet P;
				u_EventGen(P,GE_DESTROY,(*l_it)->ID());
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
	create_Skeleton();
}

void CActor::g_Physics			(Fvector& _accel, float jump, float dt)
{
	if (!g_Alive())	{

		if(m_pPhysicsShell)
			if(m_pPhysicsShell->bActive && !fsimilar(0.f,m_saved_impulse))
			{
				m_pPhysicsShell->applyHit(m_saved_position,m_saved_dir,m_saved_impulse*1.5f,m_saved_element,m_saved_hit_type);
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
	m_PhysicMovementControl->bSleep				=false;
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
		//�������� ���������� � ��������� ������� ����� � �������
		PickupModeUpdate	();	

		float				k =	(mstate_real&mcCrouch)?0.75f:1.f;
		float				tm = isAccelerated(mstate_real)?(PI/(k*10.f)):(PI/(k*7.f));
		float				s_k	= ((mstate_real&mcCrouch) ? CROUCH_SOUND_FACTOR : 1.f);
		float				s_vol = s_k * (isAccelerated(mstate_real) ? 1.f : ACCELERATED_SOUND_FACTOR);
		SGameMtlPair		*mtl_pair = GMLib.GetMaterialPair(self_material_idx(),last_material_idx());
		
		CMaterialManager::set_run_mode(isAccelerated(mstate_real));
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
	//-------------------------------------------------------------------
//*
	CWeapon* pWeapon = dynamic_cast<CWeapon*>(inventory().ActiveItem());	
	m_bZoomAimingMode = false;

	//�������� ��������� ������ � FOV 
	float dt = float(Device.dwTimeDelta)/1000.0f;

	if (eacFirstEye == cam_active && pWeapon &&
		pWeapon->IsZoomed() && (!pWeapon->ZoomTexture() ||
		(!pWeapon->IsRotatingToZoom() && pWeapon->ZoomTexture())))
		cam_Update(dt, pWeapon->GetZoomFactor());
	
	else 
		cam_Update(dt, DEFAULT_FOV);

	if(pWeapon)
	{
		if(pWeapon->IsZoomed())
		{
			float full_fire_disp = pWeapon->GetFireDispersion();

			CEffectorZoomInertion* S = dynamic_cast<CEffectorZoomInertion*>	(EffectorManager().GetEffector(eCEZoom));
			if(S) S->SetParams(full_fire_disp);

			//�������, ��� ���� m_bZoomAimingMode = true
			//pWeapon->GetFireDispersion() ������ �������� ��������� ���
			//����� ��������� �������, ��� ��� �� ���������� � ��� GetWeaponAccuracy
			m_bZoomAimingMode = true;
		}

		//if(eacFirstEye == cam_active)
		if(this == dynamic_cast<CActor*>(Level().CurrentEntity()))
		{
			float only_weapon_fire_disp = pWeapon->GetFireDispersion();
			HUD().SetCrosshairDisp(only_weapon_fire_disp);
			HUD().ShowCrosshair(true);
		}
	}
	else
	{
		if(this == dynamic_cast<CActor*>(Level().CurrentEntity()))
		{
			HUD().SetCrosshairDisp(0.f);
			HUD().ShowCrosshair(false);
		}
	}
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

	cam_shift				= 0.f;
	cam_gray				= 0.f;

	// 
	clamp					(DT,0u,100u);
	float	dt	 			=  float(DT)/1000.f;

	// Check controls, create accel, prelimitary setup "mstate_real"
	float	Jump	= 0;
	//----------- for E3 -----------------------------
	if (Local() && (OnClient() || Level().CurrentEntity()==this))
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
			mstate_real = NET_Last.mstate;
			g_Orientate					(NET_Last.mstate,dt			);
			g_Physics					(NET_Last.p_accel,Jump,dt	);
			g_cl_ValidateMState			(dt,mstate_wishful);
			g_SetAnimation				(NET_Last.mstate);

			if (NET_Last.mstate & mcCrouch)
				m_PhysicMovementControl->ActivateBox(1, true);
			else 
				m_PhysicMovementControl->ActivateBox(0, true);
		};
	}
	make_Interpolation();

	//------------------------------------------------	


	// generic stuff
	inherited::shedule_Update	(DT);


/*	if(WaitingTradePartner() &&
		!WaitingTradePartnerInvOwner()->IsTalkEnabled())
	{
		m_pWaitingTradePartner = NULL;
		m_pWaitingTradePartnerInvOwner = NULL;
	}*/
		



	//�������� ���������� ��� ������
	if (!pCamBobbing)
	{
		pCamBobbing = xr_new<CEffectorBobbing>	();
		EffectorManager().AddEffector			(pCamBobbing);
	}
	pCamBobbing->SetState						(mstate_real);


	//���� � ������ HUD, �� ���� ������ ������ �� ��������
	setVisible				(!HUDview	());

	//���������� ����� ������ HUD ��� �������� ��������� �����
	CHudItem* pHudItem = dynamic_cast<CHudItem*>(inventory().ActiveItem());	
	if(pHudItem && !pHudItem->getDestroy()) 
		pHudItem->SetHUDmode(HUDview());



	//��� ����� ����� ����� �����
	Collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	
	m_pObjectWeLookingAt = dynamic_cast<CGameObject*>(RQ.O);

	if(RQ.O &&  RQ.range<inventory().GetTakeDist()) 
	{
		inventory().m_pTarget			= dynamic_cast<PIItem>(RQ.O);
		m_pPersonWeLookingAt			= dynamic_cast<CInventoryOwner*>(RQ.O);
		m_pVehicleWeLookingAt			= dynamic_cast<CHolderCustom*>(RQ.O);
		CEntityAlive* pEntityAlive		= dynamic_cast<CEntityAlive*>(RQ.O);
		
		if (Game().type == GAME_SINGLE)
		{
			// ����������� ����� ������ �� �����, � ��������� ��������������
			// �������� �� ���������, ������� ����� ���������� ����������� 
			// ���������
			if (m_pPersonWeLookingAt && pEntityAlive->g_Alive())
				m_eDefaultObjAction = eaaTalk;

			else if (pEntityAlive && !pEntityAlive->g_Alive())
				m_eDefaultObjAction = eaaSearchCorpse;

			else if (m_pVehicleWeLookingAt)
				m_eDefaultObjAction = eaaOpenDoor;

			else if (inventory().m_pTarget)
				m_eDefaultObjAction = eaaPickup;
			else 
				m_eDefaultObjAction = eaaNoAction;
		};
	}
	else 
	{
		inventory().m_pTarget	= NULL;
		m_pPersonWeLookingAt	= NULL;
		m_eDefaultObjAction		= eaaNoAction;
	}


	//���������� ��������� � ��������
	UpdateInventoryOwner(DT);

	//��� ������ ���
	UpdateSleep();
}

void CActor::renderable_Render	()
{
	inherited::renderable_Render			();

	if (!m_holder)
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


// HUD
void CActor::OnHUDDraw	(CCustomHUD* /**hud/**/)
{
	//CWeapon* W			= Weapons->ActiveWeapon();
	//if (W)				W->renderable_Render		();
	//CWeapon *W = dynamic_cast<CWeapon*>(inventory().ActiveItem()); if(W) W->renderable_Render();

	if(inventory().ActiveItem()&&!m_holder) {
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

void CActor::SetPhPosition(const Fmatrix &transform)
{
	if(!m_pPhysicsShell){ 
		m_PhysicMovementControl->SetPosition(transform.c);
	}
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
	if (Remote()) return;

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
	//������ ���� ��������� � ������ single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TalkMenu.IsShown())
	{
		if(pGameSP->TalkMenu.IsShown())
		{
			pGameSP->TalkMenu.UpdateQuestions();
		}
		//���� ������ ��������� �� PDA ����� ��� PDA �� ��� �������
		else if(!pGameSP->m_pUserMenu->IsShown())
		{
		}
	}

    CInventoryOwner::OnReceiveInfo(info_index);
}

void CActor::ReceivePdaMessage(u16 who, EPdaMsg msg, int info_index)
{
	//������ ���� ��������� � ������ single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	//������������ � ����������
	CObject* pPdaObject =  Level().Objects.net_Find(who);
	VERIFY(pPdaObject);
	CPda* pPda = dynamic_cast<CPda*>(pPdaObject);
	VERIFY(pPda);
	HUD().GetUI()->UIMainIngameWnd.ReceivePdaMessage(pPda->GetOriginalOwner(), msg, info_index);

	SPdaMessage last_pda_message;
	bool prev_msg = GetPDA()->GetLastMessageFromLog(who, last_pda_message);
	

	//���� ������� ���������� ���������, �� ���� ����������� ��������
	//��� ������ �� �����
	if(!m_pWaitingTradePartner && prev_msg && last_pda_message.question && 
		ePdaMsgTrade == last_pda_message.msg &&
		ePdaMsgAccept == msg)
	{
		m_pWaitingTradePartnerInvOwner = pPda->GetOriginalOwner();
		m_pWaitingTradePartner = dynamic_cast<CEntityAlive*>(pPda->GetOwnerObject());
	}
	else if (ePdaMsgILeave == msg && m_pWaitingTradePartner == dynamic_cast<CEntityAlive*>(pPda->GetOriginalOwner()))
	{
		m_pWaitingTradePartner = NULL;
		m_pWaitingTradePartnerInvOwner = NULL;
	}
		

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
		return Level().timeServer() - m_dwDeathTime;
	else
		return 0;
}


void CActor::OnItemTake			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemTake(inventory_item);
	if (OnClient()) return;
	
	switch (Game().type)
	{
	case GAME_DEATHMATCH:
	case GAME_TEAMDEATHMATCH:
	case GAME_ARTEFACTHUNT:
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
	case GAME_ARTEFACTHUNT:
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
//			Msg					("ge_destroy: [%d] - %s",pAmmo->ID(),*(pAmmo->cName()));
		};
		if (UsableAmmoExist) u_EventSend			(P);
	};
};

void	CActor::SetZoomRndSeed		(s32 Seed)
{
	if (0 != Seed) m_ZoomRndSeed = Seed;
	else m_ZoomRndSeed = s32(timeGetTime());
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
