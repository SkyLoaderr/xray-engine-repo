// Actor.cpp: implementation of the CActor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\xr_area.h"
#include "..\bodyinstance.h"
#include "..\physics\PhysicsObject.h"
#include "..\effectorfall.h"
#include "..\CameraLook.h"
#include "..\CameraFirstEye.h"
#include "..\xr_level_controller.h"
#include "..\fmesh.h"
#include "customitem.h"
#include "hudmanager.h"
#include "Actor_Flags.h"
#include "UI.h"

const DWORD	NET_Latency	= 100;

// breakpoints
#include "..\xr_input.h"

//
#include "Actor.h"
#include "ActorAnimation.h"
#include "xr_weapon_list.h"

#define MIN_CRASH_SPEED	16.0f
#define MAX_CRASH_SPEED	30.0f

#define X_SIZE_2 0.35f
#define Y_SIZE_2 0.8f
#define Z_SIZE_2 0.35f
#define Y_SIZE_4 Y_SIZE_2*.5f

static Fbox		bbStandBox;
static Fbox		bbCrouchBox;
static Fvector	vFootCenter;
static Fvector	vFootExt;

DWORD psActorFlags=0;

//--------------------------------------------------------------------
void __stdcall CActor::SpinCallback(CBoneInstance* B)
{
	CActor*	A			= (CActor*)B->Callback_Param;

	Fmatrix				spin;
	float				bone_yaw	= A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta;
	float				bone_pitch	= A->r_torso.pitch;
	clamp				(bone_pitch,-PI_DIV_8,PI_DIV_4);
	spin.setXYZ			(bone_yaw,bone_pitch,0);
	B->mTransform.mul_43(spin);
}

void CActor::net_Export(NET_Packet* P)					// export to server
{
	// export 
	R_ASSERT			(net_Local);
	VERIFY				(Weapons);

	u8					flags=0;
	if (Weapons->isWorking())	flags|=MF_FIREPARAMS;

	P->w_u32			(Level().timeServer());
	P->w_u8				(flags);
	P->w_vec3			(vPosition);
	P->w_u8				(u8(mstate_real));
	P->w_angle8			(r_model_yaw);
	P->w_angle8			(r_torso.yaw);
	P->w_angle8			(r_torso.pitch);
	P->w_sdir			(NET_SavedAccel);
	P->w_sdir			(Movement.GetVelocity());

	if (flags&MF_FIREPARAMS)
	{
		Fvector			pos,dir;
		g_fireParams	(pos,dir);
		P->w_vec3		(pos);
		P->w_dir		(dir);
	}
}

