////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\xr_weapon_list.h"
#include "ai_crow.h"
#include "..\\..\\hudmanager.h"

void CAI_Crow::SAnim::Load(CKinematics* visual, LPCSTR prefix)
{
	CMotionDef* M		= visual->ID_Cycle_Safe(prefix);
	if (M)				m_Animations.push_back(M);
	for (int i=0; (i<MAX_ANIM_COUNT)&&(m_Animations.size()<MAX_ANIM_COUNT); i++){
		string128		sh_anim;
		sprintf			(sh_anim,"%s_%d",prefix,i);
		M				= visual->ID_Cycle_Safe(sh_anim);
		if (M)			m_Animations.push_back(M);
	}
	R_ASSERT(m_Animations.size());
}

void CAI_Crow::SSound::Load(LPCSTR prefix)
{
	string128 fn;
	if (FS.exist(fn,"$game_sounds$",prefix,".wav")){
		m_Sounds.push_back	(sound());
		::Sound->create		(m_Sounds.back(),TRUE,prefix,0);
	}
	for (int i=0; (i<MAX_SND_COUNT)&&(m_Sounds.size()<MAX_SND_COUNT); i++){
		string64		name;
		sprintf			(name,"%s_%d",prefix,i);
		if (FS.exist(fn,"$game_sounds$",name,".wav")){
			m_Sounds.push_back(sound());
			::Sound->create(m_Sounds.back(),TRUE,name,0);
		}
	}
	R_ASSERT(m_Sounds.size());
}

void CAI_Crow::SSound::SetPosition(const Fvector& pos)
{
	for (int i=0; i<(int)m_Sounds.size(); i++)
		m_Sounds[i].set_position(pos);
}

void CAI_Crow::SSound::Unload()
{
	for (int i=0; i<(int)m_Sounds.size(); i++)
		::Sound->destroy	(m_Sounds[i]);
}

void __stdcall	cb_OnHitEndPlaying			(CBlend* B)
{
	((CAI_Crow*)B->CallbackParam)->OnHitEndPlaying(B);
}

void CAI_Crow::OnHitEndPlaying(CBlend* B)
{
	PKinematics(pVisual)->PlayCycle	(m_Anims.m_death_idle.GetRandom());
}

CAI_Crow::CAI_Crow()
{
	st_current			= eUndef;
	st_target			= eFlyIdle;
	vGoalDir.set		(10.0f*(Random.randF()-Random.randF()),10.0f*(Random.randF()-Random.randF()),10.0f*(Random.randF()-Random.randF()));
	vCurrentDir.set		(0,0,1);
	vHPB.set			(0,0,0);
	fDHeading			= 0;
	fGoalChangeDelta	= 10.f;
	fGoalChangeTime		= 0.f;
	fSpeed				= 5.f;
	fASpeed				= 0.2f;
	fMinHeight			= 40.f;
	vVarGoal.set		(10.f,10.f,100.f);
	fIdleSoundDelta		= 10.f;
	fIdleSoundTime		= fIdleSoundDelta;
}

CAI_Crow::~CAI_Crow()
{
	// removing all data no more being neded 
	m_Sounds.m_idle.Unload		();
	xr_delete(m_pPhysicsShell);

}

void CAI_Crow::Load( LPCSTR section )
{
	inherited::Load				(section);

	// sounds
	m_Sounds.m_idle.Load		("monsters\\crow\\idle");
	// play defaut
	
	fSpeed						= pSettings->r_float	(section,"speed");
	fASpeed						= pSettings->r_float	(section,"angular_speed");
	fGoalChangeDelta			= pSettings->r_float	(section,"goal_change_delta");
	fMinHeight					= pSettings->r_float	(section,"min_height");
	vVarGoal					= pSettings->r_fvector3	(section,"goal_variability");
	fIdleSoundDelta				= pSettings->r_float	(section,"idle_sound_delta");
	fIdleSoundTime				= fIdleSoundDelta+fIdleSoundDelta*Random.randF(-.5f,.5f);


}

BOOL CAI_Crow::net_Spawn		(LPVOID DC)
{
	BOOL R		= inherited::net_Spawn	(DC);
	setVisible	(TRUE);
	AI_Node		= 0;

	// animations
	CKinematics*	M			= PKinematics(pVisual); R_ASSERT(M);
	m_Anims.m_death.Load		(M,"norm_death");
	m_Anims.m_death_dead.Load	(M,"norm_death_dead");
	m_Anims.m_death_idle.Load	(M,"norm_death_idle");
	m_Anims.m_fly.Load			(M,"norm_fly_fwd");
	m_Anims.m_idle.Load			(M,"norm_idle");

	return		R;
}

