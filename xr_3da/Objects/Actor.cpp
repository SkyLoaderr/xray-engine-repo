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

const DWORD    patch_frames = 50;

// breakpoints
#include "..\xr_input.h"

//
#include "Actor.h"
#include "ActorAnimation.h"
#include "xr_weapon_list.h"

#define SND_STEP_TIME	1.f

static Fbox		bbStandBox;
static Fbox		bbCrouchBox;
static Fvector	vFootCenter;
static Fvector	vFootExt;

DWORD			psActorFlags=0;

//--------------------------------------------------------------------
void __stdcall CActor::SpinCallback(CBoneInstance* B)
{
	CActor*	A			= dynamic_cast<CActor*>(static_cast<CObject*>(B->Callback_Param));

	Fmatrix				spin;
	float				bone_yaw	= A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta;
	float				bone_pitch	= A->r_torso.pitch;
	// clamp				(bone_pitch,-PI_DIV_8,PI_DIV_4);
	spin.setXYZ			(bone_yaw,bone_pitch,0);
	B->mTransform.mulB_43(spin);
}

void CActor::net_Export	(NET_Packet& P)					// export to server
{
	// export 
	R_ASSERT			(net_Local);
	VERIFY				(Weapons);

	u8					flags=0;
	P.w_u32				(Level().timeServer());
	P.w_u8				(flags);
	P.w_vec3			(vPosition);
	P.w_u8				(u8(mstate_real));
	P.w_angle8			(r_model_yaw);
	P.w_angle8			(r_torso.yaw);
	P.w_angle8			(r_torso.pitch);
	P.w_sdir			(NET_SavedAccel);
	P.w_sdir			(Movement.GetVelocity());
}

