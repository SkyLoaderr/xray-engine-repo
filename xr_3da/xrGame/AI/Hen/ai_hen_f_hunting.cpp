////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_f_hunting.cpp
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Free Hunting"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_hen_cover.h"

	void _HenFreeHunting::Hit		(Fvector& D)
	{
		hitTime				= Level().timeServer();
		hitDir.set			(D);
	}

	BOOL _HenFreeHunting::Parse	(CCustomMonster* Me)
	{
#ifdef WRITE_LOG
		Msg("creature : %s, mode : %s",Me->cName(),"Free hunting");
#endif
		if (Me->g_Health() <= 0) {
			Me->State_Push	(new _HenDie());
			return FALSE;
		}

		bool bWatching = false;
		EnemySelected		Enemy;
		vfSelectEnemy		(Enemy,Me);
		
		// do I see the enemies?
		if (Enemy.E)		{
			Me->State_Push	(new _HenAttack());
			return TRUE;
		}

		AIC_Look			&q_look		= Me->q_look;
		AIC_Action			&q_action	= Me->q_action;

		MemberNodes	MemberPlaces;
		CSquad&	Squad		= Level().Teams[Me->g_Team()].Squads[Me->g_Squad()];
		Squad.Groups[Me->g_Group()].GetMemberPlacementN(MemberPlaces,Me);
		
		if (Me->AI_Path.bNeedRebuild) 
		{
			//Level().AI.q_Path	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
			Level().AI.vfFindTheXestPath	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path, MemberPlaces);
			if (Me->AI_Path.Nodes.size() > 2)
				Me->AI_Path.BuildTravelLine		(Me->Position());
			else
				Me->AI_Path.TravelPath.clear();
		} else {
			CSquad&	Squad		= Level().acc_squad(Me->g_Team(),Me->g_Squad());
			CEntity* Leader		= Squad.Leader;
			R_ASSERT (Leader);
			
			// fill it with data
			SelectorFreeHunting&	S	= Me->sfFreeHunting;
			S.posMy				= Me->Position();
			S.posTarget			= Leader->Position();
			Squad.Groups[Me->g_Group()].GetMemberDedication(S.Members,Me);
			
			// *** query and move if needed
			Level().AI.q_Range	(Me->AI_NodeID,Me->Position(),30.f,S,MemberPlaces);
			//Level().AI.vfFindTheBestNode(Me->AI_NodeID,Me->Position(),35.f,S);

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
				bWatching = true;
			}
		}
		
		// *** look
		if (!bWatching) {
			if (Level().timeServer() - hitTime < HIT_REACTION_TIME)
			{
				// *** look at direction of last hit
				q_look.setup(
					AI::AIC_Look::Look, 
					AI::t_Direction, 
					&hitDir,
					1000);
				q_look.o_look_speed=_FB_look_speed;
			} else
			{
				//u_orientate(Me->AI_NodeID,vLook);
				NodeCompressed* tNode		= Level().AI.Node(Me->AI_NodeID);
				Me->tWatchDirection.y = 0.f;
				Me->tWatchDirection.x = float(tNode->cover[2])/255.f - float(tNode->cover[0])/255.f;
				Me->tWatchDirection.z = float(tNode->cover[1])/255.f - float(tNode->cover[3])/255.f;
				Me->tWatchDirection.normalize();
				q_look.setup
					(
					AI::AIC_Look::Look, 
					AI::t_Direction, 
					&(Me->tWatchDirection),
					1000
					);
				q_look.o_look_speed=_FB_look_speed;
			}
		}
		else {
			NodeCompressed* tNode		= Level().AI.Node(Me->AI_NodeID);
			//fLook.y = 0.f;
			//fLook.x = float(tNode->cover[2])/255.f - float(tNode->cover[0])/255.f;
			//fLook.z = float(tNode->cover[1])/255.f - float(tNode->cover[3])/255.f;
			//fLook.normalize();
			
			Fmatrix M;
			M.rotateY(PI/2);
			M.transform(Me->tWatchDirection);
			/**/
			q_look.setup
				(
				AI::AIC_Look::Look, 
				AI::t_Direction, 
				&(Me->tWatchDirection),
				1000
				);
			q_look.o_look_speed = 15;//_FB_look_speed
			/**/
		}

		return FALSE;
	}

	HenSelectorFreeHuntning::HenSelectorFreeHuntning()
{ 
	Name = "sel_free_hunting"; 
}

virtual	float HenSelectorFreeHuntning::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
	else
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
	
	if (tDirection.z < 0.0)
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
	else
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
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
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
			else
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
			
			if (tDirection.z < 0.0)
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
			else
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
			
			if (fResult>BestCost)
				return(fResult);
		}
	}
	// exit
	//if (fResult<EPS)	bStop = TRUE;
	return	fResult;
}

