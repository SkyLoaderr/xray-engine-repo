#pragma once

class	CEntity;

enum	EGroupState
{
	gsFreeHunting=0,			//	��������� �����					
	gsFollowMe,					//	���� �� ��� (����� ������)		
	gsGoInThisDirection,		//	���� �� �����������: "����������" 
	gsGoToThisPosition,			//	���� � �������� �������:			
	gsHoldPosition,				//	���������� �������				

	gsLast,
	gsForceDWORD = DWORD(-1)
};

enum	EGroupTriggers
{
	gtAgressive		= (1<<0),	// ������ ��� ����������� ����� / �������� ������ ��� ������
	gtQuiet			= (1<<1),	// ���� �� ����� �������� ����������� / ������ (����� �� �����������) �� ����� �����
	gtForceDWORD	= DWORD(-1)
};

DEFINE_VECTOR(CEntity*,EntityVec,EntityIt);

#define MAX_SUSPICIOUS_NODE_COUNT 32
#define MAX_GROUP_SIZE 256

typedef struct tagSSearchPlace {
	DWORD	dwNodeID:24;
	DWORD	dwSearched:2;
	DWORD	dwGroup:6;
	float	fCost;
} SSearchPlace;

typedef struct tagSSearchGroup {
	BYTE		cIndex;
	Fvector		tPoint;
	BYTE		cGroup;
} SSearchGroup;

typedef svector<Fvector,MAX_GROUP_SIZE>				MemberPlacement;
typedef svector<DWORD,MAX_GROUP_SIZE>				MemberNodes;
typedef svector<SSearchPlace,MAX_SUSPICIOUS_NODE_COUNT>	SuspiciousNodes;
typedef svector<SSearchGroup,MAX_SUSPICIOUS_NODE_COUNT>	SuspiciousGroups;
typedef svector<Fvector,MAX_SUSPICIOUS_NODE_COUNT>		SuspiciousForces;

class CGroup
{
public:
	EGroupState					State;
	DWORD						Flags;

	Fvector						vTargetDirection;
	Fvector						vTargetPosition;

	EntityVec					Members;
	Fvector						vCentroid;

public:
	
	bool						m_bLeaderViewsEnemy;
	bool						m_bEnemyNoticed;
	DWORD						m_dwLeaderChangeCount;
	Fvector						m_tLastHitDirection;
	Fvector						m_tHitPosition;
	DWORD						m_dwLastHitTime;
	DWORD						m_dwFiring;
	bool						m_bLessCoverLook;
	DWORD						m_dwLastViewChange;
	SuspiciousNodes				m_tpaSuspiciousNodes;
	SuspiciousGroups			m_tpaSuspiciousGroups;
	SuspiciousForces			m_tpaSuspiciousForces;

	// Constructor
								CGroup				();
	
								void						SetState			(EGroupState S)				{ State=S; }
	void						SetFlag				(EGroupTriggers T, BOOL f)	{ if (f) Flags|=DWORD(T); else Flags &= ~DWORD(T);}
	void						InvertFlag			(EGroupTriggers T)			{ if (Flags&DWORD(T)) Flags&=~DWORD(T); else Flags|=DWORD(T);}

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
	const Fvector&				GetCentroid			();
};
