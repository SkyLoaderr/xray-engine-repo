#include "stdafx.h"
#include "entity.h"
#include "ai_fsm.h"
#include "custommonster.h"

IC float sqr(float f) { return f*f; }

namespace AI {
	State*	State::Create	(DWORD type)
	{
		switch (type)	
		{
		case aiDie:					return new _Die;
		case aiGoInThisDirection:	return 0; // new _GoDirection;
		case aiGoToThatPosition:	return 0; // new _GoPosition;
		case aiHoldPosition:		return 0; // new _HoldPosition;
		case aiFreeHunting:			return 0; // new _FreeHunting;
		case aiFollowBackup:		return new _FollowBackup;
		case aiAttack:				return new _Attack;
		case aiPursuit:				return new _Pursuit;
		case aiRetreating:			return 0; //new _Retreating;
		}
		return NULL;
	}


	IC float heuristic_EnemySelect	(CCustomMonster* Me, CEntity* E)
	{
		if (E->g_Team()  == Me->g_Team())	return flt_max;		// don't attack our team
		int	g_strench						= E->g_Armor()+E->g_Health();
		if (g_strench<=0)					return flt_max;		// don't attack dead enemiyes
		
		float	f1	= Me->Position().distance_to_sqr(E->Position());
		float	f2	= float(g_strench);
		return  f1*f2;
	}
	
	struct EnemySelected
	{
		CEntity*	E;
		BOOL		Visible;
		float		Cost;
	};

	void	u_selectEnemy(EnemySelected& S, CCustomMonster* Me)
	{
		// Initiate process
		objVisible&	Known	= Level().Teams[Me->g_Team()].KnownEnemys;
		S.E					= 0;
		S.Visible			= FALSE;
		S.Cost				= flt_max-1;
		if (Known.size()==0)	return;

		// Get visible list
		objSET		Visible;
		Me->ai_Track.o_get	(Visible);
		std::sort			(Visible.begin(),Visible.end());

		// Iterate on known
		for (DWORD i=0; i<Known.size(); i++)
		{
			CEntity*	E = (CEntity*)Known[i].key;
			float		H = heuristic_EnemySelect(Me,E);
			if (H<S.Cost) {
				// Calculate local visibility
				CObject**	ins	 = lower_bound(Visible.begin(),Visible.end(),(CObject*)E);
				BOOL	bVisible = (ins==Visible.end())?FALSE:((E==*ins)?TRUE:FALSE);
				float	cost	 = H*(bVisible?1:_FB_invisible_hscale);
				if (cost<S.Cost)	{
					S.E			= E;
					S.Visible	= bVisible;
					S.Cost		= cost;
				}
			}
		}
	}
	
	//- Die ----------------------------------------------------------------------------
	BOOL _Die::Parse(CCustomMonster* Me)
	{
		Me->q_look.setup		(0,AI::t_None,0,0	);
		Me->q_action.setup		(AI::AIC_Action::FireEnd);
		Me->AI_Path.TravelPath.clear();
		return FALSE;
	}

	//- Follow/Backup ------------------------------------------------------------------
	void _FollowBackup::Hit		(Fvector& D)
	{
		hitTime				= Level().timeServer();
		hitDir.set			(D);
	}

