////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_templates.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Templates for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"
#include "ai_zombie_selectors.h"
#include "..\\..\\..\\xr_trims.h"
#include "..\\..\\xr_weapon_list.h"

// macroses
#define MIN_RANGE_SEARCH_TIME_INTERVAL	 500.f
#define MAX_TIME_RANGE_SEARCH			2000.f
#define	FIRE_ANGLE						PI/10
#define	FIRE_SAFETY_ANGLE				PI/10
#define	AMMO_NEED_RELOAD				6
#define EYE_WEAPON_DELTA				(0*PI/30.f)
#define MAX_PATROL_DISTANCE				6.f
#define MIN_PATROL_DISTANCE				1.f
#define MIN_COVER_MOVE					120
#define MAX_NEIGHBOUR_COUNT				9
#define DISTANCE_TO_STEP				.65f

bool CAI_Zombie::bfCheckPath(AI::Path &Path) {
	const vector<BYTE> &q_mark = Level().AI.tpfGetNodeMarks();
	for (int i=1; i<Path.Nodes.size(); i++) 
		if (q_mark[Path.Nodes[i]])
			return(false);
		return(true);
}

/**/
//id CAI_Zombie::vfBuildPathToDestinationPoint(CZombieSelectorAttack *S)
void CAI_Zombie::vfBuildPathToDestinationPoint(CZombieSelectorFreeHunting*S)
{
	// building a path from and to
	if (S)
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,*(S->m_tpEnemyNode),S->fOptEnemyDistance);
	else
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
	
	if (AI_Path.Nodes.size() >= 2) {
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
/**/

void CAI_Zombie::vfCheckForSavedEnemy()
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

void CAI_Zombie::vfInitSelector(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
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

void CAI_Zombie::vfSaveEnemy()
{
	tSavedEnemy = Enemy.Enemy;
	tSavedEnemyPosition = Enemy.Enemy->Position();
	tpSavedEnemyNode = Enemy.Enemy->AI_Node;
	dwSavedEnemyNodeID = Enemy.Enemy->AI_NodeID;
}

void CAI_Zombie::vfSearchForBetterPosition(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
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

void CAI_Zombie::vfSearchForBetterPositionWTime(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
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

void CAI_Zombie::vfSetFire(bool bFire, CGroup &Group)
{
	if (bFire) {
		if (!m_bActionStarted)
			m_bActionStarted = true;
		q_action.setup(AI::AIC_Action::AttackBegin);
	}
	else {
		if (m_bActionStarted)
			m_bActionStarted = false;
		q_action.setup(AI::AIC_Action::AttackEnd);
	}
}

void CAI_Zombie::vfSetMovementType(char cBodyState, float fSpeed)
{
	/**/
	m_fDistanceWent += m_fTimeUpdateDelta*AI_Path.fSpeed;
	if (m_fDistanceWent >= DISTANCE_TO_STEP) {
		pSounds->PlayAtPos(sndSteps[m_cStep ^= char(1)],this,vPosition);
		m_fDistanceWent = 0.f;		
	}
	/**/
	switch (cBodyState) {
		case BODY_STATE_STAND : {
			StandUp();
			m_fCurSpeed = fSpeed;
			//r_torso_speed = PI;
			break;
		}
		case BODY_STATE_CROUCH : {
			Squat();
			//r_torso_speed = 3*PI_DIV_4;
			break;
		}
		case BODY_STATE_LIE : {
			Lie();
			//r_torso_speed = PI_DIV_2;
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Insert from old zombie
//////////////////////////////////////////////////////////////////////////

IC bool CAI_Zombie::bfInsideSubNode(const Fvector &tCenter, const SSubNode &tpSubNode)
{
	return(((tCenter.x >= tpSubNode.tLeftDown.x) && (tCenter.z >= tpSubNode.tLeftDown.z)) && ((tCenter.x <= tpSubNode.tRightUp.x) && (tCenter.z <= tpSubNode.tRightUp.z)));
}

IC bool CAI_Zombie::bfInsideSubNode(const Fvector &tCenter, const float fRadius, const SSubNode &tpSubNode)
{
	float fDist0 = _sqr(tCenter.x - tpSubNode.tLeftDown.x) + _sqr(tCenter.z - tpSubNode.tLeftDown.z);
	float fDist1 = _sqr(tCenter.x - tpSubNode.tLeftDown.x) + _sqr(tCenter.z - tpSubNode.tRightUp.z);
	float fDist2 = _sqr(tCenter.x - tpSubNode.tRightUp.x) + _sqr(tCenter.z - tpSubNode.tLeftDown.z);
	float fDist3 = _sqr(tCenter.x - tpSubNode.tRightUp.x) + _sqr(tCenter.z - tpSubNode.tRightUp.z);
	return(_min(fDist0,_min(fDist1,_min(fDist2,fDist3))) <= (fRadius - 0.5f)*(fRadius - 0.5f) + EPS_L);
}

IC bool CAI_Zombie::bfInsideNode(const Fvector &tCenter, const NodeCompressed *tpNode)
{
	/**/
	Fvector tLeftDown;
	Fvector tRightUp;
	Level().AI.UnpackPosition(tLeftDown,tpNode->p0);
	Level().AI.UnpackPosition(tRightUp,tpNode->p1);
	float fSubNodeSize = Level().AI.GetHeader().size;
	return(((tCenter.x >= tLeftDown.x - fSubNodeSize/2.f) && (tCenter.z >= tLeftDown.z - fSubNodeSize/2.f)) && ((tCenter.x <= tRightUp.x + fSubNodeSize/2.f) && (tCenter.z <= tRightUp.z + fSubNodeSize/2.f)));
	/**
	NodePosition tCenterPosition;
	Level().AI.PackPosition(tCenterPosition,tCenter);
	return(((tCenterPosition.x >= tpNode->p0.x) && (tCenterPosition.z >= tpNode->p0.z)) && ((tCenterPosition.x <= tpNode->p1.x) && (tCenterPosition.z <= tpNode->p1.z)));
	/**/
}

IC float CAI_Zombie::ffComputeCost(Fvector tLeaderPosition,SSubNode &tCurrentNeighbour)
{
	Fvector tCurrentSubNode;
	tCurrentSubNode.x = (tCurrentNeighbour.tLeftDown.x + tCurrentNeighbour.tRightUp.x)/2.f;
	tCurrentSubNode.y = (tCurrentNeighbour.tLeftDown.y + tCurrentNeighbour.tRightUp.y)/2.f;
	tCurrentSubNode.z = (tCurrentNeighbour.tLeftDown.z + tCurrentNeighbour.tRightUp.z)/2.f;
	return(_sqr(tLeaderPosition.x - tCurrentSubNode.x) + 0*_sqr(tLeaderPosition.y - tCurrentSubNode.y) + _sqr(tLeaderPosition.z - tCurrentSubNode.z));
}

IC float CAI_Zombie::ffGetY(NodeCompressed &tNode, float X, float Z)
{
	// unpack plane
	Fvector	DUP, vNorm, v, v1, P0;
	DUP.set(0,1,0);
	pvDecompress(vNorm,tNode.plane);
	Fplane PL; 
	Level().AI.UnpackPosition(P0,tNode.p0);
	PL.build(P0,vNorm);
	v.set(X,P0.y,Z);	
	PL.intersectRayPoint(v,DUP,v1);	
	//v1.mad(v1,PL.n,.01f);
	//return(v1.y + .01f);
	return(v1.y);
}

/**/
IC bool CAI_Zombie::bfNeighbourNode(const SSubNode &tCurrentSubNode, const SSubNode &tMySubNode)
{
	if ((fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tLeftDown.x) < EPS_L) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tLeftDown.z) < EPS_L))
		return(true);
	if ((fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tLeftDown.x) < EPS_L) &&
		(fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tLeftDown.z) < EPS_L))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tLeftDown.x) < EPS_L) &&
		(fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tLeftDown.z) < EPS_L))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tRightUp.x) < EPS_L) &&
		(fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tLeftDown.z) < EPS_L))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tRightUp.x) < EPS_L) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tLeftDown.z) < EPS_L))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tRightUp.x) < EPS_L) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tRightUp.z) < EPS_L))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tLeftDown.x) < EPS_L) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tRightUp.z) < EPS_L))
		return(true);
	if ((fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tLeftDown.x) < EPS_L) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tRightUp.z) < EPS_L))
		return(true);
	// otherwise
	return(false);
}

