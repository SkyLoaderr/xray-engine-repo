#pragma once

#include "..\fixedset.h"

class	CEntity;

DEFINE_VECTOR(CEntity*,EntityVec,EntityIt);

#define MAX_SUSPICIOUS_NODE_COUNT 32
#define MAX_GROUP_SIZE 256

typedef struct tagSSearchPlace {
	u32	dwNodeID:24;
	u32	dwSearched:2;
	u32	dwGroup:6;
	float	fCost;
} SSearchPlace;

typedef svector<Fvector,MAX_GROUP_SIZE>						MemberPlacement;
typedef svector<u32,MAX_GROUP_SIZE>						MemberNodes;
typedef svector<SSearchPlace,MAX_SUSPICIOUS_NODE_COUNT>		SuspiciousNodes;
typedef svector<bool,MAX_SUSPICIOUS_NODE_COUNT>				SuspiciousGroups;
typedef FixedSET<CObject*>									objVisible;

class CGroup
{
public:
	u32						Flags;

	Fvector						vTargetDirection;
	Fvector						vTargetPosition;

	EntityVec					Members;
	Fvector						vCentroid;

public:
	
	bool						m_bLeaderViewsEnemy;
	bool						m_bEnemyNoticed;
	u32						m_dwLeaderChangeCount;
	Fvector						m_tLastHitDirection;
	Fvector						m_tHitPosition;
	u32						m_dwLastHitTime;
	u32						m_dwFiring;
	bool						m_bLessCoverLook;
	u32						m_dwLastViewChange;
	SuspiciousNodes				m_tpaSuspiciousNodes;
	SuspiciousGroups			m_tpaSuspiciousGroups;
	vector<u32>				m_tpaPatrolPath;

	// for DDD
	u32						m_dwLastActionTime;
	u32						m_dwLastAction;

	// active
	u32						m_dwActiveCount;
	u32						m_dwAliveCount;
	u32						m_dwStandingCount;

	// Constructor
								CGroup				();
	
	void						Member_Add			(CEntity* E);
	void						Member_Remove		(CEntity* E);

	bool						Empty				(){return Members.empty();}
	int							Size				(){return Members.size();}

	// Internal communication
	void						GetMemberPlacement	(MemberPlacement& P, CEntity* Me);
	void						GetMemberDedication	(MemberPlacement& P, CEntity* Me);
	void						GetMemberPlacementN	(MemberNodes& P, CEntity* Me);
	void						GetMemberDedicationN(MemberNodes& P, CEntity* Me);
	void						GetMemberInfo		(MemberPlacement& P0, MemberNodes& P1, MemberPlacement& P2, MemberNodes& P3, CEntity* Me);
	
	void						GetAliveMemberPlacement	(MemberPlacement& P, CEntity* Me);
	void						GetAliveMemberDedication	(MemberPlacement& P, CEntity* Me);
	void						GetAliveMemberPlacementN	(MemberNodes& P, CEntity* Me);
	void						GetAliveMemberDedicationN(MemberNodes& P, CEntity* Me);
	void						GetAliveMemberInfo		(MemberPlacement& P0, MemberNodes& P1, MemberPlacement& P2, MemberNodes& P3, CEntity* Me);

	void						GetAliveMemberPlacementWithLeader(MemberPlacement& P, CEntity* Me, CEntity* Leader);
	void						GetAliveMemberDedicationWithLeader(MemberPlacement& P, CEntity* Me, CEntity* Leader);
	void						GetAliveMemberPlacementNWithLeader(MemberNodes& P, CEntity* Me, CEntity* Leader);
	void						GetAliveMemberDedicationNWithLeader(MemberNodes& P, CEntity* Me, CEntity* Leader);
	void						GetAliveMemberInfoWithLeader(MemberPlacement& P0, MemberNodes& P1, MemberPlacement& P2, MemberNodes& P3, CEntity* Me, CEntity* Leader);
	const Fvector&				GetCentroid();
};
