////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.cpp
//	Created 	: 05.04.2002
//  Modified 	: 05.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_hen.h"
#include "xr_weapon_list.h"
#include "..\\3dsound.h"
#include "entity.h"
#include "..\\_matrix.h"

IC float sqr(float a) { return a*a; }
const float	ai_hit_relevant_time	= 5.f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI_Hen::CAI_Hen()
{
	State_Push(new AI::_HenFollowMe());
}

CAI_Hen::~CAI_Hen()
{
	int i;
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
}

void	CAI_Hen::Load	(CInifile* ini, const char* section)
{ 
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

void	CAI_Hen::HitSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	State_Get()->Hit		(D);

	// Play hit-sound
	sound3D& S = sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->Play3DAtPos(S,vPosition);
}

void	CAI_Hen::Die		( )
{
	inherited::Die			( );
	q_action.setup			(AI::AIC_Action::FireEnd);
	State_Push				(new AI::_HenDie());
	Fvector				dir;
	AI_Path.Direction	(dir);
	SelectAnimation			(clTransform.k,dir,AI_Path.fSpeed);

	// Play sound
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
}

void CAI_Hen::Update	(DWORD DT)
{
	inherited::Update	(DT);
}

namespace AI {
	IC float vfEnemyHeuristics(CCustomMonster* Me, CEntity* E)
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

	void vfSelectEnemy(EnemySelected& S, CCustomMonster* Me)
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
			float		H = vfEnemyHeuristics(Me,E);
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
	BOOL _HenDie::Parse(CCustomMonster* Me)
	{
		Me->q_look.setup		(0,AI::t_None,0,0	);
		Me->q_action.setup		(AI::AIC_Action::FireEnd);
		Me->AI_Path.TravelPath.clear();
		return FALSE;
	}

	//- Follow/Backup ------------------------------------------------------------------
	void _HenFollowMe::Hit		(Fvector& D)
	{
		hitTime				= Level().timeServer();
		hitDir.set			(D);
	}

	BOOL _HenFollowMe::Parse	(CCustomMonster* Me)
	{
		if (Me->g_Health() <= 0) {
			Me->State_Push	(new _HenDie());
			return FALSE;
		}

		bool bWatch = false;
		EnemySelected		Enemy;
		vfSelectEnemy		(Enemy,Me);
		
		// do I see the enemies?
		if (Enemy.E)		{
			Me->State_Push	(new _HenAttack());
			return TRUE;
		}

		AIC_Look			&q_look		= Me->q_look;
		AIC_Action			&q_action	= Me->q_action;

		if (Me->AI_Path.bNeedRebuild) 
		{
			//Level().AI.q_Path	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
			Level().AI.vfFindTheXestPath	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
			Me->AI_Path.BuildTravelLine		(Me->Position());
		} else {
			CSquad&	Squad		= Level().acc_squad(Me->g_Team(),Me->g_Squad());
			CEntity* Leader		= Squad.Leader;
			R_ASSERT (Leader);
			/**/
			if (Leader == Me) {
				Me->State_Push	(new _HenFreeHunting());
				return FALSE;
			}
			/**/
			
			// fill it with data
			SelectorFollow&	S	= Me->fuzzyFollow;
			S.posMy				= Me->Position();
			S.posTarget			= Leader->Position();
			Squad.Groups[Me->g_Group()].GetMemberDedication(S.Members,Me);
			
			// *** query and move if needed
			Level().AI.q_Range	(Me->AI_NodeID,Me->Position(),35.f,S);
			//Level().AI.vfFindTheBestNode(Me->AI_NodeID,Me->Position(),35.f,S);

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
				bWatch = true;
			}
		}
		
		// *** look
		if (!bWatch) {
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
			
			Fmatrix M;
			M.rotateY(PI/60.f);
			M.transform(Me->tWatchDirection);
			q_look.setup
				(
				AI::AIC_Look::Look, 
				AI::t_Direction, 
				&(Me->tWatchDirection),
				1000
				);
			q_look.o_look_speed = _FB_look_speed;
		}
		return FALSE;
	}
	
	void _HenFollowMe::u_orientate	(DWORD node, Fvector& look)
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
	BOOL _HenAttack::Parse(CCustomMonster* Me)
	{
		if (Me->g_Health() <= 0) {
			Me->State_Push	(new _HenDie());
			return FALSE;
		}
		
		EnemySelected		Enemy;
		vfSelectEnemy		(Enemy,Me);
		if (Enemy.E)		EnemySaved = Enemy.E;
		
		AIC_Look			&q_look		=Me->q_look;
		AIC_Action			&q_action	=Me->q_action;
		
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
				q_action.setup(AI::AIC_Action::FireBegin);
				
				// movement
				if (Me->AI_Path.bNeedRebuild) 
				{
					//Level().AI.q_Path			(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
					//Level().AI.vfFindTheXestPath	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
					Level().AI.vfFindTheXestPath	(Enemy.E->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
					Me->AI_Path.BuildTravelLine	(Me->Position());
				} else {
					// fill it with data
					CSquad&	Squad		= Level().Teams[Me->g_Team()].Squads[Me->g_Squad()];
					SelectorAttack&	S	= Me->fuzzyAttack;
					S.posMy				= Me->Position();
					S.posTarget			= Enemy.E->Position();
					S.tTargetDirection	= Enemy.E->Direction();
					S.tTargetDirection.normalize();
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
						
						//if (S.BestCost < (old_cost-S.laziness)) {
							Me->AI_Path.DestNode		= S.BestNode;
							Me->AI_Path.bNeedRebuild	= TRUE;
						//}
					} else {
						// we doesn't need to move anywhere
					}
				}
				return	FALSE;
			} else {
				if (!bBuildPathToLostEnemy)	{
					// We has enemy but it's invisible to us - we or him is under cover
					//Level().AI.q_Path			(Me->AI_NodeID, Enemy.E->AI_NodeID, Me->AI_Path);
					Level().AI.vfFindTheXestPath(Me->AI_NodeID, Enemy.E->AI_NodeID, Me->AI_Path);
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
					Me->State_Set		(new _HenPursuit(EnemySaved));
				}
			} else {
				// strange condition...?
				Me->State_Pop	();
			}
			return	TRUE;
		}
	}
	//- Pursuit ------------------------------------------------------------------------
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

	void _HenFreeHunting::Hit		(Fvector& D)
	{
		hitTime				= Level().timeServer();
		hitDir.set			(D);
	}

	BOOL _HenFreeHunting::Parse	(CCustomMonster* Me)
	{
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

		if (Me->AI_Path.bNeedRebuild) 
		{
			//Level().AI.q_Path	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
			Level().AI.vfFindTheXestPath	(Me->AI_NodeID,Me->AI_Path.DestNode,Me->AI_Path);
			Me->AI_Path.BuildTravelLine		(Me->Position());
		} else {
			CSquad&	Squad		= Level().acc_squad(Me->g_Team(),Me->g_Squad());
			CEntity* Leader		= Squad.Leader;
			R_ASSERT (Leader);
			
			// fill it with data
			SelectorFreeHunting&	S	= Me->fuzzyFreeHunting;
			S.posMy				= Me->Position();
			S.posTarget			= Leader->Position();
			Squad.Groups[Me->g_Group()].GetMemberDedication(S.Members,Me);
			
			// *** query and move if needed
			Level().AI.q_Range	(Me->AI_NodeID,Me->Position(),30.f,S);
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
	
};
