////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_templates.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Templates for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "ai_soldier_selectors.h"
#include "..\\..\\..\\xr_trims.h"
#include "..\\..\\xr_weapon_list.h"

bool CAI_Soldier::bfCheckPath(AI::Path &Path) {
	const vector<BYTE> &q_mark = Level().AI.tpfGetNodeMarks();
	for (int i=1; i<Path.Nodes.size(); i++) 
		if (q_mark[Path.Nodes[i]])
			return(false);
		return(true);
}

void CAI_Soldier::vfBuildPathToDestinationPoint(CSoldierSelectorAttack *S)
{
	// building a path from and to
	if (S)
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,*(S->m_tpEnemyNode),S->fOptEnemyDistance);
	else
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
	
	if (AI_Path.Nodes.size() > 2) {
		// if path is long enough then build travel line
		AI_Path.BuildTravelLine(Position());
		AI_Path.TravelStart = 0;
	}
	else {
		// if path is too short then clear it (patch for ExecMove)
		AI_Path.TravelPath.clear();
		AI_Path.bNeedRebuild = FALSE;
	}
}

void CAI_Soldier::vfCheckForPatrol()
{
	m_tpaPatrolPoints.clear();
	m_tpaPointDeviations.clear();
	if (Level().pLevel->SectionExists("patrol_path")) {
		for (int iPathLine=0; ; iPathLine++) {
			char caLine[100], monster_name[100];
			sprintf(caLine,"path_%d",iPathLine);
			
			if (!(Level().pLevel->LineExists("patrol_path",caLine)))
				break;
			
			LPCSTR buf = Level().pLevel->ReadSTRING("patrol_path",caLine), buf2 = buf;
			
			int path_count = _GetItemCount(buf);
			
			if (!path_count)
				break;
			
			R_ASSERT(path_count && (path_count % 4 == 0));
			
			DWORD team,squad,group, dwDummy;
			
			for ( ; ; buf2++)
				if (*buf2 == ',') {
					memcpy(monster_name,buf,(buf2 - buf)*sizeof(char));
					monster_name[buf2++ - buf] = 0;
					break;
				}
				
				const char *cTest = cName();
				
				if ((strlen(monster_name)) && (strcmp(monster_name,this->cName())))
					continue;
				
				sscanf(buf2,"%d,%d,%d",&team,&squad,&group);
				for (int komas=0; komas<3; buf2++)
					if (*buf2 == ',')
						komas++;
					
					if ((team != g_Team()) || (squad != g_Squad()) || (group != g_Group()))
						continue;
					
					path_count -= 4;
					path_count /= 4;
					m_tpaPatrolPoints.resize(path_count);
					m_tpaPointDeviations.resize(path_count);
					
					for (int i=0; i<path_count; i++) {
						sscanf(buf2,"%f,%f,%f,%d",&(m_tpaPatrolPoints[i].x),&(m_tpaPatrolPoints[i].y),&(m_tpaPatrolPoints[i].z),&dwDummy);
						for ( komas=0; komas<4; buf2++)
							if (*buf2 == ',')
								komas++;
					}
					
					m_dwStartPatrolNode = Level().AI.q_LoadSearch(m_tpaPatrolPoints[0]);
					AI_Path.bNeedRebuild = TRUE;
					
					break;
		}
	}
}

void CAI_Soldier::vfCheckForSavedEnemy()
{
	if (!tSavedEnemy) {
		tSavedEnemy = Enemy.Enemy;
		tSavedEnemyPosition = Enemy.Enemy->Position();
		tpSavedEnemyNode = Enemy.Enemy->AI_Node;
		dwSavedEnemyNodeID = Enemy.Enemy->AI_NodeID;
	}
	else {
		tSavedEnemy = 0;
		tSavedEnemyPosition.set(0,0,0);
		tpSavedEnemyNode = 0;
		dwSavedEnemyNodeID = -1;
	}
}

void CAI_Soldier::vfInitSelector(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
{
	// checking if leader is dead then make myself a leader
	if (Leader->g_Health() <= 0)
		Leader = this;
	// setting watch mode to false
	bool bWatch = false;
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
	
	if (Enemy.Enemy) {
		bBuildPathToLostEnemy = false;
		// saving an enemy
		vfSaveEnemy();
		
		S.m_tEnemy			= Enemy.Enemy;
		S.m_tEnemyPosition	= Enemy.Enemy->Position();
		S.m_tpEnemyNode		= Enemy.Enemy->AI_Node;
	}
	else {
		S.m_tEnemy			= tSavedEnemy;
		S.m_tEnemyPosition	= tSavedEnemyPosition;
		S.m_tpEnemyNode		= tpSavedEnemyNode;
	}
	
	S.taMembers = &(Squad.Groups[g_Group()].Members);
	
	//if (S.m_tLeader)
	//	S.taMembers.push_back(S.m_tLeader);
}

void CAI_Soldier::vfSaveEnemy()
{
	tSavedEnemy = Enemy.Enemy;
	tSavedEnemyPosition = Enemy.Enemy->Position();
	tpSavedEnemyNode = Enemy.Enemy->AI_Node;
	dwSavedEnemyNodeID = Enemy.Enemy->AI_NodeID;
}

void CAI_Soldier::vfSearchForBetterPosition(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
{
	if ((!m_dwLastRangeSearch) || ((S.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SEARCH_TIME_INTERVAL) && (::Random.randF(0,1) < float(S.m_dwCurTime - m_dwLastRangeSearch)/MAX_TIME_RANGE_SEARCH))) {
		
		bool bLastSearch = m_dwLastRangeSearch != 0;
		DWORD dwTimeDifference = S.m_dwCurTime - m_dwLastSuccessfullSearch;
		m_dwLastRangeSearch = S.m_dwCurTime;
		Device.Statistic.AI_Node.Begin();
		Squad.Groups[g_Group()].GetAliveMemberInfoWithLeader(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this,Leader);
		Device.Statistic.AI_Node.End();
		// search for the best node according to the 
		// selector evaluation function in the radius N meteres
		float fOldCost;
		if (bLastSearch)
			Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost,dwTimeDifference);
		else
			Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
		// if search has found new best node then 
		if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
			AI_Path.DestNode		= S.BestNode;
			AI_Path.bNeedRebuild	= TRUE;
		} 
		
		if (AI_Path.Nodes.size() <= 2)
			AI_Path.bNeedRebuild = TRUE;
		
		if (AI_Path.bNeedRebuild)
			m_dwLastSuccessfullSearch = S.m_dwCurTime;
	}
}