/**
IC bool CAI_Zombie::bfNeighbourNode(const SSubNode &tCurrentSubNode, const SSubNode &tMySubNode)
{
	// check if it is left node
	if ((fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tRightUp.z) < EPS_L) &&
		(fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tLeftDown.x) < EPS_L))
		return(true);
	// check if it is front node
	if ((fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tRightUp.z)  < EPS_L) &&
		(fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tLeftDown.x) < EPS_L))
		return(true);
	// check if it is right node
	if ((fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tLeftDown.z) < EPS_L) &&
		(fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tRightUp.x) < EPS_L))
		return(true);
	// check if it is back node
	if ((fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tLeftDown.z) < EPS_L) &&
		(fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tRightUp.x) < EPS_L))
		return(true);
	// otherwise
	return(false);
}
/**/

#define MAX_NEIGHBOUR_COUNT 9
//#define MAX_NEIGHBOUR_COUNT 5

int CAI_Zombie::ifDivideNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes)
{
	CAI_Space &AI = Level().AI;
	float fSubNodeSize = AI.GetHeader().size;
	SSubNode tNode;
	tpSubNodes.clear();
	int iCount = 0, iResult = -1;
	Fvector tLeftDown;
	Fvector tRightUp;
	AI.UnpackPosition(tLeftDown,tpStartNode->p0);
	AI.UnpackPosition(tRightUp,tpStartNode->p1);
	
	float x = tCurrentPosition.x - (tLeftDown.x - fSubNodeSize/2.f);
	float z = tCurrentPosition.z - (tLeftDown.z - fSubNodeSize/2.f);
	//iResult = floor(x/fSubNodeSize)*(floor((tRightUp.z - tLeftDown.z)/fSubNodeSize) + 1) + floor(z/fSubNodeSize);
	SSubNode tMySubNode;
	tMySubNode.tLeftDown.x = tLeftDown.x - fSubNodeSize/2.f + fSubNodeSize*floor(x/fSubNodeSize);
	tMySubNode.tLeftDown.z = tLeftDown.z - fSubNodeSize/2.f + fSubNodeSize*floor(z/fSubNodeSize);
	tMySubNode.tRightUp.x = tMySubNode.tLeftDown.x + fSubNodeSize;
	tMySubNode.tRightUp.z = tMySubNode.tLeftDown.z + fSubNodeSize;
	for (float i=tLeftDown.x - fSubNodeSize/2.f; i < tRightUp.x; i+=fSubNodeSize) {
		tNode.tLeftDown.x = i;
		tNode.tRightUp.x = i + fSubNodeSize;
		for (float j=tLeftDown.z - fSubNodeSize/2.f; j < tRightUp.z; j+=fSubNodeSize) {
			tNode.tLeftDown.z = j;
			tNode.tRightUp.z = j + fSubNodeSize;
			if (
				(iResult < 0) &&
				(fabs(tNode.tLeftDown.x - tMySubNode.tLeftDown.x) < EPS_L) &&
				(fabs(tNode.tLeftDown.z - tMySubNode.tLeftDown.z) < EPS_L) &&
				(fabs(tNode.tRightUp.x - tMySubNode.tRightUp.x) < EPS_L) &&
				(fabs(tNode.tRightUp.z - tMySubNode.tRightUp.z) < EPS_L)
				) {
				tNode.tLeftDown.y = ffGetY(*tpStartNode,i,j);
				tNode.tRightUp.y = ffGetY(*tpStartNode,i + fSubNodeSize,j + fSubNodeSize);
				tNode.bEmpty = true;
				tpSubNodes.push_back(tNode);
				iResult = iCount;
				iCount++;
			}
			else
				if (bfNeighbourNode(tNode,tMySubNode)) {
					tNode.tLeftDown.y = ffGetY(*tpStartNode,i,j);
					tNode.tRightUp.y = ffGetY(*tpStartNode,i + fSubNodeSize,j + fSubNodeSize);
					tNode.bEmpty = true;
					tpSubNodes.push_back(tNode);
					iCount++;
					if (iCount >= MAX_NEIGHBOUR_COUNT)
						break;
				}
		}
		if (iCount >= MAX_NEIGHBOUR_COUNT)
			break;
	}
	if (iCount < MAX_NEIGHBOUR_COUNT) {
		NodeLink *taLinks = (NodeLink *)((u8 *)tpStartNode + sizeof(NodeCompressed));
		int iLinkCount = tpStartNode->link_count;
		for (int k=0; k<iLinkCount; k++) {
			NodeCompressed *tpCurrentNode = AI.Node(AI.UnpackLink(taLinks[k]));
			AI.UnpackPosition(tLeftDown,tpCurrentNode->p0);
			AI.UnpackPosition(tRightUp,tpCurrentNode->p1);
			for (float i=tLeftDown.x - fSubNodeSize/2.f; i < tRightUp.x; i+=fSubNodeSize) {
				tNode.tLeftDown.x = i;
				tNode.tRightUp.x = i + fSubNodeSize;
				for (float j=tLeftDown.z - fSubNodeSize/2.f; j < tRightUp.z; j+=fSubNodeSize) {
					tNode.tLeftDown.z = j;
					tNode.tRightUp.z = j + fSubNodeSize;
					if (bfNeighbourNode(tNode,tMySubNode)) {
						tNode.tLeftDown.y = ffGetY(*tpCurrentNode,i,j);
						tNode.tRightUp.y = ffGetY(*tpCurrentNode,i + fSubNodeSize,j + fSubNodeSize);
						tNode.bEmpty = true;
						tpSubNodes.push_back(tNode);
						iCount++;
						if (iCount >= MAX_NEIGHBOUR_COUNT)
							break;
					}
				}
				if (iCount >= MAX_NEIGHBOUR_COUNT)
					break;
			}
			if (iCount >= MAX_NEIGHBOUR_COUNT)
				break;
		}
	}
	VERIFY(iResult >= 0);
	return(iResult);
}

