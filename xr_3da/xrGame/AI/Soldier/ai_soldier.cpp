////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\..\\3dsound.h"
#include "ai_soldier.h"
#include "ai_soldier_selectors.h"
#include "..\\..\\..\\bodyinstance.h"

#define WRITE_LOG
#define MIN_RANGE_SWITCH 500

CAI_Soldier::CAI_Soldier()
{
	dwHitTime = 0;
	tHitDir.set(0,0,1);
	dwSenseTime = 0;
	tSenseDir.set(0,0,1);
	tSavedEnemy = 0;
	tSavedEnemyPosition.set(0,0,0);
	tpSavedEnemyNode = 0;
	dwSavedEnemyNodeID = -1;
	dwLostEnemyTime = 0;
	eCurrentState = aiSoldierFollowMe;
	m_dwLastRangeSearch = 0;
}

CAI_Soldier::~CAI_Soldier()
{
	// removing all data no more being neded 
	int i;
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
}

void CAI_Soldier::Load(CInifile* ini, const char* section)
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

	SelectorAttack.Load(ini,section);
	SelectorFindEnemy.Load(ini,section);
	SelectorFollow.Load(ini,section);
	SelectorFreeHunting.Load(ini,section);
	SelectorPursuit.Load(ini,section);
	SelectorUnderFire.Load(ini,section);

	m_tpaDeathAnimations[0] = m_death;
	m_tpaDeathAnimations[1] = PKinematics(pVisual)->ID_Cycle_Safe("norm_death_2");

}

// when someone hit soldier
void CAI_Soldier::HitSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwHitTime = Level().timeServer();
	tHitDir.set(D);
	tHitDir.normalize();

	// Play hit-sound
	sound3D& S = sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->Play3DAtPos(S,vPosition);
}

// when someone hit soldier
void CAI_Soldier::SenseSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwSenseTime = Level().timeServer();
	tSenseDir.set(D);
}

// when soldier is dead
void CAI_Soldier::Death()
{
	// perform death operations
	inherited::Death( );
	q_action.setup(AI::AIC_Action::FireEnd);
	eCurrentState = aiSoldierDie;

	// removing from group
	//Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].Member_Remove(this);

	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	// Play sound
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
}

// soldier update
void CAI_Soldier::Update(DWORD DT)
{
	inherited::Update(DT);
}

float CAI_Soldier::EnemyHeuristics(CEntity* E)
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

void CAI_Soldier::SelectEnemy(SEnemySelected& S)
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

IC bool CAI_Soldier::bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint) {
	Fvector tMemberDirection;
	tMemberDirection.sub(tMyPoint,tMemberPoint);
	vfNormalizeSafe(tMemberDirection);
	float fAlpha = acosf(tFireVector.dotproduct(tMemberDirection));
	return(fAlpha < PI/10);
	//return(false);
}

bool CAI_Soldier::bfCheckPath(AI::Path &Path) {
	const vector<BYTE> &q_mark = Level().AI.tpfGetNodeMarks();
	for (int i=1; i<Path.Nodes.size(); i++) 
		if (q_mark[Path.Nodes[i]])
			return(false);
	return(true);
}

#define LEFT_NODE(Index)  ((Index + 3) & 3)
#define RIGHT_NODE(Index) ((Index + 5) & 3)

