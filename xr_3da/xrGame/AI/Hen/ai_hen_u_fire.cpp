////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_u_fire.cpp
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Under Fire"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_hen_cover.h"
_HenUnderFire::_HenUnderFire() : State(aiHenUnderFire)
{
	hitTime	= 0;
	hitDir.set(0,0,1);
	senseTime = 0;
	senseDir.set(0,0,1);
}

void _HenUnderFire::Hit		(Fvector& D)
{
	hitTime				= Level().timeServer();
	// saving hit direction
	hitDir.set			(D);
}

void _HenUnderFire::Sense	(Fvector& D)
{
	senseTime				= Level().timeServer();
	// saving hit direction
	senseDir.set			(D);
}

BOOL _HenUnderFire::Parse	(CCustomMonster* Me)
{
	// if no more healtt then hen is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",Me->cName(),"Under fire");
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
			Me->State_Pop();
			Me->State_Push(new _HenAttack());
			return TRUE;
		}
		else {
			// determining the team
			CSquad&	Squad		= Level().Teams[Me->g_Team()].Squads[Me->g_Squad()];
			// determining who is leader
			CEntity* Leader		= Squad.Leader;
			// checking if the leader exists
			R_ASSERT (Leader);
			// checking if i forget that hit or not
			if (Level().timeServer() - hitTime < HIT_REACTION_TIME) {
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
					SelectorUnderFire&	S	= Me->sfUnderFire;
					// my current position
					S.posMy				= Me->Position();
					// current fire direction
					S.tTargetDirection	= hitDir;
					S.tTargetDirection.normalize();
					S.tTargetDirection.invert();
					// fill array members and exclude myself
					Squad.Groups[Me->g_Group()].GetMemberDedication(S.Members,Me);
					Squad.Groups[Me->g_Group()].Members[0];
					// search for the best node according to the 
					// SelectFollow evaluation function in the radius 35 meteres
					Level().AI.vfSearchNodeInTheDirection(Me->AI_NodeID,Me->Position(),35.f,S,MemberPlaces,hitDir);
					/**
					if (Me->AI_Path.Nodes.size() <= 2)
						Me->AI_Path.bNeedRebuild	= TRUE;
					/**/
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
			else {
				Me->State_Pop();
				return TRUE;
			}
		}
	}
}
	
#define COVER_UNDER_FIRE			10.f
#define DEVIATION_FROM_DIRECTION	100.f
#define WRONG_DIRECTION_PENALTY		100.f

virtual	float HenSelectorUnderFire::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)	// min - best, max - worse
{
	// distance to node
	float fResult = 0.0;//*fDistance*costTravel;
	// node lighting
	fResult += ((float)(tNode->light)/255.f)*costLight;
	if (fResult>BestCost)
		return(fResult);

	// cover
	fResult += COVER_UNDER_FIRE*((float)(tNode->cover[0])/255.f + (float)(tNode->cover[1])/255.f
				+ (float)(tNode->cover[2])/255.f + (float)(tNode->cover[3])/255.f);

	/**/
	Fvector tNodeCentre;
	tNodeCentre.x = (float(tNode->p0.x)/255.f + float(tNode->p1.x)/255.f)/2.f - posMy.x;
	tNodeCentre.y = (float(tNode->p0.y)/255.f + float(tNode->p1.y)/255.f)/2.f - posMy.y;
	tNodeCentre.z = (float(tNode->p0.z)/255.f + float(tNode->p1.z)/255.f)/2.f - posMy.z;

	float cos_alpha = tNodeCentre.dotproduct(tTargetDirection);
	float sin_alpha = sqrt(1 - SQR(cos_alpha));
	float deviation = tNodeCentre.square_magnitude()*sin_alpha;

	fResult += DEVIATION_FROM_DIRECTION*deviation;

	fResult += (cos_alpha < 0.f ? WRONG_DIRECTION_PENALTY : 0.f);
	/**/
	// exit
	return	fResult;
}
