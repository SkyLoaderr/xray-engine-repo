////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_follow.cpp
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Follow Me"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_hen_cover.h"
_HenFollowMe::_HenFollowMe() : State(aiHenFollowMe)
{
	hitTime	= 0;
	hitDir.set(0,0,1);
	senseTime = 0;
	senseDir.set(0,0,1);
}

void _HenFollowMe::Hit		(Fvector& D)
{
	hitTime				= Level().timeServer();
	// saving hit direction
	hitDir.set			(D);
}

void _HenFollowMe::Sense	(Fvector& D)
{
	senseTime				= Level().timeServer();
	// saving hit direction
	senseDir.set			(D);
}

BOOL _HenFollowMe::Parse	(CCustomMonster* Me)
{
	// if no more healtt then hen is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",Me->cName(),"Follow me");
#endif
	if (Me->g_Health() <= 0) {
		Me->State_Push	(new _HenDie());
		return TRUE;
	}
	else {
		// selecting enemy if any
		EnemySelected		Enemy;
		vfSelectEnemy		(Enemy,Me);
		// do I see the enemies?
		if (Enemy.E)		{
			Me->State_Push	(new _HenAttack());
			return TRUE;
		}
		else {
			// checking if I am under fire
			if (Level().timeServer() - hitTime < HIT_REACTION_TIME) {
				Me->State_Push	(new _HenUnderFire());
				return TRUE;
			}
			else {
				if (Level().timeServer() - senseTime < SENSE_REACTION_TIME) {
					Me->State_Push	(new _HenSenseSomething());
					return TRUE;
				}
				else {
					// determining the team
					CSquad&	Squad		= Level().Teams[Me->g_Team()].Squads[Me->g_Squad()];
					// determining who is leader
					CEntity* Leader		= Squad.Leader;
					// checking if the leader exists
					R_ASSERT (Leader);
					// I am leader then go to state "Free Hunting"
					if (Leader == Me) {
						Me->State_Push	(new _HenFreeHunting());
						return TRUE;
					}
					else {
						bool bWatch = false;
						// current look
						AIC_Look			&q_look		= Me->q_look;
						// current action
						AIC_Action			&q_action	= Me->q_action;
						// array for nodes of the members
						MemberNodes	MemberPlaces;
						// filling array members with their nodes
						Squad.Groups[Me->g_Group()].GetMemberPlacementN(MemberPlaces,Me);
						// checking if I need to rebuilt path i.e. previous search
						// found better destination node
						if (Me->AI_Path.bNeedRebuild) {
							// building a path from and to
							//Level().AI.q_Path	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
							Level().AI.vfFindTheXestPath	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path,MemberPlaces);
							if (Me->AI_Path.Nodes.size() > 2)
							// if path is long enough then build travel line
								Me->AI_Path.BuildTravelLine		(Me->Position());
							else
							// if path is too short then clear it (patch for ExecMove)
								Me->AI_Path.TravelPath.clear();
						} else {
							// get pointer to the class of node estimator 
							// for finding the best node in the area
							SelectorFollow&	S	= Me->fuzzyFollow;
							// my current position
							S.posMy				= Me->Position();
							// leader current position
							S.posTarget			= Leader->Position();
							// fill array members and exclude myself
							Squad.Groups[Me->g_Group()].GetMemberDedication(S.Members,Me);
							// search for the best node according to the 
							// SelectFollow evaluation function in the radius 35 meteres
							Level().AI.q_Range	(Me->AI_NodeID,Me->Position(),35.f,S,MemberPlaces);
							// if search has found new best node then 
							if (Me->AI_Path.DestNode != S.BestNode) {
								NodeCompressed* OLD		= Level().AI.Node(Me->AI_Path.DestNode);
								BOOL			dummy	= FALSE;
								float			old_cost= S.Estimate(
									OLD,
									Level().AI.u_SqrDistance2Node(Me->Position(),OLD),
									dummy);

								// if old cost minus new cost is a little then hen is too lazy
								// to move there
								if (S.BestCost < (old_cost-S.laziness)) {
									Me->AI_Path.DestNode		= S.BestNode;
									Me->AI_Path.bNeedRebuild	= TRUE;
								}
							} else {
								// search hasn't found a better node we have to look around
								bWatch = true;
							}
						}
						// setting up a look
						// getting my current node
						NodeCompressed* tNode		= Level().AI.Node(Me->AI_NodeID);
						// if we are going somewhere
						if (!bWatch) {
							// determining node cover
							Me->tWatchDirection.y = 0.f;
							Me->tWatchDirection.x = float(tNode->cover[2])/255.f - float(tNode->cover[0])/255.f;
							Me->tWatchDirection.z = float(tNode->cover[1])/255.f - float(tNode->cover[3])/255.f;
							Me->tWatchDirection.normalize();
							// setting up a look to watch at the least safe direction
							q_look.setup
								(
								AI::AIC_Look::Look, 
								AI::t_Direction, 
								&(Me->tWatchDirection),
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
							M.transform(Me->tWatchDirection);
							// setting up a look to watch around
							q_look.setup
								(
								AI::AIC_Look::Look, 
								AI::t_Direction, 
								&(Me->tWatchDirection),
								1000
								);
							// setting up look speed
							q_look.o_look_speed = 5;
						}
						return FALSE;
					}
				}
			}
		}
	}
}
	