void CActor::net_Import(NET_Packet* P)					// import from server
{
	// import
	R_ASSERT		(!net_Local);
	net_update		N;

	N.dwTimeStamp	= P->r_u32		();
	u8	flags		= P->r_u8		();
	N.p_pos			= P->r_vec3		();
	N.mstate		= P->r_u8		();
	N.o_model		= P->r_angle8	();
	N.o_torso.yaw	= P->r_angle8	();
	N.o_torso.pitch	= P->r_angle8	();
	N.p_accel		= P->r_sdir		();
	N.p_velocity	= P->r_sdir		();

	if (flags&MF_FIREPARAMS)
	{
		N.f_pos		= P->r_vec3		();
		N.f_dir		= P->r_dir		();
	}

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	bVisible						= TRUE;
	bEnabled						= TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CActor::CActor() : CEntity()
{
	bbStandBox.set			(-X_SIZE_2,0,-Z_SIZE_2, X_SIZE_2,Y_SIZE_2*2,Z_SIZE_2);
	bbCrouchBox.set			(-X_SIZE_2,0,-Z_SIZE_2, X_SIZE_2,Y_SIZE_2,Z_SIZE_2);
	Fvector sh; sh.set		(X_SIZE_2*0.25f,0.f,Z_SIZE_2*0.25f);
	Fbox bbFootBox; Fvector vfC,vfE;
	bbFootBox.set			(-X_SIZE_2,	-0.1f, -Z_SIZE_2, X_SIZE_2, Y_SIZE_2*.4f, Z_SIZE_2);

	// уменьшим размер ступней что-бы чуть зависать в воздухе при запрыгивании на преп€тствие
	bbFootBox.shrink		(sh); 
	bbFootBox.getcenter		(vfC);
	bbFootBox.getradius		(vfE);

	// Movement
	Movement.SetParent		(this);
	Movement.SetBox			(bbStandBox);
	Movement.SetCrashSpeeds	(MIN_CRASH_SPEED,MAX_CRASH_SPEED);
	Movement.SetFoots		(vfC,vfE);
	Movement.SetMass		(120);

	// Cameras
	cameras[eacFirstEye]	= new CCameraFirstEye	(this, pSettings, "actor_firsteye_cam", false);
	cameras[eacLookAt]		= new CCameraLook		(this, pSettings, "actor_look_cam",		false);
	cameras[eacFreeLook]	= new CCameraLook		(this, pSettings, "actor_free_cam",	false);

	cam_active				= eacFirstEye;
	cam_BobCycle			= 0;
	fPrevCamPos				= 0;

	// 
	Weapons					= 0;

	r_torso.yaw				= 0;
	r_torso.pitch			= 0;
	r_model_yaw				= 0;
	r_model_yaw_delta		= 0;
	r_model_yaw_dest		= 0;
//	Device.seqRender.Add(this,REG_PRIORITY_LOW-1111);

//	Log("ACTOR-LOAD");
}

CActor::~CActor()
{
//	Log("ACTOR-UNLOAD");

//	Device.seqRender.Remove(this);
	_DELETE(Weapons);
	for (int i=0; i<eacMaxCam; i++) _DELETE(cameras[i]);

	// sounds
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
	pSounds->Delete3D(sndStep);
	pSounds->Delete3D(sndRespawn);
	pSounds->Delete3D(sndWeaponChange);
}

void CActor::Load(CInifile* ini, const char* section )
{
	Msg("Loading actor: %s",section);

	inherited::Load(ini,section);

	m_fWalkAccel		= ini->ReadFLOAT(section,"walk_accel");	
	m_fJumpSpeed		= ini->ReadFLOAT(section,"jump_speed");
	m_fRunCoef			= ini->ReadFLOAT(section,"run_coef");

	float af, gf, wf;
	af					= ini->ReadFLOAT(section,"air_friction");
	gf					= ini->ReadFLOAT(section,"ground_friction");
	wf					= ini->ReadFLOAT(section,"wall_friction");
	Movement.SetFriction(af,wf,gf);

	R_ASSERT			(pVisual->Type==MT_SKELETON);

	Weapons				= new CWeaponList(this);
	Weapons->Init		();
//	Weapons->TakeItem	(CLSID_OBJECT_W_M134,0);
	Weapons->TakeItem	(CLSID_OBJECT_W_GROZA,0);
//	Weapons->TakeItem	(CLSID_OBJECT_W_EMSRIFLE,0);

	// sounds
	char buf[256];
	pSounds->Create3D	(sndStep,			strconcat(buf,cName(),"\\step"));
	pSounds->Create3D	(sndWeaponChange,	strconcat(buf,cName(),"\\weaponchange"));
	pSounds->Create3D	(sndRespawn,		strconcat(buf,cName(),"\\respawn"));
	pSounds->Create3D	(sndHit[0],			strconcat(buf,cName(),"\\bhit_flesh-1"));
	pSounds->Create3D	(sndHit[1],			strconcat(buf,cName(),"\\bhit_flesh-2"));
	pSounds->Create3D	(sndHit[2],			strconcat(buf,cName(),"\\bhit_flesh-3"));
	pSounds->Create3D	(sndHit[3],			strconcat(buf,cName(),"\\bhit_helmet-1"));
	pSounds->Create3D	(sndHit[4],			strconcat(buf,cName(),"\\bullet_hit1"));
	pSounds->Create3D	(sndHit[5],			strconcat(buf,cName(),"\\bullet_hit2"));
	pSounds->Create3D	(sndHit[6],			strconcat(buf,cName(),"\\ric_conc-1"));
	pSounds->Create3D	(sndHit[7],			strconcat(buf,cName(),"\\ric_conc-2"));
	pSounds->Create3D	(sndDie[0],			strconcat(buf,cName(),"\\die0"));
	pSounds->Create3D	(sndDie[1],			strconcat(buf,cName(),"\\die1"));
	pSounds->Create3D	(sndDie[2],			strconcat(buf,cName(),"\\die2"));
	pSounds->Create3D	(sndDie[3],			strconcat(buf,cName(),"\\die3"));

	// take index spine bone
	int spine_bone		= PKinematics(pVisual)->LL_BoneID("bip01_spine2");
	PKinematics(pVisual)->LL_GetInstance(spine_bone).set_callback(SpinCallback,this);

	Movement.SetBox		(bbStandBox);
	
	cam_Set				(eacFirstEye);

	// motions
	PKinematics	V		= PKinematics(pVisual);
	m_current_legs_blend= 0;	
	m_current_jump_blend= 0;
	m_current_legs		= 0;
	m_current_torso		= 0;
	m_normal.Create		(V,"norm");
	m_crouch.Create		(V,"cr");

	// sheduler
	dwMinUpdate			= dwMaxUpdate = 1;
}

BOOL CActor::Spawn		( BOOL bLocal, int sid, int team, int squad, int group, Fvector4& o_pos )
{
	if (bLocal==FALSE)	Msg("**** spawn");
	if (!inherited::Spawn(bLocal,sid,team,squad,group,o_pos))	return FALSE;
	r_model_yaw			= o_pos.w;
	cameras[cam_active]->Set(o_pos.w,0,0);		// set's camera orientation

	bAlive				= TRUE;
	bEnabled			= bLocal?TRUE:FALSE;
	
	Weapons->Reset		();
	
	// *** movement state - respawn
	mstate_wishful		= 0;
	mstate_real			= 0;
	m_bJumping			= false;
	
	NET_SavedAccel.set	(0,0,0);
	NET_WasInterpolating= TRUE;
	return				TRUE;
}

BOOL CActor::Hit(int iLost, Fvector &dir, CEntity* who)
{
	if (!bAlive) return FALSE;

	if (psActorFlags&AF_GODMODE) return FALSE;
	else return inherited::Hit(iLost,dir,who);
}

void CActor::HitSignal(int perc, Fvector& vLocalDir, CEntity* who)
{
	sound3D& S = sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback) return;

	// Play hit-sound
	pSounds->Play3DAtPos(S,vPosition);

	// hit marker
	if (net_Local && (who!=this))	
	{
		int id = -1;
		float x = fabsf(vLocalDir.x);
		float z = fabsf(vLocalDir.z);
		if (z>x)	id = (vLocalDir.z<0)?2:0;
		else		id = (vLocalDir.x<0)?3:1;
		Level().HUD()->Hit(id);
	}
}

