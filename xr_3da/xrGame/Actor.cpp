// Actor.cpp: implementation of the CActor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\effectorfall.h"
#include "..\CameraLook.h"
#include "..\CameraFirstEye.h"
#include "..\xr_level_controller.h"
#include "EffectorBobbing.h"
#include "customitem.h"
#include "hudmanager.h"
#include "Actor_Flags.h"
#include "UI.h"

const u32		patch_frames	= 50;
const float		respawn_delay	= 1.f;
const float		respawn_auto	= 7.f;


	//skeleton
static	float					skel_density_factor;
static	float					skel_airr_lin_factor;
static	float					skel_airr_ang_factor;
static	float					hinge_force_factor;
static	float					hinge_force_factor1;
static	float					hinge_force_factor2;
static	float					hinge_vel;
static	float					skel_fatal_impulse_factor;

// breakpoints
#include "..\xr_input.h"

//
#include "Actor.h"
#include "ActorAnimation.h"
#include "xr_weapon_list.h"

#include "targetassault.h"
#include "targetcs.h"

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
	cameras[eacFirstEye]	= xr_new<CCameraFirstEye>	(this, pSettings, "actor_firsteye_cam", false);
	cameras[eacLookAt]		= xr_new<CCameraLook>		(this, pSettings, "actor_look_cam",		false);
	cameras[eacFreeLook]	= xr_new<CCameraLook>		(this, pSettings, "actor_free_cam",	false);

	cam_active				= eacFirstEye;
	fPrevCamPos				= 0;
	pCamBobbing				= 0;

	// 
	//Weapons					= 0;

	r_torso.yaw				= 0;
	r_torso.pitch			= 0;
	r_model_yaw				= 0;
	r_model_yaw_delta		= 0;
	r_model_yaw_dest		= 0;
	
	m_fTimeToStep			= 0;
	bStep					= FALSE;

	b_DropActivated			= 0;
	f_DropPower				= 0.f;

	m_fRunFactor			= 2.f;
	m_fCrouchFactor			= 0.2f;

	m_fFallTime				=	s_fFallTime;
	m_bAnimTorsoPlayed		=	false;

	self_gmtl_id			=	GAMEMTL_NONE;
	last_gmtl_id			=	GAMEMTL_NONE;
	m_phSkeleton			=	NULL;
	bDeathInit				=	false;
	m_saved_dir.set(0,0,0);
	m_saved_impulse=0.f;
#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW);
#endif
}

CActor::~CActor()
{
#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif
	//xr_delete(Weapons);
	for (int i=0; i<eacMaxCam; i++) xr_delete(cameras[i]);

	// sounds 2D
	::Sound->destroy(sndZoneHeart);
	::Sound->destroy(sndZoneDetector);

	// sounds 3D
	for (i=0; i<SND_HIT_COUNT; i++) ::Sound->destroy(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) ::Sound->destroy(sndDie[i]);

	if(m_phSkeleton) {
		m_phSkeleton->Deactivate();
		xr_delete<CPhysicsShell>(m_phSkeleton);}
}

