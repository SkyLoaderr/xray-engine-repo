////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.h
//	Created 	: 21.05.2003
//  Modified 	: 21.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all the monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../CustomMonster.h"
#include "../ai_monsters_misc.h"
#include "../ai_monster_motion.h"
#include "ai_biting_state.h"
#include "../ai_monster_mem.h"
#include "../ai_monster_sound.h"
#include "../ai_monster_share.h"
#include "../../path_manager_level_selector.h"

#include "../ai_monster_debug.h"
#include "../ai_monster_motion_stats.h"


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


#define STANDART_ATTACK -PI_DIV_6,PI_DIV_6,-PI_DIV_6,PI_DIV_6,3.5f
#define SIMPLE_ENEMY_HIT_TEST
//#define TEST_EAT_STATE



class CCharacterPhysicsSupport;
class PathManagers::CAbstractVertexEvaluator;


typedef VisionElem SEnemy;

#include "ai_biting_shared.h"


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

	typedef	CCustomMonster					inherited;
	typedef CSharedClass<_biting_shared>	_sd_biting;
	typedef CMovementManager				MoveMan;

	enum EMovementParameters {
		eVelocityParameterStand			= u32(1) <<  4,
		eVelocityParameterWalkNormal	= u32(1) <<  3,
		eVelocityParameterRunNormal		= u32(1) <<  2,

		eVelocityParameterWalkDamaged	= u32(1) <<  5,
		eVelocityParameterRunDamaged	= u32(1) <<  6,
		eVelocityParameterSteal			= u32(1) <<  7,
		eVelocityParameterDrag			= u32(1) <<  8,

		eVelocityParamsWalk				= eVelocityParameterStand | eVelocityParameterWalkNormal,
		eVelocityParamsRun				= eVelocityParameterStand | eVelocityParameterWalkNormal | eVelocityParameterRunNormal,
		eVelocityParamsAttackNorm		= eVelocityParameterStand | eVelocityParameterWalkNormal | eVelocityParameterRunNormal,
		eVelocityParamsAttackDamaged	= eVelocityParameterStand | eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsSteal			= eVelocityParameterStand | eVelocityParameterSteal,
	};


public:
	
	// friend definitions
	friend	class			CMotionManager;
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

	virtual void			UpdateCL						();
	virtual void			shedule_Update					(u32 dt);

	virtual void			Think							();
			void			Init							();

	virtual void			feel_sound_new					(CObject* who, int eType, const Fvector &Position, float power);
	virtual BOOL			feel_vision_isRelevant			(CObject* O);

	// Other
			void			vfUpdateParameters				();
		
			void			HitEntity						(const CEntity *pEntity, float fDamage, Fvector &dir);
			
			void			SetState						(IState *pS, bool bSkipInertiaCheck = false);

	virtual bool			AA_CheckHit						();

	// ��������� ������������� �������� 
	virtual	void			CheckSpecParams					(u32 /**spec_params/**/) {}

	// FSM
	virtual void            StateSelector					() = 0;  // should be pure 
	
	// Other 
			void			SetDirectionLook				(bool bReversed = false);
	virtual void			LookPosition					(Fvector to_point, float angular_speed = PI_DIV_3);		// ������ ������ ����� ��-������� ���������� ��� ���� (e.g. �������� � ��������� ������ � �.�.)

	// Process scripts
	virtual	bool			bfAssignMovement				(CEntityAction	*tpEntityAction);
	virtual	bool			bfAssignObject					(CEntityAction	*tpEntityAction);

			bool			IsStanding						(TTime time);		// ���������, ����� �� ������ �� ���������� ������� time
	virtual void			ProcessTurn						() {};

			CBoneInstance *GetBoneInstance					(LPCTSTR bone_name);
			CBoneInstance *GetBoneInstance					(int bone_id);
		
			CBoneInstance *GetEatBone						();

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

			Fvector			GetValidPosition				(const CEntity *entity, const Fvector &actual_position);

	// Step sounds
			void			AddStepSound					(LPCSTR section, EMotionAnim a, LPCSTR name);
			void			GetStepSound					(EMotionAnim a, float &vol, float &freq);


	virtual bool			useful							(const CGameObject *object) const;


	// Path
			void			InitSelector					(PathManagers::CAbstractVertexEvaluator &S, Fvector target_pos);
			void			Path_GetAwayFromPoint			(Fvector position, float dist);
			void			Path_ApproachPoint				(Fvector position);

			void			SetPathParams					(u32 dest_vertex_id, const Fvector &dest_pos);
			void			SetSelectorPathParams			();

			void			SetVelocity						();
			void			PreprocessAction				();

			bool			IsObstacle						(TTime time);
			void			SetupVelocityMasks				(bool force_real_speed);


	