void CActor::Die	( )
{
	// Play sound
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
	cam_Set		(eacFreeLook);
	g_fireEnd	();
	bAlive	= FALSE;
	mstate_wishful	&= ~mcAnyMove;
	mstate_real		&= ~mcAnyMove;
}

BOOL CActor::TakeItem		( DWORD CID )
{
	if (!bAlive) return FALSE;

	CCustomItem* O = (CCustomItem*) pCreator->Objects.GetObjectByCID(CID);
	int iValue = O->iValue;
	switch (O->clsid_target) 
	{
	case CLSID_OBJECT_W_RAIL:
	case CLSID_OBJECT_A_RAIL:
		return Weapons->TakeItem(O->clsid_target,iValue);
		break;
	case CLSID_OBJECT_HEALTH:
		if (iHealth<iMAX_Health)
		{
			iHealth += iValue;
			if (iHealth>iMAX_Health) iHealth = iMAX_Health;
		}else return false;
		break;
	case CLSID_OBJECT_ARMOR:
		if (iArmor<iMAX_Armor)
		{
			iArmor += iValue;
			if (iArmor>iMAX_Armor) iArmor = iMAX_Armor;
		}else return false;
		break;
	default:
		return false;
	}
	return true;
}

void CActor::g_Physics(Fvector& accel, float jump, float dt)
{
	// Calculate physics
	Movement.SetPosition	(vPosition);
	Movement.Calculate		(accel,0,jump,dt,false);
	Movement.GetPosition	(vPosition);
	
	// Test nearest object
	Fvector C; float R;		Movement.GetBoundingSphere	(C,R);
	Level().ObjectSpace.TestNearestObject				(cfModel, C, R);

	// Check ground-contact
	if (net_Local && Movement.gcontact_Was && Movement.gcontact_HealthLost) 
	{
		pCreator->Cameras.AddEffector		(new CEffectorFall(Movement.gcontact_Power));
		Fvector D; D.set	(0,1,0);
		if (Movement.gcontact_HealthLost)	Hit(int(Movement.gcontact_HealthLost),D,this);
	}
}

