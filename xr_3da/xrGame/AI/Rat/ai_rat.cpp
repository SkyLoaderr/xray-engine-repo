////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.cpp
//	Created 	: 23.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\..\\3dsound.h"
#include "ai_rat.h"
#include "ai_rat_selectors.h"
#include "..\\..\\..\\bodyinstance.h"

//#define WRITE_LOG

CAI_Rat::CAI_Rat()
{
	dwHitTime = 0;
	tHitDir.set(0,0,1);
	dwSenseTime = 0;
	tSenseDir.set(0,0,1);
	tSavedEnemy = 0;
	tSavedEnemyPosition.set(0,0,0);
	dwLostEnemyTime = 0;
	eCurrentState = aiRatFollowMe;
	m_bMobility = true;
}

CAI_Rat::~CAI_Rat()
{
	// removing all data no more being neded 
	int i;
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
}

void CAI_Rat::Load(CInifile* ini, const char* section)
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
	SelectorFollow.Load(ini,section);
	SelectorFreeHunting.Load(ini,section);
	SelectorPursuit.Load(ini,section);
	SelectorUnderFire.Load(ini,section);
}

// when someone hit rat
void CAI_Rat::HitSignal(int amount, Fvector& vLocalDir, CEntity* who)
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

// when someone hit rat
void CAI_Rat::SenseSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwSenseTime = Level().timeServer();
	tSenseDir.set(D);
}

// when rat is dead
void CAI_Rat::Death()
{
	// perform death operations
	inherited::Death( );
	q_action.setup(AI::AIC_Action::FireEnd);
	eCurrentState = aiRatDie;

	// removing from group
	//Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].Member_Remove(this);

	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	// Play sound
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
}

// rat update
void CAI_Rat::Update(DWORD DT)
{
	inherited::Update(DT);
}

float CAI_Rat::EnemyHeuristics(CEntity* E)
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

void CAI_Rat::SelectEnemy(SEnemySelected& S)
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

IC bool CAI_Rat::bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint) {
	Fvector tMemberDirection;
	tMemberDirection.sub(tMyPoint,tMemberPoint);
	vfNormalizeSafe(tMemberDirection);
	float fAlpha = acosf(tFireVector.dotproduct(tMemberDirection));
	return(fAlpha < PI/10);
	//return(false);
}

bool CAI_Rat::bfCheckPath(AI::Path &Path) {
	vector<BYTE> q_mark = Level().AI.tpfGetNodeMarks();
	for (int i=1; i<Path.Nodes.size(); i++) 
		if (q_mark[Path.Nodes[i]])
			return(false);
	return(true);
}

#define LEFT_NODE(Index)  ((Index + 3) & 3)
#define RIGHT_NODE(Index) ((Index + 5) & 3)

void CAI_Rat::SetLessCoverLook(NodeCompressed *tNode)
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

IC bool CAI_Rat::bfInsideSubNode(const Fvector &tCenter, const SSubNode &tpSubNode)
{
	return(((tCenter.x >= tpSubNode.tLeftDown.x) && (tCenter.z >= tpSubNode.tLeftDown.z)) && ((tCenter.x <= tpSubNode.tRightUp.x) && (tCenter.z <= tpSubNode.tRightUp.z)));
}

#define min(x,y) ((x) < (y) ? (x) : (y))

#define EPSILON 0.001

IC bool CAI_Rat::bfInsideSubNode(const Fvector &tCenter, const float fRadius, const SSubNode &tpSubNode)
{
	float fDist0 = SQR(tCenter.x - tpSubNode.tLeftDown.x) + SQR(tCenter.z - tpSubNode.tLeftDown.z);
	float fDist1 = SQR(tCenter.x - tpSubNode.tLeftDown.x) + SQR(tCenter.z - tpSubNode.tRightUp.z);
	float fDist2 = SQR(tCenter.x - tpSubNode.tRightUp.x) + SQR(tCenter.z - tpSubNode.tLeftDown.z);
	float fDist3 = SQR(tCenter.x - tpSubNode.tRightUp.x) + SQR(tCenter.z - tpSubNode.tRightUp.z);
	return(min(fDist0,min(fDist1,min(fDist2,fDist3))) <= fRadius*fRadius + EPSILON);
}