void CActor::net_Import		(NET_Packet& P)					// import from server
{
	// import
	R_ASSERT		(!net_Local);
	net_update		N;

	u8	 flags, tmp;
	P.r_u32			(N.dwTimeStamp	);
	P.r_u8			(flags			);
	P.r_vec3		(N.p_pos		);
	P.r_u8			(tmp			); N.mstate = DWORD(tmp);
	P.r_angle8		(N.o_model		);
	P.r_angle8		(N.o_torso.yaw	);
	P.r_angle8		(N.o_torso.pitch);
	P.r_sdir		(N.p_accel		);
	P.r_sdir		(N.p_velocity	);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	setVisible					(TRUE);
	setEnabled					(TRUE);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CActor::CActor() : CEntityAlive()
{
	// Cameras
	cameras[eacFirstEye]	= new CCameraFirstEye	(this, pSettings, "actor_firsteye_cam", false);
	cameras[eacLookAt]		= new CCameraLook		(this, pSettings, "actor_look_cam",		false);
	cameras[eacFreeLook]	= new CCameraLook		(this, pSettings, "actor_free_cam",	false);

	cam_active				= eacFirstEye;
	fPrevCamPos				= 0;
	pCamBobbing				= 0;

	// 
	Weapons					= 0;

	r_torso.yaw				= 0;
	r_torso.pitch			= 0;
	r_model_yaw				= 0;
	r_model_yaw_delta		= 0;
	r_model_yaw_dest		= 0;
	
	m_fTimeToStep			= 0;
	bStep					= FALSE;

	b_DropActivated			= 0;

	m_fRunFactor			= 2.f;
	m_fCrouchFactor			= 0.2f;
}

CActor::~CActor()
{
	_DELETE(Weapons);
	for (int i=0; i<eacMaxCam; i++) _DELETE(cameras[i]);

	// sounds 2D
	pSounds->Delete(sndStep[0]);
	pSounds->Delete(sndStep[1]);
	pSounds->Delete(sndLanding);
	pSounds->Delete(sndZoneHeart);
	pSounds->Delete(sndZoneDetector);

	// sounds 3D
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete(sndDie[i]);
}

void CActor::Load		(LPCSTR section )
{
	Msg("Loading actor: %s",section);

	inherited::Load		(section);

	m_fWalkAccel		= pSettings->ReadFLOAT(section,"walk_accel");	
	m_fJumpSpeed		= pSettings->ReadFLOAT(section,"jump_speed");
	m_fRunFactor		= pSettings->ReadFLOAT(section,"run_coef");
	m_fCrouchFactor		= pSettings->ReadFLOAT(section,"crouch_coef");

	R_ASSERT			(pVisual->Type==MT_SKELETON);

	Weapons				= new CWeaponList(this);
	Weapons->Init		("bip01_r_hand","bip01_l_finger1");

	// sounds
	char buf[256];
	pSounds->Create		(sndStep[0],		FALSE,	strconcat(buf,cName(),"\\stepL"),0,SOUND_TYPE_MONSTER_WALKING_HUMAN);
	pSounds->Create		(sndStep[1],		FALSE,	strconcat(buf,cName(),"\\stepR"),0,SOUND_TYPE_MONSTER_WALKING_HUMAN);
	pSounds->Create		(sndLanding,		FALSE,	strconcat(buf,cName(),"\\landing"),0,SOUND_TYPE_MONSTER_FALLING_HUMAN);
	pSounds->Create		(sndZoneHeart,		FALSE,	"heart\\4");
	pSounds->Create		(sndZoneDetector,	FALSE,	"detectors\\geiger",	TRUE);
	pSounds->Create		(sndHit[0],			TRUE,	strconcat(buf,cName(),"\\hurt1"),0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create		(sndHit[1],			TRUE,	strconcat(buf,cName(),"\\hurt2"),0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create		(sndHit[2],			TRUE,	strconcat(buf,cName(),"\\hurt3"),0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create		(sndHit[3],			TRUE,	strconcat(buf,cName(),"\\hurt4"),0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create		(sndDie[0],			TRUE,	strconcat(buf,cName(),"\\die0"),0,SOUND_TYPE_MONSTER_DYING_HUMAN);
	pSounds->Create		(sndDie[1],			TRUE,	strconcat(buf,cName(),"\\die1"),0,SOUND_TYPE_MONSTER_DYING_HUMAN);
	pSounds->Create		(sndDie[2],			TRUE,	strconcat(buf,cName(),"\\die2"),0,SOUND_TYPE_MONSTER_DYING_HUMAN);
	pSounds->Create		(sndDie[3],			TRUE,	strconcat(buf,cName(),"\\die3"),0,SOUND_TYPE_MONSTER_DYING_HUMAN);

	// take index spine bone
	int spine_bone		= PKinematics(pVisual)->LL_BoneID("bip01_spine2");
	PKinematics(pVisual)->LL_GetInstance(spine_bone).set_callback(SpinCallback,this);

	Movement.ActivateBox(0);
	
	cam_Set				(eacFirstEye);

	// motions
	CKinematics* V		= PKinematics(pVisual);
	m_current_legs_blend= 0;
	m_current_jump_blend= 0;
	m_current_legs		= 0;
	m_current_torso		= 0;
	m_normal.Create		(V,"norm");
	m_crouch.Create		(V,"cr");

	// sheduler
	shedule_Min			= shedule_Max = 1;

	// patch : ZoneAreas
	if (Level().pLevel->SectionExists("zone_areas"))
	{
		Log("...Using zones...");
		CInifile::Sect&		S = Level().pLevel->ReadSection("zone_areas");
		for (CInifile::SectIt I = S.begin(); I!=S.end(); I++)
		{
			Fvector4 a;
			sscanf				(I->second,"%f,%f,%f,%f",&a.x,&a.y,&a.z,&a.w);
			zone_areas.push_back(a);
		}
	}
}

BOOL CActor::net_Spawn		(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	if (!inherited::net_Spawn(bLocal,server_id,o_pos,o_angle,P,flags))	return FALSE;
	r_model_yaw			= o_angle.y;
	cameras[cam_active]->Set(o_angle.y,0,0);		// set's camera orientation

	bAlive				= TRUE;
	
	Weapons->Reset		();
	
	// *** movement state - respawn
	mstate_wishful		= 0;
	mstate_real			= 0;
	m_bJumpKeyPressed	= FALSE;
	m_bJumpInProgress	= FALSE;

	// *** weapons
	if (Local()) 		Weapons->ActivateWeaponID	(0);
	
	NET_SavedAccel.set	(0,0,0);
	NET_WasInterpolating= TRUE;

	setEnabled			(bLocal);
	setActive			(TRUE);

	patch_frame			= 0;
	patch_position.set	(vPosition);

	Engine.Sheduler.Unregister	(this);
	Engine.Sheduler.Register	(this,TRUE);

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
	sound& S = sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback) return;

	// Play hit-sound
	pSounds->PlayAtPos(S,this,vPosition);

	// hit marker
	if (net_Local && (who!=this))	
	{
		Msg("%s",who->cName());
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
	pSounds->PlayAtPos(sndDie[Random.randI(SND_DIE_COUNT)],this,vPosition);
	cam_Set		(eacFreeLook);
	g_fireEnd	();
	bAlive		= FALSE;
	mstate_wishful	&= ~mcAnyMove;
	mstate_real		&=~ mcAnyMove;
}

void CActor::feel_touch_new				(CObject* O)
{
	NET_Packet	P;

	// Test for weapon
	CWeapon* W	= dynamic_cast<CWeapon*>	(O);
	if (W)
	{
		// Search if we have similar type of weapon
		CWeapon* T = Weapons->getWeaponByWeapon	(W);
		if (T)	
		{
			// We have similar weapon - just get ammo out of it
			P.w_begin	(M_EVENT);
			P.w_u32		(Level().timeServer());
			P.w_u16		(GE_TRANSFER_AMMO);
			P.w_u16		(ID());

			P.w_u16		(W->ID());
			P.w_u16		(T->ID());
			Level().Send(P,net_flags(TRUE,TRUE));
			return;
		} else {
			// We doesn't have similar weapon - pick up it
			NET_Packet	P;
			P.w_begin	(M_EVENT);
			P.w_u32		(Level().timeServer());
			P.w_u16		(GE_OWNERSHIP_TAKE);
			P.w_u16		(ID());

			P.w_u16		(u16(W->ID()));
			Level().Send(P,net_flags(TRUE,TRUE));
			return;
		}
	}

	// 
}

void CActor::feel_touch_delete		(CObject* O)
{
}

void CActor::g_Physics				(Fvector& accel, float jump, float dt)
{
	if (patch_frame<patch_frames)	return;

	// Calculate physics
	Movement.SetPosition	(vPosition);
	float step = 0.1f;
	while (dt>step)			{
		Movement.Calculate		(accel,0,jump,step,false);
		dt -= step;
	}
	if (dt>0)	{
		Movement.Calculate		(accel,0,jump,dt,false);
	}
	Movement.GetPosition	(vPosition);
	
	// Check ground-contact
	if (net_Local && Movement.gcontact_Was) 
	{
		pSounds->PlayAtPos					(sndLanding,this,Position());
		pCreator->Cameras.AddEffector		(new CEffectorFall(Movement.gcontact_Power));
		Fvector D; D.set					(0,1,0);
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
	mstate			= (f<0.5f)?A.mstate:B.mstate;
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
	if (0==sndZoneHeart.feedback)		pSounds->PlayAtPos	(sndZoneHeart,		this,Position(),true);
//	if (0==sndZoneDetector.feedback)	pSounds->PlayAtPos	(sndZoneDetector,	this,Position(),true);
	sndZoneHeart.feedback->SetVolume			(z_amount);
	sndZoneHeart.feedback->SetPosition			(P);
//	sndZoneDetector.feedback->SetFrequencyScale	(.1f+z_amount);
//	sndZoneDetector.feedback->SetPosition		(P);
}

void CActor::Update	(DWORD DT)
{
	if (!getEnabled())	return;

	// patch
	if (patch_frame<patch_frames)	{
		vPosition.set		(patch_position);
		patch_frame			+= 1;
	}

	// zone test
	float z_amount		= 0;
	for (u32 za=0; za<zone_areas.size(); za++)
	{
		Fvector	P; 
		P.set			(zone_areas[za].x,zone_areas[za].y,zone_areas[za].z);
		float D			= 1-(Position().distance_to(P)/zone_areas[za].w);
		z_amount		= _max(D,z_amount);
	}

	if (z_amount>EPS)	ZoneEffect	(z_amount);
	else				{
		cam_shift		= 0.f;
		cam_gray		= 0.f;
		if (sndZoneHeart.feedback)		sndZoneHeart.feedback->Stop		();
		if (sndZoneDetector.feedback)	sndZoneDetector.feedback->Stop	();
	}
	cam_shift				= 0.f;
	cam_gray				= 0.f;

	// 
	clamp					(DT,0ul,100ul);
	float	dt				= float(DT)/1000.f;
	
	// Check controls, create accel, prelimitary setup "mstate_real"
	float	Jump	= 0;
	if (Local())	{
		g_cl_CheckControls		(mstate_wishful,NET_SavedAccel,Jump,dt);
		g_cl_Orientate			(mstate_real,dt);
		g_Orientate				(mstate_real,dt);
		g_Physics				(NET_SavedAccel,Jump,dt);
		Fvector C; float R;		Movement.GetBoundingSphere	(C,R);
		feel_touch_update		(C,R);
		g_cl_ValidateMState		(mstate_wishful);
		g_SetAnimation			(mstate_real);
		
		// Dropping
		if (b_DropActivated)	{
			f_DropPower			+= dt*0.1f;
		} else {
			f_DropPower			= 0.f;
		}
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
				
				// Change weapon if needed
				if (Weapons->ActiveWeaponID()!=NET_Last.weapon)
					Weapons->ActivateWeaponID(NET_Last.weapon);

				// Signal, that last time we used interpolation
				NET_WasInterpolating	= TRUE;
				NET_Time				= dwTime;
			}
		}
	}

	// generic stuff
	UpdateTransform			();
	inherited::Update		(DT);
	
	if (IsMyCamera())		{
		if (!pCamBobbing){ 
			pCamBobbing = new CEffectorBobbing	();
			Level().Cameras.AddEffector			(pCamBobbing);
		}
		pCamBobbing->SetState					(mstate_real);
		cam_Update								(dt,Weapons->getZoomFactor());
	} else {
		if (pCamBobbing)						{ Level().Cameras.RemoveEffector(cefBobbing); pCamBobbing=0; }
	}

	setVisible				(!HUDview	());

	Weapons->Update			(dt,HUDview());

	// sound step
	if (mstate_real&mcAnyMove){
		if(m_fTimeToStep<0){
			pSounds->PlayAtPos	(sndStep[bStep],this,Position());
			bStep = !bStep;
			float k				= (mstate_real&mcCrouch)?0.75f:1.f;
			float tm			= isAccelerated(mstate_real)?(PI/(k*10.f)):(PI/(k*7.f));
			m_fTimeToStep		= tm;
		}
		m_fTimeToStep -= dt;
	}
}

void CActor::OnVisible()
{
	inherited::OnVisible	();

	CWeapon* W				= Weapons->ActiveWeapon();
	if (W)					W->OnVisible		();
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
				Movement.ActivateBox(0);
			}
		}
	}
	// закончить прыжок
	if (mstate_real&mcJump)
		if (Movement.gcontact_Was)	{
			m_bJumpInProgress	=	FALSE;
			mstate_real			&= ~mcJump;
		}
	if ((mstate_wf&mcJump)==0)	m_bJumpKeyPressed = FALSE;

	// «ажало-ли мен€/уперс€ - не двигаюсь
	if (((Movement.GetVelocityActual()<0.2f)&&(!(mstate_real&mcJump))) || Movement.bSleep) 
	{
		mstate_real				&=~ mcAnyMove;
	}
}

void CActor::g_cl_CheckControls(DWORD mstate_wf, Fvector &vControlAccel, float &Jump, float dt)
{
	// ****************************** Check keyboard input and control acceleration
	vControlAccel.set	(0,0,0);

	if (m_bJumpInProgress && (Movement.Environment()==CMovementControl::peInAir)) 
	{
		mstate_real			|=	mcJump;
		m_bJumpInProgress	=	FALSE;
	}

	if (Movement.Environment()==CMovementControl::peOnGround)
	{
		// jump
		if (((mstate_real&mcJump)==0) && (mstate_wf&mcJump) && !m_bJumpKeyPressed)
		{
			m_bJumpKeyPressed	=	TRUE;
			m_bJumpInProgress	=	TRUE;
			Jump = (mstate_wf&mcCrouch)?m_fJumpSpeed*.8f:m_fJumpSpeed;
		}

		// crouch
		if ((0==(mstate_real&mcCrouch))&&(mstate_wf&mcCrouch))
		{
			mstate_real |= mcCrouch;
			Movement.ActivateBox(1);
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
				if (bAccelerated)			scale *= m_fRunFactor;
				if (mstate_real&mcCrouch)	scale *= m_fCrouchFactor;
				vControlAccel.mul			(scale);
			} else {
				mstate_real	&= ~mcAnyMove;
			}
		}
	}
	else
	{
		mstate_real			&=~ mcAnyMove;
	}

	// transform local dir to world dir
	Fmatrix				mOrient;
	mOrient.rotateY		(-r_model_yaw);
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
	mRotate.rotateY		(-(r_model_yaw + r_model_yaw_delta));
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
		Weapons->GetFireParams(fire_pos, fire_dir);
	}
}

