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

#include "../ai_monster_shared_data.h"
#include "../ai_monster_debug.h"

#include "../ai_monster_movement.h"

class CCharacterPhysicsSupport;

typedef VisionElem SEnemy;

class CAI_Biting : public CCustomMonster, 
				   public CMonsterMemory,
				   virtual public CMonsterMovement,
				   public CSharedClass<_biting_shared> {

	typedef	CCustomMonster					inherited;
	typedef CSharedClass<_biting_shared>	_sd_biting;
	typedef CMovementManager				MoveMan;


	// -------------------------------------------------------
	// attack stops
	struct SAttackStop {
		float	min_dist;		// load from ltx
		float	step;			// load from ltx
		bool	active;
		bool	prev_prev_hit;
		bool	prev_hit;
	} _as;


	enum EMovementParameters {
		eVelocityParameterStand			= u32(1) <<  4,
		eVelocityParameterWalkNormal	= u32(1) <<  3,
		eVelocityParameterRunNormal		= u32(1) <<  2,

		eVelocityParameterWalkDamaged	= u32(1) <<  5,
		eVelocityParameterRunDamaged	= u32(1) <<  6,
		eVelocityParameterSteal			= u32(1) <<  7,
		eVelocityParameterDrag			= u32(1) <<  8,

		eVelocityParamsWalk				= eVelocityParameterStand | eVelocityParameterWalkNormal,
		eVelocityParamsWalkDamaged		= eVelocityParameterStand | eVelocityParameterWalkDamaged,
		eVelocityParamsRun				= eVelocityParameterStand | eVelocityParameterWalkNormal | eVelocityParameterRunNormal,
		eVelocityParamsRunDamaged		= eVelocityParameterStand | eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsAttackNorm		= eVelocityParameterStand | eVelocityParameterWalkNormal | eVelocityParameterRunNormal,
		eVelocityParamsAttackDamaged	= eVelocityParameterStand | eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsSteal			= eVelocityParameterStand | eVelocityParameterSteal,
	};

public:
	
	// friend definitions
	friend	class			CMotionManager;
	friend	class			IState;
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

							CAI_Biting						();
	virtual					~CAI_Biting						();
	virtual	BOOL			renderable_ShadowReceive		()	{ return TRUE;	}  
	virtual void			Die								();
	virtual void			HitSignal						(float amount, Fvector& vLocalDir, CObject* who, s16 element);
	virtual void			Hit								(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void			SelectAnimation					(const Fvector& _view, const Fvector& _move, float speed );

	virtual void			Load							(LPCSTR section);

	virtual BOOL			net_Spawn						(LPVOID DC);
	virtual void			net_Destroy						();
	virtual void			net_Export						(NET_Packet& P);
	virtual void			net_Import						(NET_Packet& P);

	virtual void			UpdateCL						();
	virtual void			shedule_Update					(u32 dt);

	virtual void			Think							();
			void			Init							();
	virtual	void			reinit							();

	virtual void			feel_sound_new					(CObject* who, int eType, const Fvector &Position, float power);
	virtual BOOL			feel_vision_isRelevant			(CObject* O);

	virtual bool			useful							(const CGameObject *object) const;

	virtual void			OnHUDDraw						(CCustomHUD* hud)			{return inherited::OnHUDDraw(hud);}
	virtual void			OnEvent							(NET_Packet& P, u16 type)	{return inherited::OnEvent(P,type);}
	virtual u16				PHGetSyncItemsNumber			()							{return inherited::PHGetSyncItemsNumber();}
	virtual CPHSynchronize*	PHGetSyncItem					(u16 item)					{return inherited::PHGetSyncItem(item);}
	virtual void			PHUnFreeze						()							{return inherited::PHUnFreeze();}
	virtual void			PHFreeze						()							{return inherited::PHFreeze();}
	virtual BOOL			UsedAI_Locations				()							{return inherited::UsedAI_Locations();}
	virtual void			reload							(LPCSTR section);
	virtual const SRotation	Orientation						() const					{return inherited::Orientation();}
	virtual void			renderable_Render				()							{return inherited::renderable_Render();} 
	virtual	void			PitchCorrection					();

	virtual	void			SetAttackEffector				();
	
	virtual float			get_custom_pitch_speed			(float def_speed);

	// ---------------------------------------------------------------------------------
	// Process scripts
	virtual	bool			bfAssignMovement				(CEntityAction	*tpEntityAction);
	virtual	bool			bfAssignObject					(CEntityAction	*tpEntityAction);
	virtual	bool			bfAssignWatch					(CEntityAction	*tpEntityAction);
	virtual bool			bfAssignAnimation				(CEntityAction  *tpEntityAction);
	virtual	bool			bfAssignMonsterAction			(CEntityAction	*tpEntityAction);
	
	virtual void			ProcessScripts					();

	virtual	CEntity			*GetCurrentEnemy				();
	virtual	CEntity 		*GetCurrentCorpse				();
	virtual int				get_enemy_strength				();
	
	virtual void			SetScriptControl				(const bool bScriptControl, ref_str caSciptName);


	// ---------------------------------------------------------------------------------
	// Process FSM
	virtual bool			UpdateStateManager				() {return false;}

	// ---------------------------------------------------------------------------------

	virtual void			ProcessTurn						() {}
	virtual bool			AA_CheckHit						();
	// установка специфических анимаций 
	virtual	void			CheckSpecParams					(u32 /**spec_params/**/) {}
	virtual void			LookPosition					(Fvector to_point, float angular_speed = PI_DIV_3);		// каждый монстр может по-разному реализвать эту функ (e.g. кровосос с поворотом головы и т.п.)

	virtual bool			CanExecRotationJump				() {return false;}

	// ---------------------------------------------------------------------------------

	virtual float			GetEnemyDistances				(float &min_dist, float &max_dist);
	
	// Other
			void			vfUpdateParameters				();
			void			HitEntity						(const CEntity *pEntity, float fDamage, Fvector &dir);

	// Other 
			void			SetDirectionLook				(bool bReversed = false);
	
			
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

	// Step sounds
			void			AddStepSound					(LPCSTR section, EMotionAnim a, LPCSTR name);
			void			GetStepSound					(EMotionAnim a, float &vol, float &freq);
	
// members
public:

	CCharacterPhysicsSupport *m_pPhysics_support;
	
	float					m_fGoingSpeed;			// speed over the path
	u32						m_dwHealth;				


	// State properties
	float					m_fAttackSuccessProbability[4];

	// State flags
	bool					m_bDamaged;

	// Combat flags 
	u32						flagsEnemy; 

	// m_object
	SEnemy					m_tEnemy;				// Current frame enemy 
	SEnemy					m_tEnemyPrevFrame;		// Previous frame enemy 


	float					m_fCurMinAttackDist;		// according to attack stops

	
	// -----------------------------------------------------------------------
	// FSM
	virtual void            StateSelector					() = 0;  // should be pure 
			void			SetState						(IState *pS, bool bSkipInertiaCheck = false);

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
	CBitingNull				*stateNull;

	bool					A,B,C,D,E,F,G,H,I,J,K,L,M;
	IState					*CurrentState;

	// -------------------------------------------------------

	// State flags
	bool					flagEatNow;				// true - сейчас монстр ест

	CMotionManager			MotionMan; 

	bool RayPickEnemy(const CObject *target_obj, const Fvector &trace_from, const Fvector &dir, float dist, float radius, u32 num_picks);

#ifdef DEBUG
	CMonsterDebug	*HDebug;
	virtual void	OnRender();
#endif

	u16						fire_bone_id;
	float					GetRealDistToEnemy		(const CEntity *pE);

	void					FaceTarget				(const CEntity *entity);
	void					FaceTarget				(const Fvector &position);
	
	float					anim_speed;
	void					SetAnimSpeed			(float val) {anim_speed = val;}
	CBlend					*cur_blend;
	
	bool					b_script_state_must_execute;

#ifdef 	DEEP_TEST_SPEED
	TTime	time_next_update;
#endif

//////////////////////////////////////////////////////////////////////////
// Motion Tests
	void					TranslateActionToPathParams ();


};

#include "ai_biting_inline.h"





