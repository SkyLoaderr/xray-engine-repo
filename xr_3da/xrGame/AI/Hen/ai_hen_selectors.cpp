////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.cpp
//	Created 	: 05.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_hen_selectors.h"

void CHenSelectorBase::Load(CInifile* ini, const char* section)
{
	sscanf(ini->ReadSTRING(section,Name),
		"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
		fTravelWeight,
		
		fLightWeight,
		
		fLaziness,

		fTotalViewVectorWeight,

		fCoverFromLeaderWeight,
		fOptLeaderDistance,
		fOptLeaderDistanceWeight,
		fMinLeaderDistance,
		fMinLeaderDistanceWeight,
		fMaxLeaderDistance,
		fMaxLeaderDistanceWeight,
		fLeaderViewDeviationWeight,
		fMaxLeaderHeightDistance,
		fMaxLeaderHeightDistanceWeight,
		
		fCoverFromMemberWeight,
		fOptMemberDistance,
		fOptMemberDistanceWeight,
		fMinMemberDistance,
		fMinMemberDistanceWeight,
		fMaxMemberDistance,
		fMaxMemberDistanceWeight,
		fMemberViewDeviationWeight,
		
		fCoverFromEnemyWeight,
		fOptEnemyDistance,
		fOptEnemyDistanceWeight,
		fMinEnemyDistance,
		fMinEnemyDistanceWeight,
		fMaxEnemyDistance,
		fMaxEnemyDistanceWeight,
		fEnemyViewDeviationWeight
	);
};

CHenSelectorAttack::CHenSelectorAttack()
{ 
	Name = "selector_attack"; 
}

float CHenSelectorAttack::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	/**
	// distance to node
	float fResult = 0.0;//*fDistance*costTravel;
	// node lighting
	fResult += ((float)(tNode->light)/255.f)*fLightWeight;
	if (fResult>BestCost)
		return(fResult);
	// leader relationship
	Fvector		P,P1,P2;
	Level().AI.UnpackPosition	(P1,tNode->p0);
	Level().AI.UnpackPosition	(P2,tNode->p1);
	P.lerp						(P1,P2,.5f);
	float						fDistanceToPlayer = P.distance_to(posTarget);
	
	if (fDistanceToPlayer < MIN_ENEMY_DISTANCE)
		fResult += MIN_ENEMY_PENALTY*costTarget;
	else
		if (fDistanceToPlayer > MAX_ENEMY_DISTANCE)
			fResult += MAX_ENEMY_PENALTY*costTarget;
		else
			fResult += (fDistanceToPlayer - OPTIMAL_ENEMY_DISTANCE)/OPTIMAL_ENEMY_DISTANCE;
	if (fResult>BestCost)
		return(fResult);
	
	// cover from leader
	Fvector tDirection;
	tDirection.x = posTarget.x - P.x;
	tDirection.y = fabs(posTarget.y - P.y);
	tDirection.z = posTarget.z - P.z;

	if (tDirection.x < 0.0)
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
	else
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
	
	if (tDirection.z < 0.0)
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
	else
		fResult += COVER_PENALTY*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
	
	if (fResult>BestCost)
		return(fResult);
	
	tDirection.normalize(); 
	
	//fResult += ENEMY_VIEW_PENALTY*(1.f - sqrt(SQR(tDirection.x - tTargetDirection.x) + SQR(tDirection.y - tTargetDirection.y) + SQR(tDirection.z - tTargetDirection.z)));
	//if (fResult>BestCost)
	//	return(fResult);
	
	//if (tDirection.y > 2.0)
	//	fResult += tDirection.y*Y_PENALTY;

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
			if (fResult>BestCost)
				return(fResult);
			
			Fvector tDirection1;
			tDirection1.x += Members[it].x - P.x;
			tDirection1.y += fabs(Members[it].y - P.y);
			tDirection1.z += Members[it].z - P.z;
			tDirection1.normalize();
			tDirection.add(tDirection1);

			if (Members[it].x - P.x < 0.0)
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
			else
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
			
			if (Members[it].z - P.z < 0.0)
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
			else
				fResult += 0.5*COVER_PENALTY*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);

		}
	}

	fResult += SURROUND_PENALTY*sqrt(SQR(tDirection.x) + SQR(tDirection.y) + SQR(tDirection.z));

	// exit
	//if (fResult<EPS)	bStop = TRUE;
	return	fResult;
	/**/
	return(0);
}

CHenSelectorFreeHunting::CHenSelectorFreeHunting()
{ 
	Name = "selector_free_hunting"; 
}

