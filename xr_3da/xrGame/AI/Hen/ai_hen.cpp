////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.cpp
//	Created 	: 05.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\xr_weapon_list.h"
#include "ai_hen.h"
#include "ai_hen_selectors.h"

//#define WRITE_LOG

CAI_Hen::CAI_Hen()
{
	dwHitTime = 0;
	tHitDir.set(0,0,1);
	dwSenseTime = 0;
	tSenseDir.set(0,0,1);
	tSavedEnemy = 0;
	tSavedEnemyPosition.set(0,0,0);
	tpSavedEnemyNode = 0;
	dwSavedEnemyNodeID = DWORD(-1);
	dwLostEnemyTime = 0;
	eCurrentState = aiHenFollowMe;
}

CAI_Hen::~CAI_Hen()
{
	// removing all data no more being neded 
	int i;
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete(sndDie[i]);
}

void CAI_Hen::Load	(LPCSTR section)
{ 
	// load parameters from ".ltx" file
	inherited::Load	(section);
	
	Fvector			P = vPosition;
	P.x				+= ::Random.randF();
	P.z				+= ::Random.randF();

	// sounds
	pSounds->Create(sndHit[0],TRUE,	"actor\\bhit_flesh-1");
	pSounds->Create(sndHit[1],TRUE,	"actor\\bhit_flesh-2");
	pSounds->Create(sndHit[2],TRUE,	"actor\\bhit_flesh-3");
	pSounds->Create(sndHit[3],TRUE,	"actor\\bhit_helmet-1");
	pSounds->Create(sndHit[4],TRUE,	"actor\\bullet_hit1");
	pSounds->Create(sndHit[5],TRUE,	"actor\\bullet_hit2");
	pSounds->Create(sndHit[6],TRUE,	"actor\\ric_conc-1");
	pSounds->Create(sndHit[7],TRUE,	"actor\\ric_conc-2");
	pSounds->Create(sndDie[0],TRUE,	"actor\\die0");
	pSounds->Create(sndDie[1],TRUE,	"actor\\die1");
	pSounds->Create(sndDie[2],TRUE,	"actor\\die2");
	pSounds->Create(sndDie[3],TRUE,	"actor\\die3");

	SelectorAttack.Load(section);
	SelectorFollow.Load(section);
	SelectorFreeHunting.Load(section);
	SelectorPursuit.Load(section);
	SelectorUnderFire.Load(section);
}

// when someone hit hen
void CAI_Hen::HitSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwHitTime = Level().timeServer();
	tHitDir.set(D);
	tHitDir.normalize();

	// Play hit-sound
	sound& S				= sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->PlayAtPos	(S,this,vPosition);
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

	// removing from group
	//Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].Member_Remove(this);

	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	// Play sound
	pSounds->PlayAtPos(sndDie[Random.randI(SND_DIE_COUNT)],this,vPosition);
}

// hen update
void CAI_Hen::Update(DWORD DT)
{
	inherited::Update(DT);
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
		CEntity*	E = dynamic_cast<CEntity*>(Known[i].key);
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

IC bool CAI_Hen::bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint) {
	Fvector tMemberDirection;
	tMemberDirection.sub(tMyPoint,tMemberPoint);
	vfNormalizeSafe(tMemberDirection);
	float fAlpha = acosf(tFireVector.dotproduct(tMemberDirection));
	return(fAlpha < PI/10);
	//return(false);
}

bool CAI_Hen::bfCheckPath(AI::Path &Path) {
	CAI_Space &AI = Level().AI;
	for (int i=1; i<(int)Path.Nodes.size(); i++) 
		if (AI.q_mark[Path.Nodes[i]])
			return(false);
	return(true);
}

#define LEFT_NODE(Index)  ((Index + 3) & 3)
#define RIGHT_NODE(Index) ((Index + 5) & 3)

