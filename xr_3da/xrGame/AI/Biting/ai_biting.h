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
#include "..\\ai_monster_motion.h"
#include "ai_biting_state.h"
#include "..\\ai_monster_mem.h"
#include "..\\ai_monster_sound.h"
#include "..\\ai_monster_share.h"


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
#define FLAG_ENEMY_DOESNT_KNOW_ABOUT_ME	( 1 << 9 )
#define FLAG_ENEMY_GO_OFFLINE			( 1 << 10 )
#define FLAG_ENEMY_DOESNT_SEE_ME		( 1 << 11 )

#define SOUND_ATTACK_HIT_MIN_DELAY 1000
#define MORALE_NORMAL	0.5f


class CCharacterPhysicsSupport;

// Paths
enum EBitingPathState {
	PATH_STATE_SEARCH_NODE = 0,
	PATH_STATE_BUILD_NODE_PATH,
	PATH_STATE_BUILD_TRAVEL_LINE,
	PATH_STATE_PATH_BUILT
};

typedef VisionElem SEnemy;

class _biting_shared : public CSharedResource {
public:
	// float speed factors
	float					m_fsTurnNormalAngular;
	float					m_fsWalkFwdNormal;
	float					m_fsWalkFwdDamaged;
	float					m_fsWalkBkwdNormal;
	float					m_fsWalkTurn;
	float 					m_fsWalkAngular;
	float 					m_fsWalkTurnAngular;
	float 					m_fsRunFwdNormal;
	float 					m_fsRunFwdDamaged;
	float 					m_fsRunTurn;
	float 					m_fsRunTurnAngular;
	float 					m_fsRunAngular;
	float					m_fsDrag;
	float					m_fsSteal;

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

	float					m_fDistToCorpse;
	float					m_fMinAttackDist;
	float					m_fMaxAttackDist;

	float					m_fDamagedThreshold;		// порог здоровья, ниже которого устанавливается флаг m_bDamaged

	// -------------------------------------------------------

	TTime					m_dwIdleSndDelay;
	TTime					m_dwEatSndDelay;
	TTime					m_dwAttackSndDelay;

	// -------------------------------------------------------

	// Мораль 
	float					m_fMoraleSuccessAttackQuant;		// увеличение морали при успешной атаке
	float					m_fMoraleDeathQuant;				// уменьшение морали при смерти монстра из одной команды
	float					m_fMoraleFearQuant;					// уменьшение морали в панике
	float					m_fMoraleRestoreQuant;				// квант восстановления морали ? 
	float					m_fMoraleBroadcastDistance;			// мораль уменьшается, если в данном радиусе умер монстр из команды

	// ----------------------------------------------------------- 

	u32						m_dwDayTimeBegin;
	u32						m_dwDayTimeEnd;
	float					m_fMinSatiety;
	float					m_fMaxSatiety;
	// ----------------------------------------------------------- 

	float					m_fSoundThreshold;
	float					m_fHitPower;

};

#define BEGIN_LOAD_SHARED_MOTION_DATA() {MotionMan.PrepareSharing();}
#define END_LOAD_SHARED_MOTION_DATA()	{MotionMan.NotifyShareLoaded();}

#define SHARE_ON_LOAD(pmt) {							\
		pmt::Prepare(SUB_CLS_ID);						\
		if (!pmt::IsLoaded()) LoadShared(section);		\
		pmt::Finish();									\
}