float CHenSelectorFreeHunting::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// distance to node
	float fResult = fDistance*fTravelWeight;
	if (fResult>BestCost)
		return(fResult);
	
	// node lighting
	fResult += ((float)(tNode->light)/255.f)*fLightWeight;
	if (fResult>BestCost)
		return(fResult);
	
	// leader relationship
	Fvector	P,P1,P2;
	Level().AI.UnpackPosition(P1,tNode->p0);
	Level().AI.UnpackPosition(P2,tNode->p1);
	P.lerp(P1,P2,.5f);
	float fDistanceToLeader = P.distance_to(tLeaderPosition);
	
	if (fDistanceToLeader < fMinLeaderDistance)
		fResult += fMinLeaderDistanceWeight;
	else
		if (fDistanceToLeader > fMaxLeaderDistance)
			fResult += fMaxLeaderDistanceWeight;
		else
			fResult += (fDistanceToLeader - fOptLeaderDistance)/fOptLeaderDistance;
	
	if (fResult>BestCost)
		return(fResult);
	
	// cover from leader
	Fvector tDirection;
	tDirection.x = tLeaderPosition.x - P.x;
	tDirection.y = fabs(tLeaderPosition.y - P.y);
	tDirection.z = tLeaderPosition.z - P.z;

	if (tDirection.x < 0.0)
		fResult += fCoverFromLeaderWeight*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
	else
		fResult += fCoverFromLeaderWeight*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
	
	if (tDirection.z < 0.0)
		fResult += fCoverFromLeaderWeight*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
	else
		fResult += fCoverFromLeaderWeight*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
	
	if (fResult>BestCost)
		return(fResult);
	
	if (tDirection.y > fMaxLeaderHeightDistance)
		fResult += tDirection.y*fMaxLeaderHeightDistanceWeight;

	if (fResult>BestCost)
		return(fResult);

	// cost of members relationship
	if (taMemberPositions.size()) {
		for (DWORD it=0; it<taMemberPositions.size(); it++) {
			float fDistanceToMember	= P.distance_to	(taMemberPositions[it]);
			
			if (fDistanceToMember < fMinMemberDistance)
				fResult += fMinMemberDistanceWeight;
			else
				if (fDistanceToMember > fMaxMemberDistance)
					fResult += fMaxMemberDistanceWeight;
				else
					fResult += fOptMemberDistanceWeight*(fDistanceToMember - fOptMemberDistance)/fOptMemberDistance;
			
			tDirection.x = taMemberPositions[it].x - P.x;
			tDirection.y = fabs(taMemberPositions[it].y - P.y);
			tDirection.z = taMemberPositions[it].z - P.z;

			if (tDirection.x < 0.0)
				fResult += fCoverFromMemberWeight*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
			else
				fResult += fCoverFromMemberWeight*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
			
			if (tDirection.z < 0.0)
				fResult += fCoverFromMemberWeight*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
			else
				fResult += fCoverFromMemberWeight*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
			
			if (fResult>BestCost)
				return(fResult);
		}
	}
	//if (fResult<EPS)	bStop = TRUE;
	return	fResult;
}

CHenSelectorFollow::CHenSelectorFollow()
{ 
	Name = "selector_follow"; 
}

