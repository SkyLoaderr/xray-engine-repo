////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\..\\3dsound.h"
#include "ai_crow.h"
#include "ai_crow_selectors.h"
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
	CKinematics* V = PKinematics(pVisual);
	m_fly = V->ID_Cycle("norm_fly_fwd");
	m_death_idle = V->ID_Cycle("norm_death_idle");
}

CAI_Crow::~CAI_Crow()
{
	// removing all data no more being neded 
	int i;
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
}

void CAI_Crow::Load(CInifile* ini, const char* section)
{ 
	// load parameters from ".ini" file
	inherited::Load	(ini,section);
	
	Fvector			P = vPosition;
	P.x				+= ::Random.randF();
	P.z				+= ::Random.randF();

	// sounds
	pSounds->Create3D(sndHit[0],"actor\\bhit_flesh-1");
	pSounds->Create3D(sndHit[1],"actor\\bhit_flesh-2");
	pSounds->Create3D(sndHit[2],"actor\\bhit_flesh-3");
	pSounds->Create3D(sndHit[3],"actor\\bhit_helmet-1");
	pSounds->Create3D(sndHit[4],"actor\\bullet_hit1");
	pSounds->Create3D(sndHit[5],"actor\\bullet_hit2");
	pSounds->Create3D(sndHit[6],"actor\\ric_conc-1");
	pSounds->Create3D(sndHit[7],"actor\\ric_conc-2");
	pSounds->Create3D(sndDie[0],"actor\\die0");
	pSounds->Create3D(sndDie[1],"actor\\die1");
	pSounds->Create3D(sndDie[2],"actor\\die2");
	pSounds->Create3D(sndDie[3],"actor\\die3");

	SelectorFreeHunting.Load(ini,section);
	SelectorFreeHunting.fSearchRange += ::Random.randF(-1.f,1.f);
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
	sound3D& S = sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->Play3DAtPos(S,vPosition);
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
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
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

	bStopThinking = true;
}

IC void CAI_Crow::SetDirectionLook(NodeCompressed *tNode)
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
		CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
		// determining who is leader
		CEntity* Leader = Squad.Leader;
		// checking if the leader exists
		R_ASSERT (Leader);
		// checking if leader is dead then make myself a leader
		if (Leader->g_Health() <= 0)
			Leader = this;
		// setting up watch mode to false
		bool bWatch = false;
		// get pointer to the class of node estimator 
		// for finding the best node in the area
		CCrowSelectorFreeHunting S = SelectorFreeHunting;
		// if i am not a leader then assign leader
		if (Leader != this) {
			S.m_tLeader = Leader;
			S.m_tLeaderPosition = Leader->Position();
			S.m_tpLeaderNode = Leader->AI_Node;
			S.m_tLeaderNode = Leader->AI_NodeID;
		}
		// otherwise assign leader to null
		else {
			S.m_tLeader = 0;
			S.m_tLeaderPosition.set(0,0,0);
			S.m_tpLeaderNode = NULL;
			S.m_tLeaderNode = -1;
		}
		S.m_tHitDir			= tHitDir;
		S.m_dwHitTime		= dwHitTime;
		
		S.m_dwCurTime		= Level().timeServer();
		
		S.m_tMe				= this;
		S.m_tpMyNode		= AI_Node;
		S.m_tMyPosition		= Position();
		S.m_tDirection		= tWatchDirection;
		
		S.m_tEnemy			= 0;
		S.m_tEnemyPosition.set(0,0,0);
		S.m_tpEnemyNode		= NULL;
		
		S.taMembers = Squad.Groups[g_Group()].Members;
		// checking if I need to rebuild the path i.e. previous search
		// has found better destination node
		if (AI_Path.bNeedRebuild) {
			Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
			if (AI_Path.Nodes.size() > 2) {
			// if path is long enough then build travel line
				AI_Path.BuildTravelLine(Position());
				m_bMobility = true;
			}
			else {
			// if path is too short then clear it (patch for ExecMove)
				AI_Path.TravelPath.clear();
				m_bMobility = false;
				AI_Path.bNeedRebuild = FALSE;
			}
		} 
		else
			if ((AI_Path.TravelPath.size() - AI_Path.TravelStart < 3) || (AI_Path.TravelPath.size() < 3) || (AI_Path.DestNode == AI_NodeID)) {
				// fill arrays of members and exclude myself
				Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
				// SelectFollow evaluation function in the radius 35 meteres
				float fOldCost;
				Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
				// if search has found new best node then 
				//if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path)))){
					AI_Path.DestNode		= S.BestNode;
					AI_Path.bNeedRebuild	= TRUE;
				//} 
				//else 
					// search hasn't found a better node we have to look around
				//	bWatch = true;
				if (AI_Path.TravelPath.size() < 2)
					AI_Path.bNeedRebuild	= TRUE;
			}
			//else
			//	bWatch = true;
		// setting up a look
		
		SetDirectionLook(AI_Node);
		
		q_action.setup(AI::AIC_Action::AttackEnd);
		// checking flag to stop processing more states
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
			S = m_idle;
	
	if (S != m_current){ 
		m_current = S;
		if (S)
			m_tpCurrentBlend = PKinematics(pVisual)->PlayCycle(S);
		else
			m_tpCurrentBlend = 0;
	}
}
