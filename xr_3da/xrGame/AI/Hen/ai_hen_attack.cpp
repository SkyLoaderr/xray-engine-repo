////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_attack.cpp
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Attack"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_hen_attack.h"

bool bfCheckForMember(Fvector tFireVector, Fvector tMyPoint, Fvector tMemberPoint) {
	Fvector tMemberDirection, tMemberEnemy;
	
	tMemberDirection.x = tMyPoint.x - tMemberPoint.x;
	tMemberDirection.y = tMyPoint.y - tMemberPoint.y;
	tMemberDirection.z = tMyPoint.z - tMemberPoint.z;
	
	tMemberEnemy.x = tFireVector.x - tMemberPoint.x;
	tMemberEnemy.y = tFireVector.y - tMemberPoint.y;
	tMemberEnemy.z = tFireVector.z - tMemberPoint.z;

	float a = (float)sqrt(SQR(tFireVector.x) + SQR(tFireVector.y) + SQR(tFireVector.z));
	float b = (float)sqrt(SQR(tMemberDirection.x) + SQR(tMemberDirection.y) + SQR(tMemberDirection.z));
	float c = (float)sqrt(SQR(tMemberEnemy.x) + SQR(tMemberEnemy.y) + SQR(tMemberEnemy.z));

	if (a*b == 0.f)
		return(false);
	else {
		float cosl = (SQR(c) - SQR(a) - SQR(b))/(2*a*b);
		//return((cosl >= 0) && ((1 - SQR(cosl))*b < 0.5f));
		return(false);
	}
}

BOOL HenAttack::Parse(CCustomMonster* Me)
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",Me->cName(),"Attack!");
#endif
	if (Me->g_Health() <= 0) {
		Me->State_Push	(new HenDie());
		return FALSE;
	}
	
	EnemySelected		Enemy;
	SelectEnemy		(Enemy,Me);
	if (Enemy.E)		EnemySaved = Enemy.E;
	
	AIC_Look			&q_look		=Me->q_look;
	AIC_Action			&q_action	=Me->q_action;
	
	MemberNodes	MemberPlaces;
	MemberPlacement MemberPoints;
	CSquad&	Squad		= Level().Teams[Me->g_Team()].Squads[Me->g_Squad()];
	Squad.Groups[Me->g_Group()].GetMemberPlacementN(MemberPlaces,Me);
	Squad.Groups[Me->g_Group()].GetMemberPlacement(MemberPoints,Me);
	
	if (Enemy.E)
	{
		if (Enemy.Visible) 
		{
			bBuildPathToLostEnemy = FALSE;

			// does the enemy see me?
			
			//***** attack him
			q_look.setup(
				AI::AIC_Look::Look, 
				AI::t_Object, 
				&Enemy,
				1000);
			q_look.o_look_speed=_FB_look_speed;
			
			// action
			/**
			Fvector tFireVector, tMyPosition = Me->Position(), tEnemyPosition = Enemy.E->Position();
			tFireVector.x = tMyPosition.x - tEnemyPosition.x;
			tFireVector.y = tMyPosition.y - tEnemyPosition.y;
			tFireVector.z = tMyPosition.z - tEnemyPosition.z;

			bool bCanKillMember = false;
			for (int i=0; i<MemberPoints.size(); i++)
				if (bfCheckForMember(tFireVector,Me->Position(),MemberPoints[i])) {
					bCanKillMember = true;
					break;
				}
				
			if (!bCanKillMember)
				q_action.setup(AI::AIC_Action::FireBegin);
			else
				q_action.setup(AI::AIC_Action::FireEnd);
			/**/
			q_action.setup(AI::AIC_Action::FireBegin);
			
			// movement
			if (Me->AI_Path.bNeedRebuild) 
			{
				//Level().AI.q_Path			(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
				Level().AI.vfFindTheXestPath	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path,MemberPlaces,*(Enemy.E->AI_Node));
				//Level().AI.vfFindTheXestPath	(Enemy.E->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path,MemberPlaces);
				//Level().AI.vfFindTheXestPath(Enemy.E->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path,MemberPlaces,*(Enemy.E->AI_Node));
				if (Me->AI_Path.Nodes.size() > 1)
					Me->AI_Path.BuildTravelLine	(Me->Position());
				else
					Me->AI_Path.TravelPath.clear();
			} else {
				// fill it with data
				//CSquad&	Squad		= Level().Teams[Me->g_Team()].Squads[Me->g_Squad()];
				SelectorAttack&	S	= Me->fuzzyAttack;
				S.posMy				= Me->Position();
				S.posTarget			= Enemy.E->Position();
				S.tTargetDirection	= Enemy.E->Direction();
				S.tTargetDirection.normalize();
				Squad.Groups[Me->g_Group()].GetMemberDedication(S.Members,Me);
				
				// *** query and move if needed
				//Level().AI.q_Range	(Me->AI_NodeID,Me->Position(),30.f,S,MemberPlaces);
				Level().AI.q_Range	(Enemy.E->AI_NodeID,Enemy.E->Position(),30.f,S,MemberPlaces);
				
				if (Me->AI_Path.DestNode != S.BestNode) 
				{
					NodeCompressed* OLD		= Level().AI.Node(Me->AI_Path.DestNode);
					BOOL			dummy	= FALSE;
					float			old_cost= S.Estimate(
						OLD,
						Level().AI.u_SqrDistance2Node(Me->Position(),OLD),
						dummy);
					
					//if (S.BestCost < (old_cost-S.laziness)) {
						Me->AI_Path.DestNode		= S.BestNode;
						Me->AI_Path.bNeedRebuild	= TRUE;
					//}
				} else {
					// we doesn't need to move anywhere
					if (Me->AI_Path.Nodes.size() > 2)
						Me->AI_Path.TravelPath.clear();
				}
			}
			return	FALSE;
		} else {
			if (!bBuildPathToLostEnemy)	{
				// We has enemy but it's invisible to us - we or him is under cover
				//Level().AI.q_Path			(Me->AI_NodeID, Enemy.E->AI_NodeID, Me->AI_Path);
				Level().AI.vfFindTheXestPath(Me->AI_NodeID, Enemy.E->AI_NodeID, Me->AI_Path, MemberPlaces);
				if (Me->AI_Path.Nodes.size() > 2)
					Me->AI_Path.BuildTravelLine	(Me->Position());
				else
					Me->AI_Path.TravelPath.clear();
				bBuildPathToLostEnemy		= TRUE;
			}
			// look
			q_look.setup(
				AI::AIC_Look::Look, 
				AI::t_Object,
				&EnemySaved,
				1000);
			q_look.o_look_speed=_FB_look_speed;

			// action
			// if (Enemy.E->Position().distance_to(Me->Position())<
			q_action.setup	(AI::AIC_Action::FireEnd);

			return	FALSE;
		}
	} else {
		// we lost / killed enemy
		q_action.setup(AI::AIC_Action::FireEnd);
		
		if (EnemySaved) {
			if (EnemySaved->g_Health()<=0) {
				// we killed him - return to previous state
				Me->State_Pop		();
			} else {
				// we lost him :(
				Me->State_Set		(new _HenPursuit(EnemySaved));
			}
		} else {
			// strange condition...?
			Me->State_Pop	();
		}
		return	TRUE;
	}
}