void CActor::g_fireStart	( )
{
	Weapons->FireStart	( );
}
void CActor::g_fireEnd	( )
{
	Weapons->FireEnd	( );
}

void CActor::g_PerformDrop	( )
{
	VERIFY					(b_DropActivated);
	b_DropActivated			= FALSE;

	// We doesn't have similar weapon - pick up it
	NET_Packet		P;
	P.w_begin		(M_EVENT);
	P.w_u16			(u16(ID()));
	P.w_u16			(u16(W->ID()));
	Level().Send	(P,net_flags(TRUE,TRUE));
}

void CActor::g_WeaponBones	(int& L, int& R)
{
	VERIFY	(Weapons);
	L		=	Weapons->m_iACTboneL;
	R		=	Weapons->m_iACTboneR;
}

void CActor::Statistic	( )
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

// HUD
void CActor::OnHUDDraw	(CCustomHUD* hud)
{
	CHUDManager* HUD	= (CHUDManager*)hud;
	CUI* pUI=HUD->GetUI	();
	pUI->OutHealth		(iHealth,iArmor);
	pUI->OutWeapon		(Weapons->ActiveWeapon());
	pUI->SetHeading		(-r_torso.yaw);
	
	CWeapon* W			= Weapons->ActiveWeapon();
	if (W)				W->OnVisible		();

	/**
	char buf[128];
	sprintf(buf,"Position : %3.2f,%3.2f,%3.2f",VPUSH(vPosition));

	HUD->pHUDFont->Color(0xffffffff);
	HUD->pHUDFont->Out	(400,320,buf);
/*
	char buf[128];
	buf[0] = 0;
	CWeapon* W=Weapons->ActiveWeapon();
	if (W){
		float prec = W->GetPrecision();
		sprintf(buf,"Prec: %3.2f",prec);
		HUD->pHUDFont->Color(0xffffffff);
		HUD->pHUDFont->Out	(400,320,buf);
	}
	char buf[128];
	buf[0] = 0;
	switch (Movement.Environment())
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
	HUD->pHUDFont->Color(0xffffffff);
	HUD->pHUDFont->Out	(400,320,buf);
	HUD->pHUDFont->Out	(400,330,"Vel Actual:    %3.2f",Movement.GetVelocityActual());
	HUD->pHUDFont->Out	(400,340,"Vel:           %3.2f",Movement.GetVelocity());
	HUD->pHUDFont->Out	(400,350,"Vel Magnitude: %3.2f",Movement.GetVelocityMagnitude());
*/
}
