	////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.h
//	Created 	: 21.05.2003
//  Modified 	: 21.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all the monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\..\\CustomMonster.h"
#include "..\\ai_monsters_misc.h"
#include "ai_biting_anim.h"
#include "ai_biting_state.h"
#include "..\\ai_monster_mem.h"

// logging
#define SILENCE

#undef	WRITE_TO_LOG
#ifdef SILENCE
#define WRITE_TO_LOG(s) ;
#else
#define WRITE_TO_LOG(s) {\
	Msg("Monster %s : \n* State : %s",cName(),s);\
}
#endif

// Paths
enum EBitingPathState {
	PATH_STATE_SEARCH_NODE = 0,
	PATH_STATE_BUILD_NODE_PATH,
	PATH_STATE_BUILD_TRAVEL_LINE,
	PATH_STATE_PATH_BUILT
};

typedef VisionElem SEnemy;

// Attack 
typedef struct {
	u32		i_anim;				// анимация аттаки

	TTime	time_started;		//
	TTime	time_from;			// диапазон времени когда можно наносить hit
	TTime	time_to;

	Fvector	TraceFrom;
	float	dist;

	TTime	LastAttack;
	bool	b_fire_anyway;		// трассировка не нужна
	bool	b_attack_rat;

} SAttackAnimation;



class CAI_Biting : public CCustomMonster, 
				   public CBitingAnimations,
				   public CMonsterMemory
{

public:
	typedef	CCustomMonster	inherited;
	
	// friend definitions
	friend	class			CBitingMotion;
	friend	class			IState;

							CAI_Biting					();
	virtual					~CAI_Biting					();
	virtual	BOOL			renderable_ShadowReceive	()			{ return TRUE;	}  
	virtual void			Die							();
	virtual void			HitSignal					(float amount, Fvector& vLocalDir, CObject* who, s16 element);
	virtual void			SelectAnimation				(const Fvector& _view, const Fvector& _move, float speed );

	virtual void			Load						(LPCSTR section);

	// network routines
	virtual BOOL			net_Spawn					(LPVOID DC);
	virtual void			net_Destroy					();
	virtual void			net_Export					(NET_Packet& P);
	virtual void			net_Import					(NET_Packet& P);
	virtual void			Exec_Movement				(float dt);
	
	virtual void			UpdateCL					();
	virtual void			shedule_Update				(u32 dt);

	virtual void			Think						();
	virtual	void			Init						();

	virtual void			feel_sound_new				(CObject* who, int eType, const Fvector &Position, float power);
	virtual BOOL			feel_vision_isRelevant		(CObject* O);

	// path routines
			void			vfInitSelector					(IBaseAI_NodeEvaluator &S, CSquad &Squad);
			void			vfSearchForBetterPosition		(IBaseAI_NodeEvaluator &tNodeEvaluator, CSquad &Squad, CEntity* &Leader);
			void			vfBuildPathToDestinationPoint	(IBaseAI_NodeEvaluator *tpNodeEvaluator);
			void			vfBuildTravelLine				(Fvector *tpDestinationPosition);
			void			vfChoosePointAndBuildPath		(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDestinationPosition, bool bSearchForNode, bool bSelectorPath = false, u32 TimeToRebuild = 0);
			void			vfChooseNextGraphPoint			();
			void			vfUpdateDetourPoint				();
			void			SetReversedDirectionLook		();
			
			void			vfUpdateParameters				();
			void			OnAnimationEnd					();
			
		
			void			DoDamage						(CEntity *pEntity);

			void			CreateSkeleton					();
			void			SetState(IState *pS, bool bSkipInertiaCheck = false);

	// Animation control
			void			ControlAnimation();
	virtual	void			FillAttackStructure(u32, TTime) = 0;
	virtual	void			MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3) = 0;
	virtual	bool			IsInMotion();

// members
public:

	// graph params
	TERRAIN_VECTOR			m_tpaTerrain;
	u32						m_dwTimeToChange;			//!< время смены точки графа
	_GRAPH_ID				m_tCurGP;
	_GRAPH_ID				m_tNextGP;
	Fvector					m_tNextGraphPoint;
	
	// search and path parameters
	u32						m_dwLastRangeSearch;		//!< время последнего поиска узла
	xr_vector<Fvector>		m_tpaPoints;
	xr_vector<Fvector>		m_tpaTravelPath;
	xr_vector<u32>			m_tpaPointNodes;
	xr_vector<u32>			m_tpaNodes;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance>			m_tSelectorFreeHunting;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance>	m_tSelectorCover;
	EBitingPathState		m_tPathState;
	u32						m_dwPathBuiltLastTime;
	
	// Other fields
	float					m_fGoingSpeed;			//!< speed over the path
	u32						m_dwHealth;				
	float					m_fSoundThreshold;
	float					m_fHitPower;
	bool					bShowDeath;				//!< need to control animation
	bool					m_bActionFinished;		//!< true if animation finished
	SAttackAnimation		m_tAttack;				//!< for hit calculation

	CBitingMotion			Motion;					//!< motion frame params
	
	// FSM 
	IState					*CurrentState;

	// State properties
	float					m_fAttackSuccessProbability0;
	float					m_fAttackSuccessProbability1;
	float					m_fAttackSuccessProbability2;
	float					m_fAttackSuccessProbability3;
	bool					A,B,C,D,E,F,G,H,I,J,K,L,M;

	// Combat flags 
	bool					flagEnemyDie;
	bool					flagEnemyLostSight;
	bool					flagEnemyGoCloser;
	bool					flagEnemyGoFarther;
	bool					flagEnemyGoCloserFast;
	bool					flagEnemyGoFartherFast;
	bool					flagEnemyStanding;
	bool					flagEnemyDoesntKnowAboutMe;
	bool					flagEnemyHiding;
	bool					flagEnemyRunAway;

	// Enemy
	SEnemy					m_tEnemy;				// Current frame enemy 
	SEnemy					m_tEnemyPrevFrame;		// Previous frame enemy 

	// External Values
	// float motion factors
	float					m_ftrStandTurnRSpeed;
	float					m_ftrWalkSpeed;
	float					m_ftrWalkTurningSpeed;
	float 					m_ftrWalkRSpeed;
	float 					m_ftrWalkTurnRSpeed;
	float 					m_ftrWalkMinAngle;
	float 					m_ftrRunAttackSpeed;
	float 					m_ftrRunAttackTurnSpeed;
	float 					m_ftrRunAttackTurnRSpeed;
	float 					m_ftrRunRSpeed;
	float 					m_ftrRunAttackMinAngle;
	float 					m_ftrAttackFastRSpeed;
	float 					m_ftrAttackFastRSpeed2;
	float 					m_ftrScaredRSpeed;	

	TTime					m_timeLieIdleMin;
	TTime					m_timeLieIdleMax;
	TTime					m_timeStandIdleMin;
	TTime					m_timeStandIdleMax;
	TTime					m_timeFreeWalkMin;
	TTime					m_timeFreeWalkMax;

	u32						m_dwProbRestWalkFree;
	u32						m_dwProbRestStandIdle;
	u32						m_dwProbRestLieIdle;
	u32						m_dwProbRestTurnLeft;

};