void CAI_Soldier::SetLessCoverLook(NodeCompressed *tNode)
{
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

void CAI_Soldier::SetSmartLook(NodeCompressed *tNode, Fvector &tEnemyDirection)
{
	Fvector tLeft;
	Fvector tRight;
	Fvector tFront;
	Fvector tBack;
	tLeft.set(-1,0,0);
	tRight.set(1,0,0);
	tFront.set(0,0,1);
	tBack.set(0,0,-1);

	tEnemyDirection.normalize();
		
	float fCover = 0;
	if ((tEnemyDirection.x < 0) && (tEnemyDirection.z > 0)) {
		float fAlpha = acosf(tEnemyDirection.dotproduct(tFront));
		fCover = cosf(fAlpha)*tNode->cover[0] + sinf(fAlpha)*tNode->cover[1];
	}
	else 
		if ((tEnemyDirection.x > 0) && (tEnemyDirection.z > 0)) {
			float fAlpha = acosf(tEnemyDirection.dotproduct(tFront));
			fCover = cosf(fAlpha)*tNode->cover[1] + sinf(fAlpha)*tNode->cover[2];
		}
		else 
			if ((tEnemyDirection.x > 0) && (tEnemyDirection.z < 0)) {
				float fAlpha = acosf(tEnemyDirection.dotproduct(tBack));
				fCover = cosf(fAlpha)*tNode->cover[3] + sinf(fAlpha)*tNode->cover[2];
			}
			else 
				if ((tEnemyDirection.x < 0) && (tEnemyDirection.z < 0)) {
					float fAlpha = acosf(tEnemyDirection.dotproduct(tBack));
					fCover = cosf(fAlpha)*tNode->cover[3] + sinf(fAlpha)*tNode->cover[0];
				}
	

	Msg("%8.2f",fCover);
	if (fCover > 0.2f*255.f) {
		q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &(tEnemyDirection), 1000);
		q_look.o_look_speed=8*_FB_look_speed;
	}
	else {
		SetLessCoverLook(tNode);
		q_look.o_look_speed=8*_FB_look_speed;
	}
}

