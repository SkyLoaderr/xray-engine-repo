////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.cpp
//	Created 	: 05.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\..\\3dsound.h"

#include "ai_hen.h"
#include "ai_hen_selectors.h"

CAI_Hen::CAI_Hen()
{
	dwHitTime = 0;
	tHitDir.set(0,0,1);
	dwSenseTime = 0;
	tSenseDir.set(0,0,1);
	// creating initial state
	eCurrentState = aiHenFollowMe;
}

CAI_Hen::~CAI_Hen()
{
	// removing all data no more being neded 
	int i;
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
}

void CAI_Hen::Load(CInifile* ini, const char* section)
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
}

// when someone hit hen
void CAI_Hen::HitSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwHitTime = Level().timeServer();
	tHitDir.set(D);

	// Play hit-sound
	sound3D& S = sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->Play3DAtPos(S,vPosition);
}

// when someone hit hen
void CAI_Hen::SenseSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwSenseTime = Level().timeServer();
	tSenseDir.set(D);
}

// when hen is dead
void CAI_Hen::Death()
{
	// perform death operations
	inherited::Death( );
	q_action.setup(AI::AIC_Action::FireEnd);
	eCurrentState = aiHenDie;

	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	// Play sound
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
}

// hen update
void CAI_Hen::Update	(DWORD DT)
{
	inherited::Update	(DT);
}

float CAI_Hen::EnemyHeuristics(CEntity* E)
{
	if (E->g_Team()  == g_Team())	
		return flt_max;		// don't attack our team
	
	int	g_strench = E->g_Armor()+E->g_Health();
	
	if (g_strench <= 0)					
		return flt_max;		// don't attack dead enemiyes
	
	float	f1	= Position().distance_to_sqr(E->Position());
	float	f2	= float(g_strench);
	return  f1*f2;
}

void CAI_Hen::SelectEnemy(SEnemySelected& S)
{
	// Initiate process
	objVisible&	Known	= Level().Teams[g_Team()].KnownEnemys;
	S.Enemy					= 0;
	S.bVisible			= FALSE;
	S.fCost				= flt_max-1;
	if (Known.size()==0)	return;

	// Get visible list
	objSET		Visible;
	ai_Track.o_get	(Visible);
	std::sort			(Visible.begin(),Visible.end());

	// Iterate on known
	for (DWORD i=0; i<Known.size(); i++)
	{
		CEntity*	E = (CEntity*)Known[i].key;
		float		H = EnemyHeuristics(E);
		if (H<S.fCost) {
			// Calculate local visibility
			CObject**	ins	 = lower_bound(Visible.begin(),Visible.end(),(CObject*)E);
			bool	bVisible = (ins==Visible.end())?FALSE:((E==*ins)?TRUE:FALSE);
			float	cost	 = H*(bVisible?1:_FB_invisible_hscale);
			if (cost<S.fCost)	{
				S.Enemy		= E;
				S.bVisible	= bVisible;
				S.fCost		= cost;
			}
		}
	}
}

void CAI_Hen::Attack()
{
}

void CAI_Hen::Cover()
{
}

void CAI_Hen::Defend()
{
}

void CAI_Hen::Die()
{
	q_look.setup(0,AI::t_None,0,0	);
	q_action.setup(AI::AIC_Action::FireEnd);
	AI_Path.TravelPath.clear();
	bStopThinking = true;
}