IC bool CAI_Rat::bfInsideNode(const Fvector &tCenter, const NodeCompressed *tpNode)
{
	/**
	Fvector tLeftDown;
	Fvector tRightUp;
	Level().AI.UnpackPosition(tLeftDown,tpNode->p0);
	Level().AI.UnpackPosition(tRightUp,tpNode->p1);
	float fSubNodeSize = Level().AI.GetHeader().size;
	return(((tCenter.x >= tLeftDown.x - fSubNodeSize/2.f) && (tCenter.z >= tLeftDown.z - fSubNodeSize/2.f)) && ((tCenter.x <= tRightUp.x + fSubNodeSize/2.f) && (tCenter.z <= tRightUp.z + fSubNodeSize/2.f)));
	/**/
	NodePosition tCenterPosition;
	Level().AI.PackPosition(tCenterPosition,tCenter);
	return(((tCenterPosition.x >= tpNode->p0.x) && (tCenterPosition.z >= tpNode->p0.z)) && ((tCenterPosition.x <= tpNode->p1.x) && (tCenterPosition.z <= tpNode->p1.z)));
	/**/
}

IC bool CAI_Rat::bfNeighbourNode(const SSubNode &tCurrentSubNode, const SSubNode &tMySubNode)
{
	// check if it is left node
	if ((fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tRightUp.z) < EPSILON) &&
		(fabs(tCurrentSubNode.tRightUp.x == tMySubNode.tLeftDown.x) < EPSILON))
		return(true);
	// check if it is front node
	if ((fabs(tCurrentSubNode.tLeftDown.z == tMySubNode.tRightUp.z)  < EPSILON) &&
		(fabs(tCurrentSubNode.tLeftDown.x == tMySubNode.tLeftDown.x) < EPSILON))
		return(true);
	// check if it is right node
	if ((fabs(tCurrentSubNode.tLeftDown.z == tMySubNode.tLeftDown.z) < EPSILON) &&
		(fabs(tCurrentSubNode.tLeftDown.x == tMySubNode.tRightUp.x) < EPSILON))
		return(true);
	// check if it is back node
	if ((fabs(tCurrentSubNode.tRightUp.z == tMySubNode.tLeftDown.z) < EPSILON) &&
		(fabs(tCurrentSubNode.tRightUp.x == tMySubNode.tRightUp.x) < EPSILON))
		return(true);
	// otherwise
	return(false);
}

IC float CAI_Rat::ffComputeCost(Fvector tLeaderPosition,SSubNode &tCurrentNeighbour)
{
	Fvector tCurrentSubNode;
	tCurrentSubNode.x = (tCurrentNeighbour.tLeftDown.x + tCurrentNeighbour.tRightUp.x)/2.f;
	tCurrentSubNode.y = (tCurrentNeighbour.tLeftDown.y + tCurrentNeighbour.tRightUp.y)/2.f;
	tCurrentSubNode.z = (tCurrentNeighbour.tLeftDown.z + tCurrentNeighbour.tRightUp.z)/2.f;
	return(SQR(tLeaderPosition.x - tCurrentSubNode.x) + 0*SQR(tLeaderPosition.y - tCurrentSubNode.y) + SQR(tLeaderPosition.z - tCurrentSubNode.z));
}

IC float CAI_Rat::ffGetY(NodeCompressed &tNode, float X, float Z)
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
	//v1.direct(v1,PL.n,.01f);
	return(v1.y);
}

int CAI_Rat::ifDivideNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes)
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
	for (float i=tLeftDown.x - fSubNodeSize/2.f; i < tRightUp.x; i+=fSubNodeSize) {
		for (float j=tLeftDown.z - fSubNodeSize/2.f; j < tRightUp.z; j+=fSubNodeSize) {
			tNode.tLeftDown.x = i;
			tNode.tLeftDown.y = ffGetY(*tpStartNode,i,j);
			tNode.tLeftDown.z = j;
			tNode.tRightUp.x = i + fSubNodeSize;
			tNode.tRightUp.y = ffGetY(*tpStartNode,i + fSubNodeSize,j + fSubNodeSize);
			tNode.tRightUp.z = j + fSubNodeSize;
			tNode.bEmpty = true;
			tpSubNodes.push_back(tNode);
			if (bfInsideSubNode(tCurrentPosition,tNode))
				iResult = iCount;
			iCount++;
		}
	}
	NodeLink *taLinks = (NodeLink *)((u8 *)tpStartNode + sizeof(NodeCompressed));
	iCount = tpStartNode->link_count;
	for (int k=0; k<iCount; k++) {
		NodeCompressed *tpCurrentNode = AI.Node(AI.UnpackLink(taLinks[k]));
		AI.UnpackPosition(tLeftDown,tpCurrentNode->p0);
		AI.UnpackPosition(tRightUp,tpCurrentNode->p1);
		for (float i=tLeftDown.x - fSubNodeSize/2.f; i < tRightUp.x; i+=fSubNodeSize)
			for (float j=tLeftDown.z - fSubNodeSize/2.f; j < tRightUp.z; j+=fSubNodeSize) {
				tNode.tLeftDown.x = i;
				tNode.tLeftDown.y = ffGetY(*tpCurrentNode,i,j);
				tNode.tLeftDown.z = j;
				tNode.tRightUp.x = i + fSubNodeSize;
				tNode.tRightUp.y = ffGetY(*tpCurrentNode,i + fSubNodeSize,j + fSubNodeSize);
				tNode.tRightUp.z = j + fSubNodeSize;
				tNode.bEmpty = true;
				tpSubNodes.push_back(tNode);
			}
	}
	return(iResult);
}

