////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../CustomMonster.h"
#include "../../stalker_movement_manager.h"
#include "../../sight_manager.h"
#include "../../setup_manager.h"
#include "../../setup_action.h"
#include "../../object_handler.h"
#include "ai_stalker_animations.h"
#include "ai_stalker_space.h"
#include "../../AI_PhraseDialogManager.h"
#include "../../motivation_action_manager_stalker.h"
#include "../../seniority_hierarchy_holder.h"
#include "../../team_hierarchy_holder.h"
#include "../../squad_hierarchy_holder.h"
#include "../../group_hierarchy_holder.h"

using namespace StalkerSpace;

class CALifeSimulator;
class CCharacterPhysicsSupport;
class CWeapon;
class CCoverPoint;
class CCoverEvaluatorCloseToEnemy;
class CCoverEvaluatorFarFromEnemy;
class CCoverEvaluatorBest;
class CCoverEvaluatorAngle;
class CCoverEvaluatorSafe;
class CAgentManager;

class CAI_Stalker : 
	public CCustomMonster, 
	public CObjectHandler,
	public CSightManager,
	public CStalkerAnimations, 
	public CStalkerMovementManager,
	public CMotivationActionManagerStalker,
	public CSetupManager<CSetupAction,CAI_Stalker,u32>,
	public CAI_PhraseDialogManager
{
private:
	typedef CCustomMonster								inherited;
	
public:
	typedef CSetupManager<CSetupAction,CAI_Stalker,u32>	CSSetupManager;

private:
	u32							m_dwParticularState;
	ALife::ETaskState			m_tTaskState;
	ALife::_GRAPH_ID			m_tDestGraphPointIndex;
	ALife::_TASK_ID				m_tTaskID;
	ALife::OBJECT_VECTOR		m_tpKnownCustomers;
	int							m_r_hand;
	int							m_l_finger1;
	int							m_r_finger2;

//////////////////////////////////////////////////////////////////////////
	float						m_disp_walk_stand;
	float						m_disp_walk_crouch;
	float						m_disp_run_stand;
	float						m_disp_run_crouch;
	float						m_disp_stand_stand;
	float						m_disp_stand_crouch;
//////////////////////////////////////////////////////////////////////////
public:
	u32							m_last_best_item_frame;
	bool						m_item_actuality;
	CInventoryItem				*m_best_item_to_kill;
	CInventoryItem				*m_best_ammo;
	const CInventoryItem		*m_best_found_item_to_kill;
	const CInventoryItem		*m_best_found_ammo;

	CCoverEvaluatorCloseToEnemy	*m_ce_close;
	CCoverEvaluatorFarFromEnemy	*m_ce_far;
	CCoverEvaluatorBest			*m_ce_best;
	CCoverEvaluatorAngle		*m_ce_angle;
	CCoverEvaluatorSafe			*m_ce_safe;
	
	CCharacterPhysicsSupport	*m_pPhysics_support;

public:
	// heritage
								CAI_Stalker						();
	virtual						~CAI_Stalker					();

public:
	virtual CInventoryOwner*			cast_inventory_owner	()						{return this;}
	virtual CEntityAlive*				cast_entity_alive		()						{return this;}
	virtual CEntity*					cast_entity				()						{return this;}
	virtual CGameObject*				cast_game_object		()						{return this;}
public:

			void				init							();
	virtual void				Load							(LPCSTR	section );				
	virtual	void				reinit							();
	virtual void				reload							(LPCSTR	section );				
	
	virtual BOOL				net_Spawn						(LPVOID DC);
	virtual void				net_Export						(NET_Packet& P);
	virtual void				net_Import						(NET_Packet& P);
	virtual void				net_Destroy						();
	virtual void				net_Save						(NET_Packet& P);
	virtual	BOOL				net_SaveRelevant				();

	//save/load server serialization
	virtual void				save							(NET_Packet &output_packet);
	virtual void				load							(IReader &input_packet);

	virtual void				UpdateCL						();
	virtual void				shedule_Update					(u32 dt);
	virtual void				Think							();
	virtual void				SelectAnimation					(const Fvector& _view, const Fvector& _move, float speed );
	virtual BOOL				UsedAI_Locations				();

	virtual void				g_WeaponBones					(int &L, int &R1, int &R2);
	virtual void				g_fireParams					(const CHudItem* pHudItem, Fvector& P, Fvector& D);
	virtual void				HitSignal						(float P,	Fvector& vLocalDir, CObject* who, s16 element);
	virtual void				Die								();

	virtual void				OnEvent							(NET_Packet& P, u16 type);
	virtual void				feel_touch_new					(CObject* O);

	virtual void				renderable_Render				();
	virtual void				Exec_Look						(float dt);
	virtual void				Hit								(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual	void				PHHit							(float P,Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual BOOL				feel_vision_isRelevant			(CObject* who);
	virtual float				Radius							() const;
	virtual void				OnHUDDraw						(CCustomHUD* hud) {inherited::OnHUDDraw(hud);}
#ifdef DEBUG
	virtual void				OnRender						();
#endif

	virtual bool				useful							(const CGameObject *object) const;
	virtual	float				evaluate						(const CGameObject *object) const;
	
	// PDA && Dialogs
	virtual void				ReceivePdaMessage				(u16 who, EPdaMsg msg, INFO_INDEX info_index);

	// scripts
	virtual void				UseObject				(const CObject			*tpObject);
	virtual ALife::EStalkerRank	GetRank					() const;
	virtual CWeapon				*GetCurrentWeapon		() const;
	virtual u32					GetWeaponAmmo			() const;
	virtual CInventoryItem		*GetCurrentEquipment	() const;
	virtual CInventoryItem		*GetMedikit				() const;
	virtual CInventoryItem		*GetFood				() const;
	virtual	bool				bfAssignMovement		(CScriptEntityAction			*tpEntityAction);
	virtual	bool				bfAssignWatch			(CScriptEntityAction			*tpEntityAction);
	virtual	void				ResetScriptData			(void					*P = 0);
	virtual	bool				bfAssignObject			(CScriptEntityAction			*tpEntityAction);
	virtual	bool				bfAssignAnimation		(CScriptEntityAction			*tpEntityAction);
	
	// physics
	virtual u16					PHGetSyncItemsNumber	()			{return inherited ::PHGetSyncItemsNumber();}
	virtual CPHSynchronize*		PHGetSyncItem			(u16 item)	{return inherited ::PHGetSyncItem(item);}
	virtual void				PHUnFreeze				()			{return inherited ::PHUnFreeze();}
	virtual void				PHFreeze				()			{return inherited ::PHFreeze();}

	// miscellanious functions
			void				DropItemSendMessage				(CObject *O);
			bool				bfCheckForNodeVisibility		(u32 dwNodeID, bool bIfRyPick = false);
	virtual	ALife::ERelationType tfGetRelationType				(const CEntityAlive *tpEntityAlive) const;

	virtual const SRotation		Orientation						() const
	{
		return					(m_head.current);
	};

	virtual bool				use_model_pitch					() const;

	//////////////////////////////////////////////////////////////////////////
	// action/evaluators/motivations support functions
	//////////////////////////////////////////////////////////////////////////
public:
	virtual void				OnItemTake						(CInventoryItem *inventory_item);
	virtual void				OnItemDrop						(CInventoryItem *inventory_item);
			bool				item_to_kill					();
			bool				item_can_kill					();
			bool				remember_item_to_kill			();
			bool				remember_ammo					();
			bool				ready_to_kill					();
			bool				kill_distance					();
			void				update_best_item_info			();
	virtual float				GetWeaponAccuracy				() const;
	virtual	void				spawn_supplies					();
			void				adjust_speed_to_animation		(const EMovementDirection movement_direction);
	IC		const CAgentManager	&agent_manager					() const;
	virtual bool				human_being						() const
	{
		return					(true);
	}
			bool				undetected_anomaly				();
			bool				inside_anomaly					();

	//игровое имя 
	virtual LPCSTR	Name										() const {return CInventoryOwner::Name();}


	virtual	Feel::Sound*		dcast_FeelSound			()		{ return this;	}
			bool				can_kill_member			();
			bool				can_kill_member			(const Fvector &position, const Fvector &direction) const;
			void				dbg_animation			(LPCSTR caption, CMotionDef *animation);

};

#include "ai_stalker_inline.h"