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


// flags
#define FLAG_ENEMY_DIE					( 1 << 0 )
#define FLAG_ENEMY_LOST_SIGHT			( 1 << 1 )
#define FLAG_ENEMY_GO_CLOSER			( 1 << 2 )
#define FLAG_ENEMY_GO_FARTHER			( 1 << 3 )
#define FLAG_ENEMY_GO_CLOSER_FAST		( 1 << 4 )
#define FLAG_ENEMY_GO_FARTHER_FAST		( 1 << 5 )
#define FLAG_ENEMY_STANDING				( 1 << 6 )
#define FLAG_ENEMY_HIDING				( 1 << 7 )
#define FLAG_ENEMY_RUN_AWAY				( 1 << 8 )
#define FLAG_ENEMY_DOESN_KNOW_ABOUT_ME	( 1 << 9 )
#define FLAG_ENEMY_GO_OFFLINE			( 1 << 10 )




// logging
//#define SILENCE

#undef	WRITE_TO_LOG
#ifdef SILENCE
#define WRITE_TO_LOG(s) ;
#else
#define WRITE_TO_LOG(s) {\
	Msg("%s",s);\
}
#endif



class CCharacterPhysicsSupport;

// Paths
enum EBitingPathState {
	PATH_STATE_SEARCH_NODE = 0,
	PATH_STATE_BUILD_NODE_PATH,
	PATH_STATE_BUILD_TRAVEL_LINE,
	PATH_STATE_PATH_BUILT
};

typedef VisionElem SEnemy;

class CAI_Biting : public CCustomMonster, 
				   public CMonsterMemory
{

public:
	typedef	CCustomMonster	inherited;
	
	// friend definitions
	friend	class			CBitingMotion;
	friend	class			IState;

							CAI_Biting						();
	virtual					~CAI_Biting						();
	virtual	BOOL			renderable_ShadowReceive		()	{ return TRUE;	}  
	virtual void			Die								();
	virtual void			HitSignal						(float amount, Fvector& vLocalDir, CObject* who, s16 element);
	virtual void			Hit								(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type = eHitTypeWound);
	virtual void			SelectAnimation					(const Fvector& _view, const Fvector& _move, float speed );

	virtual void			Load							(LPCSTR section);

	// network routines
	virtual BOOL			net_Spawn						(LPVOID DC);
	virtual void			net_Destroy						();
	virtual void			net_Export						(NET_Packet& P);
	virtual void			net_Import						(NET_Packet& P);

	virtual void			Exec_Movement					(float dt);
	
	virtual void			UpdateCL						();
	virtual void			shedule_Update					(u32 dt);

	virtual void			Think							();
	virtual	void			Init							();

	virtual void			feel_sound_new					(CObject* who, int eType, const Fvector &Position, float power);
	virtual BOOL			feel_vision_isRelevant			(CObject* O);

	// path routines
			void			vfInitSelector					(IBaseAI_NodeEvaluator &S, bool hear_sound = false);
			void			vfSearchForBetterPosition		(IBaseAI_NodeEvaluator &tNodeEvaluator, CSquad &Squad, CEntity* &Leader);
			void			vfBuildPathToDestinationPoint	(IBaseAI_NodeEvaluator *tpNodeEvaluator);
			void			vfBuildTravelLine				(Fvector *tpDestinationPosition);
			void			vfChoosePointAndBuildPath		(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDestinationPosition, bool bSearchForNode, bool bSelectorPath = false, u32 TimeToRebuild = 0);
			void			vfChooseNextGraphPoint			();
			void			vfUpdateDetourPoint				();
			void			Path_GetAwayFromPoint			(CEntity *pE, Fvector position, float dist, TTime rebuild_time);
			void			Path_CoverFromPoint				(CEntity *pE, Fvector position, TTime rebuild_time);
			void			Path_ApproachPoint				(CEntity *pE, Fvector position, TTime rebuild_time);

			void			vfUpdateParameters				();
		
			void			DoDamage						(CEntity *pEntity, float fDamage, float yaw, float pitch);
			void			SetState						(IState *pS, bool bSkipInertiaCheck = false);

	virtual void			CheckAttackHit					();

	virtual	bool			CheckSpecParams					(u32 spec_params) {return false;}

	// FSM
	virtual void            StateSelector					() = 0;  // should be pure 
	
	
	// Other
			void			SetDirectionLook				(bool bReversed = false);
	virtual void			LookPosition					(Fvector to_point);		// каждый монстр может по-разному реализвать эту функ (e.g. кровосос с поворотом головы и т.п.)
			
// members
public:

	CCharacterPhysicsSupport *m_pPhysics_support;
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
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance>			m_tSelectorGetAway;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance>			m_tSelectorApproach;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance>			m_tSelectorHearSnd;

	EBitingPathState		m_tPathState;
	u32						m_dwPathBuiltLastTime;
	
	// Other fields
	float					m_fGoingSpeed;			// speed over the path
	u32						m_dwHealth;				
	float					m_fSoundThreshold;
	float					m_fHitPower;

	// FSM 
	IState					*CurrentState;

	// State properties
	float					m_fAttackSuccessProbability[4];
	bool					A,B,C,D,E,F,G,H,I,J,K,L,M;

	// Combat flags 
	u32						flagsEnemy; 

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
	TTime					m_timeSleepMin;
	TTime					m_timeSleepMax;

	u32						m_dwProbRestWalkFree;
	u32						m_dwProbRestStandIdle;
	u32						m_dwProbRestLieIdle;
	u32						m_dwProbRestTurnLeft;

	float					m_fImpulseMin;
	float					m_fImpulseMax;

	// Biting-specific states
	CBitingRest				*stateRest;
	CBitingEat				*stateEat;
	CBitingAttack			*stateAttack;
	CBitingHide				*stateHide;
	CBitingDetour			*stateDetour;
	CBitingPanic			*statePanic;
	CBitingExploreDNE		*stateExploreDNE;
	CBitingExploreDE		*stateExploreDE;
	CBitingExploreNDE		*stateExploreNDE;
	
	friend	class			CBitingRest;
	friend  class 			CBitingAttack;
	friend	class 			CBitingEat;
	friend	class 			CBitingHide;
	friend	class 			CBitingDetour;
	friend	class 			CBitingPanic;
	friend	class 			CBitingExploreDNE;
	friend	class 			CBitingExploreDE;
	friend	class 			CBitingExploreNDE;

	// State flags
	bool					flagEatNow;				// true - сейчас монстр ест]

	CMotionManager			MotionMan; 

	u32						m_dwDayTimeBegin;
	u32						m_dwDayTimeEnd;
	float					m_fMinSatiety;
	float					m_fMaxSatiety;

};