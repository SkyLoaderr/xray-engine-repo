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
class	SelectorFollow	: public SelectorBase
{
public:
	SelectorFollow	() { Name="sel_follow"; };

	virtual void	Load	(CInifile* ini, const char* section)
	{
		SelectorBase::Load	(ini,section);
	};

	virtual	float	Estimate(NodeCompressed* Node, float dist, BOOL& bStop)	// min - best, max - worse
	{
		float	cost				= 0;
		
		// travel cost
		float	cDist	= dist*costTravel;
		if (cost>BestCost)			return cost;

		// lighting cost
		float	cLight	= float(Node->light)/255.f;
		cost	+=	cLight*costLight;
		if (cost>BestCost)			return cost;
		
		// tactical cost
		float	cCover	= float(Node->cover[0])+float(Node->cover[1])+float(Node->cover[2])+float(Node->cover[3]);
		cCover			= cCover/1024.f;
		cost	+=	cCover*costCover;
		if (cost>BestCost)			return cost;

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
		if (cost>BestCost)			return cost;

		// lighting cost
		float	cLight	= float(Node->light)/255.f;
		cost	+=	cLight*costLight;
		if (cost>BestCost)			return cost;
		
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
		if (cost>BestCost)			return cost;

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
		if (cost>BestCost)			return cost;

		// lighting cost
		float	cLight	= float(Node->light)/255.f;
		cost	+=	cLight*costLight;
		if (cost>BestCost)			return cost;
		
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
		if (cost>BestCost)			return cost;

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