int CAI_Zombie::ifDivideNearestNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes)
{
	CAI_Space &AI = Level().AI;
	float fSubNodeSize = AI.GetHeader().size;
	SSubNode tNode;
	tpSubNodes.clear();
	int iCount = 0, iMySubNode = -1;
	float fBestCost = 100000000.f;
	Fvector tLeftDown;
	Fvector tRightUp;
	AI.UnpackPosition(tLeftDown,tpStartNode->p0);
	AI.UnpackPosition(tRightUp,tpStartNode->p1);
	for (float i=tLeftDown.x - fSubNodeSize/2.f; i < tRightUp.x; i+=fSubNodeSize) {
		tNode.tLeftDown.x = i;
		tNode.tRightUp.x = i + fSubNodeSize;
		for (float j=tLeftDown.z - fSubNodeSize/2.f; j < tRightUp.z; j+=fSubNodeSize) {
			tNode.tLeftDown.z = j;
			tNode.tRightUp.z = j + fSubNodeSize;
			float fCurrentCost = ffComputeCost(tCurrentPosition,tNode);
			if (fCurrentCost < fBestCost) {
				tNode.tLeftDown.y = ffGetY(*tpStartNode,i,j);
				tNode.tRightUp.y = ffGetY(*tpStartNode,i + fSubNodeSize,j + fSubNodeSize);
				tNode.bEmpty = true;
				tpSubNodes.push_back(tNode);
				fBestCost = fCurrentCost;
				iMySubNode = iCount;
				iCount++;
			}
		}
	}
	VERIFY(iMySubNode >= 0);
	return(iMySubNode);
}