void CAI_Hen::SetLessCoverLook(NodeCompressed *tNode)
{
	//Fvector tWatchDirection;
	for (int i=1, iMaxOpenIndex=0, iMaxOpen = tNode->cover[0]; i<4; i++)
		if (tNode->cover[i] > iMaxOpen) {
			iMaxOpenIndex = i; 
			iMaxOpen = tNode->cover[i];
		}
	
	if (tNode->cover[iMaxOpenIndex]) {
		float fAngleOfView = eye_fov/180.f*PI;
		float fDirection = fAngleOfView/2 + (PI - fAngleOfView)*(float(tNode->cover[iMaxOpenIndex])/255.f + float(tNode->cover[RIGHT_NODE(iMaxOpenIndex)])/255.f)/(2*float(tNode->cover[iMaxOpenIndex])/255.f + float(tNode->cover[LEFT_NODE(iMaxOpenIndex)])/255.f + float(tNode->cover[RIGHT_NODE(iMaxOpenIndex)])/255.f);
		float fSinus,fCosinus;
		_sincos(fDirection,fSinus,fCosinus);
		switch (iMaxOpenIndex) {
			case 0 : {
				tWatchDirection.set(-fSinus,0,fCosinus);
				break;
			}
			case 1 : {
				tWatchDirection.set(fCosinus,0,fSinus);
				break;
			}
			case 2 : {
				tWatchDirection.set(fSinus,0,-fCosinus);
				break;
			}
			case 3 : {
				tWatchDirection.set(-fCosinus,0,-fSinus);
				break;
			}
		}
	}
	else 
		tWatchDirection.set(1,0,0);
	
	q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &(tWatchDirection), 1000);
	q_look.o_look_speed=_FB_look_speed;
}