void CAI_Soldier::Attack()
{
	// if no more health then soldier is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Attack");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
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
				q_action.setup(AI::AIC_Action::FireEnd);
				m_dwLastRangeSearch = Level().timeServer();
			}
			//  no, we lost him
			else {
				dwLostEnemyTime = Level().timeServer();
				eCurrentState = aiSoldierPursuit;
				q_action.setup(AI::AIC_Action::FireEnd);
				m_dwLastRangeSearch = Level().timeServer();
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
				CSoldierSelectorAttack S = SelectorAttack;
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
					S.m_tLeaderNode = -1;
				}
				S.m_tHitDir			= tHitDir;
				S.m_dwHitTime		= dwHitTime;
				
				S.m_dwCurTime		= Level().timeServer();
				//Msg("%d : %d",S.m_dwHitTime,S.m_dwCurTime);
				
				S.m_tMe				= this;
				S.m_tpMyNode		= AI_Node;
				S.m_tMyPosition		= Position();
				
				S.m_tEnemy			= Enemy.Enemy;
				S.m_tEnemyPosition	= Enemy.Enemy->Position();
				S.m_tpEnemyNode		= Enemy.Enemy->AI_Node;
				
				S.taMembers = &(Squad.Groups[g_Group()].Members);
				
				//if (S.m_tLeader)
				//	S.taMembers.push_back(S.m_tLeader);

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
				else 
					if (S.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SWITCH){
						m_dwLastRangeSearch = S.m_dwCurTime;
						Device.Statistic.AI_Node.Begin();
						Squad.Groups[g_Group()].GetAliveMemberInfoWithLeader(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this,Leader);
						Device.Statistic.AI_Node.End();
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
				if (Leader)
					if ((Leader->g_Health() > 0) && (bfCheckForMember(tFireVector,tMyPosition,Leader->Position())))
						bCanKillMember = true;
				if (!bCanKillMember)
					for (int i=0; i<S.taMemberPositions.size(); i++)
						if (((*S.taMembers)[i]->g_Health() > 0) && (bfCheckForMember(tFireVector,tMyPosition,S.taMemberPositions[i]))) {
							bCanKillMember = true;
							break;
						}
					
				if (!bCanKillMember)
					q_action.setup(AI::AIC_Action::AttackBegin);
				else
					q_action.setup(AI::AIC_Action::FireEnd);
				/**/
				// checking flag to stop processing more states
				//q_action.setup(AI::AIC_Action::AttackBegin);
				m_fCurSpeed = m_fMaxSpeed;
				bStopThinking = true;
				return;
			}
			/**/
			else {
				/**/
				tSavedEnemy = Enemy.Enemy;
				tSavedEnemyPosition = Enemy.Enemy->Position();
				tpSavedEnemyNode = Enemy.Enemy->AI_Node;
				dwSavedEnemyNodeID = Enemy.Enemy->AI_NodeID;
				if ((dwSavedEnemyNodeID != AI_Path.DestNode) || (!bBuildPathToLostEnemy)) {
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
					// building a path from and to
					AI_Path.DestNode = dwSavedEnemyNodeID;
					Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
					if (AI_Path.Nodes.size() > 2) {
					// if path is long enough then build travel line
						bBuildPathToLostEnemy = true;
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
				/**
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
				CSoldierSelectorFindEnemy S = SelectorFindEnemy;
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
					S.m_tLeaderNode = -1;
				}
				S.m_tHitDir			= tHitDir;
				S.m_dwHitTime		= dwHitTime;
				
				S.m_dwCurTime		= Level().timeServer();
				//Msg("%d : %d",S.m_dwHitTime,S.m_dwCurTime);
				
				S.m_tMe				= this;
				S.m_tpMyNode		= AI_Node;
				S.m_tMyPosition		= Position();
				
				S.m_tEnemy			= Enemy.Enemy;
				S.m_tEnemyPosition	= Enemy.Enemy->Position();
				S.m_tpEnemyNode		= Enemy.Enemy->AI_Node;
				
				S.m_tLastEnemyPosition	= tSavedEnemyPosition;

				if (AI_Path.bNeedRebuild) {
					// building a path from and to
					Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,*(S.m_tpEnemyNode),S.fOptEnemyDistance);
					if (AI_Path.Nodes.size() >= 2) {
					// if path is long enough then build travel line
						AI_Path.BuildTravelLine(Position());
					}
					else {
					// if path is too short then clear it (patch for ExecMove)
						AI_Path.TravelPath.clear();
						AI_Path.bNeedRebuild = FALSE;
					}
				} 
				else 
					//if (S.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SWITCH)
					{
						m_dwLastRangeSearch = S.m_dwCurTime;
						Device.Statistic.AI_Node.Begin();
						Squad.Groups[g_Group()].GetAliveMemberInfoWithLeader(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this,Leader);
						Device.Statistic.AI_Node.End();
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
				
				tWatchDirection.sub(tSavedEnemyPosition,Position());
				if (tWatchDirection.magnitude() > 0.0001f)
					SetSmartLook(AI_Node,tWatchDirection);
				else
					SetLessCoverLook(AI_Node);

				q_action.setup(AI::AIC_Action::FireEnd);
				
				m_fCurSpeed = m_fMaxSpeed;
				bStopThinking = true;
				return;
				/**/
			}
			/**/
		}
	}
}

void CAI_Soldier::Cover()
{
	bStopThinking = true;
}

void CAI_Soldier::Defend()
{
	bStopThinking = true;
}

void CAI_Soldier::Die()
{
	q_look.setup(0,AI::t_None,0,0	);
	q_action.setup(AI::AIC_Action::FireEnd);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	bStopThinking = true;
}