void CAI_Hen::FollowMe()
{
	// if no more health then hen is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Follow me");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiHenDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do I see the enemies?
		if (Enemy.Enemy)		{
			eCurrentState = aiHenAttack;
			return;
		}
		else {
			// checking if I am under fire
			if (Level().timeServer() - dwHitTime < HIT_REACTION_TIME) {
				eCurrentState = aiHenUnderFire;
				return;
			}
			else {
				if (Level().timeServer() - dwSenseTime < SENSE_REACTION_TIME) {
					eCurrentState = aiHenSenseSomething;
					return;
				}
				else {
					// determining the team
					CSquad&	Squad		= Level().Teams[g_Team()].Squads[g_Squad()];
					// determining who is leader
					CEntity* Leader		= Squad.Leader;
					// checking if the leader exists
					R_ASSERT (Leader);
					// I am leader then go to state "Free Hunting"
					if (Leader == this) {
						eCurrentState = aiHenFreeHunting;
						return;
					}
					else {
						bool bWatch = false;
						// current look
						//AI::AIC_Look			&q_look		= q_look;
						// current action
						//AI::AIC_Action			&q_action	= q_action;
						// array for nodes of the members
						MemberNodes	MemberPlaces;
						// filling array members with their nodes
						Squad.Groups[g_Group()].GetMemberPlacementN(MemberPlaces,this);
						// checking if I need to rebuilt path i.e. previous search
						// found better destination node
						if (AI_Path.bNeedRebuild) {
							// building a path from and to
							//Level().AI.q_Path	(AI_NodeID,AI_Path.DestNode,AI_Path);
							Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,MemberPlaces);
							if (AI_Path.Nodes.size() > 2)
							// if path is long enough then build travel line
								AI_Path.BuildTravelLine(Position());
							else
							// if path is too short then clear it (patch for ExecMove)
								AI_Path.TravelPath.clear();
						} else {
							// get pointer to the class of node estimator 
							// for finding the best node in the area
							CHenSelectorFollow S;
							// my current position
							//S.posMy	= Position();
							// leader current position
							//S.posTarget	= Leader->Position();
							// fill array members and exclude myself
							//Squad.Groups[g_Group()].GetMemberDedication(S.taMembers,this);
							// search for the best node according to the 
							// SelectFollow evaluation function in the radius 35 meteres
							Level().AI.q_Range	(AI_NodeID,Position(),35.f,S,MemberPlaces);
							// if search has found new best node then 
							if (AI_Path.DestNode != S.BestNode) {
								NodeCompressed* OLD		= Level().AI.Node(AI_Path.DestNode);
								BOOL			dummy	= FALSE;
								float			old_cost= S.Estimate(
									OLD,
									Level().AI.u_SqrDistance2Node(Position(),OLD),
									dummy);

								// if old cost minus new cost is a little then hen is too lazy
								// to move there
								if (S.BestCost < (old_cost-S.fLaziness)) {
									AI_Path.DestNode		= S.BestNode;
									AI_Path.bNeedRebuild	= TRUE;
								}
							} else {
								// search hasn't found a better node we have to look around
								bWatch = true;
							}
						}
						// setting up a look
						// getting my current node
						NodeCompressed* tNode		= Level().AI.Node(AI_NodeID);
						// if we are going somewhere
						if (!bWatch) {
							// determining node cover
							tWatchDirection.y = 0.f;
							tWatchDirection.x = float(tNode->cover[2])/255.f - float(tNode->cover[0])/255.f;
							tWatchDirection.z = float(tNode->cover[1])/255.f - float(tNode->cover[3])/255.f;
							tWatchDirection.normalize();
							// setting up a look to watch at the least safe direction
							q_look.setup
								(
								AI::AIC_Look::Look, 
								AI::t_Direction, 
								&(tWatchDirection),
								1000
								);
							// setting up look speed
							q_look.o_look_speed=_FB_look_speed;
						}
						// if we are staying
						else {
							// looking around
							Fmatrix M;
							M.rotateY(PI/60.f);
							M.transform(tWatchDirection);
							// setting up a look to watch around
							q_look.setup
								(
								AI::AIC_Look::Look, 
								AI::t_Direction, 
								&(tWatchDirection),
								1000
								);
							// setting up look speed
							q_look.o_look_speed = 5;
						}
						bStopThinking = true;
						return;
					}
				}
			}
		}
	}
}

void CAI_Hen::FreeHunting()
{
}

void CAI_Hen::GoInThisDirection()
{
}

void CAI_Hen::GoToThisPosition()
{
}

void CAI_Hen::HoldPositionUnderFire()
{
}

void CAI_Hen::HoldThisPosition()
{
}

void CAI_Hen::Pursuit()
{
}

void CAI_Hen::Retreat()
{
}

void CAI_Hen::SenseSomething()
{
}

void CAI_Hen::UnderFire()
{
}

void CAI_Hen::WaitOnPosition()
{
}

void CAI_Hen::Think()
{
	bStopThinking = false;
	do {
		switch(eCurrentState) {
			case aiHenDie : {
				Die();
				break;
			}
			case aiHenUnderFire : {
				UnderFire();
				break;
			}
			case aiHenSenseSomething : {
				SenseSomething();
				break;
			}
			case aiHenGoInThisDirection : {
				GoInThisDirection();
				break;
			}
			case aiHenGoToThisPosition : {
				GoToThisPosition();
				break;
			}
			case aiHenWaitOnPosition : {
				WaitOnPosition();
				break;
			}
			case aiHenHoldThisPosition : {
				HoldThisPosition();
				break;
			}
			case aiHenHoldPositionUnderFire : {
				HoldPositionUnderFire();
				break;
			}
			case aiHenFreeHunting : {
				FreeHunting();
				break;
			}
			case aiHenFollowMe : {
				FollowMe();
				break;
			}
			case aiHenAttack : {
				Attack();
				break;
			}
			case aiHenDefend : {
				Defend();
				break;
			}
			case aiHenPursuit : {
				Pursuit();
				break;
			}
			case aiHenRetreat : {
				Retreat();
				break;
			}
			case aiHenCover : {
				Cover();
				break;
			}
		}
	}
	while (!bStopThinking);
}