void CAI_Hen::Attack()
{
	// if no more health then hen is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Attack");
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
		if (!(Enemy.Enemy)) {
			// did we kill the enemy ?
			if ((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) {
				eCurrentState = tStateStack.top();
				tStateStack.pop();
			}
			//  no, we lost him
			else {
				dwLostEnemyTime = Level().timeServer();
				eCurrentState = aiHenPursuit;
			}
			return;
		}
		else {
			if (Enemy.bVisible) {
				bBuildPathToLostEnemy = false;
				// saving an enemy
				tSavedEnemy = Enemy.Enemy;
				tSavedEnemyPosition = Enemy.Enemy->Position();
				tpSavedEnemyNode = Enemy.Enemy->AI_Node;
				dwSavedEnemyNodeID = Enemy.Enemy->AI_NodeID;
				// determining the team
				CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
				// determining who is leader
				CEntity* Leader = Squad.Leader;
				// checking if the leader exists
				R_ASSERT (Leader);
				// checking if leader is dead then make myself a leader
				if (Leader->g_Health() <= 0)
					Leader = this;
				// setting watch mode to false
				bool bWatch = false;
				// get pointer to the class of node estimator 
				// for finding the best node in the area
				CHenSelectorAttack S = SelectorAttack;
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
					S.m_tLeaderNode = DWORD(-1);
				}
				S.m_tHitDir			= tHitDir;
				S.m_dwHitTime		= dwHitTime;
				
				S.m_dwCurTime		= Level().timeServer();
				
				S.m_tMe				= this;
				S.m_tpMyNode		= AI_Node;
				S.m_tMyPosition		= Position();
				
				S.m_tEnemy			= Enemy.Enemy;
				S.m_tEnemyPosition	= Enemy.Enemy->Position();
				S.m_tpEnemyNode		= Enemy.Enemy->AI_Node;
				
				S.taMembers = &(Squad.Groups[g_Group()].Members);

				// checking if I need to rebuild the path i.e. previous search
				// has found better destination node
				if (AI_Path.bNeedRebuild) {
					// building a path from and to
					Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,*(S.m_tpEnemyNode),S.fOptEnemyDistance);
					if (AI_Path.Nodes.size() > 2) {
					// if path is long enough then build travel line
						AI_Path.BuildTravelLine(Position());
					}
					else {
					// if path is too short then clear it (patch for ExecMove)
						AI_Path.TravelPath.clear();
						AI_Path.bNeedRebuild = FALSE;
					}
				} 
				else {
					Squad.Groups[g_Group()].GetAliveMemberInfoWithLeader(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this,Leader);
					// search for the best node according to the 
					// SelectFollow evaluation function in the radius N meteres
					float fOldCost;
					Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
					// if search has found new best node then 
					if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
						AI_Path.DestNode		= S.BestNode;
						AI_Path.bNeedRebuild	= TRUE;
					} 
					else
						// search hasn't found a better node we have to look around
						bWatch = true;
					if (AI_Path.Nodes.size() <= 2)
						AI_Path.bNeedRebuild = TRUE;
				}
				// setting up a look
				q_look.setup(
					AI::AIC_Look::Look, 
					AI::t_Object, 
					&Enemy,
					1000);
				q_look.o_look_speed=_FB_look_speed;
				
				/**/
				// setting up an action
				Fvector tFireVector, tMyPosition = S.m_tMyPosition, tEnemyPosition = S.m_tEnemyPosition;
				tFireVector.x = tMyPosition.x - tEnemyPosition.x;
				tFireVector.y = tMyPosition.y - tEnemyPosition.y;
				tFireVector.z = tMyPosition.z - tEnemyPosition.z;
				vfNormalizeSafe(tFireVector);

				bool bCanKillMember = false;
				if (Leader != this)
					if ((Leader->g_Health() > 0) && (bfCheckForMember(tFireVector,tMyPosition,Leader->Position())))
						bCanKillMember = true;
				if (!bCanKillMember)
					for (int i=0; i<(int)S.taMemberPositions.size(); i++)
						if (((*S.taMembers)[i]->g_Health() > 0) && (bfCheckForMember(tFireVector,tMyPosition,S.taMemberPositions[i]))) {
							bCanKillMember = true;
							break;
						}
					
				if (!bCanKillMember) {
					q_action.setup(AI::AIC_Action::FireBegin);
				}
				else {
					q_action.setup(AI::AIC_Action::FireEnd);
				}
				/**/
				// checking flag to stop processing more states
				//q_action.setup(AI::AIC_Action::FireBegin);
				m_fCurSpeed = m_fMaxSpeed;
				bStopThinking = true;
				return;
			}
			/**/
			else {
				tSavedEnemy = Enemy.Enemy;
				tSavedEnemyPosition = Enemy.Enemy->Position();
				tpSavedEnemyNode = Enemy.Enemy->AI_Node;
				dwSavedEnemyNodeID = Enemy.Enemy->AI_NodeID;
				if ((dwSavedEnemyNodeID != AI_Path.DestNode) || (!bBuildPathToLostEnemy)) {
					bBuildPathToLostEnemy = true;
					// determining the team
					CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
					// determining who is leader
					CEntity* Leader = Squad.Leader;
					// checking if the leader exists
					R_ASSERT (Leader);
					// checking if leader is dead then make myself a leader
					if (Leader->g_Health() <= 0)
						Leader = this;
					// setting watch mode to false
//					bool bWatch = false;
					// get pointer to the class of node estimator 
					// for finding the best node in the area
					CHenSelectorAttack S = SelectorAttack;
					// if i am not a leader then assign leader
					/**/
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
						S.m_tLeaderNode = DWORD(-1);
					}
					/**/
					S.m_tHitDir			= tHitDir;
					S.m_dwHitTime		= dwHitTime;
					
					S.m_dwCurTime		= Level().timeServer();
					
					S.m_tMe				= this;
					S.m_tpMyNode        = AI_Node;
					S.m_tMyPosition		= Position();
					
					S.m_tEnemy			= Enemy.Enemy;
					S.m_tEnemyPosition	= Enemy.Enemy->Position();
					S.m_tpEnemyNode		= Enemy.Enemy->AI_Node;
					
					S.taMembers = &(Squad.Groups[g_Group()].Members);
					// building a path from and to
					AI_Path.DestNode = dwSavedEnemyNodeID;
					Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
					if (AI_Path.Nodes.size() > 2) {
					// if path is long enough then build travel line
						AI_Path.BuildTravelLine(Position());
					}
					else
					// if path is too short then clear it (patch for ExecMove)
						AI_Path.TravelPath.clear();
				}
				q_look.setup(
					AI::AIC_Look::Look, 
					AI::t_Object, 
					&Enemy,
					1000);
				q_look.o_look_speed=_FB_look_speed;
				
				q_action.setup(AI::AIC_Action::FireEnd);

				// checking flag to stop processing more states
				m_fCurSpeed = m_fMaxSpeed;
				bStopThinking = true;
				return;
			}
			/**/
		}
	}
}

