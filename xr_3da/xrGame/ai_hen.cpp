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

//#define WRITE_LOG

IC float sqr(float a) { return a*a; }
const float	ai_hit_relevant_time	= 5.f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI_Hen::CAI_Hen()
{
	// creating initial state
	State_Push(new AI::_HenFollowMe());
}

CAI_Hen::~CAI_Hen()
{
	// removing all data no more being neded 
	int i;
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
}

void	CAI_Hen::Load	(CInifile* ini, const char* section)
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
void	CAI_Hen::HitSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	State_Get()->Hit(D);

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
	State_Get()->Sense(D);
}

// when hen is dead
void	CAI_Hen::Die		( )
{
	// perform death operations
	inherited::Die			( );
	q_action.setup			(AI::AIC_Action::FireEnd);
	State_Push				(new AI::_HenDie());
	Fvector				dir;
	AI_Path.Direction	(dir);
	SelectAnimation			(clTransform.k,dir,AI_Path.fSpeed);

	// Play sound
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
}

// hen update
void CAI_Hen::Update	(DWORD DT)
{
	inherited::Update	(DT);
}

namespace AI {

struct EnemySelected
{
	CEntity*	E;
	BOOL		Visible;
	float		Cost;
};

IC float vfEnemyHeuristics(CCustomMonster* Me, CEntity* E)
{
	if (E->g_Team()  == Me->g_Team())	return flt_max;		// don't attack our team
	int	g_strench						= E->g_Armor()+E->g_Health();
	if (g_strench<=0)					return flt_max;		// don't attack dead enemiyes
	
	float	f1	= Me->Position().distance_to_sqr(E->Position());
	float	f2	= float(g_strench);
	return  f1*f2;
}

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
	
//******************************************************************
// Death
//******************************************************************
BOOL _HenDie::Parse(CCustomMonster* Me)
{
	Me->q_look.setup		(0,AI::t_None,0,0	);
	Me->q_action.setup		(AI::AIC_Action::FireEnd);
	Me->AI_Path.TravelPath.clear();
	return FALSE;
}

//******************************************************************
// Mode "Under Fire!"
//******************************************************************

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
	
//******************************************************************
// Mode "Follow me"
//******************************************************************

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
	
//******************************************************************
// Mode "Attack!"
//******************************************************************
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
	//- Attack -------------------------------------------------------------------------
	BOOL _HenAttack::Parse(CCustomMonster* Me)
	{
#ifdef WRITE_LOG
		Msg("creature : %s, mode : %s",Me->cName(),"Attack!");
#endif
		if (Me->g_Health() <= 0) {
			Me->State_Push	(new _HenDie());
			return FALSE;
		}
		
		EnemySelected		Enemy;
		vfSelectEnemy		(Enemy,Me);
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
	
//******************************************************************
// Mode "Sense Something"
//******************************************************************

_HenSenseSomething::_HenSenseSomething() : State(aiHenSenseSomething)
{
	hitTime	= 0;
	hitDir.set(0,0,1);
	senseTime = 0;
	senseDir.set(0,0,1);
}

void _HenSenseSomething::Hit		(Fvector& D)
{
	hitTime				= Level().timeServer();
	// saving hit direction
	hitDir.set			(D);
}

void _HenSenseSomething::Sense	(Fvector& D)
{
	senseTime				= Level().timeServer();
	// saving hit direction
	senseDir.set			(D);
}

BOOL _HenSenseSomething::Parse	(CCustomMonster* Me)
{
	// if no more healtt then hen is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",Me->cName(),"Sense something");
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
				}
				else {
					Me->State_Pop();
					return TRUE;
				}
			}
		}
	}
}
	
};
