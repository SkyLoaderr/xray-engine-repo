#pragma once

#include "group.h"

class	SelectorBase	: public AI::NodeEstimator
{
public:
	LPSTR			Name;
	float			laziness;
	float			costLight,costCover,costMembers,costTarget,costTravel;
	float			distTargetMin,distTargetMax,distMemberMin;

	Fvector			posMy;
	Fvector			posTarget;
	MemberPlacement	Members;

	virtual	void	Load	(CInifile* ini, const char* section)
	{
		sscanf(ini->ReadSTRING(section,Name),"%f,%f,%f,%f,%f,%f,%f,%f,%f",
			&laziness,
			&costLight,&costCover,&costMembers,&costTarget,&costTravel,
			&distTargetMin,&distTargetMax,&distMemberMin);
	}

	// Helper functions
	IC float		h_factor(int s1, int s2, float f1, float f2, BYTE* data)
	{
		return	(float(data[s1])*_abs(f1) + float(data[s2])*_abs(f2))/255.f;
	}
	IC float		h_slerp	(Fvector& T, Fvector& S, BYTE* data)
	{
		Fvector2	D;
		D.x			= T.x - S.x;
		D.y			= T.z - S.z;
		D.norm		();

		if (D.y>0)	{
			// 0-1-2
			if (D.x>0)	return h_factor(1,2,D.y,D.x,data);	// 1-2
			else		return h_factor(0,1,D.x,D.y,data);	// 0-1
		} else {
			// 2-3-0
			if (D.x>0)	return h_factor(2,3,D.x,D.y,data);	// 2-3
			else		return h_factor(3,0,D.y,D.x,data);	// 3-0
		}
	}
};

//*****************************************************************************************************************
//*****************************************************************************************************************
//*****************************************************************************************************************
#define SQR(x) ((x)*(x))
#define OPTIMAL_PLAYER_DISTANCE  30.f
#define OPTIMAL_MEMBER_DISTANCE  80.f
#define MIN_PLAYER_PENALTY		1000.f
#define MAX_PLAYER_PENALTY		1000.f
#define MIN_MEMBER_PENALTY		 100.f
#define MAX_MEMBER_PENALTY		 100.f
#define MIN_PLAYER_DISTANCE		distTargetMin
#define MAX_PLAYER_DISTANCE		distTargetMax
#define MIN_MEMBER_DISTANCE		3*distTargetMin
#define MAX_MEMBER_DISTANCE		2*distTargetMax
#define Y_PENALTY				1000.f;

class	SelectorFollow	: public SelectorBase
{
public:
	SelectorFollow	() { Name="sel_follow"; };

	virtual void	Load	(CInifile* ini, const char* section)
	{
		SelectorBase::Load	(ini,section);
	};

/**/
virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)	// min - best, max - worse
{
	// distance to node
	float fResult = 0.0;//*fDistance*costTravel;
	// node lighting
	fResult += ((float)(tNode->light)/255.f)*costLight;
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
	
	// cover from leader
	Fvector tDirection;
	tDirection.x = posTarget.x - P.x;
	tDirection.y = fabs(posTarget.y - P.y);
	tDirection.z = posTarget.z - P.z;

	if (tDirection.x < 0.0)
		fResult += costCover*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
	else
		fResult += costCover*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
	
	if (tDirection.z < 0.0)
		fResult += costCover*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
	else
		fResult += costCover*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
	
	if (tDirection.y > 2.0)
		fResult += tDirection.y*Y_PENALTY;

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
				fResult += 0.5*costCover*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
			else
				fResult += 0.5*costCover*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
			
			if (tDirection.z < 0.0)
				fResult += 0.5*costCover*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
			else
				fResult += 0.5*costCover*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
			
			//if (tDirection.y > 2.0)
			//	fResult += tDirection.y*Y_MEMBER_PENALTY;
			/**/
		}
	}
	// exit
	if (fResult<EPS)	bStop = TRUE;
	return	fResult;
}