void CActor::net_update::lerp(CActor::net_update& A, CActor::net_update& B, float f)
{
	// 
	o_model			= CEntity::u_lerp_angle	(A.o_model,B.o_model,		f);
	o_torso.yaw		= CEntity::u_lerp_angle	(A.o_torso.yaw,B.o_torso.yaw,f);
	o_torso.pitch	= CEntity::u_lerp_angle	(A.o_torso.pitch,B.o_torso.pitch,f);
	p_pos.lerp		(A.p_pos,B.p_pos,f);
	p_accel			= (f<0.5f)?A.p_accel:B.p_accel;
	p_velocity.lerp	(A.p_velocity,B.p_velocity,f);
	f_pos.lerp		(A.f_pos,B.f_pos,f);
	f_dir.lerp		(A.f_dir,B.f_dir,f);	f_dir.normalize_safe();
	mstate			= (f<0.5f)?A.mstate:B.mstate;
}

void CActor::Update	(DWORD DT)
{
	if (pInput && pInput->iGetAsyncKeyState(DIK_RSHIFT)){ 
		Die();
//		__asm	int 3;
	}

	if (!bEnabled)	return;

	float	dt		= float(DT)/1000.f;

	// Check controls, create accel, prelimitary setup "mstate_real"
	float	Jump	= 0;
	if (Local())	{
		g_cl_CheckControls	(mstate_wishful,NET_SavedAccel,Jump,dt);
		g_cl_Orientate		(mstate_real,dt);
		g_Orientate			(mstate_real,dt);
		g_Physics			(NET_SavedAccel,Jump,dt);
		g_cl_ValidateMState	(mstate_wishful);
		g_SetAnimation		(mstate_real);
	} else {
		// distinguish interpolation/extrapolation
		DWORD	dwTime		= Level().timeServer()-NET_Latency;
		net_update&	N		= NET.back();
		if ((dwTime > N.dwTimeStamp) || (NET.size()<2))
		{
			// BAD.	extrapolation
			DWORD	delta				= dwTime-N.dwTimeStamp;
			if (delta>500)				return;	// no extrapolation more than 500 msec

			if (NET_WasInterpolating)
			{
				NET_WasInterpolating	= FALSE;
				NET_Last				= N;

				// Setup last known data
				Movement.SetVelocity	(NET_Last.p_velocity);
				vPosition.set			(NET_Last.p_pos);
				UpdateTransform			();

				if (delta)				dt = float(delta)/1000.f;
				else					dt = EPS_S;
			}

			g_sv_Orientate				(NET_Last.mstate,dt);
			g_Orientate					(NET_Last.mstate,dt);
			g_Physics					(NET_Last.p_accel,Jump,dt);
			g_SetAnimation				(NET_Last.mstate);

		} else {
			// OK.	interpolation

			// Search 2 keyframes for interpolation
			int select		= -1;
			for (DWORD id=0; id<NET.size()-1; id++)
			{
				if ((NET[id].dwTimeStamp<=dwTime)&&(dwTime<=NET[id+1].dwTimeStamp))	select=id;
			}
			if (select>=0)	
			{
//				R_ASSERT		(select>=0);
				if (select>0)	NET.erase(NET.begin(),NET.begin()+select);
				
				// Interpolate state
				net_update&	A	= NET[0];
				net_update&	B	= NET[1];
				DWORD	d1		= dwTime-A.dwTimeStamp;
				DWORD	d2		= B.dwTimeStamp - A.dwTimeStamp;
				float	factor	= (float(d1)/float(d2));
				NET_Last.lerp	(A,B,factor);
				
				// Use interpolated state
				g_sv_Orientate	(NET_Last.mstate,dt);
				g_Orientate		(NET_Last.mstate,dt);
				vPosition.set	(NET_Last.p_pos);		// physics :)
				g_SetAnimation	(NET_Last.mstate);
				
				// Signal, that last time we used interpolation
				NET_WasInterpolating	= TRUE;
				NET_Time				= dwTime;
			}
		}
	}

	// generic stuff
	UpdateTransform			();
	CObject::Update			(DT);
	
	if (IsMyCamera())		cam_Update	(dt);

	bVisible				= !HUDview	();

	Weapons->Update			(dt,HUDview());
}

void CActor::OnMoveVisible()
{
	inherited::OnMoveVisible();

	Weapons->OnRender(HUDview());
}