void CActor::Load		(LPCSTR section )
{
	Msg("Loading actor: %s",section);
	inherited::Load		(section);
	

		// Movement: General
	//Movement.SetParent		(this);
	Fbox	bb;

	// Movement: BOX
	Fvector	vBOX0_center= pSettings->r_fvector3	(section,"ph_box0_center"	);
	Fvector	vBOX0_size	= pSettings->r_fvector3	(section,"ph_box0_size"		);
	bb.set	(vBOX0_center,vBOX0_center); bb.grow(vBOX0_size);
	ph_Movement.SetBox		(0,bb);

	// Movement: BOX
	Fvector	vBOX1_center= pSettings->r_fvector3	(section,"ph_box1_center"	);
	Fvector	vBOX1_size	= pSettings->r_fvector3	(section,"ph_box1_size"		);
	bb.set	(vBOX1_center,vBOX1_center); bb.grow(vBOX1_size);
	ph_Movement.SetBox		(1,bb);

	// Movement: Foots
	Fvector	vFOOT_center= pSettings->r_fvector3	(section,"ph_foot_center"	);
	Fvector	vFOOT_size	= pSettings->r_fvector3	(section,"ph_foot_size"		);
	bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	ph_Movement.SetFoots	(vFOOT_center,vFOOT_size);

	// Movement: Crash speed and mass
	float	cs_min		= pSettings->r_float	(section,"ph_crash_speed_min"	);
	float	cs_max		= pSettings->r_float	(section,"ph_crash_speed_max"	);
	float	mass		= pSettings->r_float	(section,"ph_mass"				);
	ph_Movement.SetCrashSpeeds	(cs_min,cs_max);
	ph_Movement.SetMass		(mass);


	// Movement: Frictions
	
	float af, gf, wf;
	af					= pSettings->r_float	(section,"ph_friction_air"	);
	gf					= pSettings->r_float	(section,"ph_friction_ground");
	wf					= pSettings->r_float	(section,"ph_friction_wall"	);
	ph_Movement.SetFriction	(af,wf,gf);

	// BOX activate
	ph_Movement.ActivateBox	(0);




	ph_Movement.Load(section);
	ph_Movement.SetParent(this);

	m_fWalkAccel		= pSettings->r_float(section,"walk_accel");	
	m_fJumpSpeed		= pSettings->r_float(section,"jump_speed");
	m_fRunFactor		= pSettings->r_float(section,"run_coef");
	m_fCrouchFactor		= pSettings->r_float(section,"crouch_coef");
	skel_density_factor = pSettings->r_float(section,"ph_skeleton_mass_factor");
	skel_airr_lin_factor=pSettings->r_float(section,"ph_skeleton_airr_lin_factor");
	skel_airr_ang_factor=pSettings->r_float(section,"ph_skeleton_airr_ang_factor");
	hinge_force_factor  =pSettings->r_float(section,"ph_skeleton_hinger_factor");
	hinge_force_factor1 =pSettings->r_float(section,"ph_skeleton_hinger_factor1");
	skel_ddelay			=pSettings->r_s32(section,"ph_skeleton_ddelay");
	hinge_force_factor2 =pSettings->r_float(section,"ph_skeleton_hinger_factor2");
	hinge_vel			=pSettings->r_float(section,"ph_skeleton_hinge_vel");
	skel_fatal_impulse_factor=pSettings->r_float(section,"ph_skel_fatal_impulse_factor");
	ph_Movement.SetJumpUpVelocity(m_fJumpSpeed);

	//Weapons				= xr_new<CWeaponList> (this);

	// sounds
	char buf[256];

	sndStep[0].g_type	= SOUND_TYPE_MONSTER_WALKING_HUMAN;
	sndStep[1].g_type	= SOUND_TYPE_MONSTER_WALKING_HUMAN;
	sndLanding.g_type	= SOUND_TYPE_MONSTER_FALLING_HUMAN;
	::Sound->create		(sndZoneHeart,		TRUE,	"heart\\4");
	::Sound->create		(sndZoneDetector,	TRUE,	"detectors\\geiger");
	::Sound->create		(sndHit[0],			TRUE,	strconcat(buf,cName(),"\\hurt1"),SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->create		(sndHit[1],			TRUE,	strconcat(buf,cName(),"\\hurt2"),SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->create		(sndHit[2],			TRUE,	strconcat(buf,cName(),"\\hurt3"),SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->create		(sndHit[3],			TRUE,	strconcat(buf,cName(),"\\hurt4"),SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->create		(sndDie[0],			TRUE,	strconcat(buf,cName(),"\\die0"),SOUND_TYPE_MONSTER_DYING_HUMAN);
	::Sound->create		(sndDie[1],			TRUE,	strconcat(buf,cName(),"\\die1"),SOUND_TYPE_MONSTER_DYING_HUMAN);
	::Sound->create		(sndDie[2],			TRUE,	strconcat(buf,cName(),"\\die2"),SOUND_TYPE_MONSTER_DYING_HUMAN);
	::Sound->create		(sndDie[3],			TRUE,	strconcat(buf,cName(),"\\die3"),SOUND_TYPE_MONSTER_DYING_HUMAN);

	ph_Movement.ActivateBox	(0);
	ph_Movement.ActivateBox	(0);
	cam_Set					(eacFirstEye);

	// motions
	m_current_legs_blend= 0;
	m_current_jump_blend= 0;
	m_current_legs		= 0;
	m_current_torso		= 0;

	// sheduler
	shedule_Min			= shedule_Max = 1;

	// get self game material id
	self_gmtl_id		= GMLib.GetMaterialIdx("actor");
	last_gmtl_id		= GMLib.GetMaterialIdx("default");
}

//--------------------------------------------------------------------
void CActor::net_Export	(NET_Packet& P)					// export to server
{
	// export 
	R_ASSERT			(Local());
	//VERIFY				(Weapons);

	u8					flags=0;
	CGameObject::net_Export(P);
	P.w_u32				(Level().timeServer());
	P.w_u8				(flags);
	P.w_vec3			(vPosition);
	P.w_u16				(u16(mstate_real));
	P.w_angle8			(r_model_yaw);
	P.w_angle8			(r_torso.yaw);
	P.w_angle8			(r_torso.pitch);
	P.w_sdir			(NET_SavedAccel);
	P.w_sdir			(ph_Movement.GetVelocity());
	P.w_float_q16		(fHealth,-1000,1000);
	P.w_float_q16		(fArmor,-1000,1000);

	int w_id = -1;//Weapons->ActiveWeaponID	();
	if (w_id<0)			P.w_u8(0xff);
	else				P.w_u8(u8(w_id&0xff));
}

void CActor::net_Import		(NET_Packet& P)					// import from server
{
	// import
	R_ASSERT				(Remote());

	net_update			N;

	u8	flags;
	u16	tmp;
	CGameObject::net_Import(P);
	P.r_u32				(N.dwTimeStamp	);
	P.r_u8				(flags			);
	P.r_vec3			(N.p_pos		);
	P.r_u16				(tmp			); N.mstate = u32(tmp);
	P.r_angle8			(N.o_model		);
	P.r_angle8			(N.o_torso.yaw	);
	P.r_angle8			(N.o_torso.pitch);
	P.r_sdir			(N.p_accel		);
	P.r_sdir			(N.p_velocity	);
	P.r_float_q16		(fHealth,-1000,1000);
	P.r_float_q16		(fArmor,-1000,1000);

	u8					wpn;
	P.r_u8				(wpn);
	if (0xff==wpn)		N.weapon		= -1;
	else				N.weapon		= (int)(wpn);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	setVisible					(TRUE);
	setEnabled					(TRUE);
}

BOOL CActor::net_Spawn		(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))	return FALSE;
	ph_Movement.CreateCharacter();
	ph_Movement.SetPLastMaterial(&last_gmtl_id);
	ph_Movement.SetPosition	(vPosition);
	ph_Movement.SetVelocity	(0,0,0);

	xrSE_Actor*			E	= (xrSE_Actor*)DC;

	// Dima : 24.02.2003
	cNameVisual_set			(E->caModel);

	//
	r_model_yaw				= E->o_Angle.y;
	cam_Active()->Set		(E->o_Angle.y,0,0);		// set's camera orientation
	
	// *** movement state - respawn
	mstate_wishful			= 0;
	mstate_real				= 0;
	m_bJumpKeyPressed		= FALSE;

	// *** weapons
	//if (Local()) 			Weapons->ActivateWeaponID	(0);
	//if(Local()) m_inventory.Activate(1);
	
	NET_SavedAccel.set		(0,0,0);
	NET_WasInterpolating	= TRUE;

	setEnabled				(E->s_flags.is(M_SPAWN_OBJECT_LOCAL));
	setActive				(TRUE);

	patch_frame				= 0;
	patch_position.set		(vPosition);

	Engine.Sheduler.Unregister	(this);
	Engine.Sheduler.Register	(this,TRUE);

	hit_slowmo				= 0.f;
	hit_factor				= 1.f;

	m_pArtifact				= 0;


// @@@: WT - !!!ВРЕМЕННО!!! - спавним каждому актеру детектор
	if(Local()) for(u32 i = 0; i < 2; i++) {
		// Create
		//xrServerEntity*		D	= F_entity_Create("detector_simple");
		xrServerEntity*		D	= F_entity_Create("bolt");
		R_ASSERT			(D);
		// Fill
		strcpy				(D->s_name,"bolt");
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xfffd - i;
		D->ID_Parent		=	E->ID;
		D->ID_Phantom		=	0xffff;
		D->s_flags.set		(M_SPAWN_OBJECT_ACTIVE | M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}
// @@@: WT

	// take index spine bone
	CKinematics* V		= PKinematics(pVisual);
	R_ASSERT			(V);
	int spine_bone		= V->LL_BoneID("bip01_spine1");
	int shoulder_bone	= V->LL_BoneID("bip01_spine2");
	int head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetInstance(spine_bone).set_callback		(SpinCallback,this);
	V->LL_GetInstance(shoulder_bone).set_callback	(ShoulderCallback,this);
	V->LL_GetInstance(head_bone).set_callback		(HeadCallback,this);

	m_normal.Create		(V,"norm");
	m_crouch.Create		(V,"cr");
	m_climb.Create		(V,"cr");
	//
	//Weapons->Init		("bip01_r_hand","bip01_l_finger1");

	// load damage params
	if (pSettings->line_exist(cNameSect(),"damage"))
	{
		CInifile::Sect& dam_sect	= pSettings->r_section(pSettings->r_string(cNameSect(),"damage"));
		for (CInifile::SectIt it=dam_sect.begin(); it!=dam_sect.end(); it++)
		{
			if (0==strcmp(it->first,"default")){
				hit_factor	= (float)atof(it->second);
			}else{
				int bone	= V->LL_BoneID(it->first); 
				R_ASSERT2(bone!=BONE_NONE,it->first);
				V->LL_GetInstance(bone).set_param(0,(float)atof(it->second));
			}
		}
	}

	return					TRUE;
}

void CActor::net_Relcase	(CObject* O)
{
	inherited::net_Relcase	(O);
}

void CActor::net_Destroy	()
{
	inherited::net_Destroy	();

	::Sound->destroy			(sndZoneHeart);
	::Sound->destroy			(sndZoneDetector);

	u32 it;
	for (it=0; it<SND_HIT_COUNT; it++)	::Sound->destroy	(sndHit[it]);
	for (it=0; it<SND_DIE_COUNT; it++)	::Sound->destroy	(sndDie[it]);
}

void CActor::Hit		(float iLost, Fvector &dir, CObject* who, s16 element, float impulse)
{
	if (g_Alive()<=0) return;
	Fvector position_in_bone_space;
	position_in_bone_space.set(0.f,0.f,0.f);
	switch (GameID())
	{
	case GAME_SINGLE:		
		{
			if (psActorFlags.test(AF_GODMODE))	return;
			else inherited::Hit		(iLost,dir,who,element,position_in_bone_space, impulse);
		}
		break;
	default:
		inherited::Hit	(iLost,dir,who,element,position_in_bone_space, impulse);
		break;
	}
}


void CActor::Hit		(float iLost, Fvector &dir, CObject* who, s16 element,Fvector position_in_bone_space, float impulse)
{
	if (g_Alive()>0){
		ph_Movement.ApplyImpulse(dir,impulse);
		m_saved_dir.set(dir);
		m_saved_position.set(position_in_bone_space);
		m_saved_impulse=impulse*skel_fatal_impulse_factor;
		m_saved_element=element;
	}
	else if(m_phSkeleton) 
		m_phSkeleton->applyImpulseTrace(position_in_bone_space,dir,impulse,element);
		//m_phSkeleton->applyImpulseTrace(position_in_bone_space,dir,impulse);
	else{
		m_saved_dir.set(dir);
		m_saved_impulse=impulse*skel_fatal_impulse_factor;
		m_saved_element=element;
		m_saved_position.set(position_in_bone_space);
	}

	if (g_Alive()<=0) return;

	switch (GameID())
	{
	case GAME_SINGLE:		
		{
			if (psActorFlags.test(AF_GODMODE))	return;
			else inherited::Hit		(iLost,dir,who,element,position_in_bone_space, impulse);
		}
		break;
	default:
		inherited::Hit	(iLost,dir,who,element,position_in_bone_space, impulse);
		break;
	}
}

void CActor::HitSignal(float perc, Fvector& vLocalDir, CObject* who, s16 element)
{
	if (g_Alive()) 
	{
		sound& S = sndHit[Random.randI(SND_HIT_COUNT)];
		if (S.feedback) return;

		// Play hit-sound
		::Sound->play_at_pos	(S,this,vPosition);

		// hit marker
		if (Local() && (who!=this))	
		{
			int id		= -1;
			float x		= _abs(vLocalDir.x);
			float z		= _abs(vLocalDir.z);
			if (z>x)	id = (vLocalDir.z<0)?2:0;
			else		id = (vLocalDir.x<0)?3:1;
			HUD().Hit	(id);
		}

		// stop-motion
		if (ph_Movement.Environment()==CPHMovementControl::peOnGround || ph_Movement.Environment()==CPHMovementControl::peAtWall)
		{
			Fvector zeroV;
			zeroV.set			(0,0,0);
			ph_Movement.SetVelocity(zeroV);
		}
		hit_slowmo				= perc/100.f;
		if (hit_slowmo>1.f)		hit_slowmo = 1.f;

		// check damage bone
	}
}

void CActor::Die	( )
{
	// Dima
	b_DropActivated			= TRUE;
	g_PerformDrop			();
	// Play sound
	::Sound->play_at_pos		(sndDie[Random.randI(SND_DIE_COUNT)],this,vPosition);
	cam_Set					(eacFreeLook);
	g_fireEnd				();
	mstate_wishful	&=		~mcAnyMove;
	mstate_real		&=		~mcAnyMove;
	ph_Movement.GetDeathPosition(vPosition);
	ph_Movement.DestroyCharacter();
	create_Skeleton();
}

void CActor::g_Physics			(Fvector& _accel, float jump, float dt)
{
	if (!g_Alive())	{

		if(m_phSkeleton)
		if(m_phSkeleton->bActive && m_saved_impulse!=0.f)
		{
			m_phSkeleton->applyImpulseTrace(m_saved_position,m_saved_dir,m_saved_impulse*1.5f,m_saved_element);
			m_saved_impulse=0.f;
		}
		

		if(m_phSkeleton){
			mRotate.set(m_phSkeleton->mXFORM);
			mRotate.c.set(0,0,0);
			vPosition.set(m_phSkeleton->mXFORM.c);
			if(skel_ddelay==0)
			{
			m_phSkeleton->set_JointResistance(5.f*hinge_force_factor1);
			m_phSkeleton->Enable();
			}
			skel_ddelay--;
		}
		else
		{if(bDeathInit)
		{
			//create_Skeleton();
		//	create_Skeleton1();
			bDeathInit=false;
			return;
		}
			bDeathInit=true;
		}
		return;
	}

	if (patch_frame<patch_frames)	return;

	// Correct accel
	Fvector		accel;
	accel.set					(_accel);
	hit_slowmo					-=	dt;
	if (hit_slowmo<0)			hit_slowmo = 0.f;
	accel.mul					(1.f-hit_slowmo);

	// Calculate physics

	//ph_Movement.SetPosition		(vPosition);
	//ph_Movement.Calculate		(accel,0,jump,dt,false);
	//ph_Movement.GetPosition		(vPosition);
	//Fvector vAccel;
	//ph_Movement.vExternalImpulse.div(dt);
	
	//ph_Movement.SetPosition		(vPosition);

	ph_Movement.Calculate			(_accel,0,jump,dt,false);
	ph_Movement.GetPosition		(vPosition);
	ph_Movement.bSleep=false;
	///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////Update ph_Movement///////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	
	//ph_Movement.SetEnvironment(ph_Movement.Environment(),ph_Movement.OldEnvironment());//peOnGround,peAtWall,peInAir
	//ph_Movement.SetPosition		(vPosition);
	//Fvector velocity=ph_Movement.GetVelocity();
	//ph_Movement.SetVelocity(velocity);
	//ph_Movement.gcontact_Was=ph_Movement.gcontact_Was;
	//ph_Movement.SetContactSpeed(ph_Movement.GetContactSpeed());
	//velocity.y=0.f;
//	ph_Movement.SetActualVelocity(velocity.magnitude());

	//ph_Movement.gcontact_HealthLost=ph_Movement.gcontact_HealthLost;
	//ph_Movement.gcontact_Power=ph_Movement.gcontact_Power;

	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
/*
	if (ph_Movement.gcontact_Was) 
	{
		Fvector correctV					= ph_Movement.GetVelocity	();
		correctV.x							*= 0.1f;
		correctV.z							*= 0.1f;
		ph_Movement.SetVelocity				(correctV);

		if (Local()) {
			pCreator->Cameras.AddEffector		(xr_new<CEffectorFall> (ph_Movement.gcontact_Power));
			Fvector D; D.set					(0,1,0);
			if (ph_Movement.gcontact_HealthLost)	Hit	(1.5f * ph_Movement.gcontact_HealthLost,D,this,-1);
		}
	}
*/	


	{
//		SGameMtlPair* pair	= GMLib.GetMaterialPair(0,1); R_ASSERT(pair);
//		::Sound->play_at_pos	(pair->HitSounds[0],this,Position());
//		::Sound->play_at_pos						(sndLanding,this,Position());

		if (Local()) {
			
			if (ph_Movement.gcontact_Was) 
				pCreator->Cameras.AddEffector		(xr_new<CEffectorFall> (ph_Movement.gcontact_Power));
			Fvector D; D.set					(0,1,0);
			if (ph_Movement.gcontact_HealthLost)	{
				Hit	(ph_Movement.gcontact_HealthLost,D,this,-1,0);
				if(g_Alive()<=0)
					ph_Movement.GetDeathPosition(vPosition);
			}
		}
	}	
}

void CActor::net_update::lerp(CActor::net_update& A, CActor::net_update& B, float f)
{
	float invf		= 1.f-f;
	// 
	o_model			= angle_lerp	(A.o_model,B.o_model,		f);
	o_torso.yaw		= angle_lerp	(A.o_torso.yaw,B.o_torso.yaw,f);
	o_torso.pitch	= angle_lerp	(A.o_torso.pitch,B.o_torso.pitch,f);
	p_pos.lerp		(A.p_pos,B.p_pos,f);
	p_accel			= (f<0.5f)?A.p_accel:B.p_accel;
	p_velocity.lerp	(A.p_velocity,B.p_velocity,f);
	mstate			= (f<0.5f)?A.mstate:B.mstate;
	weapon			= (f<0.5f)?A.weapon:B.weapon;
	fHealth			= invf*A.fHealth+f*B.fHealth;
	fArmor			= invf*A.fArmor+f*B.fArmor;
	weapon			= (f<0.5f)?A.weapon:B.weapon;
}

void CActor::ZoneEffect	(float z_amount)
{
	clamp				(z_amount,0.f,1.f);

	// Gray
	::Render->getTarget()->set_gray	(z_amount*z_amount);

	// Calc shift func
	float f_x			= Device.fTimeGlobal;
	float f_sin4x		= sinf(4.f*f_x);
	float f_sin4x_s		= sinf(PI/3.f + 4.f*f_x);
	float f_sin4x_sa	= _abs(f_sin4x_s);
	float F				= (f_sin4x+f_sin4x_sa)+(1+f_sin4x*f_sin4x_sa)+ 0.3f*sinf(tanf(PI/(2.1f)*sinf(f_x)));

	// Fov/Shift + Pulse
	CCameraBase* C		= cameras	[cam_active];
	float	shift		= z_amount*F*.1f;
	C->f_fov			= 90.f+z_amount*15.f + shift;
	C->f_aspect			= 1.f+cam_shift/3;
	cam_shift			= shift/(3.f*3.f);

	// Sounds
	Fvector				P;
	clCenter			(P);
	if (0==sndZoneHeart.feedback)		::Sound->play_at_pos	(sndZoneHeart,		this,Position(),true);
	sndZoneHeart.set_volume				(z_amount);
	sndZoneHeart.set_position			(P);
}

void CActor::UpdateCL()
{

	inherited::UpdateCL();
	if (!g_Alive())			
	
		if(m_phSkeleton){
			mRotate.set(m_phSkeleton->mXFORM);
			mRotate.c.set(0,0,0);
			vPosition.set(m_phSkeleton->mXFORM.c);
			UpdateTransform();
			}
	// Analyze Die-State
	/*
	if (!g_Alive())			
	{
		float dt			=	Device.fTimeDelta;
		setEnabled	(FALSE);
		if (die_hide>0)		
		{
			die_hide			-=	.01f*dt;
			if (die_hide>0)		{
				Fmatrix					mScale,mTranslate;
				float	down			= (1.f-die_hide)/10.f;
				vScale.set				(1,die_hide,1);
				mScale.scale			(vScale);
				mTranslate.translate	(vPosition.x,vPosition.y-down,vPosition.z);
				clTransform.mul_43		(mTranslate,mRotate);
				clTransform.mulB_43		(mScale);
			}
			else if (Local()) 
			{
				// Request destroy
				NET_Packet			P;
				u_EventGen			(P,GE_DESTROY,ID());
				u_EventSend			(P);
			}
		}
	}
	*/
}

void CActor::Update	(u32 DT)
{
	if (!getEnabled())	return;
	if (!Ready())		return;

	// patch
	if (patch_frame<patch_frames)	{
		vPosition.set		(patch_position);
		if(!m_phSkeleton)
		ph_Movement.SetPosition(patch_position);
		patch_frame			+= 1;
	}
	cam_shift				= 0.f;
	cam_gray				= 0.f;

	//********************** just for vitya's pleasure
	//CRender_target*		T	= ::Render->getTarget();
	////T->set_duality_h		(.015f*sinf(1.f*Device.fTimeGlobal));
	////T->set_duality_v		(.017f*cosf(1.1f*Device.fTimeGlobal));
	//T->set_duality_h		(.0f);
	//T->set_duality_v		(.0f);
	//T->set_noise			(.5f);//(.5f);
	//T->set_noise_scale		(1.f);
	//T->set_noise_color		(color_rgba(255,255,127,0));
	//T->set_noise_fps		(10.f);

	// 
	clamp					(DT,0u,100u);
	float	dt				= float(DT)/1000.f;

	// Check controls, create accel, prelimitary setup "mstate_real"
	float	Jump	= 0;
	if (Local())	{
		g_cl_CheckControls		(mstate_wishful,NET_SavedAccel,Jump,dt);
		g_cl_Orientate			(mstate_real,dt);
		g_Orientate				(mstate_real,dt);
		g_Physics				(NET_SavedAccel,Jump,dt);
		g_cl_ValidateMState		(dt,mstate_wishful);
		g_SetAnimation			(mstate_real);

		// Check for game-contacts
		Fvector C; float R;		ph_Movement.GetBoundingSphere	(C,R);
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
		// distinguish interpolation/extrapolation
		u32	dwTime			= Level().timeServer()-NET_Latency;
		net_update&	N		= NET.back();
		if ((dwTime > N.dwTimeStamp) || (NET.size()<2))
		{
			// BAD.	extrapolation
			if (NET_WasInterpolating)
			{
				NET_WasInterpolating	= FALSE;
				NET_Last				= N;

				// Setup last known data
				ph_Movement.SetVelocity	(NET_Last.p_velocity);
				ph_Movement.SetVelocity	(NET_Last.p_velocity);
				vPosition.set			(NET_Last.p_pos);
				if(!m_phSkeleton)
				ph_Movement.SetPosition(NET_Last.p_pos);
				
				UpdateTransform			();
			}

			g_sv_Orientate				(NET_Last.mstate,dt			);
			g_Orientate					(NET_Last.mstate,dt			);
			g_Physics					(NET_Last.p_accel,Jump,dt	);
			g_SetAnimation				(NET_Last.mstate			);
		} else {
			// OK.	interpolation

			// Search 2 keyframes for interpolation
			int select		= -1;
			for (u32 id=0; id<NET.size()-1; id++)
			{
				if ((NET[id].dwTimeStamp<=dwTime)&&(dwTime<=NET[id+1].dwTimeStamp))	select=id;
			}
			if (select>=0)	
			{
				if (select>0)	NET.erase(NET.begin(),NET.begin()+select);
				
				// Interpolate state
				net_update&	A	= NET[0];
				net_update&	B	= NET[1];
				u32	d1		= dwTime-A.dwTimeStamp;
				u32	d2		= B.dwTimeStamp - A.dwTimeStamp;
				float	factor	= (float(d1)/float(d2));
				NET_Last.lerp	(A,B,factor);
				
				// Use interpolated state
				g_sv_Orientate	(NET_Last.mstate,dt);
				g_Orientate		(NET_Last.mstate,dt);
				vPosition.set	(NET_Last.p_pos);		// physics :)
				if(!m_phSkeleton)
				ph_Movement.SetPosition(NET_Last.p_pos);
				g_SetAnimation	(NET_Last.mstate);
				
				// Change weapon if needed
				//if (Weapons->ActiveWeaponID()!=NET_Last.weapon)
				//{
				//	Weapons->ActivateWeaponID(NET_Last.weapon);
				//}

				// Signal, that last time we used interpolation
				NET_WasInterpolating	= TRUE;
				NET_Time				= dwTime;
			}
		}

		mstate_real			= NET_Last.mstate;
	}

	// generic stuff
	UpdateTransform			();
	inherited::Update		(DT);
	
	if (IsMyCamera())		{
		if (!pCamBobbing){ 
			pCamBobbing = xr_new<CEffectorBobbing>	();
			Level().Cameras.AddEffector			(pCamBobbing);
		}
		pCamBobbing->SetState					(mstate_real);
		//cam_Update								(dt,Weapons->getZoomFactor());
		CWeapon *l_pW = dynamic_cast<CWeapon*>(m_inventory.m_activeSlot < 0xffffff ? m_inventory.m_slots[m_inventory.m_activeSlot].m_pIItem : NULL);
		cam_Update(dt,l_pW?l_pW->GetZoomFactor():DEFAULT_FOV);
	} else {
		if (pCamBobbing)						{Level().Cameras.RemoveEffector(cefBobbing); pCamBobbing=0;}
	}

	setVisible				(!HUDview	());

	//Weapons->Update			(dt,HUDview());
	if(m_inventory.ActiveItem()) m_inventory.ActiveItem()->m_showHUD = !!HUDview();
	CWeapon *l_pW = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	if(l_pW) l_pW->SetHUDmode(HUDview());

	R_ASSERT(last_gmtl_id!=GAMEMTL_NONE);
	SGameMtlPair* mtl_pair		= GMLib.GetMaterialPair(self_gmtl_id,last_gmtl_id);
	R_ASSERT3(mtl_pair,"Undefined material pair: Actor # ", GMLib.GetMaterial(last_gmtl_id)->name);
	// sound step
	if ((mstate_real&mcAnyMove)&&(!(mstate_real&(mcJump|mcFall|mcLanding|mcLanding2)))){
		if(m_fTimeToStep<0){
			bStep				= !bStep;
			float k				= (mstate_real&mcCrouch)?0.75f:1.f;
			float tm			= isAccelerated(mstate_real)?(PI/(k*10.f)):(PI/(k*7.f));
			m_fTimeToStep		= tm;
			sndStep[bStep].clone		(mtl_pair->StepSounds[bStep]);
			sndStep[bStep].play_at_pos	(this,Position());
		}
		m_fTimeToStep -= dt;
	}

	// sounds update
	float	s_k			=	(mstate_real&mcCrouch)?0.85f:1.f;
	float	s_vol		=	s_k * (isAccelerated(mstate_real)?1.f:.85f);
	Fvector	s_pos		=	Position	();
	s_pos.y				+=	.15f;
	if (sndStep[0].feedback)		{
		sndStep[0].set_position(s_pos);
		sndStep[0].set_volume	(s_vol);
	}
	if (sndStep[1].feedback)		{
		sndStep[1].set_position(s_pos);
		sndStep[1].set_volume	(s_vol);
	}

	// landing sounds
	if (!sndLanding.feedback&&(mstate_real&(mcLanding|mcLanding2))){
		sndLanding.clone	(mtl_pair->HitSounds[0]);
		::Sound->play_at_pos	(sndLanding,this,s_pos);
	}

	m_inventory.Update(DT);
}

void CActor::OnVisible	()
{
	inherited::OnVisible	();

	//CWeapon* W				= Weapons->ActiveWeapon();
	//CWeapon *W = dynamic_cast<CWeapon*>(m_inventory.ActiveItem()); if(W) W->OnVisible();
	if(m_inventory.ActiveItem()) m_inventory.ActiveItem()->OnVisible();
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
			start_pos.add		(vPosition);
			//if (!pCreator->ObjectSpace.EllipsoidCollide(cfModel,svTransform,start_pos,bbStandBox))
			Fbox stand_box=ph_Movement.Boxes()[0];
			stand_box.y1+=ph_Movement.FootExtent().y;
			ph_Movement.GetPosition(start_pos);
			start_pos.y+=(
						//-(ph_Movement.Box().y2-ph_Movement.Box().y1)+
						(ph_Movement.Boxes()[0].y2-ph_Movement.Boxes()[0].y1)
						)/2.f;
			start_pos.y+=ph_Movement.FootExtent().y/2.f;
			if (!pCreator->ObjectSpace.EllipsoidCollide(cfModel,svTransform,start_pos,stand_box))
			{
				mstate_real &= ~mcCrouch;
				ph_Movement.ActivateBox(0);
				ph_Movement.ActivateBox(0);
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
	if (ph_Movement.gcontact_Was){
		if (mstate_real&mcFall){
			if (ph_Movement.GetContactSpeed()>4.f){
				if (fis_zero(ph_Movement.gcontact_HealthLost)){	
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
	if (((ph_Movement.GetVelocityActual()<0.2f)&&(!(mstate_real&(mcFall|mcJump)))) || ph_Movement.bSleep) 
	{
		mstate_real				&=~ mcAnyMove;
	}
	if (ph_Movement.Environment()==CPHMovementControl::peOnGround || ph_Movement.Environment()==CPHMovementControl::peAtWall)
	{
		// если на земле гарантированно снимать флажок Jump
		if (((s_fJumpTime-m_fJumpTime)>s_fJumpGroundTime)&&(mstate_real&mcJump))
		{
			mstate_real			&=~	mcJump;
			m_fJumpTime			= s_fJumpTime;
		}
	}
	if(ph_Movement.Environment()==CPHMovementControl::peAtWall)

		mstate_real				|=mcClimb;

	else
		mstate_real				&=~mcClimb;
}

void CActor::g_cl_CheckControls(u32 mstate_wf, Fvector &vControlAccel, float &Jump, float dt)
{
	// ****************************** Check keyboard input and control acceleration
	vControlAccel.set	(0,0,0);

	if (!(mstate_real&mcFall) && (ph_Movement.Environment()==CPHMovementControl::peInAir)) 
	{
		m_fFallTime				-=	dt;
		if (m_fFallTime<=0.f){
			m_fFallTime			=	s_fFallTime;
			mstate_real			|=	mcFall;
			mstate_real			&=~	mcJump;
		}
	}

	if (ph_Movement.Environment()==CPHMovementControl::peOnGround || ph_Movement.Environment()==CPHMovementControl::peAtWall )
	{
		// jump
		m_fJumpTime				-=	dt;
		
		if (((mstate_real&mcJump)==0) && (mstate_wf&mcJump) && (m_fJumpTime<=0.f) && !m_bJumpKeyPressed)
		{
			mstate_real			|=	mcJump;
			m_bJumpKeyPressed	=	TRUE;
			Jump				= m_fJumpSpeed;
			m_fJumpTime			= s_fJumpTime;
		}
		
		/*
		if(m_bJumpKeyPressed)
				Jump				= m_fJumpSpeed;
			*/
		// crouch
		if ((0==(mstate_real&mcCrouch))&&(mstate_wf&mcCrouch))
		{
			mstate_real			|=	mcCrouch;
			ph_Movement.ActivateBox(1);
			ph_Movement.ActivateBox(1);
		}
		
		// mask input into "real" state
		u32 move	= mcAnyMove|mcAccel;
		mstate_real &= (~move);
		mstate_real |= (mstate_wf & move);
		
		// check player move state
		if (mstate_real&mcAnyMove)
		{
			BOOL	bAccelerated		= isAccelerated(mstate_real);
			
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
	mRotate.rotateY		(-(r_model_yaw + r_model_yaw_delta));
}

// ****************************** Update actor orientation according to camera orientation
void CActor::g_cl_Orientate	(u32 mstate_rl, float dt)
{
	// capture camera into torso (only for FirstEye & LookAt cameras)
	if (cam_active!=eacFreeLook){
		r_torso.yaw		=	cam_Active()->GetWorldYaw	();
		r_torso.pitch	=	cam_Active()->GetWorldPitch	();
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

void CActor::g_sv_Orientate(u32 mstate_rl, float dt)
{
	// rotation
	r_model_yaw		= NET_Last.o_model;
	r_torso.yaw		= NET_Last.o_torso.yaw;
	r_torso.pitch	= NET_Last.o_torso.pitch;
}

void CActor::g_fireParams	(Fvector &fire_pos, Fvector &fire_dir)
{
	if (Local()) {
		if (HUDview()) 
		{
			fire_pos = Device.vCameraPosition;
			fire_dir = Device.vCameraDirection;
		} //else Weapons->GetFireParams(fire_pos, fire_dir);
	} else {
		//Weapons->GetFireParams(fire_pos, fire_dir);
	}
}

void CActor::g_fireStart	( )
{
	//Weapons->FireStart		( );
}

void CActor::g_fireEnd	( )
{
	//Weapons->FireEnd	( );
}

void CActor::g_fire2Start	( )
{
	//Weapons->Fire2Start		( );
}

void CActor::g_fire2End	( )
{
	//Weapons->Fire2End	( );
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
		CObject*		O		= m_inventory.ActiveItem();//Weapons->ActiveWeapon();
		if (0==O)				return;

		NET_Packet				P;
		u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16					(u16(O->ID()));
		u_EventSend				(P);
	}
}

void CActor::g_WeaponBones	(int& L, int& R)
{
	CKinematics* V	= PKinematics(Visual());
	R = V->LL_BoneID("bip01_r_hand");
	L = V->LL_BoneID("bip01_l_finger1");

	////VERIFY	(Weapons);
	//L		=	0;//Weapons->m_iACTboneL;
	//R		=	0;//Weapons->m_iACTboneR;
}

void CActor::Statistic		()
{
}

// HUD
void CActor::OnHUDDraw	(CCustomHUD* hud)
{
	//CWeapon* W			= Weapons->ActiveWeapon();
	//if (W)				W->OnVisible		();
	//CWeapon *W = dynamic_cast<CWeapon*>(m_inventory.ActiveItem()); if(W) W->OnVisible();
	if(m_inventory.ActiveItem()) m_inventory.ActiveItem()->OnVisible();


#ifdef _DEBUG
	string128 buf;
	HUD().pFontSmall->SetColor(0xffffffff);
	HUD().pFontSmall->OutSet	(120,530);
	HUD().pFontSmall->OutNext("Position:      [%3.2f, %3.2f, %3.2f]",VPUSH(vPosition));
	HUD().pFontSmall->OutNext("Velocity:      [%3.2f, %3.2f, %3.2f]",VPUSH(ph_Movement.GetVelocity()));
	HUD().pFontSmall->OutNext("Vel Magnitude: [%3.2f]",ph_Movement.GetVelocityMagnitude());
	HUD().pFontSmall->OutNext("Vel Actual:    [%3.2f]",ph_Movement.GetVelocityActual());
	switch (ph_Movement.Environment())
	{
	case CPHMovementControl::peOnGround:	strcpy(buf,"ground");			break;
	case CPHMovementControl::peInAir:		strcpy(buf,"air");				break;
	case CPHMovementControl::peAtWall:	strcpy(buf,"wall");				break;
	}
	HUD().pFontSmall->OutNext	(buf);
#endif
/**
	CUI* pUI=HUD().GetUI	();
	pUI->OutHealth		(iFloor(fHealth),iFloor(fArmor));
	pUI->OutWeapon		(Weapons->ActiveWeapon());
	pUI->SetHeading		(-r_torso.yaw);
	

	char buf[128];
	sprintf(buf,"Position : %3.2f,%3.2f,%3.2f",VPUSH(vPosition));

	HUD().pHUDFont->Color(0xffffffff);
	HUD().pHUDFont->Out	(400,320,buf);
/*
	char buf[128];
	buf[0] = 0;
	CWeapon* W=Weapons->ActiveWeapon();
	if (W){
		float prec = W->GetPrecision();
		sprintf(buf,"Prec: %3.2f",prec);
		HUD().pHUDFont->Color(0xffffffff);
		HUD().pHUDFont->Out	(400,320,buf);
	}
	char buf[128];
	buf[0] = 0;
	switch (ph_Movement.Environment())
	{
	case CMovementControl::peOnGround:	strcat(buf,"ground:");			break;
	case CMovementControl::peInAir:		strcat(buf,"air:");				break;
	case CMovementControl::peAtWall:	strcat(buf,"wall:");			break;
	}

	if (mstate_real&mcAccel)	strcat(buf,"Accel ");
	if (mstate_real&mcCrouch)	strcat(buf,"Crouch ");
	if (mstate_real&mcFwd)		strcat(buf,"Fwd ");
	if (mstate_real&mcBack)		strcat(buf,"Back ");
	if (mstate_real&mcLStrafe)	strcat(buf,"LStrafe ");
	if (mstate_real&mcRStrafe)	strcat(buf,"RStrafe ");
	if (mstate_real&mcJump)		strcat(buf,"Jump ");
	if (mstate_real&mcTurn)		strcat(buf,"Turn ");
	if (m_bJumpKeyPressed)		strcat(buf,"+Jumping ");
	HUD().pHUDFont->Color(0xffffffff);
	HUD().pHUDFont->Out	(400,320,buf);
	HUD().pHUDFont->Out	(400,330,"Vel Actual:    %3.2f",ph_Movement.GetVelocityActual());
	HUD().pHUDFont->Out	(400,340,"Vel:           %3.2f",ph_Movement.GetVelocity());
	HUD().pHUDFont->Out	(400,350,"Vel Magnitude: %3.2f",ph_Movement.GetVelocityMagnitude());
*/
}

float CActor::HitScale	(int element)
{
	CKinematics* V		= PKinematics(pVisual);			VERIFY(V);
	float scale			= fis_zero(V->LL_GetInstance(element).get_param(0))?1.f:V->LL_GetInstance(element).get_param(0);
	return hit_factor*scale;
}

void CActor::create_Skeleton(){
	Fmatrix ident;
	float density=100.f*skel_density_factor;
	float hinge_force=5.f*hinge_force_factor;
	float hinge_force1=5.f*hinge_force_factor2;
	//u32 material=0;
	LPCSTR material="actor";
	ident.identity();

	Fmatrix m1;
	m1.set(ident);
	m1._11=0.f;
	m1._12=1.f;
	m1._21=-1.f;
	m1._22=0.f;


	Fmatrix m2;
	m2.set(ident);
	m2._11=-1.f;
	m2._33=-1.f;

	Fmatrix m3;
	m3.set(ident);
	m3._11=-1.f;
	m3._22=-1.f;

	Fmatrix m4;
	m4.set(ident);
	m4._22=-1.f;
	m4._33=-1.f;

	Fmatrix m5;
	m5.set(ident);

	Fmatrix m6;
	m6.set(m1);
	m6._12=-1.f;
	m6._21=1.f;

	//create shell
	CKinematics* M		= PKinematics(pVisual);			VERIFY(M);
	m_phSkeleton		= P_create_Shell();
	m_phSkeleton->set_Kinematics(M);
	CPhysicsJoint*		joint;
	//get bone instance
	int id=M->LL_BoneID("bip01_pelvis");
	CBoneInstance& instance=M->LL_GetInstance				(id);

	//create root element
	CPhysicsElement* element=P_create_Element				();
	element->mXFORM.set(m1);
	instance.set_callback(m_phSkeleton->GetBonesCallback(),element);

	element->add_Box(M->LL_GetBox(id));
	//Fsphere sphere;
	//sphere.P.set(0,0,0);
	//sphere.R=0.3f;
	//pelvis->add_Sphere(sphere);
	element->setMass(density);
	m_phSkeleton->add_Element(element);
	element->SetMaterial("materials\\skel1");
	CPhysicsElement* parent=element;
	CPhysicsElement* root=parent;

	//spine
	
	id=M->LL_BoneID("bip01_spine");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1.f/4.f,M_PI*1.f/2.f,0);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);
	//Fquaternion k;
	//k.get_axis_angle
	//Fmatrix m;
	
	
	//parent=element;
	
	id=M->LL_BoneID("bip01_spine1");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(1,0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI/4.f,M_PI/4.f,0);//
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,2);
	joint->SetLimits(-M_PI/8.f,M_PI/8.f,1);
	joint->SetForceAndVelocity(hinge_force);
	joint->SetForceAndVelocity(hinge_force*10,1.5f,2);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	CPhysicsElement* root1=parent;
	id=M->LL_BoneID("bip01_neck");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	/*
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,0);
	*/
	
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetAxisVsSecondElement(0,0,1,1);
	joint->SetAxisVsSecondElement(1,0,0,0);

	joint->SetLimits(-M_PI/5.f,M_PI/5.f,0);
	joint->SetLimits(0.f,0.f,1);
	joint->SetLimits(-M_PI/3.f,M_PI/3.f,2);
	
	//joint->SetLimits(0.1f,0.f,0);
	//joint->SetLimits(0.f,0.f,1);
	//joint->SetLimits(0.f,0.f,2);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	id=M->LL_BoneID("bip01_head");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density*5);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/3.f,M_PI/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=root1;
	
	id=M->LL_BoneID("bip01_l_clavicle");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
//	const Fobb& box=M->LL_GetBox(id);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);//box.m_halfsize.y box.m_halfsize.x*2.f
	joint->SetAxisVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	//parent=element;
	
	id=M->LL_BoneID("bip01_l_upperarm");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,0,1,0);
	joint->SetLimits(-M_PI/3.f,M_PI/2.2f,0);
	//joint->SetLimits(0.f,0.f,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,2);
	joint->SetLimits(0.f,0.f,1);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	id=M->LL_BoneID("bip01_l_forearm");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*3.f/4.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	id=M->LL_BoneID("bip01_l_hand");
	element=P_create_Element				();
	element->mXFORM.set(m3);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density*20.f);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/3.f,M_PI*1/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);


	parent=root1;
	id=M->LL_BoneID("bip01_r_clavicle");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/3.f,M_PI/4.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	//parent=element;
	id=M->LL_BoneID("bip01_r_upperarm");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,0,1,0);
	joint->SetLimits(-M_PI/2.2f,M_PI/3.f,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,2);
	joint->SetLimits(0.f,0.f,1);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	id=M->LL_BoneID("bip01_r_forearm");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*3.f/4.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	id=M->LL_BoneID("bip01_r_hand");
	element=P_create_Element				();
	element->mXFORM.set(m5);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density*20.f);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/3.f,M_PI*1/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=root;
	id=M->LL_BoneID("bip01_r_thigh");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,0,1,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI*1.f/6.f,M_PI*1.f/4.f,2);
	joint->SetLimits(0.f,0.f,1);
	//joint->SetLimits(0,M_PI*1/3.5f,0);
	joint->SetLimits(-M_PI*1/8.f,0,0);
	joint->SetForceAndVelocity(hinge_force*2.f);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=element;
	id=M->LL_BoneID("bip01_r_calf");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*2/3.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=element;
	id=M->LL_BoneID("bip01_r_foot");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density*5.f);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/6.f,M_PI*1/6.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=root;
	id=M->LL_BoneID("bip01_l_thigh");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,0,1,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI*1.f/6.f,M_PI*1.f/4.f,2);
	joint->SetLimits(0.f,0.f,1);
	joint->SetLimits(0,M_PI*1/8.f,0);
	joint->SetForceAndVelocity(hinge_force*2.f);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=element;
	id=M->LL_BoneID("bip01_l_calf");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*2/3.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

 	parent=element;
	id=M->LL_BoneID("bip01_l_foot");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density*20.f);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/6.f,M_PI*1/6.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");


	//set shell start position
	Fmatrix m;
	m.set(mRotate);
	m.c.set(vPosition);
	//ph_Movement.GetDeathPosition(m.c);
	//m.c.y-=0.4f;
	m_phSkeleton->mXFORM.set(m);
	m_phSkeleton->SetAirResistance(0.002f*skel_airr_lin_factor,
								   0.3f*skel_airr_ang_factor);
	
}