/**
	virtual	float	Estimate(tNodeCompressed* Node, float dist, BOOL& bStop)	// min - best, max - worse
	{
		float	cost				= 0;
		
		// travel cost
		float	cDist	= dist*costTravel;
		cost += cDist;
		//if (cost>BestCost)			return cost;

		// lighting cost
		float	cLight	= float(Node->light)/255.f;
		cost	+=	cLight*costLight;
		//if (cost>BestCost)			return cost;
		
		// tactical cost
		float	cCover	= float(Node->cover[0])+float(Node->cover[1])+float(Node->cover[2])+float(Node->cover[3]);
		cCover			= cCover/1024.f;
		cost	+=	cCover*costCover;
		//if (cost>BestCost)			return cost;

		// cost of leader  relation
		Fvector		P,P1,P2;
		Level().AI.UnpackPosition	(P1,Node->p0);
		Level().AI.UnpackPosition	(P2,Node->p1);
		P.lerp						(P1,P2,.5f);
		float						cL	= P.distance_to(posTarget);
		if (cL<distTargetMin)		cL	= (distTargetMin-cL)*15;
		else if (cL>distTargetMax)	cL	= (cL-distTargetMax);
		else cL = 0;
		cost	+=	cL*costTarget;
		//if (cost>BestCost)			return cost;

		// cost of members relationship
		if (Members.size()) {
			for (DWORD it=0; it<Members.size(); it++)
			{
				float c					= P.distance_to	(Members[it]);
				if (c<distMemberMin)	{
					cost += (distMemberMin-c)*costMembers;
					if (cost>BestCost)	return cost;
				}
			}
		}

		// exit
		if (cost<EPS)	bStop = TRUE;
		return	cost;
	}
/**/
};

//*****************************************************************************************************************
//*****************************************************************************************************************
//*****************************************************************************************************************
class	SelectorAttack	: public SelectorBase
{
public:
	SelectorAttack	() { Name="sel_attack"; };

	virtual void	Load	(CInifile* ini, const char* section)
	{
		SelectorBase::Load	(ini,section);
	};

	virtual	float	Estimate(NodeCompressed* Node, float dist, BOOL& bStop)	// min - best, max - worse
	{
		float	cost				= 0;
		
		// travel cost
		float	cDist	= dist*costTravel;
		//if (cost>BestCost)			return cost;

		// lighting cost
		float	cLight	= float(Node->light)/255.f;
		cost	+=	cLight*costLight;
		//if (cost>BestCost)			return cost;
		
		// cost of leader  relation
		Fvector		P,P1,P2;
		Level().AI.UnpackPosition	(P1,Node->p0);
		Level().AI.UnpackPosition	(P2,Node->p1);
		P.lerp						(P1,P2,.5f);
		float						cL	= P.distance_to(posTarget);
		if (cL<distTargetMin)		cL	= (distTargetMin-cL)*15;
		else if (cL>distTargetMax)	cL	= (cL-distTargetMax);
		else cL = 0;
		cost	+=	cL*costTarget;
		//if (cost>BestCost)			return cost;

		// tactical cost
		cost	+=	costCover * h_slerp(posTarget,P,Node->cover);
		//if (cost>BestCost)			return cost;

		// cost of members relationship
		if (Members.size()) {
			for (DWORD it=0; it<Members.size(); it++)
			{
				float c					= P.distance_to	(Members[it]);
				if (c<distMemberMin)	{
					cost += (distMemberMin-c)*costMembers;
					if (cost>BestCost)	return cost;
				}
			}
		}

		// exit
		if (cost<EPS)	bStop = TRUE;
		return	cost;
	}
};

//*****************************************************************************************************************
//*****************************************************************************************************************
//*****************************************************************************************************************
class	SelectorPursuit	: public SelectorBase
{
public:
	SelectorPursuit	() { Name="sel_pursuit"; };

	virtual void	Load	(CInifile* ini, const char* section)
	{
		SelectorBase::Load	(ini,section);
	};

	virtual	float	Estimate(NodeCompressed* Node, float dist, BOOL& bStop)	// min - best, max - worse
	{
		float	cost				= 0;
		
		// travel cost
		float	cDist	= dist*costTravel;
		//if (cost>BestCost)			return cost;

		// lighting cost
		float	cLight	= float(Node->light)/255.f;
		cost	+=	cLight*costLight;
		//if (cost>BestCost)			return cost;
		
		// cost of leader  relation
		Fvector		P,P1,P2;
		Level().AI.UnpackPosition	(P1,Node->p0);
		Level().AI.UnpackPosition	(P2,Node->p1);
		P.lerp						(P1,P2,.5f);
		float						cL	= P.distance_to(posTarget);
		if (cL<distTargetMin)		cL	= (distTargetMin-cL)*15;
		else if (cL>distTargetMax)	cL	= (cL-distTargetMax);
		else cL = 0;
		cost	+=	cL*costTarget;
		if (cost>BestCost)			return cost;

		// tactical cost
		cost	+=	costCover * h_slerp(posTarget,P,Node->cover);
		//if (cost>BestCost)			return cost;

		// cost of members relationship
		if (Members.size()) {
			for (DWORD it=0; it<Members.size(); it++)
			{
				float c					= P.distance_to	(Members[it]);
				if (c<distMemberMin)	{
					cost += (distMemberMin-c)*costMembers;
					if (cost>BestCost)	return cost;
				}
			}
		}

		// exit
		if (cost<EPS)	bStop = TRUE;
		return	cost;
	}
};