void CAI_Hen::Cover()
{
	bStopThinking = true;
}

void CAI_Hen::Defend()
{
	bStopThinking = true;
}

void CAI_Hen::Die()
{
	q_look.setup(0,AI::t_None,0,0	);
	q_action.setup(AI::AIC_Action::FireEnd);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	setActive		(false);
	setEnabled		(false);
	
	bStopThinking	= true;
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
			tStateStack.push(eCurrentState);
			eCurrentState = aiHenAttack;
			return;
		}
		else {
			// checking if I am under fire
			DWORD dwCurTime = Level().timeServer();
			if (dwCurTime - dwHitTime < HIT_JUMP_TIME) {
				tStateStack.push(eCurrentState);
				eCurrentState = aiHenUnderFire;
				return;
			}
			else {
				if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiHenSenseSomething;
					return;
				}
				else {
					// determining the team
					CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
					// determining who is leader
					CEntity* Leader = Squad.Leader;
					// checking if the leader exists
					R_ASSERT (Leader);
					// checking if leader is dead then make myself a leader
					if (Leader->g_Health() <= 0)
						Leader = this;
					// I am leader then go to state "Free Hunting"
					if (Leader == this) {
						eCurrentState = aiHenFreeHunting;
						return;
					}
					else {
						bool bWatch = false;
						// get pointer to the class of node estimator 
						// for finding the best node in the area
						CHenSelectorFollow S = SelectorFollow;
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
							S.m_tLeaderNode = DWORD(-1);
						}
						S.m_tHitDir			= tHitDir;
						S.m_dwHitTime		= dwHitTime;
						
						S.m_dwCurTime		= Level().timeServer();
						
						S.m_tMe				= this;
						S.m_tpMyNode		= AI_Node;
						S.m_tMyPosition		= Position();
						
						S.m_tEnemy			= 0;
						S.m_tEnemyPosition.set(0,0,0);
						S.m_tpEnemyNode		= NULL;
						
						//S.taMembers = &(Squad.Groups[g_Group()].Members);
						// checking if I need to rebuild the path i.e. previous search
						// has found better destination node
						if (AI_Path.bNeedRebuild) {
							Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
							if (AI_Path.Nodes.size() > 2) {
							// if path is long enough then build travel line
								AI_Path.BuildTravelLine(Position());
							}
							else {
							// if path is too short then clear it (patch for ExecMove)
								AI_Path.TravelPath.clear();
								AI_Path.bNeedRebuild = FALSE;
							}
						} 
						else {
							// fill arrays of members and exclude myself
							Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
							// search for the best node according to the 
							// SelectFollow evaluation function in the radius 35 meteres
							float fOldCost;
							Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S, fOldCost);
							// if search has found new best node then 
							if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))) {
								// if old cost minus new cost is a little then hen is too lazy
								// to move there
								AI_Path.DestNode		= S.BestNode;
								AI_Path.bNeedRebuild	= TRUE;
							}
							else { 
								// search hasn't found a better node we have to look around
								bWatch = true;
							}
							
							if (AI_Path.TravelPath.size() < 2)
								AI_Path.bNeedRebuild	= TRUE;
						}
						// setting up a look
						// getting my current node
						NodeCompressed* tNode = Level().AI.Node(AI_NodeID);
						// if we are going somewhere
						SetLessCoverLook(tNode);
						// setting up an action
						q_action.setup(AI::AIC_Action::FireEnd);
						// checking flag to stop processing more states
						m_fCurSpeed = m_fMinSpeed;
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
	// if no more health then hen is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Free hunting");
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
			tStateStack.push(eCurrentState);
			eCurrentState = aiHenAttack;
			return;
		}
		else {
			// checking if I am under fire
			if (Level().timeServer() - dwHitTime < HIT_JUMP_TIME) {
				tStateStack.push(eCurrentState);
				eCurrentState = aiHenUnderFire;
				return;
			}
			else {
				if (Level().timeServer() - dwSenseTime < SENSE_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiHenSenseSomething;
					return;
				}
				else {
					// determining the team
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
					CHenSelectorFreeHunting S = SelectorFreeHunting;
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
						S.m_tLeaderNode = DWORD(-1);
					}
					S.m_tHitDir			= tHitDir;
					S.m_dwHitTime		= dwHitTime;
					
					S.m_dwCurTime		= Level().timeServer();
					
					S.m_tMe				= this;
					S.m_tpMyNode		= AI_Node;
					S.m_tMyPosition		= Position();
					
					S.m_tEnemy			= 0;
					S.m_tEnemyPosition.set(0,0,0);
					S.m_tpEnemyNode		= NULL;
					
					//S.taMembers = Squad.Groups[g_Group()].Members;
					// checking if I need to rebuild the path i.e. previous search
					// has found better destination node
					if (AI_Path.bNeedRebuild) {
						Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
						if (AI_Path.Nodes.size() > 2) {
						// if path is long enough then build travel line
							AI_Path.BuildTravelLine(Position());
						}
						else
						// if path is too short then clear it (patch for ExecMove)
							AI_Path.TravelPath.clear();
					} 
					else {
						// fill arrays of members and exclude myself
						Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
						// SelectFollow evaluation function in the radius 35 meteres
						float fOldCost;
						Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
						// if search has found new best node then 
						if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
							AI_Path.DestNode		= S.BestNode;
							AI_Path.bNeedRebuild	= TRUE;
						} 
						else 
							// search hasn't found a better node we have to look around
							bWatch = true;
						if (AI_Path.TravelPath.size() < 2)
							AI_Path.bNeedRebuild	= TRUE;
					}
					// setting up a look
					// getting my current node
					NodeCompressed* tNode		= Level().AI.Node(AI_NodeID);
					
					SetLessCoverLook(tNode);
					
					q_action.setup(AI::AIC_Action::FireEnd);
					// checking flag to stop processing more states
					m_fCurSpeed = m_fMinSpeed;
					bStopThinking = true;
					return;
				}
			}
		}
	}
}

