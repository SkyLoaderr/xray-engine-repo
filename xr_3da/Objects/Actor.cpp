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

u32			psActorFlags=0;

//--------------------------------------------------------------------
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
	f_DropPower				= 0.f;

	m_fRunFactor			= 2.f;
	m_fCrouchFactor			= 0.2f;

	m_fFallTime				= s_fFallTime;
	m_bAnimTorsoPlayed		= false;
}

CActor::~CActor()
{
	_DELETE(Weapons);
	for (int i=0; i<eacMaxCam; i++) _DELETE(cameras[i]);

	// sounds 2D
	::Sound->Delete(sndStep[0]);
	::Sound->Delete(sndStep[1]);
	::Sound->Delete(sndLanding);
	::Sound->Delete(sndZoneHeart);
	::Sound->Delete(sndZoneDetector);

	// sounds 3D
	for (i=0; i<SND_HIT_COUNT; i++) ::Sound->Delete(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) ::Sound->Delete(sndDie[i]);
}

void CActor::Load		(LPCSTR section )
{
	Msg("Loading actor: %s",section);

	inherited::Load		(section);

	m_fWalkAccel		= pSettings->ReadFLOAT(section,"walk_accel");	
	m_fJumpSpeed		= pSettings->ReadFLOAT(section,"jump_speed");
	m_fRunFactor		= pSettings->ReadFLOAT(section,"run_coef");
	m_fCrouchFactor		= pSettings->ReadFLOAT(section,"crouch_coef");

	Weapons				= new CWeaponList(this);

	// sounds
	char buf[256];
	::Sound->Create		(sndStep[0],		TRUE,	strconcat(buf,cName(),"\\stepL"),0,SOUND_TYPE_MONSTER_WALKING_HUMAN);
	::Sound->Create		(sndStep[1],		TRUE,	strconcat(buf,cName(),"\\stepR"),0,SOUND_TYPE_MONSTER_WALKING_HUMAN);
	::Sound->Create		(sndLanding,		TRUE,	strconcat(buf,cName(),"\\landing"),0,SOUND_TYPE_MONSTER_FALLING_HUMAN);
	::Sound->Create		(sndZoneHeart,		TRUE,	"heart\\4");
	::Sound->Create		(sndZoneDetector,	TRUE,	"detectors\\geiger",	TRUE);
	::Sound->Create		(sndHit[0],			TRUE,	strconcat(buf,cName(),"\\hurt1"),0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->Create		(sndHit[1],			TRUE,	strconcat(buf,cName(),"\\hurt2"),0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->Create		(sndHit[2],			TRUE,	strconcat(buf,cName(),"\\hurt3"),0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->Create		(sndHit[3],			TRUE,	strconcat(buf,cName(),"\\hurt4"),0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->Create		(sndDie[0],			TRUE,	strconcat(buf,cName(),"\\die0"),0,SOUND_TYPE_MONSTER_DYING_HUMAN);
	::Sound->Create		(sndDie[1],			TRUE,	strconcat(buf,cName(),"\\die1"),0,SOUND_TYPE_MONSTER_DYING_HUMAN);
	::Sound->Create		(sndDie[2],			TRUE,	strconcat(buf,cName(),"\\die2"),0,SOUND_TYPE_MONSTER_DYING_HUMAN);
	::Sound->Create		(sndDie[3],			TRUE,	strconcat(buf,cName(),"\\die3"),0,SOUND_TYPE_MONSTER_DYING_HUMAN);

	Movement.ActivateBox(0);
	
	cam_Set				(eacFirstEye);

	// motions
	m_current_legs_blend= 0;
	m_current_jump_blend= 0;
	m_current_legs		= 0;
	m_current_torso		= 0;

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

//--------------------------------------------------------------------
void CActor::net_Export	(NET_Packet& P)					// export to server
{
	// export 
	R_ASSERT			(Local());
	VERIFY				(Weapons);

	u8					flags=0;
	P.w_u32				(Level().timeServer());
	P.w_u8				(flags);
	P.w_vec3			(vPosition);
	P.w_u16				(u16(mstate_real));
	P.w_angle8			(r_model_yaw);
	P.w_angle8			(r_torso.yaw);
	P.w_angle8			(r_torso.pitch);
	P.w_sdir			(NET_SavedAccel);
	P.w_sdir			(Movement.GetVelocity());
	P.w_float_q16		(fHealth,-1000,1000);
	P.w_float_q16		(fArmor,-1000,1000);

	int w_id = Weapons->ActiveWeaponID	();
	if (w_id<0)			P.w_u8(0xff);
	else				P.w_u8(u8(w_id&0xff));
}

void CActor::net_Import		(NET_Packet& P)					// import from server
{
	// import
	R_ASSERT			(Remote());
	net_update			N;

	u8	flags;
	u16	tmp;
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
	xrSE_Actor*			E	= (xrSE_Actor*)DC;

	//
	r_model_yaw				= E->o_Angle.y;
	cameras[cam_active]->Set(E->o_Angle.y,0,0);		// set's camera orientation
	
	// *** movement state - respawn
	mstate_wishful			= 0;
	mstate_real				= 0;
	m_bJumpKeyPressed		= FALSE;

	// *** weapons
	if (Local()) 			Weapons->ActivateWeaponID	(0);
	
	NET_SavedAccel.set		(0,0,0);
	NET_WasInterpolating	= TRUE;

	setEnabled				(E->s_flags&M_SPAWN_OBJECT_LOCAL);
	setActive				(TRUE);

	patch_frame				= 0;
	patch_position.set		(vPosition);
	die_hide				= 1.f;

	Engine.Sheduler.Unregister	(this);
	Engine.Sheduler.Register	(this,TRUE);

	hit_slowmo				= 0.f;
	hit_factor				= 1.f;

	m_pArtifact				= 0;
	return					TRUE;
}

void CActor::net_Relcase	(CObject* O)
{
	inherited::net_Relcase	(O);
}

void CActor::Hit		(float iLost, Fvector &dir, CObject* who, s16 element)
{
	if (g_Alive()<=0) return;

	switch (GameID())
	{
	case GAME_SINGLE:		
		{
			if (psActorFlags&AF_GODMODE)	return;
			else inherited::Hit		(iLost,dir,who,element);
		}
		break;
	default:
		inherited::Hit	(iLost,dir,who,element);
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
		::Sound->PlayAtPos	(S,this,vPosition);

		// hit marker
		if (Local() && (who!=this))	
		{
			int id		= -1;
			float x		= _abs(vLocalDir.x);
			float z		= _abs(vLocalDir.z);
			if (z>x)	id = (vLocalDir.z<0)?2:0;
			else		id = (vLocalDir.x<0)?3:1;
			Level().HUD()->Hit(id);
		}

		// stop-motion
		if (Movement.Environment()==CMovementControl::peOnGround)
		{
			Fvector zeroV;
			zeroV.set			(0,0,0);
			Movement.SetVelocity(zeroV);
		}
		hit_slowmo				= perc/100.f;
		if (hit_slowmo>1.f)		hit_slowmo = 1.f;

		// check damage bone
	}
}

void CActor::Die	( )
{
	// Play sound
	::Sound->PlayAtPos		(sndDie[Random.randI(SND_DIE_COUNT)],this,vPosition);
	cam_Set					(eacFreeLook);
	g_fireEnd				();
	mstate_wishful	&=		~mcAnyMove;
	mstate_real		&=		~mcAnyMove;

	// Drop active weapon(s)
	/*
	if (Local())			{
		Weapons->weapon_die	();
	}
	*/

	die_hide				= 1.f;
}

void CActor::g_Physics			(Fvector& _accel, float jump, float dt)
{
	if (!g_Alive())					return;
	if (patch_frame<patch_frames)	return;

	// Correct accel
	Fvector		accel;
	accel.set					(_accel);
	hit_slowmo					-=	dt;
	if (hit_slowmo<0)			hit_slowmo = 0.f;
	accel.mul					(1.f-hit_slowmo);

	// Calculate physics
	Movement.SetPosition		(vPosition);
	Movement.Calculate			(accel,0,jump,dt,false);
	Movement.GetPosition		(vPosition);

	// Check ground-contact
	if (Movement.gcontact_Was) 
	{
		::Sound->PlayAtPos					(sndLanding,this,Position());

		Fvector correctV					= Movement.GetVelocity	();
		correctV.x							*= 0.1f;
		correctV.z							*= 0.1f;
		Movement.SetVelocity				(correctV);

		if (Local()) {
			pCreator->Cameras.AddEffector		(new CEffectorFall(Movement.gcontact_Power));
			Fvector D; D.set					(0,1,0);
			if (Movement.gcontact_HealthLost)	Hit	(1.5f * Movement.gcontact_HealthLost,D,this,-1);
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
	if (0==sndZoneHeart.feedback)		::Sound->PlayAtPos	(sndZoneHeart,		this,Position(),true);
//	if (0==sndZoneDetector.feedback)	::Sound->PlayAtPos	(sndZoneDetector,	this,Position(),true);
	sndZoneHeart.feedback->SetVolume			(z_amount);
	sndZoneHeart.feedback->SetPosition			(P);
//	sndZoneDetector.feedback->SetFrequencyScale	(.1f+z_amount);
//	sndZoneDetector.feedback->SetPosition		(P);
}

void CActor::UpdateCL()
{
	inherited::UpdateCL();

	// Analyze Die-State
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
}

void CActor::Update	(u32 DT)
{
	if (!getEnabled())	return;
	if (!Ready())		return;

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
		if (Local())
		{
			Fvector C; float R;		Movement.GetBoundingSphere	(C,R);
			feel_touch_update		(C,R);
		}
		
		// Dropping
		if (b_DropActivated)	{
			f_DropPower			+= dt*0.1f;
			clamp				(f_DropPower,0.f,1.f);
		} else {
			f_DropPower			= 0.f;
		}
	} else {
		// distinguish interpolation/extrapolation
		u32	dwTime			= Level().timeServer()-NET_Latency;
		net_update&	N		= NET.back();
		if ((dwTime > N.dwTimeStamp) || (NET.size()<2))
		{
			// BAD.	extrapolation
			// u32	delta				= dwTime-N.dwTimeStamp;

			if (NET_WasInterpolating)
			{
				NET_WasInterpolating	= FALSE;
				NET_Last				= N;

				// Setup last known data
				Movement.SetVelocity	(NET_Last.p_velocity);
				vPosition.set			(NET_Last.p_pos);
				UpdateTransform			();

				/*
				if (delta)				dt = float(delta)/1000.f;
				else					dt = EPS_S;
				*/
			}

			g_sv_Orientate				(NET_Last.mstate,dt);
			g_Orientate					(NET_Last.mstate,dt);
			g_Physics					(NET_Last.p_accel,Jump,dt);
			g_SetAnimation				(NET_Last.mstate);
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
//				R_ASSERT		(select>=0);
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
				g_SetAnimation	(NET_Last.mstate);
				
				// Change weapon if needed
				if (Weapons->ActiveWeaponID()!=NET_Last.weapon)
				{
					Weapons->ActivateWeaponID(NET_Last.weapon);
				}

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
	if ((mstate_real&mcAnyMove)&&(!(mstate_real&(mcJump|mcFall|mcLanding|mcLanding2)))){
		if(m_fTimeToStep<0){
			bStep				= !bStep;
			float k				= (mstate_real&mcCrouch)?0.75f:1.f;
			float tm			= isAccelerated(mstate_real)?(PI/(k*10.f)):(PI/(k*7.f));
			m_fTimeToStep		= tm;
			::Sound->PlayAtPos	(sndStep[bStep],this,Position());
		}
		m_fTimeToStep -= dt;
	}

	// sounds update
	float	s_k			=	(mstate_real&mcCrouch)?0.75f:1.f;
	float	s_vol		=	s_k * (isAccelerated(mstate_real)?1.f:.75f);
	Fvector	s_pos		=	Position	();
	s_pos.y				+=	.5f;
	if (sndStep[0].feedback)		{
		sndStep[0].feedback->SetPosition(Position());
		sndStep[0].feedback->SetVolume	(s_vol);
	}
	if (sndStep[1].feedback)		{
		sndStep[1].feedback->SetPosition(Position());
		sndStep[1].feedback->SetVolume	(s_vol);
	}
}

void CActor::OnVisible	()
{
	inherited::OnVisible	();

	CWeapon* W				= Weapons->ActiveWeapon();
	if (W)					W->OnVisible		();
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
			bbStandBox.getcenter(start_pos);
			start_pos.add		(vPosition);
			if (!pCreator->ObjectSpace.EllipsoidCollide(cfModel,svTransform,start_pos,bbStandBox))
			{
				mstate_real &= ~mcCrouch;
				Movement.ActivateBox(0);
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
	if (Movement.gcontact_Was){
		if (mstate_real&mcFall){
			if (Movement.GetContactSpeed()>4.f){
				if (fis_zero(Movement.gcontact_HealthLost)){	
					m_fLandingTime	= s_fLandingTime1;
					mstate_real		|= mcLanding;
				}else{
					m_fLandingTime	= s_fLandingTime2;
					mstate_real		|= mcLanding2;
				}
			}
		}
		m_bJumpKeyPressed	= TRUE;
		m_fJumpTime			= s_fJumpTime;
		mstate_real			&=~	(mcFall|mcJump);
	}
	if ((mstate_wf&mcJump)==0)	m_bJumpKeyPressed	=	FALSE;

	// Зажало-ли меня/уперся - не двигаюсь
	if (((Movement.GetVelocityActual()<0.2f)&&(!(mstate_real&(mcFall|mcJump)))) || Movement.bSleep) 
	{
		mstate_real				&=~ mcAnyMove;
	}
	if (Movement.Environment()==CMovementControl::peOnGround)
	{
		// если на земле гарантированно снимать флажок Jump
		if (((s_fJumpTime-m_fJumpTime)>s_fJumpGroundTime)&&(mstate_real&mcJump))
		{
			mstate_real			&=~	mcJump;
			m_fJumpTime			= s_fJumpTime;
		}
	}
}

void CActor::g_cl_CheckControls(u32 mstate_wf, Fvector &vControlAccel, float &Jump, float dt)
{
	// ****************************** Check keyboard input and control acceleration
	vControlAccel.set	(0,0,0);

	if (!(mstate_real&mcFall) && (Movement.Environment()==CMovementControl::peInAir)) 
	{
		m_fFallTime				-=	dt;
		if (m_fFallTime<=0.f){
			m_fFallTime			=	s_fFallTime;
			mstate_real			|=	mcFall;
			mstate_real			&=~	mcJump;
		}
	}

	if (Movement.Environment()==CMovementControl::peOnGround)
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

		// crouch
		if ((0==(mstate_real&mcCrouch))&&(mstate_wf&mcCrouch))
		{
			mstate_real			|=	mcCrouch;
			Movement.ActivateBox(1);
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
		r_torso.yaw		=	cameras[cam_active]->GetWorldYaw	();
		r_torso.pitch	=	cameras[cam_active]->GetWorldPitch	();
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
		} else Weapons->GetFireParams(fire_pos, fire_dir);
	} else {
		Weapons->GetFireParams(fire_pos, fire_dir);
	}
}

void CActor::g_fireStart	( )
{
	Weapons->FireStart		( );
}

void CActor::g_fireEnd	( )
{
	Weapons->FireEnd	( );
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
		CObject*		O		= Weapons->ActiveWeapon();
		if (0==O)				return;

		// We doesn't have similar weapon - pick up it
		NET_Packet				P;
		u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16					(u16(O->ID()));
		u_EventSend				(P);
	}
}

void CActor::g_WeaponBones	(int& L, int& R)
{
	VERIFY	(Weapons);
	L		=	Weapons->m_iACTboneL;
	R		=	Weapons->m_iACTboneR;
}

void CActor::Statistic		()
{
}

// HUD
void CActor::OnHUDDraw	(CCustomHUD* hud)
{
	CWeapon* W			= Weapons->ActiveWeapon();
	if (W)				W->OnVisible		();

#ifdef _DEBUG
	CHUDManager* HUD	= (CHUDManager*)hud;
	string128 buf;
	HUD->pFontSmall->SetColor(0xffffffff);
	HUD->pFontSmall->OutSet	(120,530);
	HUD->pFontSmall->OutNext("Position:      [%3.2f, %3.2f, %3.2f]",VPUSH(vPosition));
	HUD->pFontSmall->OutNext("Velocity:      [%3.2f, %3.2f, %3.2f]",VPUSH(Movement.GetVelocity()));
	HUD->pFontSmall->OutNext("Vel Magnitude: [%3.2f]",Movement.GetVelocityMagnitude());
	HUD->pFontSmall->OutNext("Vel Actual:    [%3.2f]",Movement.GetVelocityActual());
	switch (Movement.Environment())
	{
	case CMovementControl::peOnGround:	strcpy(buf,"ground");			break;
	case CMovementControl::peInAir:		strcpy(buf,"air");				break;
	case CMovementControl::peAtWall:	strcpy(buf,"wall");				break;
	}
	HUD->pFontSmall->OutNext	(buf);
#endif
/**
	CHUDManager* HUD	= (CHUDManager*)hud;
	CUI* pUI=HUD->GetUI	();
	pUI->OutHealth		(iFloor(fHealth),iFloor(fArmor));
	pUI->OutWeapon		(Weapons->ActiveWeapon());
	pUI->SetHeading		(-r_torso.yaw);
	

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

void CActor::OnDeviceCreate()
{
	inherited::OnDeviceCreate();

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

	//
	Weapons->Init		("bip01_r_hand","bip01_l_finger1");

	// load damage params
	if (pSettings->LineExists(cNameSect(),"damage"))
	{
		CInifile::Sect& dam_sect	= pSettings->ReadSection(pSettings->ReadSTRING(cNameSect(),"damage"));
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
}

float CActor::HitScale	(int element)
{
	CKinematics* V		= PKinematics(pVisual);			VERIFY(V);
	float scale			= fis_zero(V->LL_GetInstance(element).get_param(0))?1.f:V->LL_GetInstance(element).get_param(0);
	return hit_factor*scale;
}

#ifdef DEBUG
void dbg_draw_frustum (float FOV, float _FAR, float A, Fvector &P, Fvector &D, Fvector &U);
void CActor::OnRender	()
{
	if (!bDebug)				return;

	Movement.dbg_Draw			();

	CCameraBase* C				= cameras	[cam_active];
	dbg_draw_frustum			(C->f_fov, 20.f, C->f_aspect, C->vPosition, C->vDirection, C->vNormal);
}
#endif