// crow update
void CAI_Crow::switch2_FlyUp()
{
	PKinematics(pVisual)->PlayCycle	(m_Anims.m_fly.GetRandom());
}
void CAI_Crow::switch2_FlyIdle()
{
	PKinematics(pVisual)->PlayCycle	(m_Anims.m_idle.GetRandom());
}
void CAI_Crow::switch2_DeathDead()
{
	PKinematics(pVisual)->PlayCycle	(m_Anims.m_death_dead.GetRandom());
}
void CAI_Crow::switch2_DeathFall()
{
	Fvector V;
	V.mul(mRotate.k,fSpeed);
//	Movement.SetVelocity(V);
	PKinematics(pVisual)->PlayCycle	(m_Anims.m_death.GetRandom(),TRUE,cb_OnHitEndPlaying,this);
}
void CAI_Crow::Update(u32 DT)
{
	inherited::Update(DT);
	UpdatePhysicsShell();
	if (st_target!=st_current){
		switch(st_target){
		case eFlyUp: 
			switch2_FlyUp();
			break;
		case eFlyIdle:
			switch2_FlyIdle();
			break;
		case eDeathFall:
			switch2_DeathFall();
			break;
		case eDeathDead:
			switch2_DeathDead();
			break;
		}
		st_current = st_target;
	}

	switch (st_current){
	case eFlyIdle:
		if (vPosition.y>vOldPosition.y) st_target = eFlyUp;
		break;
	case eFlyUp:
		if (vPosition.y<=vOldPosition.y) st_target = eFlyIdle;
		break;
	case eDeathFall:
		state_DeathFall();
		break;
	}
	if ((st_current!=eDeathFall)&&(st_current!=eDeathDead)){
		// At random times, change the direction (goal) of the plane
		if(fGoalChangeTime<=0){
			fGoalChangeTime += fGoalChangeDelta+fGoalChangeDelta*Random.randF(-0.5f,0.5f);
			Fvector vP;
			vP.set(Device.vCameraPosition.x,Device.vCameraPosition.y+fMinHeight,Device.vCameraPosition.z);
			vGoalDir.x = vP.x+vVarGoal.x*Random.randF(-0.5f,0.5f); 
			vGoalDir.y = vP.y+vVarGoal.y*Random.randF(-0.5f,0.5f);
			vGoalDir.z = vP.z+vVarGoal.z*Random.randF(-0.5f,0.5f);
		}
		fGoalChangeTime-=float(DT)/1000.f;
		// sounds
		if (fIdleSoundTime<=0){
			fIdleSoundTime = fIdleSoundDelta+fIdleSoundDelta*Random.randF(-0.5f,0.5f);
			//if (st_current==eFlyIdle)
			::Sound->play_at_pos(m_Sounds.m_idle.GetRandom(),H_Root(),vPosition);
		}
		fIdleSoundTime-=float(DT)/1000.f;
	}
	m_Sounds.m_idle.SetPosition(vPosition);
}

void CAI_Crow::state_Flying()
{
	// Update position and orientation of the planes
	float fAT = fASpeed * Device.fTimeDelta;

	Fvector& vDirection = mRotate.k;

	// Tweak orientation based on last position and goal
	Fvector vOffset;
	vOffset.sub(vGoalDir,vPosition);

	// First, tweak the pitch
	if( vOffset.y > 1.0){			// We're too low
		vHPB.y += fAT;
		if( vHPB.y > 0.8f )	vHPB.y = 0.8f;
	}else if( vOffset.y < -1.0){	// We're too high
		vHPB.y -= fAT;
		if( vHPB.y < -0.8f )vHPB.y = -0.8f;
	}else							// Add damping
		vHPB.y *= 0.95f;

	// Now figure out yaw changes
	vOffset.y           = 0.0f;
	vDirection.y		= 0.0f;

	vDirection.normalize();
	vOffset.normalize	();

	float fDot = vDirection.dotproduct(vOffset);
	fDot = (1.0f-fDot)/2.0f * fAT * 10.0f;

	vOffset.crossproduct(vOffset,vDirection);

	if( vOffset.y > 0.01f )		fDHeading = ( fDHeading * 9.0f + fDot ) * 0.1f;
	else if( vOffset.y < 0.01f )fDHeading = ( fDHeading * 9.0f - fDot ) * 0.1f;

	vHPB.x  +=  fDHeading;
	vHPB.z  = -fDHeading * 9.0f;

	// Build the local matrix for the pplane
	mRotate.setHPB(vHPB.x,vHPB.y,vHPB.z);

	// Update position
	vOldPosition.set(vPosition);
	vPosition.mad(vDirection,fSpeed*Device.fTimeDelta);

	UpdateTransform();
}