int CAI_Rat::ifDivideNearestNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes)
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
		for (float j=tLeftDown.z - fSubNodeSize/2.f; j < tRightUp.z; j+=fSubNodeSize) {
			tNode.tLeftDown.x = i;
			tNode.tLeftDown.y = ffGetY(*tpStartNode,i,j);
			tNode.tLeftDown.z = j;
			tNode.tRightUp.x = i + fSubNodeSize;
			tNode.tRightUp.y = ffGetY(*tpStartNode,i + fSubNodeSize,j + fSubNodeSize);
			tNode.tRightUp.z = j + fSubNodeSize;
			tNode.bEmpty = true;
			tpSubNodes.push_back(tNode);
			float fCurrentCost = ffComputeCost(tCurrentPosition,tNode);
			if (fCurrentCost < fBestCost) {
				fBestCost = fCurrentCost;
				iMySubNode = iCount;
			}
			iCount++;
		}
	}
	return(iMySubNode);
}

void CAI_Rat::FollowLeader(CSquad &Squad, CEntity* Leader) 
{
	Fvector tCurrentPosition = Position();
	NodeCompressed* tpCurrentNode = AI_Node;
	if (bfInsideNode(tCurrentPosition,tpCurrentNode)) {
		vector<SSubNode> tpSubNodes;
		// divide the nearest nodes into the subnodes 0.7x0.7 m^2
		//Level().AI.UnpackPosition(tCurrentPosition,AI_Node->p0);
		int iMySubNode = ifDivideNode(tpCurrentNode,tCurrentPosition,tpSubNodes);
		// filling the subnodes with the moving objects
		/**
		CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
		vector<CEntity*> Members = Squad.Groups[g_Group()].Members;
		if (Squad.Leader != this)
			Members.push_back(Squad.Leader);
		for (int i=0; i<Members.size(); i++)
			Members[i]->tpfGetCCFModel()->Enable(TRUE);
		Level().ObjectSpace.GetNearest(tCurrentPosition,2*(Level().AI.GetHeader().size));
		//Level().ObjectSpace.GetNearest(tpSubNodes[iMySubNode].tLeftDown,3*(Level().AI.GetHeader().size));
		//Level().ObjectSpace.GetNearest(this->cfModel,3*(Level().AI.GetHeader().size));
		CObjectSpace::NL_TYPE tpNearestList = Level().ObjectSpace.nearest_list;
		Fvector tCenter;
		if (!(tpNearestList.empty())) {
			int i=0;
			for (CObjectSpace::NL_IT tppObjectIterator=tpNearestList.begin(); tppObjectIterator!=tpNearestList.end(); tppObjectIterator++) {
				CObject* tpCurrentObject = (*tppObjectIterator)->Owner();
				if (tpCurrentObject == this)
					continue;
				float fRadius = tpCurrentObject->Radius();
				tpCurrentObject->clCenter(tCenter);
				for (int j=0; j<tpSubNodes.size(); j++)
					if (bfInsideSubNode(tCenter,fRadius,tpSubNodes[j]))
						tpSubNodes[j].bEmpty = false;
				i++;
			}
			Msg("%d",i);
		}
		else {
			Msg("empty");
		}
		/**/
		CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
		vector<CEntity*> Members = Squad.Groups[g_Group()].Members;
		if (Squad.Leader != this)
			Members.push_back(Squad.Leader);
		for (int i=0; i<Members.size(); i++)
			if (Members[i] != this) {
				float fRadius = Members[i]->Radius();
				Fvector tCenter;
				Members[i]->clCenter(tCenter);
				for (int j=0; j<tpSubNodes.size(); j++)
					if (bfInsideSubNode(tCenter,fRadius,tpSubNodes[j]))
						tpSubNodes[j].bEmpty = false;
				/**
				CAI_Rat *tMember = dynamic_cast<CAI_Rat*>(Members[i]);
				if (tMember)
					if (tMember->AI_Path.TravelPath.size() > 1) {
						CAI_Space &AI = Level().AI;
						tCenter = tMember->AI_Path.TravelPath[1].P;
						for (int j=0; j<tpSubNodes.size(); j++)
							if (bfInsideSubNode(tCenter,fRadius,tpSubNodes[j]))
								tpSubNodes[j].bEmpty = false;
					}
				/**/
			}
		/**/
		// checking the nearest nodes
		vector<SSubNode> tpFreeNeighbourNodes;
		tpFreeNeighbourNodes.clear();
		for ( i=0; i<tpSubNodes.size(); i++)
			if ((i != iMySubNode) && (tpSubNodes[i].bEmpty) && (bfNeighbourNode(tpSubNodes[i],tpSubNodes[iMySubNode])))
				tpFreeNeighbourNodes.push_back(tpSubNodes[i]);
		tpSubNodes.clear();
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart = 0;
		if (!tpFreeNeighbourNodes.empty()) {
			m_bMobility = true;
			Fvector tLeaderPosition = Leader->Position();
			float fBestCost = SQR(tLeaderPosition.x - tCurrentPosition.x) + 0*SQR(tLeaderPosition.y - tCurrentPosition.y) + SQR(tLeaderPosition.z - tCurrentPosition.z);
			for (int i=0, iBestI=-1; i<tpFreeNeighbourNodes.size(); i++) {
				float fCurCost = ffComputeCost(tLeaderPosition,tpFreeNeighbourNodes[i]);
				if (fCurCost < fBestCost) {
					iBestI = i;
					fBestCost = fCurCost;
				}
			}
			if (iBestI >= 0) {
				Fvector tFinishPosition;
				tFinishPosition.x = (tpFreeNeighbourNodes[iBestI].tLeftDown.x + tpFreeNeighbourNodes[iBestI].tRightUp.x)/2.f;
				tFinishPosition.y = (tpFreeNeighbourNodes[iBestI].tLeftDown.y + tpFreeNeighbourNodes[iBestI].tRightUp.y)/2.f;
				tFinishPosition.z = (tpFreeNeighbourNodes[iBestI].tLeftDown.z + tpFreeNeighbourNodes[iBestI].tRightUp.z)/2.f;
				CTravelNode	tCurrentPoint,tFinishPoint;
				tCurrentPoint.P.set(tCurrentPosition);
				tCurrentPoint.floating = FALSE;
				AI_Path.TravelPath.push_back(tCurrentPoint);
				tFinishPoint.P.set(tFinishPosition);
				tFinishPoint.floating = FALSE;
				AI_Path.TravelPath.push_back(tFinishPoint);
			}
			tpFreeNeighbourNodes.clear();
		}
		else {
			m_bMobility = false;
		}
	}
	else {
		vector<SSubNode> tpSubNodes;

		//Msg("Outside the node");

		int iMySubNode = ifDivideNearestNode(tpCurrentNode,tCurrentPosition,tpSubNodes);

		/**
		Level().ObjectSpace.GetNearest(tpSubNodes[iMySubNode].tLeftDown,2*(Level().AI.GetHeader().size));
		//Level().ObjectSpace.GetNearest(tCurrentPosition,3*(Level().AI.GetHeader().size));
		//Level().ObjectSpace.GetNearest(this->cfModel,3*(Level().AI.GetHeader().size));
		CObjectSpace::NL_TYPE &tpNearestList = Level().ObjectSpace.nearest_list;
		if (!tpNearestList.empty()) {
			for (CObjectSpace::NL_IT tppObjectIterator=tpNearestList.begin(); tppObjectIterator!=tpNearestList.end(); tppObjectIterator++) {
				CObject* tpCurrentObject = (*tppObjectIterator)->Owner();
				if (tpCurrentObject == this)
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
		/**/
		CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
		vector<CEntity*> Members = Squad.Groups[g_Group()].Members;
		if (Squad.Leader != this)
			Members.push_back(Squad.Leader);
		for (int i=0; i<Members.size(); i++)
			if (Members[i] != this) {
				float fRadius = Members[i]->Radius();
				Fvector tCenter;
				Members[i]->clCenter(tCenter);
				if (bfInsideSubNode(tCenter,fRadius,tpSubNodes[iMySubNode])) {
					tpSubNodes[iMySubNode].bEmpty = false;
					break;
				}
				/**
				CAI_Rat *tMember = dynamic_cast<CAI_Rat*>(Members[i]);
				if (tMember)
					if (tMember->AI_Path.TravelPath.size() > 1) {
						CAI_Space &AI = Level().AI;
						tCenter = tMember->AI_Path.TravelPath[1].P;
						if (bfInsideSubNode(tCenter,fRadius,tpSubNodes[iMySubNode])) {
							tpSubNodes[iMySubNode].bEmpty = false;
							break;
						}
					}
				/**/
			}
		/**/
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart = 0;
		if (tpSubNodes[iMySubNode].bEmpty) {
			Fvector tFinishPosition;
			tFinishPosition.x = (tpSubNodes[iMySubNode].tLeftDown.x + tpSubNodes[iMySubNode].tRightUp.x)/2.f;
			tFinishPosition.y = (tpSubNodes[iMySubNode].tLeftDown.y + tpSubNodes[iMySubNode].tRightUp.y)/2.f;
			tFinishPosition.z = (tpSubNodes[iMySubNode].tLeftDown.z + tpSubNodes[iMySubNode].tRightUp.z)/2.f;
			CTravelNode	tCurrentPoint,tFinishPoint;
			tCurrentPoint.P.set(tCurrentPosition);
			tCurrentPoint.floating = FALSE;
			AI_Path.TravelPath.push_back(tCurrentPoint);
			tFinishPoint.P.set(tFinishPosition);
			tFinishPoint.floating = FALSE;
			AI_Path.TravelPath.push_back(tFinishPoint);
			m_bMobility = true;
		}
		else {
			m_bMobility = false;
		}
		tpSubNodes.clear();
	}
}

