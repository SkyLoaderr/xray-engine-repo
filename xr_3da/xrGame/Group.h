#pragma once

class	CEntity;

enum	EGroupState
{
	gsFreeHunting=0,			//	свободная охота					
	gsFollowMe,					//	идти ко мне (нужна помощь)		
	gsGoInThisDirection,		//	идти по направлению: "бесконечно" 
	gsGoToThisPosition,			//	идти в заданную позицию:			
	gsHoldPosition,				//	удерживать позицию				

	gsLast,
	gsForceDWORD = DWORD(-1)
};

enum	EGroupTriggers
{
	gtAgressive		= (1<<0),	// всегда при обнаружении врага / стрелять только для защиты
	gtQuiet			= (1<<1),	// тихо но более медленно пробираться / быстро (особо не прикрываясь) но более шумно
	gtForceDWORD	= DWORD(-1)
};

DEFINE_VECTOR(CEntity*,EntityVec,EntityIt);

#define MAX_SUSPICIOUS_NODE_COUNT 32
#define MAX_GROUP_SIZE 256

typedef struct tagSSearchPlace {
	DWORD	dwNodeID:31;
	DWORD	bSearched:1;
	float	fCost;
} SSearchPlace;

typedef svector<Fvector,MAX_GROUP_SIZE>				MemberPlacement;
typedef svector<DWORD,MAX_GROUP_SIZE>				MemberNodes;
typedef svector<SSearchPlace,MAX_SUSPICIOUS_NODE_COUNT>	SuspiciousNodes;
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