#define OPTIMAL_ENEMY_DISTANCE  5.f
#define MIN_ENEMY_PENALTY		0.f
#define MAX_ENEMY_PENALTY		10.f
#define MIN_ENEMY_DISTANCE		distTargetMin
#define MAX_ENEMY_DISTANCE		distTargetMax
#define ENEMY_VIEW_PENALTY		1000.f
#define SURROUND_PENALTY		1000.f
#define COVER_PENALTY			100.f

HenSelectorAttack::HenSelectorAttack()
{ 
	Name = "sel_attack"; 
}

virtual	float HenSelectorAttack::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
	
	if (fDistanceToPlayer < MIN_ENEMY_DISTANCE)
		fResult += MIN_ENEMY_PENALTY*costTarget;
	else
		if (fDistanceToPlayer > MAX_ENEMY_DISTANCE)
			fResult += MAX_ENEMY_PENALTY*costTarget;
		else
			fResult += (fDistanceToPlayer - OPTIMAL_ENEMY_DISTANCE)/OPTIMAL_ENEMY_DISTANCE;
	if (fResult>BestCost)
		return(fResult);
	
	// cover from leader
	Fvector tDirection;
	tDirection.x = posTarget.x - P.x;
	tDirection.y = fabs(posTarget.y - P.y);
	tDirection.z = posTarget.z - P.z;

	if (tDirection.x < 0.0)
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
	else
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
	
	if (tDirection.z < 0.0)
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
	else
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
	
	if (fResult>BestCost)
		return(fResult);
	
	tDirection.normalize(); 
	
	//fResult += ENEMY_VIEW_PENALTY*(1.f - sqrt(SQR(tDirection.x - tTargetDirection.x) + SQR(tDirection.y - tTargetDirection.y) + SQR(tDirection.z - tTargetDirection.z)));
	//if (fResult>BestCost)
	//	return(fResult);
	
	//if (tDirection.y > 2.0)
	//	fResult += tDirection.y*Y_PENALTY;

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
			if (fResult>BestCost)
				return(fResult);
			
			Fvector tDirection1;
			tDirection1.x += Members[it].x - P.x;
			tDirection1.y += fabs(Members[it].y - P.y);
			tDirection1.z += Members[it].z - P.z;
			tDirection1.normalize();
			tDirection.add(tDirection1);

			/**/
			if (Members[it].x - P.x < 0.0)
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
			else
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
			
			if (Members[it].z - P.z < 0.0)
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
			else
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);

			/**/
		}
	}

	fResult += SURROUND_PENALTY*sqrt(SQR(tDirection.x) + SQR(tDirection.y) + SQR(tDirection.z));

	// exit
	//if (fResult<EPS)	bStop = TRUE;
	return	fResult;
}

