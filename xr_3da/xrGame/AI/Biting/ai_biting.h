////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.h
//	Created 	: 21.05.2003
//  Modified 	: 21.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all the monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../CustomMonster.h"
#include "../ai_monster_motion.h"
#include "../ai_monster_shared_data.h"
#include "../ai_monster_movement.h"

#include "../monster_enemy_memory.h"
#include "../monster_corpse_memory.h"
#include "../monster_sound_memory.h"
#include "../monster_hit_memory.h"

#include "../monster_enemy_manager.h"
#include "../monster_corpse_manager.h"

#include "../ai_monster_group.h"

#include "ai_biting_state.h"

class CMonsterDebug;
class CCharacterPhysicsSupport;
class CAnimTriple;
class CMonsterCorpseCoverEvaluator;
class CCoverEvaluatorFarFromEnemy;

class CAI_Biting : public CCustomMonster, 
				   virtual public CMonsterMovement,
				   public CSharedClass<_biting_shared, CLASS_ID> {

	typedef	CCustomMonster							inherited;
	typedef CSharedClass<_biting_shared,CLASS_ID>	inherited_shared;
	typedef CSharedClass<_biting_shared,CLASS_ID>	_sd_biting;
	typedef CMovementManager						MoveMan;
	


	// -------------------------------------------------------
	// attack stops
	struct SAttackStop {
		float	min_dist;		// load from ltx
		float	step;			// load from ltx
		bool	active;
		bool	prev_prev_hit;
		bool	prev_hit;
	} _as;

protected:
	enum EMovementParameters {
		eVelocityParameterStand			= u32(1) <<  4,
		eVelocityParameterWalkNormal	= u32(1) <<  3,
		eVelocityParameterRunNormal		= u32(1) <<  2,

		eVelocityParameterWalkDamaged	= u32(1) <<  5,
		eVelocityParameterRunDamaged	= u32(1) <<  6,
		eVelocityParameterSteal			= u32(1) <<  7,
		eVelocityParameterDrag			= u32(1) <<  8,
		eVelocityParameterInvisible		= u32(1) <<	 9,

		eVelocityParamsWalk				= eVelocityParameterStand		| eVelocityParameterWalkNormal,
		eVelocityParamsWalkDamaged		= eVelocityParameterStand		| eVelocityParameterWalkDamaged,
		eVelocityParamsRun				= eVelocityParameterStand		| eVelocityParameterWalkNormal | eVelocityParameterRunNormal,
		eVelocityParamsRunDamaged		= eVelocityParameterStand		| eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsAttackNorm		= eVelocityParameterStand		| eVelocityParameterWalkNormal | eVelocityParameterRunNormal,
		eVelocityParamsAttackDamaged	= eVelocityParameterStand		| eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsSteal			= eVelocityParameterStand		| eVelocityParameterSteal,
		eVelocityParamsInvisible		= eVelocityParameterInvisible	| eVelocityParamsRun,

	};

public:
	
	// friend definitions
	friend	class			CMotionManager;
	friend	class			CBitingAttack;

							CAI_Biting						();
	virtual					~CAI_Biting						();
	
	virtual	BOOL			renderable_ShadowReceive		()	{ return TRUE;	}  
	virtual void			Die								();
	virtual void			HitSignal						(float amount, Fvector& vLocalDir, CObject* who, s16 element);
	virtual void			Hit								(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual	void			PHHit							(Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void			SelectAnimation					(const Fvector& _view, const Fvector& _move, float speed );
	virtual	Feel::Sound*	dcast_FeelSound					()			{ return this;	}

	virtual void			Load							(LPCSTR section);

	virtual BOOL			net_Spawn						(LPVOID DC);
	virtual void			net_Destroy						();
	virtual void			net_Export						(NET_Packet& P);
	virtual void			net_Import						(NET_Packet& P);

	virtual void			UpdateCL						();
	virtual void			shedule_Update					(u32 dt);

	virtual void			Think							();
	virtual	void			reinit							();
	virtual void			reload							(LPCSTR section);

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

	virtual const SRotation	Orientation						() const					{return inherited::Orientation();}
	virtual void			renderable_Render				()							{return inherited::renderable_Render();} 
	virtual	void			PitchCorrection					();

	virtual	void			SetAttackEffector				();
	
	virtual float			get_custom_pitch_speed			(float def_speed);

			void			Init							() {}

	virtual void			load_shared						(LPCSTR section);

	// ---------------------------------------------------------------------------------
	// Process scripts
	virtual	bool			bfAssignMovement				(CScriptEntityAction	*tpEntityAction);
	virtual	bool			bfAssignObject					(CScriptEntityAction	*tpEntityAction);
	virtual	bool			bfAssignWatch					(CScriptEntityAction	*tpEntityAction);
	virtual bool			bfAssignAnimation				(CScriptEntityAction  *tpEntityAction);
	virtual	bool			bfAssignMonsterAction			(CScriptEntityAction	*tpEntityAction);
	virtual bool			bfAssignSound					(CScriptEntityAction *tpEntityAction);
	
	virtual void			ProcessScripts					();

	virtual	CEntity			*GetCurrentEnemy				();
	virtual	CEntity 		*GetCurrentCorpse				();
	virtual int				get_enemy_strength				();
	
	virtual void			SetScriptControl				(const bool bScriptControl, ref_str caSciptName);


	// ---------------------------------------------------------------------------------
	// Process FSM
	virtual bool			UpdateStateManager				() {return false;}
	virtual void            StateSelector					() {}  
	// ---------------------------------------------------------------------------------

	virtual void			ProcessTurn						();
	virtual void			AA_CheckHit						();
	// ��������� ������������� �������� 
	virtual	void			CheckSpecParams					(u32 /**spec_params/**/) {}
	virtual void			LookPosition					(Fvector to_point, float angular_speed = PI_DIV_3);		// ������ ������ ����� ��-������� ���������� ��� ���� (e.g. �������� � ��������� ������ � �.�.)

	virtual bool			CanExecRotationJump				() {return false;}

	virtual void			on_travel_point_change			();
		
	// ---------------------------------------------------------------------------------
	// Abilities

	virtual bool			ability_invisibility			() {return false;}
	virtual bool			ability_can_drag				() {return false;}
	virtual bool			ability_psi_attack				() {return false;}
	virtual bool			ability_earthquake				() {return false;}
	virtual bool			ability_can_jump				() {return false;}
	virtual bool			ability_distant_feel			() {return false;}

	// ---------------------------------------------------------------------------------
			u16				m_FootBones[eLegsMaxNumber];

			Fvector			get_foot_position				(ELegType leg_type);
			void			LoadFootBones					();

	virtual u8				get_legs_number					() = 0;
	
	virtual void			event_on_step					() {}

	// ---------------------------------------------------------------------------------

	virtual float			GetEnemyDistances				(float &min_dist, float &max_dist);
	
	// Other
			void			vfUpdateParameters				();
			void			HitEntity						(const CEntity *pEntity, float fDamage, float impulse, Fvector &dir);

	// Other 
			void			SetDirectionLook				(bool bReversed = false);
	
			
			CBoneInstance *GetBoneInstance					(LPCTSTR bone_name);
			CBoneInstance *GetBoneInstance					(int bone_id);
		
			CBoneInstance *GetEatBone						();

			// attack-stop
	IC		void			AS_Init							();
	IC		void			AS_Load							(LPCSTR section);
	IC		void			AS_Start						();
	IC		void			AS_Stop							();
	IC		void			AS_Check						(bool hit_success);
	IC		bool			AS_Active						();
	
	// Morale
			void			MoraleBroadcast					(float fValue);
			void			LoadShared						(LPCSTR section);


	// Cover
			bool			GetCorpseCover					(Fvector &position, u32 &vertex_id);
			bool			GetCoverFromEnemy				(const Fvector &enemy_pos, Fvector &position, u32 &vertex_id);

	// Team	
			void			ChangeTeam						(int team, int squad, int group);


// members
public:

	CCharacterPhysicsSupport	*m_pPhysics_support;
	
	float						m_fGoingSpeed;			// speed over the path
	u32							m_dwHealth;				

	// State flags
	bool						m_bDamaged;
	bool						m_bAngry;
	bool						m_bGrowling;
	bool						flagEatNow;				// true - ������ ������ ���

	float						m_fCurMinAttackDist;		// according to attack stops

	
	SCurrentAnim				cur_anim;


	CMonsterCorpseCoverEvaluator	*m_corpse_cover_evaluator;
	CCoverEvaluatorFarFromEnemy		*m_enemy_cover_evaluator;

	// -----------------------------------------------------------------------
	// FSM
			void			SetState						(IState *pS, bool bSkipInertiaCheck = false);

	IState				*stateRest;
	IState				*stateEat;
	IState				*stateAttack;
	IState				*stateHide;
	IState				*stateDetour;
	IState				*statePanic;
	IState				*stateExploreDNE;
	IState				*stateExploreDE;
	IState				*stateExploreNDE;
	IState				*stateSquadTask;
	IState				*stateTest;
	IState				*stateNull;

	IState				*CurrentState;

	void					State_PlaySound(u32 internal_type, u32 max_stop_time);
	
	// -----------------------------------------------------------------------------	
	

	
	CMonsterEnemyMemory		EnemyMemory;
	CMonsterSoundMemory		SoundMemory;
	CMonsterCorpseMemory	CorpseMemory;
	CMonsterHitMemory		HitMemory;

	CMonsterEnemyManager	EnemyMan;
	CMonsterCorpseManager	CorpseMan;

	bool					hear_dangerous_sound;
	bool					hear_interesting_sound;

	// -----------------------------------------------------------------------------

	CMotionManager			MotionMan; 

	bool					RayPickEnemy			(const CObject *target_obj, const Fvector &trace_from, const Fvector &dir, float dist, float radius, u32 num_picks);


	float					GetRealDistToEnemy		(const CEntity *pE);

	void					FaceTarget				(const CEntity *entity);
	void					FaceTarget				(const Fvector &position);
	
	bool					b_script_state_must_execute;

	void					TranslateActionToPathParams ();

	bool					state_invisible;

	// ��������� ���� � ������
	virtual void			play_effect_sound		() {}
	

	// �������������� �������� ������ �������
	u32						m_dwFrameLoad;
	u32						m_dwFrameReload;
	u32						m_dwFrameReinit;
	u32						m_dwFrameSpawn;
	u32						m_dwFrameDestroy;
	u32						m_dwFrameClient;

/////////////////////////////////////////////////////////
// TEMP
	u32						prev_size;
////////////////////////////////////////////////////////


	u16						bone_part;
	
	bool					b_velocity_reset;


#ifdef DEBUG
	CMonsterDebug	*HDebug;
	virtual void	OnRender();
#endif

#ifdef 	DEEP_TEST_SPEED
	TTime	time_next_update;
#endif

};

#include "ai_biting_inline.h"