void CAI_Hen::GoInThisDirection()
{
}

void CAI_Hen::GoToThisPosition()
{
}

void CAI_Hen::HoldPositionUnderFire()
{
	bStopThinking = true;
}

void CAI_Hen::HoldThisPosition()
{
	bStopThinking = true;
}

void CAI_Hen::Pursuit()
{
	// if no more health then hen is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Pursuit");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiHenDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do the enemies exist?
		if (Enemy.Enemy) {
			eCurrentState = aiHenAttack;
			return;
		}
		else {
			DWORD dwCurrentTime = Level().timeServer();
			if (dwCurrentTime - dwLostEnemyTime < LOST_ENEMY_REACTION_TIME) {
				// checking if I am under fire
				if (dwCurrentTime - dwHitTime < HIT_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiHenUnderFire;
					return;
				}
				else {
					if (dwCurrentTime - dwSenseTime < SENSE_JUMP_TIME) {
						tStateStack.push(eCurrentState);
						eCurrentState = aiHenSenseSomething;
						return;
					}
					else {
						// determining the team
						CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
						// determining who is leader
						CEntity* Leader = Squad.Leader;
						// checking if the leader exists
						R_ASSERT (Leader);
						// checking if leader is dead then make myself a leader
						if (Leader->g_Health() <= 0)
							Leader = this;
						// get pointer to the class of node estimator 
						// for finding the best node in the area
						CHenSelectorPursuit S = SelectorPursuit;
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
							S.m_tLeaderNode = DWORD(-1);
						}
						S.m_tHitDir			= tHitDir;
						S.m_dwHitTime		= dwHitTime;
						
						S.m_dwCurTime		= Level().timeServer();
						
						S.m_tMe				= this;
						S.m_tpMyNode		= AI_Node;
						S.m_tMyPosition		= Position();
						
						S.m_tEnemy			= tSavedEnemy;
						S.m_tEnemyPosition	= tSavedEnemyPosition;
						S.m_tpEnemyNode		= tpSavedEnemyNode;
						
						//S.taMembers = Squad.Groups[g_Group()].Members;
						bool bWatch = false;
						// checking if I need to rebuild the path i.e. previous search
						// has found better destination node
						if (AI_Path.bNeedRebuild) {
							Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
							if (AI_Path.Nodes.size() > 2) {
							// if path is long enough then build travel line
								AI_Path.BuildTravelLine(Position());
							}
							else {
							// if path is too short then clear it (patch for ExecMove)
								AI_Path.TravelPath.clear();
								AI_Path.bNeedRebuild = FALSE;
							}
						} 
						else {
							// fill arrays of members and exclude myself
							Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
							// search for the best node according to the 
							// SelectFollow evaluation function in the radius 35 meteres
							float fOldCost;
							Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
							// if search has found new best node then 
							if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
								AI_Path.DestNode		= S.BestNode;
								AI_Path.bNeedRebuild	= TRUE;
							} 
							else
								// search hasn't found a better node we have to look around
								bWatch = true;
							if (AI_Path.TravelPath.size() < 2)
								AI_Path.bNeedRebuild	= TRUE;
						}
						// setting up a look
						// getting my current node
						NodeCompressed* tNode		= Level().AI.Node(AI_NodeID);
						//SetLessCoverLook(tNode);
						Fvector tWatchDirection;
						tWatchDirection.sub(tSavedEnemyPosition,S.m_tMyPosition);
						//if ((tWatchDirection.x == tWatchDirection.y) && (tWatchDirection.y == tWatchDirection.z) && (tWatchDirection.z == 0.f))
							SetLessCoverLook(tNode);
						//else 
						//	q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &tWatchDirection,	1000);
						q_look.o_look_speed=_FB_look_speed;
						// checking flag to stop processing more states
						q_action.setup(AI::AIC_Action::FireEnd);
						bStopThinking = true;
						m_fCurSpeed = m_fMaxSpeed;
						return;
					}
				}
			}
			else {
				eCurrentState = tStateStack.top();
				tStateStack.pop();
				q_action.setup(AI::AIC_Action::FireEnd);
				m_fCurSpeed = m_fMaxSpeed;
				bStopThinking = true;
				return;
			}
		}
	}
}