float ffGetDistance(Fvector P1, Fvector P2)
{
	return(float(sqrt(float(SQR(P2.x - P1.x) + SQR(P2.z - P1.z) + SQR(P2.y - P1.y)))));
}

void CAI_Rat::Attack()
{
	// if no more health then rat is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Attack");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiRatDie;
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
			else 
				if (tSavedEnemy) {
					dwLostEnemyTime = Level().timeServer();
					eCurrentState = aiRatPursuit;
				}
				else {
					eCurrentState = tStateStack.top();
					tStateStack.pop();
				}
			return;
		}
		else {
			CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
			// determining who is leader
			CEntity* Leader = Squad.Leader;
			// checking if the leader exists
			R_ASSERT (Leader);
			// checking if leader is dead then make myself a leader
			if (Leader != this)
				if ((Leader->g_Health() <= 0) 
					|| (!(Leader->m_bMobility)) 
					/**/
					|| (
					 (!(Squad.Groups[g_Group()].m_bLeaderViewsEnemy)) && 
					 (Enemy.bVisible)
					 )
					 /**/
					 ) {
						Leader = this;
						Squad.Groups[g_Group()].m_dwLeaderChangeCount++;
						//Msg("%d",Squad.Groups[g_Group()].m_dwLeaderChangeCount);
					}
			
			if (Leader == this) {
				if (Enemy.bVisible) {
					Squad.Groups[g_Group()].m_bLeaderViewsEnemy = true;
					bBuildPathToLostEnemy = false;
					// saving an enemy
					tSavedEnemy = Enemy.Enemy;
					tSavedEnemyPosition = Enemy.Enemy->Position();
					tpSavedEnemyNode = Enemy.Enemy->AI_Node;
					dwSavedEnemyNodeID = Enemy.Enemy->AI_NodeID;
					// determining the team
					// setting watch mode to false
					bool bWatch = false;
					// get pointer to the class of node estimator 
					// for finding the best node in the area
					CRatSelectorAttack S = SelectorAttack;
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
					
					S.taMembers = Squad.Groups[g_Group()].Members;
					if (S.m_tLeader)
						S.taMembers.push_back(S.m_tLeader);

					// checking if I need to rebuild the path i.e. previous search
					// has found better destination node
					if (AI_Path.bNeedRebuild) {
						// building a path from and to
						Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,*(S.m_tpEnemyNode),S.fOptEnemyDistance);
						if (AI_Path.Nodes.size() > 2) {
						// if path is long enough then build travel line
							AI_Path.BuildTravelLine(Position());
							//m_bMobility = true;
						}
						else {
						// if path is too short then clear it (patch for ExecMove)
							AI_Path.TravelPath.clear();
							AI_Path.bNeedRebuild = FALSE;
							//m_bMobility = false;
						}
					} 
					else {
						Squad.Groups[g_Group()].GetAliveMemberInfoWithLeader(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this,Leader);
						// search for the best node according to the 
						// SelectFollow evaluation function in the radius N meteres
						float fOldCost;
						Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
						// if search has found new best node then 
						if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path)) || (!(Leader->m_bMobility))) && (S.BestCost < (fOldCost - S.fLaziness))){
							AI_Path.DestNode		= S.BestNode;
							AI_Path.bNeedRebuild	= TRUE;
							if (AI_Path.DestNode != S.BestNode)
								m_bMobility = true;
						} 
						else {
							// search hasn't found a better node we have to look around
							bWatch = true;
							//if (AI_Path.Nodes.size() <= 2)
							m_bMobility = false;
						}
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
					
					if (ffGetDistance(Position(),Enemy.Enemy->Position()) < S.fOptEnemyDistance) 
						q_action.setup(AI::AIC_Action::FireBegin);
					else
						q_action.setup(AI::AIC_Action::FireEnd);

					m_fCurSpeed = m_fMaxSpeed;
					bStopThinking = true;
					return;
				}
				else {
					Squad.Groups[g_Group()].m_bLeaderViewsEnemy = false;
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
						// setting watch mode to false
						bool bWatch = false;
						// get pointer to the class of node estimator 
						// for finding the best node in the area
						CRatSelectorAttack S = SelectorAttack;
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
							S.m_tLeaderNode = -1;
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
						
						S.taMembers = Squad.Groups[g_Group()].Members;
						if (S.m_tLeader)
							S.taMembers.push_back(S.m_tLeader);
						// building a path from and to
						AI_Path.DestNode = dwSavedEnemyNodeID;
						Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
						if (AI_Path.Nodes.size() > 2) {
						// if path is long enough then build travel line
							AI_Path.BuildTravelLine(Position());
							m_bMobility = true;
						}
						else {
						// if path is too short then clear it (patch for ExecMove)
							AI_Path.TravelPath.clear();
							m_bMobility = false;
						}
					}
					else {
						if (AI_Path.Nodes.size() < 2)
							m_bMobility = false;
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
			}
			else {
				if ((Enemy.Enemy) && (ffGetDistance(Position(),Enemy.Enemy->Position()) < SelectorAttack.fMaxEnemyDistance)) 
					FollowLeader(Squad,Enemy.Enemy);				
				else
					FollowLeader(Squad,Leader);				
				q_look.setup(
					AI::AIC_Look::Look, 
					AI::t_Object, 
					&Enemy,
					1000);
				q_look.o_look_speed=_FB_look_speed;
				
				q_action.setup(AI::AIC_Action::FireEnd);

				if (ffGetDistance(Position(),Enemy.Enemy->Position()) < SelectorAttack.fOptEnemyDistance) 
					q_action.setup(AI::AIC_Action::FireBegin);
				else
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

void CAI_Rat::Cover()
{
	bStopThinking = true;
}

void CAI_Rat::Defend()
{
	bStopThinking = true;
}

void CAI_Rat::Die()
{
	q_look.setup(0,AI::t_None,0,0	);
	q_action.setup(AI::AIC_Action::FireEnd);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	bStopThinking = true;
}

void CAI_Rat::FollowMe()
{
	// if no more health then rat is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Follow me");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiRatDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do I see the enemies?
		if (Enemy.Enemy) {
			tStateStack.push(eCurrentState);
			eCurrentState = aiRatAttack;
			return;
		}
		else {
			// checking if I am under fire
			DWORD dwCurTime = Level().timeServer();
			if (dwCurTime - dwHitTime < HIT_JUMP_TIME) {
				tStateStack.push(eCurrentState);
				eCurrentState = aiRatUnderFire;
				return;
			}
			else {
				if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiRatSenseSomething;
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
					if ((Leader != this) && ((Leader->g_Health() <= 0) || (!(Leader->m_bMobility)))) {
						Leader = this;
						Squad.Groups[g_Group()].m_dwLeaderChangeCount++;
						//Msg("%d",Squad.Groups[g_Group()].m_dwLeaderChangeCount);
					}
					// I am leader then go to state "Free Hunting"
					if ((Leader == this) || (Leader->g_Health() <= 0) || (!(Leader->m_bMobility))) {
						eCurrentState = aiRatFreeHunting;
						return;
					}
					else {
						FollowLeader(Squad,Leader);
						// setting up a look
						// getting my current node
						NodeCompressed* tNode = Level().AI.Node(AI_NodeID);
						// if we are going somewhere
						/**
						Fvector tLook;
						if (!AI_Path.TravelPath.empty()) {
							tLook.sub(AI_Path.TravelPath[1].P,AI_Path.TravelPath[0].P);
							q_look.setup(
								AI::AIC_Look::Look, 
								AI::t_Direction, 
								&tLook,
								1000);
							q_look.o_look_speed=_FB_look_speed;
						}
						/**/
						SetLessCoverLook(tNode);
						// setting up an action
						q_action.setup(AI::AIC_Action::FireEnd);
						// checking flag to stop processing more states
						m_fCurSpeed = m_fMaxSpeed;
						bStopThinking = true;
						return;
					}
				}
			}
		}
	}
}