static Fvector vV={0,0,0};
void CAI_Crow::state_DeathFall()
{
	Fvector tAcceleration;
	tAcceleration.set(0,-10.f,0);
	//Movement.SetPosition(vPosition);
	//Movement.Calculate	(tAcceleration,0,0,Device.fTimeDelta > .1f ? .1f : Device.fTimeDelta,false);
	//Movement.GetPosition(vPosition);

	UpdateTransform();
	
	if (m_pPhysicsShell)
	{
		Fvector velocity;
		m_pPhysicsShell->get_LinearVel(velocity);
		if(velocity.y>-0.001f) st_target = eDeathDead;
	}
}

void CAI_Crow::UpdateCL()
{
	inherited::UpdateCL();
	if (!getActive()) return;

	switch (st_current){
	case eFlyIdle:
	case eFlyUp:
		state_Flying();
		break;
	case eDeathFall:
		state_DeathFall();
		break;
	}
}

// Core events
void CAI_Crow::net_Export	(NET_Packet& P)					// export to server
{
	// export 
	R_ASSERT			(Local());

	u8					flags = 0;
	P.w_float_q16		(fHealth,-1000,1000);

	P.w_float			(0);
	P.w_u32				(0);
	P.w_u32				(0);

	P.w_u32				(Level().timeServer());
	P.w_u8				(flags);
	
	float				yaw, pitch, bank;
	mRotate.getHPB		(yaw,pitch,bank);
	P.w_angle8			(yaw);
	P.w_angle8			(yaw);
	P.w_angle8			(pitch);
}
//---------------------------------------------------------------------
void CAI_Crow::net_Import	(NET_Packet& P)
{
	// import
	R_ASSERT			(Remote());

	u8					flags;
	P.r_float_q16		(fHealth,-1000,1000);

	float fDummy;
	u32 dwDummy;
	P.r_float			(fDummy);
	P.r_u32				(dwDummy);
	P.r_u32				(dwDummy);

	P.r_u32				(dwDummy);
	P.r_u8				(flags);
	
	float				yaw, pitch, bank = 0;
	
	P.r_angle8			(yaw);
	P.r_angle8			(yaw);
	P.r_angle8			(pitch);
	
	mRotate.setHPB		(yaw,pitch,bank);
}
//---------------------------------------------------------------------
void CAI_Crow::HitSignal	(float HitAmount, Fvector& local_dir, CObject* who, s16 element)
{
	fHealth		= 0;
	m_dwDeathTime = Level().timeServer();
	if (st_current!=eDeathDead) 
	{	
		Die();
		st_target = eDeathFall;
	}
	else PKinematics(pVisual)->PlayCycle(m_Anims.m_death_dead.GetRandom());
}
//---------------------------------------------------------------------
void CAI_Crow::HitImpulse	(float	amount,		Fvector& vWorldDir, Fvector& vLocalDir)
{
/*
	switch (st_current){
	case eDeathDead:{
		float Q	= float(amount)/Movement.GetMass();
		Movement.vExternalImpulse.mad(vWorldDir,Q);
	}break;
	}
*/
//	if(m_pPhysicsShell) inherited::Hit(amount,vWorldDir,0,0,)
}
//---------------------------------------------------------------------
void CAI_Crow::CreateSkeleton()
{
	m_pPhysicsShell=P_create_Shell();
	Fobb obb; Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); obb.m_rotate.identity();
	CPhysicsElement* E = P_create_Element(); R_ASSERT(E); E->add_Box(obb);
	m_pPhysicsShell->add_Element(E);
	m_pPhysicsShell->setMass(0.3f);
	m_pPhysicsShell->SetMaterial("creatures\\crow");
	m_pPhysicsShell->Activate(svXFORM(),0,svXFORM());
	
}

void CAI_Crow::UpdatePhysicsShell()
{
	if(!m_pPhysicsShell) return;

		m_pPhysicsShell->Update();
		mRotate.i.set(m_pPhysicsShell->mXFORM.i);
		mRotate.j.set(m_pPhysicsShell->mXFORM.j);
		mRotate.k.set(m_pPhysicsShell->mXFORM.k);
		mRotate.c.set(0,0,0);
		vPosition.set(m_pPhysicsShell->mXFORM.c);
		UpdateTransform();

}

void CAI_Crow::Hit(float P, Fvector &dir, CObject* who, s16 element,Fvector p_in_object_space, float impulse)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse/100.f);
}