	BOOL _FollowBackup::Parse	(CCustomMonster* Me)
	{
		EnemySelected		Enemy;
		u_selectEnemy		(Enemy,Me);
		if (Enemy.E)		{
			Me->State_Push	(new _Attack());
			return TRUE;
		}

		AIC_Look			&q_look		= Me->q_look;
		AIC_Action			&q_action	= Me->q_action;

		if (Me->AI_Path.bNeedRebuild) 
		{
			Level().AI.q_Path			(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
			Me->AI_Path.BuildTravelLine	(Me->Position());
		} else {
			CSquad&	Squad		= Level().acc_squad(Me->g_Team(),Me->g_Squad());
			CEntity* Leader		= Squad.Leader;
			R_ASSERT (Leader);
			
			// fill it with data
			SelectorFollow&	S	= Me->fuzzyFollow;
			S.posMy				= Me->Position();
			S.posTarget			= Leader->Position();
			Squad.Groups[Me->g_Group()].GetMemberDedication(S.Members,Me);
			
			// *** query and move if needed
			Level().AI.q_Range	(Me->AI_NodeID,Me->Position(),35.f,S);

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
		
		// *** look
		if (Level().timeServer() - hitTime < _FB_hit_RelevantTime)
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
			u_orientate(Me->AI_NodeID,vLook);
			q_look.setup
				(
				AI::AIC_Look::Look, 
				AI::t_Direction, 
				&vLook,
				1000
				);
			q_look.o_look_speed=_FB_look_speed;
		}

		return FALSE;
	}
	void _FollowBackup::u_orientate	(DWORD node, Fvector& look)
	{
		// *** look at focus direction (temporarily at less covered direction)
		NodeCompressed*	NC			= Level().AI.Node(node);
		R_ASSERT		(NC);
		
		// extract 8 directions
		DWORD	ld[8];
		ld[0]	=	DWORD(NC->cover[0]);
		ld[1]	=	(DWORD(NC->cover[0])+DWORD(NC->cover[1]))/2;
		ld[2]	=	DWORD(NC->cover[1]);
		ld[3]	=	(DWORD(NC->cover[1])+DWORD(NC->cover[2]))/2;
		ld[4]	=	DWORD(NC->cover[2]);
		ld[5]	=	(DWORD(NC->cover[2])+DWORD(NC->cover[3]))/2;
		ld[6]	=	DWORD(NC->cover[3]);
		ld[7]	=	(DWORD(NC->cover[3])+DWORD(NC->cover[0]))/2;
		
		// select best one to look at
		DWORD	best = 0;
		if (ld[1]>ld[best])	best=1;
		if (ld[2]>ld[best])	best=2;
		if (ld[3]>ld[best])	best=3;
		if (ld[4]>ld[best])	best=4;
		if (ld[5]>ld[best])	best=5;
		if (ld[6]>ld[best])	best=6;
		if (ld[7]>ld[best])	best=7;
		
		static float dirs[8][2] = {
			-1, 0,	// L
			-1, 1,	// LF
			 0, 1,	// F
			 1, 1,	// FR
			 1, 0,  // R
			 1,-1,  // RB
			 0,-1,  // B
			-1,-1,  // BL
		};
		look.set		(dirs[best][0],0,dirs[best][1]);
		look.normalize	();
	}


	//- Attack -------------------------------------------------------------------------
	BOOL _Attack::Parse(CCustomMonster* Me)
	{
		EnemySelected		Enemy;
		u_selectEnemy		(Enemy,Me);
		if (Enemy.E)		EnemySaved = Enemy.E;
		
		AIC_Look			&q_look		=Me->q_look;
		AIC_Action			&q_action	=Me->q_action;
		
		if (Enemy.E)
		{
			if (Enemy.Visible) 
			{
				bBuildPathToLostEnemy = FALSE;

				//***** attack him
				q_look.setup(
					AI::AIC_Look::Look, 
					AI::t_Object, 
					&Enemy,
					1000);
				q_look.o_look_speed=_FB_look_speed;
				
				// action
				q_action.setup(AI::AIC_Action::FireBegin);
				
				// movement
				if (Me->AI_Path.bNeedRebuild) 
				{
					Level().AI.q_Path			(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
					Me->AI_Path.BuildTravelLine	(Me->Position());
				} else {
					// fill it with data
					CSquad&	Squad		= Level().Teams[Me->g_Team()].Squads[Me->g_Squad()];
					SelectorAttack&	S	= Me->fuzzyAttack;
					S.posMy				= Me->Position();
					S.posTarget			= Enemy.E->Position();
					Squad.Groups[Me->g_Group()].GetMemberDedication(S.Members,Me);
					
					// *** query and move if needed
					Level().AI.q_Range	(Me->AI_NodeID,Me->Position(),30.f,S);
					
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
				return	FALSE;
			} else {
				if (!bBuildPathToLostEnemy)	{
					// We has enemy but it's invisible to us - we or him is under cover
					Level().AI.q_Path			(Me->AI_NodeID, Enemy.E->AI_NodeID, Me->AI_Path);
					Me->AI_Path.BuildTravelLine	(Me->Position());
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
					Me->State_Set		(new _Pursuit(EnemySaved));
				}
			} else {
				// strange condition...?
				Me->State_Pop	();
			}
			return	TRUE;
		}
	}
	//- Pursuit ------------------------------------------------------------------------
	_Pursuit::_Pursuit(CEntity* E) : State(aiPursuit)
	{
		R_ASSERT			(E);
		victim				= E;
		savedPosition		= E->Position();
		savedTime			= Level().timeServer();
		savedNode			= E->AI_NodeID;
		bDirectPathBuilded	= FALSE;
	}
	_Pursuit::_Pursuit() : State(aiPursuit)
	{
		victim				= 0;
		savedPosition.set	(0,0,0);
		savedTime			= 0;
		savedNode			= 0;
		bDirectPathBuilded	= FALSE;
	}
	BOOL _Pursuit::Parse	(CCustomMonster* Me)
	{
		EnemySelected		Enemy;
		u_selectEnemy		(Enemy,Me);

		AIC_Look			&q_look		=Me->q_look;
		AIC_Action			&q_action	=Me->q_action;
		
		if (Enemy.E) 
		{
			// enemy exist - switch to attack
			Me->State_Set	(new _Attack());
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
				
				// action
				q_action.setup(AI::AIC_Action::FireBegin);

				// movement
				if (bDirectPathBuilded || (Me->Position().distance_to(savedPosition)<30)) 
				{
					// Direct path
					if (!bDirectPathBuilded)	{
						Level().AI.q_Path			(Me->AI_NodeID,savedNode,Me->AI_Path);
						Me->AI_Path.BuildTravelLine	(Me->Position());
						bDirectPathBuilded			= TRUE;
					}
				} else {
					// Fuzzy(selectored) path
					if (Me->AI_Path.bNeedRebuild) 
					{
						Level().AI.q_Path			(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
						Me->AI_Path.BuildTravelLine	(Me->Position());
					} else {
						// fill it with data
						CSquad&	Squad		= Level().Teams[Me->g_Team()].Squads[Me->g_Squad()];
						SelectorPursuit& S	= Me->fuzzyPursuit;
						S.posMy				= Me->Position		();
						S.posTarget			= savedPosition;
						Squad.Groups		[Me->g_Group()].GetMemberDedication(S.Members,Me);
						
						// *** query and move if needed
						Level().AI.q_Range	(Me->AI_NodeID,Me->Position(),30.f,S);
						
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
};