void CAI_Rat::FreeHunting()
{
	// if no more health then rat is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Free hunting");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiRatDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do I see the enemies?
		/**/
		if (Enemy.Enemy)		{
			tStateStack.push(eCurrentState);
			eCurrentState = aiRatAttack;
			return;
		}
		else {
			// checking if I am under fire
			if (Level().timeServer() - dwHitTime < HIT_JUMP_TIME) {
				tStateStack.push(eCurrentState);
				eCurrentState = aiRatUnderFire;
				return;
			}
			else {
				if (Level().timeServer() - dwSenseTime < SENSE_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiRatSenseSomething;
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
					if ((Leader != this) && ((Leader->g_Health() <= 0) || (!(Leader->m_bMobility)))) {
						Leader = this;
						Squad.Groups[g_Group()].m_dwLeaderChangeCount++;
						//Msg("%d",Squad.Groups[g_Group()].m_dwLeaderChangeCount);
					}
					
					if (Leader == this) {
						// setting up watch mode to false
						bool bWatch = false;
						// get pointer to the class of node estimator 
						// for finding the best node in the area
						CRatSelectorFreeHunting S = SelectorFreeHunting;
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
						
						S.taMembers = Squad.Groups[g_Group()].Members;
						// checking if I need to rebuild the path i.e. previous search
						// has found better destination node
						if (AI_Path.bNeedRebuild) {
							Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
							if (AI_Path.Nodes.size() > 2) {
							// if path is long enough then build travel line
								AI_Path.BuildTravelLine(Position());
								//m_bMobility = true;
							}
							else {
							// if path is too short then clear it (patch for ExecMove)
								AI_Path.TravelPath.clear();
								AI_Path.bNeedRebuild = FALSE;
							}
							//if (AI_Path.Nodes.size() < 2)
							//	m_bMobility = false;
						} 
						else {
							// fill arrays of members and exclude myself
							Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
							// SelectFollow evaluation function in the radius 35 meteres
							float fOldCost;
							Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
							// if search has found new best node then 
							//if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
							if (((AI_Path.DestNode != S.BestNode)) && (S.BestCost < (fOldCost - S.fLaziness))){
								AI_Path.DestNode		= S.BestNode;
								AI_Path.bNeedRebuild	= TRUE;
								//m_bMobility = true;
							} 
							else {
								// search hasn't found a better node we have to look around
								bWatch = true;
								//if (AI_Path.TravelPath.size() < 2)
								//	m_bMobility = false;
							}
							if (AI_Path.TravelPath.size() <= 2)
								AI_Path.bNeedRebuild	= TRUE;
						}
			/**/
					}
					else {
						FollowLeader(Squad,Leader);
					}
					// setting up a look
					// getting my current node
					NodeCompressed* tNode		= Level().AI.Node(AI_NodeID);
					
					SetLessCoverLook(tNode);
					
					q_action.setup(AI::AIC_Action::FireEnd);
					// checking flag to stop processing more states
					m_fCurSpeed = m_fMaxSpeed;
					bStopThinking = true;
					return;
				}
			}
		}
		/**/
	}
}