void CActor::create_Skeleton1(){
	Fmatrix ident;
	float density=100.f;
	float hinge_force=5.f*hinge_force_factor;
	//u32 material=0;
	LPCSTR material="actor";
	ident.identity();

	Fmatrix m1;
	m1.set(ident);


	Fmatrix m2;
	/*
	m2._11=	0.80434370f;
	m2._12=	-0.59369904f;
	m2._13=	0.00000000f;

	m2._14=	0.00000000f;

	m2._21=	0.59369904f;
	m2._22=	0.80434370f;
	m2._23=	0.00000000f;

	m2._24=	0.00000000f;

	m2._31=	0.00000000f;
	m2._32=	0.00000000f;
	m2._33=	1.0000000f;

	m2._34=	0.00000000f;
*/

	m2._11=	0.0f;
	m2._12=	-1.0f;
	m2._13=	0.00000000f;

	m2._14=	0.00000000f;

	m2._21=	1.0f;
	m2._22=	0.0f;
	m2._23=	0.00000000f;

	m2._24=	0.00000000f;

	m2._31=	0.00000000f;
	m2._32=	0.00000000f;
	m2._33=	1.0000000f;

	m2._34=	0.00000000f;



	//create shell
	CKinematics* M		= PKinematics(pVisual);			VERIFY(M);
	m_phSkeleton		= P_create_Shell();
	CPhysicsJoint*		joint;
	//get bone instance
	int id=M->LL_BoneID("bone01");
	CBoneInstance& instance=M->LL_GetInstance				(id);

	//create root element
	CPhysicsElement* element=P_create_Element				();
	element->mXFORM.set(m1);
	instance.set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	//Fsphere sphere;
	//sphere.P.set(0,0,0);
	//sphere.R=0.3f;
	//pelvis->add_Sphere(sphere);
	element->setMass(density);
	m_phSkeleton->add_Element(element);
	element->SetMaterial(material);

	CPhysicsElement* parent=element;


	//spine
	id=M->LL_BoneID("bone02");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,0,1,0);
	joint->SetLimits(0,M_PI*1.f/2.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);
	//Fquaternion k;
	//k.get_axis_angle
	//Fmatrix m;
	
	





	//set shell start position
	Fmatrix m;
	m.set(mRotate);
	ph_Movement.GetDeathPosition(m.c);
	m_phSkeleton->mXFORM.set(m);

}


void CActor::SetPhPosition(const Fmatrix &pos)
{

	if(!m_phSkeleton) ph_Movement.SetPosition(pos.c);
	//else m_phSkeleton->S
}

void CActor::ForceTransform(const Fmatrix& m)
{
	if(g_Alive()<=0) return;
	mRotate.set(m);
	mRotate.c.set(0,0,0);
	vPosition.set(m.c);
	UpdateTransform();
	ph_Movement.EnableCharacter();
	ph_Movement.SetPosition(m.c);

}
#ifdef DEBUG
void dbg_draw_frustum (float FOV, float _FAR, float A, Fvector &P, Fvector &D, Fvector &U);
void CActor::OnRender	()
{
	if (!bDebug)				return;
	
	ph_Movement.dbg_Draw			();
	//if(g_Alive()>0)
	ph_Movement.dbg_Draw();
	CCameraBase* C				= cameras	[cam_active];
	dbg_draw_frustum			(C->f_fov, 230.f, 1.f, C->vPosition, C->vDirection, C->vNormal);
}
#endif