void CAI_Hen::Retreat()
{
	bStopThinking = true;
}

void CAI_Hen::SenseSomething()
{
	//bStopThinking = true;
	//dwSenseTime = 0;
	// setting up a look to watch at the least safe direction
	q_look.setup(AI::AIC_Look::Look,AI::t_Direction,&tSenseDir,1000);
	// setting up look speed
	q_look.o_look_speed=_FB_look_speed;

	eCurrentState = tStateStack.top();
	tStateStack.pop();
	bStopThinking = true;
}

void CAI_Hen::UnderFire()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Under fire");
#endif
	/**
	//bStopThinking = true;
	// dwHitTime = 0;
	// setting up a look to watch at the least safe direction
	q_look.setup(AI::AIC_Look::Look,AI::t_Direction,&tHitDir,1000);
	// setting up look speed
	q_look.o_look_speed=_FB_look_speed;

	eCurrentState = tStateStack.top();
	tStateStack.pop();
	bStopThinking = true;
	/**/
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
			tStateStack.push(eCurrentState);
			eCurrentState = aiHenAttack;
			return;
		}
		else {
			// checking if I am under fire
			DWORD dwCurTime = Level().timeServer();
			if (dwCurTime - dwHitTime > HIT_REACTION_TIME) {
				eCurrentState = tStateStack.top();
				tStateStack.pop();
				return;
			}
			else {
				if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiHenSenseSomething;
					return;
				}
				else {
					// determining the team
					CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
					// determining who is leader
					CEntity* Leader = Squad.Leader;
					// checking if the leader exists
					R_ASSERT (Leader);
					// checking if leader is dead then make myself a leader
					if (Leader->g_Health() <= 0)
						Leader = this;
					// I am leader then go to state "Free Hunting"
					bool bWatch = false;
					// get pointer to the class of node estimator 
					// for finding the best node in the area
					CHenSelectorUnderFire S = SelectorUnderFire;
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
						S.m_tLeaderNode = DWORD(-1);
					}
					S.m_tHitDir			= tHitDir;
					S.m_dwHitTime		= dwHitTime;
					
					S.m_dwCurTime		= Level().timeServer();
					
					S.m_tMe				= this;
					S.m_tpMyNode		= AI_Node;
					S.m_tMyPosition		= Position();
					
					S.m_tEnemy			= 0;
					S.m_tEnemyPosition.set(0,0,0);
					S.m_tpEnemyNode		= NULL;
					
					S.taMembers = &(Squad.Groups[g_Group()].Members);
					// checking if I need to rebuild the path i.e. previous search
					// has found better destination node
					if (AI_Path.bNeedRebuild) {
						// building a path from and to
						Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
						if (AI_Path.Nodes.size() > 2) {
						// if path is long enough then build travel line
							AI_Path.BuildTravelLine(Position());
						}
						else {
						// if path is too short then clear it (patch for ExecMove)
							AI_Path.TravelPath.clear();
							AI_Path.bNeedRebuild = FALSE;
						}
					} 
					else {
						// fill arrays of members and exclude myself
						Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
						// search for the best node according to the 
						// SelectFollow evaluation function in the radius 35 meteres
						float fOldCost;
						Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
						// if search has found new best node then 
						if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
							AI_Path.DestNode		= S.BestNode;
							AI_Path.bNeedRebuild	= TRUE;
						}
						else
							// search hasn't found a better node we have to look around
							bWatch = true;
						if (AI_Path.TravelPath.size() < 2)
							AI_Path.bNeedRebuild	= TRUE;
					}
					// setting up a look
					// getting my current node
					NodeCompressed* tNode = Level().AI.Node(AI_NodeID);
					// if we are going somewhere
					if (dwCurTime - dwHitTime < HIT_JUMP_TIME) {
						q_look.setup(AI::AIC_Look::Look,AI::t_Direction,&tHitDir,1000);
						
						bool bCanKillMember = false;
						if (Leader)
							if ((Leader->g_Health() > 0) && (bfCheckForMember(tHitDir,S.m_tMyPosition,Leader->Position())))
								bCanKillMember = true;
						if (!bCanKillMember)
							for (int i=0; i<(int)S.taMemberPositions.size(); i++)
								if (((*S.taMembers)[i]->g_Health() > 0) && (bfCheckForMember(tHitDir,S.m_tMyPosition,S.taMemberPositions[i]))) {
									bCanKillMember = true;
									break;
								}
							
						if (!bCanKillMember)
							q_action.setup(AI::AIC_Action::FireBegin);
						else
							q_action.setup(AI::AIC_Action::FireEnd);
						m_fCurSpeed = m_fMinSpeed;
					}
					else {
						SetLessCoverLook(tNode);
						q_action.setup(AI::AIC_Action::FireEnd);
						m_fCurSpeed = m_fMaxSpeed;
					}
					// setting up look speed
					q_look.o_look_speed=_FB_look_speed;
					// checking flag to stop processing more states
					bStopThinking = true;
					return;
				}
			}
		}
	}
}

void CAI_Hen::WaitOnPosition()
{
	bStopThinking = true;
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