void CAI_Zombie::GoToPointViaSubnodes(Fvector &tLeaderPosition) 
{
	Fvector tCurrentPosition = Position();
	NodeCompressed* tpCurrentNode = AI_Node;
	bool bInsideNode = false;
	int iMySubNode = -1;
	if (bfInsideNode(tCurrentPosition,tpCurrentNode)) {
		// divide the nearest nodes into the subnodes 0.7x0.7 m^2
		iMySubNode = ifDivideNode(tpCurrentNode,tCurrentPosition,tpSubNodes);
		bInsideNode = iMySubNode >= 0;
	}
	if (bInsideNode) {
		// filling the subnodes with the moving objects
		Level().ObjectSpace.GetNearest(tCurrentPosition,3.f);//20*(Level().AI.GetHeader().size));
		CObjectSpace::NL_TYPE &tpNearestList = Level().ObjectSpace.q_nearest;
		Fvector tCenter;

		int i;
		/**
		SSubNode *tpFrontSubNode=0;
		SSubNode *tpBackSubNode=0;
		SSubNode *tpLeftSubNode=0;
		SSubNode *tpRightSubNode=0;
		SSubNode &tMySubNode = tpSubNodes[iMySubNode];

		
		for ( i=0; i<tpSubNodes.size(); i++) {
			if (i == iMySubNode)
				continue;
			// check if it is left node
			if ((fabs(tpSubNodes[i].tRightUp.z - tMySubNode.tRightUp.z) < EPS_L) &&
				(fabs(tpSubNodes[i].tRightUp.x - tMySubNode.tLeftDown.x) < EPS_L))
				tpLeftSubNode = &(tpSubNodes[i]);
			// check if it is front node
			if ((fabs(tpSubNodes[i].tLeftDown.z - tMySubNode.tRightUp.z)  < EPS_L) &&
				(fabs(tpSubNodes[i].tLeftDown.x - tMySubNode.tLeftDown.x) < EPS_L))
				tpFrontSubNode = &(tpSubNodes[i]);
			// check if it is right node
			if ((fabs(tpSubNodes[i].tLeftDown.z - tMySubNode.tLeftDown.z) < EPS_L) &&
				(fabs(tpSubNodes[i].tLeftDown.x - tMySubNode.tRightUp.x) < EPS_L))
				tpRightSubNode = &(tpSubNodes[i]);
			// check if it is back node
			if ((fabs(tpSubNodes[i].tRightUp.z - tMySubNode.tLeftDown.z) < EPS_L) &&
				(fabs(tpSubNodes[i].tRightUp.x - tMySubNode.tRightUp.x) < EPS_L))
				tpBackSubNode = &(tpSubNodes[i]);
		}
		/**/

		/**/
		if (!(tpNearestList.empty())) {
			for (CObjectSpace::NL_IT tppObjectIterator=tpNearestList.begin(); tppObjectIterator!=tpNearestList.end(); tppObjectIterator++) {
				CObject* tpCurrentObject = (*tppObjectIterator);
				if ((tpCurrentObject->CLS_ID != CLSID_ENTITY) || (tpCurrentObject == this) || (tpCurrentObject == tSavedEnemy))
					continue;
				float fRadius = tpCurrentObject->Radius();
				tpCurrentObject->clCenter(tCenter);
				
				//bool bFront=0,bBack=0,bLeft=0,bRight=0;
				for (int j=0; j<tpSubNodes.size(); j++)
					if (bfInsideSubNode(tCenter,fRadius,tpSubNodes[j])) {
						tpSubNodes[j].bEmpty = false;
						//if (&(tpSubNodes[j]) == tpFrontSubNode)
						//	bFront = true;
						//if (&(tpSubNodes[j]) == tpLeftSubNode)
						//	bLeft = true;
						//if (&(tpSubNodes[j]) == tpRightSubNode)
						//	bRight = true;
						//if (&(tpSubNodes[j]) == tpBackSubNode)
						//	bBack = true;
					}

				CAI_Zombie *tMember = dynamic_cast<CAI_Zombie*>(tpCurrentObject);
				if (tMember)
					if (tMember->AI_Path.TravelPath.size() > tMember->AI_Path.TravelStart + 2) {
						CAI_Space &AI = Level().AI;
						tCenter = tMember->AI_Path.TravelPath[tMember->AI_Path.TravelStart + 1].P;
						for (int j=0; j<tpSubNodes.size(); j++)
							if (bfInsideSubNode(tCenter,fRadius,tpSubNodes[j])) {
								tpSubNodes[j].bEmpty = false;
								//if (&(tpSubNodes[j]) == tpFrontSubNode)
								//	bFront = true;
								//if (&(tpSubNodes[j]) == tpLeftSubNode)
								//	bLeft = true;
								//if (&(tpSubNodes[j]) == tpRightSubNode)
								//	bRight = true;
								//if (&(tpSubNodes[j]) == tpBackSubNode)
								//	bBack = true;
							}
						//if ((bFront && bLeft) || (bFront && bRight) || (bBack && bLeft) || (bBack && bRight)) {						
						//	for (int j=0; j<tpSubNodes.size(); j++)
						//		tpSubNodes[j].bEmpty = false;
						//	break;
						//}
					}
			}
		}
		/**/
		// checking the nearest nodes
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart = 0;

		//Fvector tLeaderPosition = Leader->Position();
		DWORD dwTime = Level().timeServer();
		int iBestI = -1;
		float fBestCost = _sqr(tLeaderPosition.x - tCurrentPosition.x) + 0*_sqr(tLeaderPosition.y - tCurrentPosition.y) + _sqr(tLeaderPosition.z - tCurrentPosition.z);
		bool bMobility = false;
		for ( i=0; i<tpSubNodes.size(); i++)
			if ((i != iMySubNode) && (tpSubNodes[i].bEmpty)) {
				bMobility = true;
				float fCurCost = ffComputeCost(tLeaderPosition,tpSubNodes[i]);
				if (fCurCost < fBestCost) {
					iBestI = i;
					fBestCost = fCurCost;
				}
			}

		if (bMobility) {
			m_bMobility = true;
			/**
			if (iBestI < 0)
				if (dwTime - m_dwLastUpdate > 3000) {
					m_dwLastUpdate = dwTime;
					iBestI = ::Random.randI(0,tpSubNodes.size());
				}
				//else
				//	m_bMobility
			/**/
			if (iBestI >= 0) {
				m_dwLastRangeSearch = dwTime;
				Fvector tFinishPosition;
				tFinishPosition.x = (tpSubNodes[iBestI].tLeftDown.x + tpSubNodes[iBestI].tRightUp.x)/2.f;
				tFinishPosition.y = (tpSubNodes[iBestI].tLeftDown.y + tpSubNodes[iBestI].tRightUp.y)/2.f;
				tFinishPosition.z = (tpSubNodes[iBestI].tLeftDown.z + tpSubNodes[iBestI].tRightUp.z)/2.f;
				VERIFY(tFinishPosition.x < 1000000.f);
				CTravelNode	tCurrentPoint,tFinishPoint;
				tCurrentPoint.P.set(tCurrentPosition);
				tCurrentPoint.floating = FALSE;
				AI_Path.TravelPath.push_back(tCurrentPoint);
				tFinishPoint.P.set(tFinishPosition);
				tFinishPoint.floating = FALSE;
				AI_Path.TravelPath.push_back(tFinishPoint);
			}
		}
		else {
			m_bMobility = false;
		}
	}
	else {
		int iMySubNode = ifDivideNearestNode(tpCurrentNode,tCurrentPosition,tpSubNodes);
		VERIFY(iMySubNode >= 0);
		Level().ObjectSpace.GetNearest(tpSubNodes[iMySubNode].tLeftDown,2*(Level().AI.GetHeader().size));
		CObjectSpace::NL_TYPE &tpNearestList = Level().ObjectSpace.q_nearest;
		if (!tpNearestList.empty()) {
			for (CObjectSpace::NL_IT tppObjectIterator=tpNearestList.begin(); tppObjectIterator!=tpNearestList.end(); tppObjectIterator++) {
				CObject* tpCurrentObject = (*tppObjectIterator);
				if ((tpCurrentObject->CLS_ID != CLSID_ENTITY) || (tpCurrentObject == this) || (tpCurrentObject == tSavedEnemy))
					continue;
				float fRadius = tpCurrentObject->Radius();
				Fvector tCenter;
				tpCurrentObject->clCenter(tCenter);
				if (bfInsideSubNode(tCenter,fRadius,tpSubNodes[iMySubNode])) {
					tpSubNodes[iMySubNode].bEmpty = false;
					break;
				}
			}
		}
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart = 0;
		if (tpSubNodes[iMySubNode].bEmpty) {
			Fvector tFinishPosition;
			tFinishPosition.x = (tpSubNodes[iMySubNode].tLeftDown.x + tpSubNodes[iMySubNode].tRightUp.x)/2.f;
			tFinishPosition.y = (tpSubNodes[iMySubNode].tLeftDown.y + tpSubNodes[iMySubNode].tRightUp.y)/2.f;
			tFinishPosition.z = (tpSubNodes[iMySubNode].tLeftDown.z + tpSubNodes[iMySubNode].tRightUp.z)/2.f;
			VERIFY(tFinishPosition.x < 1000000.f);
			CTravelNode	tCurrentPoint,tFinishPoint;
			tCurrentPoint.P.set(tCurrentPosition);
			tCurrentPoint.floating = FALSE;
			AI_Path.TravelPath.push_back(tCurrentPoint);
			tFinishPoint.P.set(tFinishPosition);
			tFinishPoint.floating = FALSE;
			AI_Path.TravelPath.push_back(tFinishPoint);
			m_bMobility = true;
		}
		else
			m_bMobility = false;
	}
}