void CAI_Rat::GoInThisDirection()
{
}

void CAI_Rat::GoToThisPosition()
{
}

void CAI_Rat::HoldPositionUnderFire()
{
	bStopThinking = true;
}

void CAI_Rat::HoldThisPosition()
{
	bStopThinking = true;
}

void CAI_Rat::Pursuit()
{
	// if no more health then rat is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Pursuit");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiRatDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do the enemies exist?
		if (Enemy.Enemy) {
			eCurrentState = aiRatAttack;
			return;
		}
		else {
			DWORD dwCurrentTime = Level().timeServer();
			if (dwCurrentTime - dwLostEnemyTime < LOST_ENEMY_REACTION_TIME) {
				// checking if I am under fire
				if (dwCurrentTime - dwHitTime < HIT_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiRatUnderFire;
					return;
				}
				else {
					if (dwCurrentTime - dwSenseTime < SENSE_JUMP_TIME) {
						tStateStack.push(eCurrentState);
						eCurrentState = aiRatSenseSomething;
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
						if ((Leader != this) && ((Leader->g_Health() <= 0) || (!(Leader->m_bMobility)))) {
							Leader = this;
							Squad.Groups[g_Group()].m_dwLeaderChangeCount++;
							//Msg("%d",Squad.Groups[g_Group()].m_dwLeaderChangeCount);
						}
						// get pointer to the class of node estimator 
						// for finding the best node in the area
						CRatSelectorPursuit S = SelectorPursuit;
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
						
						S.taMembers = Squad.Groups[g_Group()].Members;
						bool bWatch = false;
						// checking if I need to rebuild the path i.e. previous search
						// has found better destination node
						if (AI_Path.bNeedRebuild) {
							Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
							if (AI_Path.Nodes.size() > 2) {
							// if path is long enough then build travel line
								AI_Path.BuildTravelLine(Position());
								m_bMobility = true;
							}
							else {
							// if path is too short then clear it (patch for ExecMove)
								AI_Path.TravelPath.clear();
								AI_Path.bNeedRebuild = FALSE;
								m_bMobility = false;
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
							else {
								// search hasn't found a better node we have to look around
								bWatch = true;
								m_bMobility = false;
							}
							if (AI_Path.TravelPath.size() < 2)
								AI_Path.bNeedRebuild	= TRUE;
						}
						// setting up a look
						// getting my current node
						NodeCompressed* tNode		= Level().AI.Node(AI_NodeID);
						SetLessCoverLook(tNode);
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

void CAI_Rat::Retreat()
{
	bStopThinking = true;
}

void CAI_Rat::SenseSomething()
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

void CAI_Rat::UnderFire()
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
		eCurrentState = aiRatDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do I see the enemies?
		if (Enemy.Enemy)		{
			tStateStack.push(eCurrentState);
			eCurrentState = aiRatAttack;
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
					eCurrentState = aiRatSenseSomething;
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
					if ((Leader != this) && ((Leader->g_Health() <= 0) || (!(Leader->m_bMobility)))) {
						Leader = this;
						Squad.Groups[g_Group()].m_dwLeaderChangeCount++;
						//Msg("%d",Squad.Groups[g_Group()].m_dwLeaderChangeCount);
					}
					// I am leader then go to state "Free Hunting"
					bool bWatch = false;
					// get pointer to the class of node estimator 
					// for finding the best node in the area
					CRatSelectorUnderFire S = SelectorUnderFire;
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
					
					S.taMembers = Squad.Groups[g_Group()].Members;
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
						for (int i=0; i<S.taMemberPositions.size(); i++)
							if ((S.taMembers[i]->g_Health() > 0) && (bfCheckForMember(tHitDir,S.m_tMyPosition,S.taMemberPositions[i]))) {
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

void CAI_Rat::WaitOnPosition()
{
	bStopThinking = true;
}

void CAI_Rat::Think()
{
	bStopThinking = false;
	do {
		switch(eCurrentState) {
			case aiRatDie : {
				Die();
				break;
			}
			case aiRatUnderFire : {
				UnderFire();
				break;
			}
			case aiRatSenseSomething : {
				SenseSomething();
				break;
			}
			case aiRatGoInThisDirection : {
				GoInThisDirection();
				break;
			}
			case aiRatGoToThisPosition : {
				GoToThisPosition();
				break;
			}
			case aiRatWaitOnPosition : {
				WaitOnPosition();
				break;
			}
			case aiRatHoldThisPosition : {
				HoldThisPosition();
				break;
			}
			case aiRatHoldPositionUnderFire : {
				HoldPositionUnderFire();
				break;
			}
			case aiRatFreeHunting : {
				FreeHunting();
				break;
			}
			case aiRatFollowMe : {
				FollowMe();
				break;
			}
			case aiRatAttack : {
				Attack();
				break;
			}
			case aiRatDefend : {
				Defend();
				break;
			}
			case aiRatPursuit : {
				Pursuit();
				break;
			}
			case aiRatRetreat : {
				Retreat();
				break;
			}
			case aiRatCover : {
				Cover();
				break;
			}
		}
	}
	while (!bStopThinking);
}

void CAI_Rat::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	R_ASSERT(fsimilar(_view.magnitude(),1));
	R_ASSERT(fsimilar(_move.magnitude(),1));

	CMotionDef*	S=0;
/**/
	if (iHealth<=0)
		S = m_death;
	else {
		if (speed<0.2f)
			S = m_idle;
		else {
			Fvector view = _view; 
			Fvector move = _move; 
			view.y=0; 
			move.y=0; 
			view.normalize_safe();
			move.normalize_safe();
			float	dot  = view.dotproduct(move);
			
			SAnimState* AState = &m_walk;
			AState = &m_walk;
			
			if (speed>2.f)
				AState = &m_run;
			
			S = AState->fwd;
		}
	}
/**/
	if (S!=m_current){ 
		m_current = S;
		if (S) PKinematics(pVisual)->PlayCycle(S);
	}
}

void CAI_Rat::net_Export(NET_Packet* P)					// export to server
{
	R_ASSERT				(net_Local);

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P->w_u32				(N.dwTimeStamp);
	P->w_u8					(0);
	P->w_vec3				(N.p_pos);
	P->w_angle8				(N.o_model);
	P->w_angle8				(N.o_torso.yaw);
	P->w_angle8				(N.o_torso.pitch);
}

void CAI_Rat::net_Import(NET_Packet* P)
{
	R_ASSERT				(!net_Local);
	net_update				N;

	u8 flags;
	P->r_u32				(N.dwTimeStamp);
	P->r_u8					(flags);
	P->r_vec3				(N.p_pos);
	P->r_angle8				(N.o_model);
	P->r_angle8				(N.o_torso.yaw);
	P->r_angle8				(N.o_torso.pitch);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	bVisible				= TRUE;
	bEnabled				= TRUE;
}

void CAI_Rat::Exec_Action	( float dt )
{
	//*** process action commands
	AI::C_Command* C	= &q_action;
	AI::AIC_Action* L	= (AI::AIC_Action*)C;
	switch (L->Command) {
		case AI::AIC_Action::AttackJumpBegin: {
			break;
		}
		case AI::AIC_Action::AttackJumpEnd: {
			break;
		}
		default:
			break;
	}
	if (Device.dwTimeGlobal>=L->o_timeout)	L->setTimeout();
}

