////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_pursuit.cpp
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Pursuit"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_hen_cover.h"
	_HenPursuit::_HenPursuit(CEntity* E) : State(aiHenPursuit)
	{
		R_ASSERT			(E);
		victim				= E;
		savedPosition		= E->Position();
		savedTime			= Level().timeServer();
		savedNode			= E->AI_NodeID;
		bDirectPathBuilded	= FALSE;
	}
	_HenPursuit::_HenPursuit() : State(aiHenPursuit)
	{
		victim				= 0;
		savedPosition.set	(0,0,0);
		savedTime			= 0;
		savedNode			= 0;
		bDirectPathBuilded	= FALSE;
	}
	BOOL _HenPursuit::Parse	(CCustomMonster* Me)
	{
#ifdef WRITE_LOG
		Msg("creature : %s, mode : %s",Me->cName(),"Pursuit");
#endif
		EnemySelected		Enemy;
		vfSelectEnemy		(Enemy,Me);

		AIC_Look			&q_look		=Me->q_look;
		AIC_Action			&q_action	=Me->q_action;
		
		if (Me->g_Health() <= 0) {
			Me->State_Push	(new _HenDie());
			return FALSE;
		}
		
		if (Enemy.E) 
		{
			// enemy exist - switch to attack
			Me->State_Set	(new _HenAttack());
			return			TRUE;
		} else {
			DWORD dwTimeElapsed = Level().timeServer()-savedTime;
			if (dwTimeElapsed > 30*1000) 
			{
				Me->State_Pop	();
				return			TRUE;
			} else {
				// predict it's position
				PositionPredicted	= victim->Position();
				
				// look
				q_look.setup(
					AI::AIC_Look::Look, 
					AI::t_Point, 
					&PositionPredicted,
					1000);
				q_look.o_look_speed=PI/2;
				
				// movement
				MemberNodes	MemberPlaces;
				MemberPlacement MemberPoints;
				CSquad&	Squad		= Level().Teams[Me->g_Team()].Squads[Me->g_Squad()];
				Squad.Groups[Me->g_Group()].GetMemberPlacementN(MemberPlaces,Me);

				// action
				if (Enemy.E) {
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

				}
				else {
					q_action.setup(AI::AIC_Action::FireEnd);
				}
				
				if (bDirectPathBuilded || (Me->Position().distance_to(savedPosition)<30)) 
				{
					// Direct path
					if (!bDirectPathBuilded)	{
						Level().AI.vfFindTheXestPath	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path, MemberPlaces);
						if (Me->AI_Path.Nodes.size() > 2)
							Me->AI_Path.BuildTravelLine	(Me->Position());
						else
							Me->AI_Path.TravelPath.clear();
						bDirectPathBuilded			= TRUE;
					}
				} else {
					// Fuzzy(selectored) path
					if (Me->AI_Path.bNeedRebuild) 
					{
						Level().AI.vfFindTheXestPath	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path, MemberPlaces);
						if (Me->AI_Path.Nodes.size() > 2)
							Me->AI_Path.BuildTravelLine	(Me->Position());
						else
							Me->AI_Path.TravelPath.clear();
					} else {
						// fill it with data
						CSquad&	Squad		= Level().Teams[Me->g_Team()].Squads[Me->g_Squad()];
						SelectorPursuit& S	= Me->fuzzyPursuit;
						S.posMy				= Me->Position		();
						S.posTarget			= savedPosition;
						Squad.Groups		[Me->g_Group()].GetMemberDedication(S.Members,Me);
						
						// *** query and move if needed
						Level().AI.q_Range	(Me->AI_NodeID,Me->Position(),30.f,S,MemberPlaces);
						
						if (Me->AI_Path.DestNode != S.BestNode) 
						{
							NodeCompressed* OLD		= Level().AI.Node(Me->AI_Path.DestNode);
							BOOL			dummy	= FALSE;
							float			old_cost= S.Estimate(
								OLD,
								Level().AI.u_SqrDistance2Node(Me->Position(),OLD),
								dummy);
							
							if (S.BestCost < (old_cost-S.laziness)) {
								Me->AI_Path.DestNode		= S.BestNode;
								Me->AI_Path.bNeedRebuild	= TRUE;
							}
						} else {
							// we doesn't need to move anywhere
						}
					}
				}
			}
		}
		return FALSE;
	}

	#define COVER_FREEHUNTING 1000.f
	
HenSelectorPursuit::HenSelectorPursuit()
{ 
	Name = "sel_pursuit"; 
}

virtual	float HenSelectorPursuit::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// distance to node
	float fResult = 0.0;//*fDistance*costTravel;
	// node lighting
	fResult += ((float)(tNode->light)/255.f)*costLight;
	if (fResult>BestCost)
		return(fResult);

	// cover
	fResult += COVER_FREEHUNTING*((float)(tNode->cover[0])/255.f + (float)(tNode->cover[1])/255.f
				+ (float)(tNode->cover[2])/255.f + (float)(tNode->cover[3])/255.f);

	// exit
	//if (fResult<EPS)	bStop = TRUE;
	return	fResult;
}