void CActor::g_cl_ValidateMState(DWORD mstate_wf)
{
	// изменилось состо€ние
	if (mstate_wf != mstate_real)
	{
		// могу ли € встать
		if ((mstate_real&mcCrouch)&&(0==(mstate_wf&mcCrouch)))
		{
			// can we change size to "bbStandBox"
			Fvector				start_pos;
			bbStandBox.getcenter(start_pos);
			start_pos.add		(vPosition);
			if (!pCreator->ObjectSpace.EllipsoidCollide(cfModel,svTransform,start_pos,bbStandBox))
			{
				mstate_real &= ~mcCrouch;
				Movement.SetBox(bbStandBox);
			}
		}
	}
	// закончить прыжок
	if (mstate_real&mcJump)
		if (Movement.gcontact_Was)	mstate_real &= ~mcJump;
	if ((mstate_wf&mcJump)==0)	m_bJumping = false;

	// «ажало-ли мен€/уперс€ - не двигаюсь
	if (Movement.GetVelocityActual()<0.2f || Movement.bSleep) 
	{
		mstate_real &= ~mcAnyMove;
	}
}

void CActor::g_cl_CheckControls(DWORD mstate_wf, Fvector &vControlAccel, float &Jump, float dt)
{
	// ****************************** Check keyboard input and control acceleration
	vControlAccel.set	(0,0,0);
	if (Movement.Environment()==CMovementControl::peOnGround)
	{
		// jump
		if (((mstate_real&mcJump)==0) && (mstate_wf&mcJump) && !m_bJumping){
			m_bJumping = true;
			Jump = (mstate_wf&mcCrouch)?m_fJumpSpeed*.8f:m_fJumpSpeed;
			mstate_real |= mcJump;
		}

		// crouch
		if ((0==(mstate_real&mcCrouch))&&(mstate_wf&mcCrouch))
		{
			mstate_real |= mcCrouch;
			Movement.SetBox(bbCrouchBox);
		}
		
		// mask input into "real" state
		DWORD move	= mcAnyMove|mcAccel;
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
				if (bAccelerated)			scale *= m_fRunCoef;
				if (mstate_real&mcCrouch)	scale *= 0.2f;
				vControlAccel.mul			(scale);
			} else {
				mstate_real	&= ~mcAnyMove;
			}
		}
	}
	else
	{
		mstate_real	&= ~mcAnyMove;
	}

	// transform local dir to world dir
	Fmatrix				mOrient;
	mOrient.rotateY		(r_model_yaw);
	mOrient.transform_dir(vControlAccel);
}

void CActor::g_Orientate	(DWORD mstate_rl, float dt)
{
	// visual effect of "fwd+strafe" like motion
	float calc_yaw = 0;
	switch(mstate_rl&mcAnyMove)
	{
	case mcFwd+mcLStrafe:
	case mcBack+mcRStrafe:
		calc_yaw = +PI_DIV_4; break;
	case mcFwd+mcRStrafe:
	case mcBack+mcLStrafe:
		calc_yaw = -PI_DIV_4; break;
	}
	
	// lerp angle for "effect" and capture torso data from camera
	u_lerp_angle		(r_model_yaw_delta,calc_yaw,PI_MUL_2,dt);

	// build matrix
	mRotate.rotateY		(r_model_yaw + r_model_yaw_delta);
}

// ****************************** Update actor orientation according to camera orientation
void CActor::g_cl_Orientate	(DWORD mstate_rl, float dt)
{
	// capture camera into torso (only for FirstEye & LookAt cameras)
	if (cam_active!=eacFreeLook){
		r_torso.yaw		=	cameras[cam_active]->GetWorldYaw	();
		r_torso.pitch	=	cameras[cam_active]->GetWorldPitch	();
	}

	// если хоть что-то нажато - выровн€ть модель по камере
	if (mstate_rl&mcAnyMove)	{
		r_model_yaw		= normalize_angle(r_torso.yaw);
		mstate_real		&=~mcTurn;
	} else {
		// if camera rotated more than 45 degrees - align model with it
		float ty = normalize_angle(r_torso.yaw);
		if (_abs(r_model_yaw-ty)>PI_DIV_4)	{
			r_model_yaw_dest = ty;
			// 
			mstate_real	|= mcTurn;
		}
		if (_abs(r_model_yaw-r_model_yaw_dest)<EPS_L)
			mstate_real	&=~mcTurn;

		if (mstate_rl&mcTurn)
			u_lerp_angle	(r_model_yaw,r_model_yaw_dest,PI_MUL_4,dt);
	}
}