void CAI_Soldier::vfSearchForBetterPositionWTime(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
{
	DWORD dwTimeDifference = S.m_dwCurTime - m_dwLastSuccessfullSearch;
	m_dwLastRangeSearch = S.m_dwCurTime;
	Device.Statistic.AI_Node.Begin();
	Squad.Groups[g_Group()].GetAliveMemberInfoWithLeader(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this,Leader);
	Device.Statistic.AI_Node.End();
	// search for the best node according to the 
	// selector evaluation function in the radius N meteres
	float fOldCost;
	Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
	// if search has found new best node then 
	//if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
	if ((AI_Path.DestNode != S.BestNode) && (S.BestCost < (fOldCost - S.fLaziness))){
		AI_Path.DestNode		= S.BestNode;
		AI_Path.bNeedRebuild	= TRUE;
	} 
	
	if (AI_Path.Nodes.size() <= 2)
		AI_Path.bNeedRebuild = TRUE;
	
	if (AI_Path.bNeedRebuild)
		m_dwLastSuccessfullSearch = S.m_dwCurTime;
}

void CAI_Soldier::vfSetFire(bool bFire, CGroup &Group)
{
	bool bSafeFire = m_bFiring;
	
	if (bFire)
		if (m_bFiring)
			if (m_dwStartFireAmmo - Weapons->ActiveWeapon()->GetAmmoElapsed() > ::Random.randI(m_dwFireRandomMin,m_dwFireRandomMax + 1)) {
				q_action.setup(AI::AIC_Action::FireEnd);
				m_bFiring = false;
				m_dwNoFireTime = Level().timeServer();
			}
			else {
				if (bfCheckIfCanKillEnemy())
					if (!bfCheckIfCanKillMember()) {
						q_action.setup(AI::AIC_Action::FireBegin);
						m_bFiring = true;
					}
					else {
						q_action.setup(AI::AIC_Action::FireEnd);
						m_bFiring = false;
						m_dwNoFireTime = Level().timeServer();
					}
					else {
						q_action.setup(AI::AIC_Action::FireEnd);
						m_bFiring = false;
						m_dwNoFireTime = Level().timeServer();
					}
			}
			else {
				if (Level().timeServer() - m_dwNoFireTime > ::Random.randI(m_dwNoFireTimeMin,m_dwNoFireTimeMax + 1))
					if (bfCheckIfCanKillEnemy())
						if (!bfCheckIfCanKillMember()) {
							m_dwStartFireAmmo = Weapons->ActiveWeapon()->GetAmmoElapsed();
							q_action.setup(AI::AIC_Action::FireBegin);
							m_bFiring = true;
						}
						else {
							q_action.setup(AI::AIC_Action::FireEnd);
							m_bFiring = false;
						}
						else {
							q_action.setup(AI::AIC_Action::FireEnd);
							m_bFiring = false;
						}
						else {
							q_action.setup(AI::AIC_Action::FireEnd);
							m_bFiring = false;
						}
			}
			else {
				q_action.setup(AI::AIC_Action::FireEnd);
				m_bFiring = false;
			}
			
			if ((bSafeFire) && (!m_bFiring))
				Group.m_dwFiring--;
			else
				if ((!bSafeFire) && (m_bFiring))
					Group.m_dwFiring++;
}

void CAI_Soldier::vfSetMovementType(char cBodyState, float fSpeed)
{
	switch (cBodyState) {
		case BODY_STATE_STAND : {
			StandUp();
			m_fCurSpeed = fSpeed;
			r_torso_speed = PI;
			break;
		}
		case BODY_STATE_CROUCH : {
			Squat();
			m_fCurSpeed = m_fCrouchCoefficient*fSpeed;
			r_torso_speed = 3*PI_DIV_4;
			break;
		}
		case BODY_STATE_LIE : {
			Lie();
			m_fCurSpeed = 3*m_fCrouchCoefficient*fSpeed;
			r_torso_speed = PI_DIV_2;
			break;
		}
	}
}