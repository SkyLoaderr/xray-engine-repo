////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters.h
//	Created 	: 24.04.2002
//  Modified 	: 24.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for all the monsters
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_MONSTERS__
#define __XRAY_AI_MONSTERS__

#define MAGNITUDE_EPSILON 0.01
#define CHECK_RESULT \
	if (m_fResult > BestCost)\
		return(m_fResult);

const DWORD		_FB_hit_RelevantTime	= 10;
const DWORD		_FB_sense_RelevantTime	= 10;
const float		_FB_look_speed			= PI;
const float		_FB_invisible_hscale	= 2.f;
const Fvector	tLeft					= {-1,0,0};
const Fvector	tRight					= {1,0,0};

struct SEnemySelected
{
	CEntity*	Enemy;
	bool		bVisible;
	float		fCost;
};

extern IC void vfNormalizeSafe(Fvector& Vector);

class CAISelectorBase : public AI::NodeEstimator 
{
	public:
		
		// section name
		LPSTR			Name;
		
		// .ini file constants
		float		
						fSearchRange,
						fEnemySurround,
						fTotalCover,
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
						fEnemyViewDeviationWeight;
		
		// hit information
		Fvector			m_tHitDir;
		DWORD			m_dwHitTime;
		
		// current time information
		DWORD			m_dwCurTime;

		// myself
		CEntity*        m_tMe;
		NodeCompressed* m_tpMyNode;
		Fvector			m_tMyPosition;
		Fvector			m_tDirection;
		
		// leader
		CEntity*        m_tLeader;
		NodeCompressed* m_tpLeaderNode;
		Fvector			m_tLeaderPosition;
		Fvector			m_tLeaderDirection;
		DWORD			m_tLeaderNode;

		// enemy
		CEntity*        m_tEnemy;
		NodeCompressed* m_tpEnemyNode;
		DWORD			m_dwEnemyNode;
		Fvector			m_tEnemyPosition;
		Fvector			m_tEnemyDirection;
		Fvector			m_tEnemySurroundDirection;
		Fvector			m_tLastEnemyPosition;
		float			m_fRadius;
		
		// postion being tested
		float			m_fDistance;
		NodeCompressed*	m_tpCurrentNode;
		Fvector			m_tCurrentPosition;

		// members
		int				m_iAliveMemberCount;
		int				m_iCurrentMember;
		NodeCompressed*	m_tpCurrentMemberNode;
		Fvector			m_tCurrentMemberPosition;
		Fvector			m_tCurrentMemberDirection;

		// common
		float			m_fResult;
		float			m_fFireDispersionAngle;

		virtual	void Load	(LPCSTR section);
		void vfAddCoverFromEnemyCost();
		void vfAddCoverFromLeaderCost();
		void vfAddCoverFromMemberCost();
		void vfAddDeviationFromMemberViewCost();
		void vfAddDeviationFromPreviousDirectionCost();
		void vfAddDistanceToEnemyCost();
		void vfAddDistanceToLastPositionCost();
		void vfAddDistanceToLeaderCost();
		void vfAddDistanceToMemberCost();
		void vfAddEnemyLookCost();
		void vfAddEnemyLookCost(float fAngle);
		void vfAddLightCost();
		void vfAddSurroundEnemyCost();
		void vfAddTotalCoverCost();
		void vfAddTravelCost();
		void vfAssignMemberPositionAndNode();
		void vfCheckForEpsilon(BOOL &bStop);
		void vfInit();
		void vfComputeMemberDirection();
		void vfComputeSurroundEnemy();
		void vfAddMemberDanger();
};

#endif