class CAI_Biting : public CCustomMonster, 
				   public CMonsterMemory,
				   public CMonsterSound,
				   public CSharedClass<_biting_shared> {

	typedef	CCustomMonster	inherited;
	typedef CSharedClass<_biting_shared> _sd_biting;

public:
	
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

	// Other
			void			vfUpdateParameters				();
		
			void			DoDamage						(CEntity *pEntity, float fDamage, float yaw, float pitch);
			void			SetState						(IState *pS, bool bSkipInertiaCheck = false);

	virtual bool			AA_CheckHit						();

	// установка специфических анимаций 
	virtual	void			CheckSpecParams					(u32 spec_params) {}

	// FSM
	virtual void            StateSelector					() = 0;  // should be pure 
	
	// Other 
			void			SetDirectionLook				(bool bReversed = false);
	virtual void			LookPosition					(Fvector to_point);		// каждый монстр может по-разному реализвать эту функ (e.g. кровосос с поворотом головы и т.п.)


	virtual	bool			bfAssignMovement				(CEntityAction	*tpEntityAction);
	virtual	bool			bfAssignObject					(CEntityAction	*tpEntityAction);

			bool			IsStanding						(TTime time);		// проверить, стоит ли монстр на протяжении времени time
	virtual void			ProcessTurn						() {};

			CBoneInstance *GetBoneInstance					(LPCTSTR bone_name);
			CBoneInstance *GetBoneInstance					(int bone_id);
		
			// attack-stop
			void			AS_Init							();
			void			AS_Load							(LPCSTR section);
			void			AS_Start						();
			void			AS_Stop							();
			void			AS_Check						(bool hit_success);
			bool			AS_Active						();
	
	// Morale
			void			MoraleBroadcast					(float fValue);

			void			LoadShared						(LPCSTR section);

	// Common stuff
			
			bool			IsRightSide						(float ty, float cy) {return ((angle_normalize_signed(ty - cy) > 0));}

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
	
	float					m_fGoingSpeed;			// speed over the path
	u32						m_dwHealth;				

	// FSM 
	IState					*CurrentState;

	// State properties
	float					m_fAttackSuccessProbability[4];
	bool					A,B,C,D,E,F,G,H,I,J,K,L,M;

	// State flags
	bool					m_bDamaged;

	// Combat flags 
	u32						flagsEnemy; 

	// Enemy
	SEnemy					m_tEnemy;				// Current frame enemy 
	SEnemy					m_tEnemyPrevFrame;		// Previous frame enemy 
	
	// local standing params
	Fvector					cur_pos, prev_pos;
	bool					bStanding;
	TTime					time_start_stand;			


	float					m_fCurMinAttackDist;		// according to attack stops


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
	bool					flagEatNow;				// true - сейчас монстр ест

	CMotionManager			MotionMan; 


	// -------------------------------------------------------
	// attack stops
	struct SAttackStop {
		float	min_dist;		// load from ltx
		float	step;			// load from ltx
		bool	active;
		bool	prev_prev_hit;
		bool	prev_hit;
	} _as;


// TEMP
	struct s_dbg{
		bool	active;
		Fvector pos;

		s_dbg() {active = false;}
		void set(const Fvector &f) {pos = f; active = true;}
	} dbg_info;

};


/////////////////////
// Inline Section
//////////////////////

/////////////////////////////////////////////////////////////////////////////////////
// Attack Stops
IC void CAI_Biting::AS_Init() {
	_as.active	= false;
}
IC void CAI_Biting::AS_Load(LPCSTR section) {
	_as.min_dist	= pSettings->r_float(section,"as_min_dist");
	_as.step		= pSettings->r_float(section,"as_step");
}
IC void CAI_Biting::AS_Start() {
	_as.active			= true;
	_as.prev_prev_hit	= true;
	_as.prev_hit		= true;
}
IC void CAI_Biting::AS_Stop() {
	_as.active				= false;
	m_fCurMinAttackDist		= _sd->m_fMinAttackDist;
}
IC void CAI_Biting::AS_Check(bool hit_success) {
	if (!_as.active) return;

	_as.prev_prev_hit	= _as.prev_hit;
	_as.prev_hit		= hit_success;

	if ((!_as.prev_prev_hit && !_as.prev_prev_hit) && ((m_fCurMinAttackDist >= _as.min_dist) && (m_fCurMinAttackDist >= _as.min_dist + _as.step))) {
			m_fCurMinAttackDist -= _as.step;
	} else if (_as.prev_prev_hit && _as.prev_prev_hit && (m_fCurMinAttackDist < _sd->m_fMinAttackDist - _as.step)) {
			m_fCurMinAttackDist += _as.step;
	}
}

IC bool CAI_Biting::AS_Active() {return _as.active;}

