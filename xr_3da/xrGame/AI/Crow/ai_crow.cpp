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
#include "..\\ai_monsters.h"
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
	if (Engine.FS.Exist(fn,Path.Sounds,prefix,".wav")){
		m_Sounds.push_back(sound());
		pSounds->Create(m_Sounds.back(),TRUE,prefix,false,0);
	}
	for (int i=0; (i<MAX_SND_COUNT)&&(m_Sounds.size()<MAX_SND_COUNT); i++){
		string64		name;
		sprintf			(name,"%s_%d",prefix,i);
		if (Engine.FS.Exist(fn,Path.Sounds,name,".wav")){
			m_Sounds.push_back(sound());
			pSounds->Create(m_Sounds.back(),TRUE,name,false,0);
		}
	}
	R_ASSERT(m_Sounds.size());
}

void CAI_Crow::SSound::SetPosition(const Fvector& pos)
{
	for (int i=0; i<m_Sounds.size(); i++)
		if (m_Sounds[i].feedback) m_Sounds[i].feedback->SetPosition(pos);
}

void CAI_Crow::SSound::Unload()
{
	for (int i=0; i<m_Sounds.size(); i++)
		pSounds->Delete	(m_Sounds[i]);
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
}

void CAI_Crow::Load( LPCSTR section )
{
	inherited::Load				(section);
	// animations
	CKinematics*	M			= PKinematics(pVisual); R_ASSERT(M);
	m_Anims.m_death.Load		(M,"norm_death");
	m_Anims.m_death_dead.Load	(M,"norm_death_dead");
	m_Anims.m_death_idle.Load	(M,"norm_death_idle");
	m_Anims.m_fly.Load			(M,"norm_fly_fwd");
	m_Anims.m_idle.Load			(M,"norm_idle");
	// sounds
	m_Sounds.m_idle.Load		("monsters\\crow\\idle");
	// play defaut
//	M->PlayCycle				(m_Anims.m_idle.GetRandom());
	
	fSpeed						= pSettings->ReadFLOAT	(section,"speed");
	fASpeed						= pSettings->ReadFLOAT	(section,"angular_speed");
	fGoalChangeDelta			= pSettings->ReadFLOAT	(section,"goal_change_delta");
	fMinHeight					= pSettings->ReadFLOAT	(section,"min_height");
	vVarGoal					= pSettings->ReadVECTOR	(section,"goal_variability");
	fIdleSoundDelta				= pSettings->ReadFLOAT	(section,"idle_sound_delta");
	fIdleSoundTime				= fIdleSoundDelta+fIdleSoundDelta*Random.randF(-.5f,.5f);

	Movement.SetParent			(this);
}

BOOL CAI_Crow::Spawn(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	BOOL R = inherited::Spawn	(bLocal,server_id,o_pos,o_angle,P,flags);
	bVisible					= TRUE;
	AI_Node = 0;
	return R;
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
	Movement.SetVelocity(V);
	PKinematics(pVisual)->PlayCycle	(m_Anims.m_death.GetRandom(),TRUE,cb_OnHitEndPlaying,this);
}
void CAI_Crow::Update(DWORD DT)
{
	inherited::Update(DT);
	
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
			pSounds->PlayAtPos(m_Sounds.m_idle.GetRandom(),H_Root(),vPosition);
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
	tAcceleration.set(0,10.f,0);
	Movement.SetPosition(vPosition);
	Movement.Calculate	(tAcceleration,0,0,Device.fTimeDelta > .1f ? .1f : Device.fTimeDelta,false);
	Movement.GetPosition(vPosition);

	UpdateTransform();

	if (Movement.Environment() == CMovementControl::peOnGround)
		st_target = eDeathDead;
}

void CAI_Crow::UpdateCL()
{
	inherited::UpdateCL();
	if (!bActive) return;
//---------------------------------------------------------------------
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
//---------------------------------------------------------------------

// Core events
void CAI_Crow::net_Export(NET_Packet* P)					// export to server
{
}
//---------------------------------------------------------------------


void CAI_Crow::net_Import(NET_Packet* P)
{
}
//---------------------------------------------------------------------

void CAI_Crow::HitSignal(int HitAmount, Fvector& local_dir, CEntity* who)
{
	iHealth		= 0;
	if (st_current!=eDeathDead) st_target = eDeathFall;
	else PKinematics(pVisual)->PlayCycle(m_Anims.m_death_dead.GetRandom());
}
//---------------------------------------------------------------------
