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

//#define WRITE_LOG

CAI_Crow::CAI_Crow()
{
	dwHitTime = 0;
	tHitDir.set(0,0,1);
	dwSenseTime = 0;
	tSenseDir.set(0,0,1);
	m_tpCurrentBlend = 0;
	eCurrentState = aiCrowFreeHunting;
}

CAI_Crow::~CAI_Crow()
{
	// removing all data no more being neded 
	int i;
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete(sndDie[i]);
}

void CAI_Crow::Load(CInifile* ini, const char* section)
{ 
	// load parameters from ".ini" file
	inherited::Load	(ini,section);
	
	Fvector			P = vPosition;
	P.x				+= ::Random.randF();
	P.z				+= ::Random.randF();

	// sounds
	pSounds->Create(sndHit[0],TRUE,	"actor\\bhit_flesh-1");
	pSounds->Create(sndHit[1],TRUE,	"actor\\bhit_flesh-2");
	pSounds->Create(sndHit[2],TRUE,	"actor\\bhit_flesh-3");
	pSounds->Create(sndHit[3],TRUE,	"actor\\bhit_helmet-1");
	pSounds->Create(sndHit[4],TRUE,	"actor\\bullet_hit1");
	pSounds->Create(sndHit[5],TRUE,	"actor\\bullet_hit2");
	pSounds->Create(sndHit[6],TRUE,	"actor\\ric_conc-1");
	pSounds->Create(sndHit[7],TRUE,	"actor\\ric_conc-2");
	pSounds->Create(sndDie[0],TRUE,	"actor\\die0");
	pSounds->Create(sndDie[1],TRUE,	"actor\\die1");
	pSounds->Create(sndDie[2],TRUE,	"actor\\die2");
	pSounds->Create(sndDie[3],TRUE,	"actor\\die3");

	CKinematics* V = PKinematics(pVisual);
	m_fly = V->ID_Cycle("norm_fly_fwd");
	m_death_idle = V->ID_Cycle("norm_death_idle");
}

// when someone hit crow
void CAI_Crow::HitSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwHitTime = Level().timeServer();
	tHitDir.set(D);
	tHitDir.normalize();

	// Play hit-sound
	sound& S				= sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->PlayAtPos	(S,this,vPosition);
}

// when someone hit crow
void CAI_Crow::SenseSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwSenseTime = Level().timeServer();
	tSenseDir.set(D);
}

// when crow is dead
void CAI_Crow::Death()
{
	// perform death operations
	inherited::Death( );
	q_action.setup(AI::AIC_Action::AttackEnd);
	eCurrentState = aiCrowDie;

	// removing from group
	//Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].Member_Remove(this);

	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	// Play sound
	pSounds->PlayAtPos(sndDie[Random.randI(SND_DIE_COUNT)],this,vPosition);
}

// crow update
void CAI_Crow::Update(DWORD DT)
{
	inherited::Update(DT);
}

void CAI_Crow::Die()
{
	q_look.setup(0,AI::t_None,0,0	);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	bActive = false;
	bEnabled = false;
	
	bStopThinking = true;
}

IC void CAI_Crow::SetDirectionLook()
{
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.square_magnitude() > 0.000001) {
			tWatchDirection.normalize();
			q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &(tWatchDirection), 1000);
			q_look.o_look_speed=_FB_look_speed;
		}
	}
}

bool CAI_Crow::bfChangeFlyTrajectory(Fvector &tActorPosition, DWORD dwTime)
{
	Fvector tTemp;
	tTemp.sub(tActorPosition,m_tSavedActorPosition);
	if (tTemp.magnitude() < m_fActorChange)
		return(false);
	else
		if (dwTime - m_dwLastTrajectoryChange < m_dwChangeInterval)
			return(false);
		else
			return(true);
}

void CAI_Crow::FreeHunting()
{
	// if no more health then crow is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Free hunting");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiCrowDie;
		return;
	}
	else {
		Fvector tActorPosition = Level().CurrentViewEntity()->Position();
		if (bfChangeFlyTrajectory(tActorPosition,Level().timeServer())) {
			m_fRadius = m_fOptRadius + ::Random.randF(-m_fRadiusDeviation, m_fRadiusDeviation);
			m_tCenter = tActorPosition;
			m_tCenter.x += ::Random.randF(-m_fXDeviation, m_fXDeviation);
			m_tCenter.z += ::Random.randF(-m_fZDeviation, m_fZDeviation);
			float fNewHeight = m_fOptHeight + ::Random.randF(-m_fYDeviation, m_fYDeviation);
			m_tCenter.y += fNewHeight;
		}
		else {
			
		}
		SetDirectionLook();
		m_fCurSpeed = m_fMinSpeed;
		bStopThinking = true;
		return;
	}
}

void CAI_Crow::Think()
{
	bStopThinking = false;
	do {
		switch(eCurrentState) {
			case aiCrowDie : {
				Die();
				break;
			}
			case aiCrowFreeHunting : {
				FreeHunting();
				break;
			}
		}
	}
	while (!bStopThinking);
}

void CAI_Crow::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	//R_ASSERT(fsimilar(_view.magnitude(),1));
	//R_ASSERT(fsimilar(_move.magnitude(),1));

	CMotionDef*	S=0;
	
	if (iHealth<=0)
		S = m_death;
	else
		if (::Random.randI(0,10) == 0)
			S = m_fly;
		else
			S = m_fly;
	
	if (S != m_current){ 
		m_current = S;
		if (S)
			m_tpCurrentBlend = PKinematics(pVisual)->PlayCycle(S);
		else
			m_tpCurrentBlend = 0;
	}
}

void CAI_Crow::net_Export(NET_Packet* P)					// export to server
{
	R_ASSERT				(net_Local);

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P->w_u32				(N.dwTimeStamp);
	P->w_u8					(0);
	P->w_vec3				(N.p_pos);
	P->w_angle8				(N.o_model);
	P->w_angle8				(N.o_torso.yaw);
	P->w_angle8				(N.o_torso.pitch);
}

void CAI_Crow::net_Import(NET_Packet* P)
{
	R_ASSERT				(!net_Local);
	net_update				N;

	u8 flags;
	P->r_u32				(N.dwTimeStamp);
	P->r_u8					(flags);
	P->r_vec3				(N.p_pos);
	P->r_angle8				(N.o_model);
	P->r_angle8				(N.o_torso.yaw);
	P->r_angle8				(N.o_torso.pitch);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	bVisible				= TRUE;
	bEnabled				= TRUE;
}
