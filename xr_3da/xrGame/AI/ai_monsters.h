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
#define SQR(x) ((x)*(x))
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
		
		// leader
		CEntity*        m_tLeader;
		NodeCompressed* m_tpLeaderNode;
		Fvector			m_tLeaderPosition;
		Fvector			m_tLeaderDirection;
		DWORD			m_tLeaderNode;

		// enemy
		CEntity*        m_tEnemy;
		NodeCompressed* m_tpEnemyNode;
		Fvector			m_tEnemyPosition;
		Fvector			m_tEnemyDirection;
		Fvector			m_tEnemySurroundDirection;
		
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

		virtual	void Load(CInifile* ini, const char* section);
		
		void vfAddTravelCost();
		void vfAddLightCost();
		void vfComputeCurrentPosition();
		void vfAssignMemberPositionAndNode();
		void vfAddDistanceToEnemyCost();
		void vfAddDistanceToLeaderCost();
		void vfAddDistanceToMemberCost();
		void vfAddCoverFromEnemyCost();
		void vfAddCoverFromLeaderCost();
		void vfAddCoverFromMemberCost();
		void vfAddCoverFromSavedEnemyCost();
		void vfAddTotalCoverCost();
		void vfAddEnemyLookCost();
		void vfComputeMemberDirection();
		void vfComputeSurroundEnemy();
		void vfAddSurroundEnemyCost();
		void vfCheckForEpsilon(BOOL &bStop);
};

#endif