void CActor::g_sv_Orientate(DWORD mstate_rl, float dt)
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
		} else Weapons->GetFireParams(fire_pos, fire_dir);
	} else {
		fire_pos	= NET_Last.f_pos;
		fire_dir	= NET_Last.f_dir;
	}
}

void CActor::g_cl_fireStart	( )
{
	VERIFY	(Local());
	Weapons->FireStart	( );
	
	NET_Packet	P;
	P.w_begin	(M_FIRE_BEGIN);
	P.w_u8		(u8(net_ID));

	Fvector		pos,dir;
	g_fireParams(pos,dir);
	P.w_vec3	(pos);
	P.w_dir		(dir);

	Level().Send(P,net_flags(TRUE));
}

void CActor::g_sv_fireStart	(NET_Packet* P)
{
	VERIFY	(Remote());

	// Correct last packet if available
	if (!NET.empty())	
	{
		NET.back().f_pos	= P->r_vec3		();
		NET.back().f_dir	= P->r_dir		();
	}

	Weapons->FireStart	( );
}

void CActor::g_fireEnd	( )
{
	if (Local())	{
		NET_Packet	P;
		P.w_begin	(M_FIRE_END);
		P.w_u8		(u8(net_ID));
		Level().Send(P,net_flags(TRUE));
	}
	Weapons->FireEnd	( );
}

void CActor::Statistic()
{
	//-------------------------------------------------------------------
	pApp->pFont->OutSet(0,0);
	switch(Movement.Environment()){
	case CMovementControl::peOnGround:	pApp->pFont->OutNext("Environment: Ground");	break;
	case CMovementControl::peInAir:		pApp->pFont->OutNext("Environment: Air");		break;
	case CMovementControl::peAtWall:	pApp->pFont->OutNext("Environment: At Wall");	break;
	default:							pApp->pFont->OutNext("Environment: ???");	
	}
	
	pApp->pFont->OutNext("Speed:    %f,%f,%f",VPUSH(Movement.GetVelocity()));
	pApp->pFont->OutNext("Position: %f,%f,%f",VPUSH(vPosition));
	pApp->pFont->OutNext("CamPos:   %f,%f,%f",VPUSH(Device.vCameraPosition));
	pApp->pFont->OutNext("Sleep?:   %s",Movement.bSleep?"TRUE":"FALSE");
	//-------------------------------------------------------------------
}

void CActor::OnRender()
{
	Movement.dbg_Draw();
	PKinematics(pVisual)->DebugRender(svTransform);
}

// HUD
void CActor::OnHUDDraw(CCustomHUD* hud)
{
	if (HUDview())					Weapons->OnRender(HUDview());

	CHUDManager* HUD = (CHUDManager*)hud;
	CUI* pUI=HUD->GetUI();
	pUI->OutHealth(iHealth,iArmor);
	pUI->OutWeapon(Weapons->ActiveWeaponName(),Weapons->ActiveWeaponAmmoElapsed(),Weapons->ActiveWeaponAmmoLimit(),0,0);
	pUI->SetHeading(r_torso.yaw);
	
	char buf[128];
	buf[0] = 0;
	if (mstate_real&mcAccel)	strcat(buf,"Accel ");
	if (mstate_real&mcCrouch)	strcat(buf,"Crouch ");
	if (mstate_real&mcFwd)		strcat(buf,"Fwd ");
	if (mstate_real&mcBack)		strcat(buf,"Back ");
	if (mstate_real&mcLStrafe)	strcat(buf,"LStrafe ");
	if (mstate_real&mcRStrafe)	strcat(buf,"RStrafe ");
	if (mstate_real&mcJump)		strcat(buf,"Jump ");
	if (mstate_real&mcTurn)		strcat(buf,"Turn ");
	if (m_bJumping)				strcat(buf,"+Jumping ");

	HUD->pHUDFont->Out(400,320,buf);
}

void CActor::g_wpn_Set(DWORD id)
{
	switch (id)
	{
	case 1: Weapons->ActivateWeapon(CLSID_OBJECT_W_M134);		break; 
	case 2: Weapons->ActivateWeapon(CLSID_OBJECT_W_GROZA);		break; 
	case 3: Weapons->ActivateWeapon(CLSID_OBJECT_W_PROTECTA);	break; 
	}
	
	if (Local()) {
		NET_Packet	P;
		P.w_begin	(M_WPN_SET);
		P.w_u8		(u8(net_ID));
		P.w_u8		(u8(id));
		Level().Send(P,net_flags(TRUE));
	}
}