HenSelectorFollow::HenSelectorFollow()
{ 
	Name = "sel_follow"; 
}

virtual	float HenSelectorFollow::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// distance to node
	float fResult = 0.0;//*fDistance*costTravel;
	// node lighting
	fResult += ((float)(tNode->light)/255.f)*costLight;
	if (fResult>BestCost)
		return(fResult);
	// leader relationship
	Fvector		P,P1,P2;
	Level().AI.UnpackPosition	(P1,tNode->p0);
	Level().AI.UnpackPosition	(P2,tNode->p1);
	P.lerp						(P1,P2,.5f);
	float						fDistanceToPlayer = P.distance_to(posTarget);
	
	if (fDistanceToPlayer < MIN_PLAYER_DISTANCE)
		fResult += MIN_PLAYER_PENALTY*costTarget;
	else
		if (fDistanceToPlayer > MAX_PLAYER_DISTANCE)
			fResult += MAX_PLAYER_PENALTY*costTarget;
		else
			fResult += (fDistanceToPlayer - OPTIMAL_PLAYER_DISTANCE)/OPTIMAL_PLAYER_DISTANCE;
	if (fResult>BestCost)
		return(fResult);
	
	// cover from leader
	Fvector tDirection;
	tDirection.x = posTarget.x - P.x;
	tDirection.y = fabs(posTarget.y - P.y);
	tDirection.z = posTarget.z - P.z;

	if (tDirection.x < 0.0)
		fResult += costCover*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
	else
		fResult += costCover*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
	
	if (tDirection.z < 0.0)
		fResult += costCover*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
	else
		fResult += costCover*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
	if (fResult>BestCost)
		return(fResult);
	
	if (tDirection.y > 2.0)
		fResult += tDirection.y*Y_PENALTY;
	if (fResult>BestCost)
		return(fResult);

	// cost of members relationship
	if (Members.size()) {
		for (DWORD it=0; it<Members.size(); it++)
		{
			float fDistanceToMember	= P.distance_to	(Members[it]);
			if (fDistanceToMember < MIN_MEMBER_DISTANCE)
				fResult += MIN_MEMBER_PENALTY*costMembers;
			else
				if (fDistanceToMember > MAX_MEMBER_DISTANCE)
					fResult += MAX_MEMBER_PENALTY*costMembers;
				else
					fResult += (fDistanceToMember - OPTIMAL_MEMBER_DISTANCE)/OPTIMAL_MEMBER_DISTANCE;
			/**/
			tDirection.x = Members[it].x - P.x;
			tDirection.y = fabs(Members[it].y - P.y);
			tDirection.z = Members[it].z - P.z;

			if (tDirection.x < 0.0)
				fResult += 0.5*costCover*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
			else
				fResult += 0.5*costCover*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
			
			if (tDirection.z < 0.0)
				fResult += 0.5*costCover*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
			else
				fResult += 0.5*costCover*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
			
			if (fResult>BestCost)
				return(fResult);
			
			//if (tDirection.y > 2.0)
			//	fResult += tDirection.y*Y_MEMBER_PENALTY;
			/**/
		}
	}
	// exit
	//if (fResult<EPS)	bStop = TRUE;
	return	fResult;
}