void CAI_Soldier::FollowMe()
{
	// if no more health then soldier is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Follow me");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do I see the enemies?
		if (Enemy.Enemy)		{
			tStateStack.push(eCurrentState);
			eCurrentState = aiSoldierAttack;
			m_dwLastRangeSearch = Level().timeServer();
			return;
		}
		else {
			// checking if I am under fire
			DWORD dwCurTime = Level().timeServer();
			if (dwCurTime - dwHitTime < HIT_JUMP_TIME) {
				tStateStack.push(eCurrentState);
				eCurrentState = aiSoldierUnderFire;
				m_dwLastRangeSearch = dwCurTime;
				return;
			}
			else {
				if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiSoldierSenseSomething;
					m_dwLastRangeSearch = dwCurTime;
					return;
				}
				else {
					CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
					CGroup& Group = Squad.Groups[g_Group()];
					if (dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) {
						tHitDir = Group.m_tLastHitDirection;
						dwHitTime = Group.m_dwLastHitTime;
						tStateStack.push(eCurrentState);
						eCurrentState = aiSoldierUnderFire;
						m_dwLastRangeSearch = Level().timeServer();
						return;
					}
					else {
						// determining who is leader
						CEntity* Leader = Squad.Leader;
						// checking if the leader exists
						R_ASSERT (Leader);
						// checking if leader is dead then make myself a leader
						if (Leader->g_Health() <= 0)
							Leader = this;
						// I am leader then go to state "Free Hunting"
						if (Leader == this) {
							eCurrentState = aiSoldierFreeHunting;
							return;
						}
						else {
							bool bWatch = false;
							// get pointer to the class of node estimator 
							// for finding the best node in the area
							CSoldierSelectorFollow S = SelectorFollow;
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
								S.m_tLeaderNode = -1;
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
								else {
								// if path is too short then clear it (patch for ExecMove)
									AI_Path.TravelPath.clear();
									AI_Path.bNeedRebuild = FALSE;
								}
							} 
							else 
								if (S.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SWITCH) {
									m_dwLastRangeSearch = S.m_dwCurTime;
									// fill arrays of members and exclude myself
									Device.Statistic.AI_Node.Begin();
									Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
									Device.Statistic.AI_Node.End();
									// search for the best node according to the 
									// SelectFollow evaluation function in the radius 35 meteres
									float fOldCost;
									Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S, fOldCost);
									// if search has found new best node then 
									if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))) {
										// if old cost minus new cost is a little then soldier is too lazy
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
}

void CAI_Soldier::FreeHunting()
{
	// if no more health then soldier is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Free hunting");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do I see the enemies?
		if (Enemy.Enemy)		{
			tStateStack.push(eCurrentState);
			eCurrentState = aiSoldierAttack;
			m_dwLastRangeSearch = Level().timeServer();
			return;
		}
		else {
			// checking if I am under fire
			if (Level().timeServer() - dwHitTime < HIT_JUMP_TIME) {
				tStateStack.push(eCurrentState);
				eCurrentState = aiSoldierUnderFire;
				m_dwLastRangeSearch = Level().timeServer();
				return;
			}
			else {
				if (Level().timeServer() - dwSenseTime < SENSE_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiSoldierSenseSomething;
					m_dwLastRangeSearch = Level().timeServer();
					return;
				}
				else {
					CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
					CGroup& Group = Squad.Groups[g_Group()];
					if (Level().timeServer() - Group.m_dwLastHitTime < HIT_JUMP_TIME) {
						tHitDir = Group.m_tLastHitDirection;
						dwHitTime = Group.m_dwLastHitTime;
						tStateStack.push(eCurrentState);
						eCurrentState = aiSoldierUnderFire;
						m_dwLastRangeSearch = Level().timeServer();
						return;
					}
					else {
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
						CSoldierSelectorFreeHunting S = SelectorFreeHunting;
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
							S.m_tLeaderNode = -1;
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
						else 
							if (S.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SWITCH){
								m_dwLastRangeSearch = S.m_dwCurTime;
								// fill arrays of members and exclude myself
								Device.Statistic.AI_Node.Begin();
								Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
								Device.Statistic.AI_Node.End();
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
}

void CAI_Soldier::GoInThisDirection()
{
}

void CAI_Soldier::GoToThisPosition()
{
}

void CAI_Soldier::HoldPositionUnderFire()
{
	bStopThinking = true;
}

void CAI_Soldier::HoldThisPosition()
{
	bStopThinking = true;
}

void CAI_Soldier::Pursuit()
{
	// if no more health then soldier is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Pursuit");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do the enemies exist?
		if (Enemy.Enemy) {
			eCurrentState = aiSoldierAttack;
			m_dwLastRangeSearch = Level().timeServer();
			return;
		}
		else {
			DWORD dwCurrentTime = Level().timeServer();
			if (dwCurrentTime - dwLostEnemyTime < LOST_ENEMY_REACTION_TIME) {
				// checking if I am under fire
				if (dwCurrentTime - dwHitTime < HIT_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiSoldierUnderFire;
					m_dwLastRangeSearch = Level().timeServer();
					return;
				}
				else
					if (dwCurrentTime - dwSenseTime < SENSE_JUMP_TIME) {
						tStateStack.push(eCurrentState);
						eCurrentState = aiSoldierSenseSomething;
						m_dwLastRangeSearch = Level().timeServer();
						return;
					}
					else {
						CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
						CGroup& Group = Squad.Groups[g_Group()];
						if (dwCurrentTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) {
							tHitDir = Group.m_tLastHitDirection;
							dwHitTime = Group.m_dwLastHitTime;
							tStateStack.push(eCurrentState);
							eCurrentState = aiSoldierUnderFire;
							m_dwLastRangeSearch = Level().timeServer();
							return;
						}
						else {
							// determining who is leader
							CEntity* Leader = Squad.Leader;
							// checking if the leader exists
							R_ASSERT (Leader);
							// checking if leader is dead then make myself a leader
							if (Leader->g_Health() <= 0)
								Leader = this;
							// get pointer to the class of node estimator 
							// for finding the best node in the area
							CSoldierSelectorPursuit S = SelectorPursuit;
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
								S.m_tLeaderNode = -1;
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
							else 
								if (S.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SWITCH){
									m_dwLastRangeSearch = S.m_dwCurTime;
									// fill arrays of members and exclude myself
									Device.Statistic.AI_Node.Begin();
									Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
									Device.Statistic.AI_Node.End();
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
							tWatchDirection.sub(tSavedEnemyPosition,Position());
							if (tWatchDirection.magnitude() > 0.0001f)
								SetSmartLook(AI_Node,tWatchDirection);
							else
								SetLessCoverLook(AI_Node);
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

void CAI_Soldier::Retreat()
{
	bStopThinking = true;
}

void CAI_Soldier::SenseSomething()
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

void CAI_Soldier::UnderFire()
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
		eCurrentState = aiSoldierDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do I see the enemies?
		if (Enemy.Enemy)		{
			tStateStack.push(eCurrentState);
			eCurrentState = aiSoldierAttack;
			m_dwLastRangeSearch = Level().timeServer();
			return;
		}
		else {
			// checking if I am under fire
			DWORD dwCurTime = Level().timeServer();
			if (dwCurTime - dwHitTime > HIT_REACTION_TIME) {
				eCurrentState = tStateStack.top();
				tStateStack.pop();
				m_dwLastRangeSearch = Level().timeServer();
				return;
			}
			else {
				if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiSoldierSenseSomething;
					m_dwLastRangeSearch = Level().timeServer();
					return;
				}
				else {
					
					// determining the team
					CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
					Squad.Groups[g_Group()].m_tLastHitDirection = tHitDir;
					Squad.Groups[g_Group()].m_dwLastHitTime = dwHitTime;
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
					CSoldierSelectorUnderFire S = SelectorUnderFire;
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
						S.m_tLeaderNode = -1;
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
					else 
						if (S.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SWITCH){
							m_dwLastRangeSearch = S.m_dwCurTime;
							// fill arrays of members and exclude myself
							Device.Statistic.AI_Node.Begin();
							Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
							Device.Statistic.AI_Node.End();
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
							for (int i=0; i<S.taMemberPositions.size(); i++)
								if (((*S.taMembers)[i]->g_Health() > 0) && (bfCheckForMember(tHitDir,S.m_tMyPosition,S.taMemberPositions[i]))) {
									bCanKillMember = true;
									break;
								}
							
						if (!bCanKillMember)
							q_action.setup(AI::AIC_Action::AttackBegin);
						else
							q_action.setup(AI::AIC_Action::FireEnd);
						m_fCurSpeed = m_fMaxSpeed;
					}
					else {
						SetLessCoverLook(tNode);
						q_action.setup(AI::AIC_Action::FireEnd);
						m_fCurSpeed = m_fMaxSpeed;
					}
					// setting up look speed
					q_look.o_look_speed=8*_FB_look_speed;
					// checking flag to stop processing more states
					bStopThinking = true;
					return;
				}
			}
		}
	}
}

void CAI_Soldier::WaitOnPosition()
{
	bStopThinking = true;
}

void CAI_Soldier::Think()
{
	bStopThinking = false;
	do {
		switch(eCurrentState) {
			case aiSoldierDie : {
				Die();
				break;
			}
			case aiSoldierUnderFire : {
				UnderFire();
				break;
			}
			case aiSoldierSenseSomething : {
				SenseSomething();
				break;
			}
			case aiSoldierGoInThisDirection : {
				GoInThisDirection();
				break;
			}
			case aiSoldierGoToThisPosition : {
				GoToThisPosition();
				break;
			}
			case aiSoldierWaitOnPosition : {
				WaitOnPosition();
				break;
			}
			case aiSoldierHoldThisPosition : {
				HoldThisPosition();
				break;
			}
			case aiSoldierHoldPositionUnderFire : {
				HoldPositionUnderFire();
				break;
			}
			case aiSoldierFreeHunting : {
				FreeHunting();
				break;
			}
			case aiSoldierFollowMe : {
				FollowMe();
				break;
			}
			case aiSoldierAttack : {
				Attack();
				break;
			}
			case aiSoldierDefend : {
				Defend();
				break;
			}
			case aiSoldierPursuit : {
				Pursuit();
				break;
			}
			case aiSoldierRetreat : {
				Retreat();
				break;
			}
			case aiSoldierCover : {
				Cover();
				break;
			}
		}
	}
	while (!bStopThinking);
}

void CAI_Soldier::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	R_ASSERT(fsimilar(_view.magnitude(),1));
	R_ASSERT(fsimilar(_move.magnitude(),1));

	CMotionDef*	S=0;

	bool bCrouched = false; 

	if (iHealth<=0) {
		for (int i=0 ;i<2; i++)
			if (m_tpaDeathAnimations[i] == m_current) {
				S = m_current;
				break;
			}
		if (!S)
			S = m_tpaDeathAnimations[::Random.randI(0,2)];
	} else {
		if (speed<0.2f) {
			// idle
			S = m_idle;
		} else {
			Fvector view = _view; view.y=0; view.normalize_safe();
			Fvector move = _move; move.y=0; move.normalize_safe();
			float	dot  = view.dotproduct(move);
			
			SAnimState* AState = &m_walk;
//			if (bCrouched)	AState = &m_crouch_walk;
//			else			
			AState = &m_walk;
			
			if (speed>2.f){
//				if (bCrouched)	AState = &m_crouch_run;
//				else			
				AState = &m_run;
			}
			
			if (dot>0.7f){
				S = AState->fwd;
			}else if ((dot<=0.7f)&&(dot>=-0.7f)){
				Fvector cross; cross.crossproduct(view,move);
				if (cross.y>0){
					S = AState->rs;
				}else{
					S = AState->ls;
				}
			}else //if (dot<-0.7f)
			{
				S = AState->back;
			}
		}
	}
	if (S!=m_current){ 
		m_current = S;
		if (S) PKinematics(pVisual)->PlayCycle(S);
	}
}

void CAI_Soldier::g_fireParams(Fvector &fire_pos, Fvector &fire_dir)
{
	//Weapons->GetFireParams(fire_pos, fire_dir);
	fire_pos.set	(Weapons->ActiveWeapon()->Position());
	fire_dir.set	(eye_matrix.k);
}

float CAI_Soldier::OnVisible()
{
	float ret = inherited::OnVisible();

	Weapons->OnRender(FALSE);

	return ret;
}