float CHenSelectorFollow::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// distance to node
	float fResult = fDistance*fTravelWeight;
	if (fResult>BestCost)
		return(fResult);
	
	// node lighting
	fResult += ((float)(tNode->light)/255.f)*fLightWeight;
	if (fResult>BestCost)
		return(fResult);
	
	// leader relationship
	Fvector	P,P1,P2;
	Level().AI.UnpackPosition(P1,tNode->p0);
	Level().AI.UnpackPosition(P2,tNode->p1);
	P.lerp(P1,P2,.5f);
	float fDistanceToLeader = P.distance_to(tLeaderPosition);
	
	if (fDistanceToLeader < fMinLeaderDistance)
		fResult += fMinLeaderDistanceWeight;
	else
		if (fDistanceToLeader > fMaxLeaderDistance)
			fResult += fMaxLeaderDistanceWeight;
		else
			fResult += (fDistanceToLeader - fOptLeaderDistance)/fOptLeaderDistance;
	
	if (fResult>BestCost)
		return(fResult);
	
	// cover from leader
	Fvector tDirection;
	tDirection.x = tLeaderPosition.x - P.x;
	tDirection.y = fabs(tLeaderPosition.y - P.y);
	tDirection.z = tLeaderPosition.z - P.z;

	if (tDirection.x < 0.0)
		fResult += fCoverFromLeaderWeight*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
	else
		fResult += fCoverFromLeaderWeight*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
	
	if (tDirection.z < 0.0)
		fResult += fCoverFromLeaderWeight*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
	else
		fResult += fCoverFromLeaderWeight*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
	
	if (fResult>BestCost)
		return(fResult);
	
	if (tDirection.y > fMaxLeaderHeightDistance)
		fResult += tDirection.y*fMaxLeaderHeightDistanceWeight;

	if (fResult>BestCost)
		return(fResult);

	// cost of members relationship
	if (taMemberPositions.size()) {
		for (DWORD it=0; it<taMemberPositions.size(); it++) {
			float fDistanceToMember	= P.distance_to	(taMemberPositions[it]);
			
			if (fDistanceToMember < fMinMemberDistance)
				fResult += fMinMemberDistanceWeight;
			else
				if (fDistanceToMember > fMaxMemberDistance)
					fResult += fMaxMemberDistanceWeight;
				else
					fResult += fOptMemberDistanceWeight*(fDistanceToMember - fOptMemberDistance)/fOptMemberDistance;
			
			tDirection.x = taMemberPositions[it].x - P.x;
			tDirection.y = fabs(taMemberPositions[it].y - P.y);
			tDirection.z = taMemberPositions[it].z - P.z;

			if (tDirection.x < 0.0)
				fResult += fCoverFromMemberWeight*(1.0 - float(tNode->cover[0])/255.f + float(tNode->cover[2])/255.f);
			else
				fResult += fCoverFromMemberWeight*(1.0 - float(tNode->cover[2])/255.f + float(tNode->cover[0])/255.f);
			
			if (tDirection.z < 0.0)
				fResult += fCoverFromMemberWeight*(1.0 - float(tNode->cover[3])/255.f + float(tNode->cover[1])/255.f);
			else
				fResult += fCoverFromMemberWeight*(1.0 - float(tNode->cover[1])/255.f + float(tNode->cover[3])/255.f);
			
			if (fResult>BestCost)
				return(fResult);
		}
	}
	//if (fResult<EPS)	bStop = TRUE;
	return	fResult;
}

CHenSelectorPursuit::CHenSelectorPursuit()
{ 
	Name = "selector_pursuit"; 
}

float CHenSelectorPursuit::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	/**
	// distance to node
	float fResult = 0.0;//*fDistance*costTravel;
	// node lighting
	fResult += ((float)(tNode->light)/255.f)*fLightWeight;
	if (fResult>BestCost)
		return(fResult);

	// cover
	fResult += COVER_FREEHUNTING*((float)(tNode->cover[0])/255.f + (float)(tNode->cover[1])/255.f
				+ (float)(tNode->cover[2])/255.f + (float)(tNode->cover[3])/255.f);

	// exit
	//if (fResult<EPS)	bStop = TRUE;
	return	fResult;
	/**/
	return(0);
}

#define COVER_UNDER_FIRE			10.f
#define DEVIATION_FROM_DIRECTION	100.f
#define WRONG_DIRECTION_PENALTY		100.f

CHenSelectorUnderFire::CHenSelectorUnderFire()
{ 
	Name = "selector_under_fire"; 
}

float CHenSelectorUnderFire::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)	// min - best, max - worse
{
	/**
	// distance to node
	float fResult = 0.0;//*fDistance*costTravel;
	// node lighting
	fResult += ((float)(tNode->light)/255.f)*fLightWeight;
	if (fResult>BestCost)
		return(fResult);

	// cover
	fResult += COVER_UNDER_FIRE*((float)(tNode->cover[0])/255.f + (float)(tNode->cover[1])/255.f
				+ (float)(tNode->cover[2])/255.f + (float)(tNode->cover[3])/255.f);

	Fvector tNodeCentre;
	tNodeCentre.x = (float(tNode->p0.x)/255.f + float(tNode->p1.x)/255.f)/2.f - posMy.x;
	tNodeCentre.y = (float(tNode->p0.y)/255.f + float(tNode->p1.y)/255.f)/2.f - posMy.y;
	tNodeCentre.z = (float(tNode->p0.z)/255.f + float(tNode->p1.z)/255.f)/2.f - posMy.z;

	float cos_alpha = tNodeCentre.dotproduct(tTargetDirection);
	float sin_alpha = sqrt(1 - SQR(cos_alpha));
	float deviation = tNodeCentre.square_magnitude()*sin_alpha;

	fResult += DEVIATION_FROM_DIRECTION*deviation;

	fResult += (cos_alpha < 0.f ? WRONG_DIRECTION_PENALTY : 0.f);
	// exit
	return	fResult;
	/**/
	return(0);
}