// members
public:

	CCharacterPhysicsSupport *m_pPhysics_support;
	// graph params
	TERRAIN_VECTOR			m_tpaTerrain;
	u32						m_dwTimeToChange;			//!< ����� ����� ����� �����
	_GRAPH_ID				m_tCurGP;
	_GRAPH_ID				m_tNextGP;
	Fvector					m_tNextGraphPoint;
	

	PathManagers::CAbstractVertexEvaluator	*m_tSelectorGetAway;
	PathManagers::CAbstractVertexEvaluator	*m_tSelectorApproach;


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

	// m_object
	SEnemy					m_tEnemy;				// Current frame enemy 
	SEnemy					m_tEnemyPrevFrame;		// Previous frame enemy 
	
	// local standing params
	Fvector					cur_pos, prev_pos;
	bool					bStanding;				// true - ������� � ����������� ������� ������������� ������� �� ���� �������
	TTime					time_start_stand;			

	bool					bSpeedDiffer;
	TTime					time_start_speed_differ;
		

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
	CBitingSquadTask		*stateSquadTask;
	
	CBitingTest				*stateTest;

	friend	class			CBitingRest;
	friend  class 			CBitingAttack;
	friend	class 			CBitingEat;
	friend	class 			CBitingHide;
	friend	class 			CBitingDetour;
	friend	class 			CBitingPanic;
	friend	class 			CBitingExploreDNE;
	friend	class 			CBitingExploreDE;
	friend	class 			CBitingExploreNDE;
	friend	class 			CBitingSquadTask;

	friend	class			CBitingRest;

	// State flags
	bool					flagEatNow;				// true - ������ ������ ���

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

	// PathManagement Bridge
	void MoveToTarget			(const CEntity *entity); 
	void MoveToTarget			(const Fvector &pos, u32 node_id);
	void FaceTarget				(const CEntity *entity);
	void FaceTarget				(const Fvector &position);
	bool IsObjectPositionValid	(const CEntity *entity);
	bool IsMoveAlongPathFinished();
	bool IsMovingOnPath			();
	bool ObjectNotReachable		(const CEntity *entity);

	Fvector	RandomPosInR		(const Fvector &p, float r);

	void	UpdateVelocities	(STravelParams cur_velocity);
	
	xr_vector<STravelParams> velocities;

	u32		GetNextGameVertex	(float R);

	bool	NeedRebuildPath		(u32 n_points);
	bool	NeedRebuildPath		(float dist_to_end);
	bool	NeedRebuildPath		(u32 n_points, float dist_to_end);


	bool RayPickEnemy(const CObject *target_obj, const Fvector &trace_from, const Fvector &dir, float dist, float radius, u32 num_picks);

	CMotionStats	*MotionStats;

	//////////////////////////////////////////////////////////////////////////
	// DEBUG
#ifdef DEBUG
	CMonsterDebug	*HDebug;
	virtual void	OnRender();
#endif

	bool	is_angle_between(float yaw, float yaw_from, float yaw_to);

	u16		fire_bone_id;
	float	GetRealDistToEnemy();
	void	WalkNextGraphPoint();

	bool	b_try_min_time;

};


#include "ai_biting_inline.h"


