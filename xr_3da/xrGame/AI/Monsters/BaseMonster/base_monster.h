////////////////////////////////////////////////////////////////////////////
//	Module 		: base_monster.h
//	Created 	: 21.05.2003
//  Modified 	: 21.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all the monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../../CustomMonster.h"
#include "../ai_monster_motion.h"
#include "../ai_monster_shared_data.h"
#include "../state_manager.h"

#include "../monster_enemy_memory.h"
#include "../monster_corpse_memory.h"
#include "../monster_sound_memory.h"
#include "../monster_hit_memory.h"

#include "../monster_enemy_manager.h"
#include "../monster_corpse_manager.h"

#include "../../../step_manager.h"
#include "../monster_event_manager.h"
#include "../direction_manager.h"
#include "../melee_checker.h"
#include "../monster_morale.h"

class CCharacterPhysicsSupport;
class CAnimTriple;
class CMonsterCorpseCoverEvaluator;
class CCoverEvaluatorFarFromEnemy;
class CCoverEvaluatorCloseToEnemy;
class CMonsterEventManager;
class CCriticalActionInfo;
class CJumping;
class CMovementManager;
class CMonsterMovement;

class CBaseMonster : 
	public CCustomMonster, 
	public CSharedClass<_base_monster_shared, CLASS_ID>, 
	public CStepManager
{
	typedef	CCustomMonster								inherited;
	typedef CSharedClass<_base_monster_shared,CLASS_ID>	inherited_shared;
	typedef CSharedClass<_base_monster_shared,CLASS_ID>	_sd_base;
	typedef CMovementManager							MoveMan;
	
public:
	enum EMovementParameters {
		eVelocityParameterStand			= u32(1) <<  4,
		eVelocityParameterWalkNormal	= u32(1) <<  3,
		eVelocityParameterRunNormal		= u32(1) <<  2,

		eVelocityParameterWalkDamaged	= u32(1) <<  5,
		eVelocityParameterRunDamaged	= u32(1) <<  6,
		eVelocityParameterSteal			= u32(1) <<  7,
		eVelocityParameterDrag			= u32(1) <<  8,
		eVelocityParameterInvisible		= u32(1) <<	 9,
		eVelocityParameterRunAttack		= u32(1) <<	 10,

		eVelocityParamsWalk				= eVelocityParameterStand		| eVelocityParameterWalkNormal,
		eVelocityParamsWalkDamaged		= eVelocityParameterStand		| eVelocityParameterWalkDamaged,
		eVelocityParamsRun				= eVelocityParameterStand		| eVelocityParameterWalkNormal	| eVelocityParameterRunNormal,
		eVelocityParamsRunDamaged		= eVelocityParameterStand		| eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsAttackNorm		= eVelocityParameterStand		| eVelocityParameterWalkNormal	| eVelocityParameterRunNormal,
		eVelocityParamsAttackDamaged	= eVelocityParameterStand		| eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsSteal			= eVelocityParameterStand		| eVelocityParameterSteal,
		eVelocityParamsInvisible		= eVelocityParameterInvisible	| eVelocityParamsRun,
		eVelocityParamsRunAttack		= eVelocityParameterRunAttack	| eVelocityParameterStand, 

		eVelocityParameterCustom		= u32(1) <<	 12,
	};

	// friend definitions
	friend	class			CMotionManager;
	friend	class			CBaseMonsterAttack;

							CBaseMonster						();
	virtual					~CBaseMonster						();

public:
	virtual	Feel::Sound*				dcast_FeelSound				()	{ return this;	}
	virtual	CCharacterPhysicsSupport*	character_physics_support	()	{return m_pPhysics_support;}
	virtual CPHDestroyable*				ph_destroyable				();
	virtual CEntityAlive*				cast_entity_alive			()	{return this;}
	virtual CEntity*					cast_entity					()	{return this;}
	virtual CPhysicsShellHolder*		cast_physics_shell_holder	()	{return this;}
	virtual CParticlesPlayer*			cast_particles_player		()	{return this;}
	virtual CCustomMonster*				cast_custom_monster			()	{return this;}
	virtual CScriptEntity*				cast_script_entity			()	{return this;}

public:
	
	virtual	BOOL			renderable_ShadowReceive		()	{ return TRUE;	}  
	virtual void			Die								(CObject* who);
	virtual void			HitSignal						(float amount, Fvector& vLocalDir, CObject* who, s16 element);
	virtual void			Hit								(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual	void			PHHit							(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void			SelectAnimation					(const Fvector& _view, const Fvector& _move, float speed );

	virtual void			Load							(LPCSTR section);
	virtual DLL_Pure		*_construct						();

	virtual BOOL			net_Spawn						(CSE_Abstract* DC);
	virtual void			net_Destroy						();
	virtual void			net_Save						(NET_Packet& P);
	virtual	BOOL			net_SaveRelevant				();
	virtual void			net_Export						(NET_Packet& P);
	virtual void			net_Import						(NET_Packet& P);

	//save/load server serialization
	virtual void			save							(NET_Packet &output_packet) {inherited::save(output_packet);}
	virtual void			load							(IReader &input_packet)		{inherited::load(input_packet);}


	virtual void			UpdateCL						();
	virtual void			shedule_Update					(u32 dt);

	virtual void			InitThink						() {}
	virtual void			Think							();
	virtual	void			reinit							();
	virtual void			reload							(LPCSTR section);

	virtual void			init							() {}

	virtual void			feel_sound_new					(CObject* who, int eType, CSoundUserDataPtr user_data, const Fvector &Position, float power);
	virtual BOOL			feel_vision_isRelevant			(CObject* O);
	virtual BOOL			feel_touch_on_contact			(CObject* O);

	virtual bool			useful							(const CGameObject *object) const;
	virtual float			evaluate						(const CGameObject *object) const;

	virtual void			OnHUDDraw						(CCustomHUD* hud)			{return inherited::OnHUDDraw(hud);}
	virtual void			OnEvent							(NET_Packet& P, u16 type)	{return inherited::OnEvent(P,type);}
	virtual u16				PHGetSyncItemsNumber			()							{return inherited::PHGetSyncItemsNumber();}
	virtual CPHSynchronize*	PHGetSyncItem					(u16 item)					{return inherited::PHGetSyncItem(item);}
	virtual void			PHUnFreeze						()							{return inherited::PHUnFreeze();}
	virtual void			PHFreeze						()							{return inherited::PHFreeze();}
	virtual BOOL			UsedAI_Locations				()							{return inherited::UsedAI_Locations();}

	virtual const SRotation	Orientation						() const					{return inherited::Orientation();}
	virtual void			renderable_Render				()							{return inherited::renderable_Render();} 

	virtual	void			SetAttackEffector				();
	
	virtual float			get_custom_pitch_speed			(float def_speed);
	virtual void			load_shared						(LPCSTR section);
	virtual void			update_fsm						();

	// ---------------------------------------------------------------------------------
	// Process scripts
	virtual	bool			bfAssignMovement				(CScriptEntityAction	*tpEntityAction);
	virtual	bool			bfAssignObject					(CScriptEntityAction	*tpEntityAction);
	virtual	bool			bfAssignWatch					(CScriptEntityAction	*tpEntityAction);
	virtual bool			bfAssignAnimation				(CScriptEntityAction  *tpEntityAction);
	virtual	bool			bfAssignMonsterAction			(CScriptEntityAction	*tpEntityAction);
	virtual bool			bfAssignSound					(CScriptEntityAction *tpEntityAction);
	
	virtual	void			vfFinishAction					(CScriptEntityAction		*tpEntityAction);
	
	virtual void			ProcessScripts					();

	virtual	CEntity			*GetCurrentEnemy				();
	virtual	CEntity 		*GetCurrentCorpse				();
	virtual int				get_enemy_strength				();
	
	virtual void			SetScriptControl				(const bool bScriptControl, shared_str caSciptName);

	//----------------------------------------------------------------------------------


	virtual void			Exec_Look						( float dt );

	virtual void			SetTurnAnimation				(bool turn_left);
	virtual void			AA_CheckHit						();
	// установка специфических анимаций 
	virtual	void			CheckSpecParams					(u32 /**spec_params/**/) {}
	virtual void			ForceFinalAnimation				() {}
	virtual void			LookPosition					(Fvector to_point, float angular_speed = PI_DIV_3);		// каждый монстр может по-разному реализвать эту функ (e.g. кровосос с поворотом головы и т.п.)


	virtual bool			CanExecRotationJump				() {return false;}

	virtual void			on_travel_point_change			();

	virtual	void			PitchCorrection					();
		
	// ---------------------------------------------------------------------------------
	// Abilities

	virtual bool			ability_invisibility			() {return false;}
	virtual bool			ability_can_drag				() {return false;}
	virtual bool			ability_psi_attack				() {return false;}
	virtual bool			ability_earthquake				() {return false;}
	virtual bool			ability_can_jump				() {return false;}
	virtual bool			ability_distant_feel			() {return false;}
	virtual bool			ability_run_attack				() {return false;}

	// ---------------------------------------------------------------------------------
	virtual void			event_on_step					() {}
	// ---------------------------------------------------------------------------------
	
	// Other
			void			vfUpdateParameters				();
			void			HitEntity						(const CEntity *pEntity, float fDamage, float impulse, Fvector &dir);
	virtual	void			HitEntityInJump					(const CEntity *pEntity) {}
			void			PsyHit							(const CGameObject *object, float value);
			
			CBoneInstance *GetBoneInstance					(LPCTSTR bone_name);
			CBoneInstance *GetBoneInstance					(int bone_id);
		
			CBoneInstance *GetEatBone						();

			void			LoadShared						(LPCSTR section);

	// Cover
			bool			GetCorpseCover					(Fvector &position, u32 &vertex_id);
			bool			GetCoverFromEnemy				(const Fvector &enemy_pos, Fvector &position, u32 &vertex_id);
			bool			GetCoverFromPoint				(const Fvector &pos, Fvector &position, u32 &vertex_id, float min_dist, float max_dist, float radius);
			bool			GetCoverCloseToPoint			(const Fvector &dest_pos, float min_dist, float max_dist, float deviation, float radius ,Fvector &position, u32 &vertex_id);

	// Team	
	virtual void			ChangeTeam						(int team, int squad, int group);
			bool			IsVisibleObject					(const CGameObject *object);
			bool			can_eat_now						();

// members
public:

	CCharacterPhysicsSupport	*m_pPhysics_support;
	
	u32							m_dwHealth;				

	// --------------------------------------------------------------------------------------
	// State flags
	bool						m_bDamaged;
	bool						m_bAngry;
	bool						m_bGrowling;
	bool						m_bAggressive;
	bool						m_bSleep;

	bool						flagEatNow;				// true - сейчас монстр ест (todo: remove it)

	
	void						set_aggressive				(bool val = true) {m_bAggressive = val;}

	//---------------------------------------------------------------------------------------


	CMonsterCorpseCoverEvaluator	*m_corpse_cover_evaluator;
	CCoverEvaluatorFarFromEnemy		*m_enemy_cover_evaluator;
	CCoverEvaluatorCloseToEnemy		*m_cover_evaluator_close_point;

	// ---------------------------------------------------------------------------------
	IStateManagerBase		*StateMan;
	// ---------------------------------------------------------------------------------

	CMonsterEnemyMemory		EnemyMemory;
	CMonsterSoundMemory		SoundMemory;
	CMonsterCorpseMemory	CorpseMemory;
	CMonsterHitMemory		HitMemory;

	CMonsterEnemyManager	EnemyMan;
	CMonsterCorpseManager	CorpseMan;

	bool					hear_dangerous_sound;
	bool					hear_interesting_sound;

	// -----------------------------------------------------------------------------
	CMonsterEventManager	EventMan;
	CMotionManager			MotionMan; 
	CDirectionManager		DirMan;
	// -----------------------------------------------------------------------------

	CMeleeChecker			MeleeChecker;
	CMonsterMorale			Morale;

	// -----------------------------------------------------------------------------

	u32						get_attack_rebuild_time	();

	IC	virtual	EAction		CustomVelocityIndex2Action	(u32 velocity_index) {return ACT_STAND_IDLE;}
		virtual	void		TranslateActionToPathParams ();
	
	
	bool					state_invisible;

	// проиграть звук у актера
	virtual void			play_effect_sound		() {}
	

	u16						m_default_bone_part;
	
	bool					b_velocity_reset;
	bool					force_real_speed;

	bool					script_processing_active;
	bool					b_script_state_must_execute;

IC	void					set_action			(EAction action);
	void					set_state_sound		(u32 type, bool once = false);
IC	void					fall_asleep			(){m_bSleep = true;}
IC	void					wake_up				(){m_bSleep = false;}


	CCriticalActionInfo		*CriticalActionInfo;
	
	// Temp
	u32						m_time_last_attack_success;

	void					on_kill_enemy		(const CEntity *obj);

	CJumping				*m_jumping;

	virtual void			on_first_update		();

#ifdef DEBUG
	struct SDebugInfo {
		bool	active;
		float	x;
		float	y;
		float	delta_y;
		u32		color;
		u32		delimiter_color;

		SDebugInfo() : active(false) {}
		SDebugInfo(float px, float py, float dy, u32 c, u32 dc) : active(true), x(px), y(py), delta_y(dy), color (c), delimiter_color(dc) {}
	};
	
	
	u8						m_show_debug_info;	// 0 - none, 1 - first column, 2 - second column
	void					set_show_debug_info	(u8 show = 1){m_show_debug_info = show;}
	virtual	SDebugInfo		show_debug_info		();
#endif

private:
	bool					m_first_update_initialized;

private:
	CMonsterMovement			*m_movement_manager;
protected:
	virtual CMovementManager	*create_movement_manager();
public:
	IC		CMonsterMovement	&movement				() const;
};

#include "base_monster_inline.h"

