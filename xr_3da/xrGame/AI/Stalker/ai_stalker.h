////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

//#define OLD_OBJECT_HANDLER
//#define OLD_DECISION_BLOCK

#include "../../CustomMonster.h"
#include "../../stalker_movement_manager.h"
#include "../../sight_manager.h"
#include "../../script_binder.h"
#include "../../setup_manager.h"
#include "../../setup_action.h"
#include "../../cover_manager.h"
#include "ai_stalker_animations.h"
#include "ai_stalker_space.h"

#ifdef OLD_OBJECT_HANDLER
	#include "../../object_handler.h"
#else
	#include "../../object_handler_goap.h"
#endif

#ifdef OLD_DECISION_BLOCK
	#include "../../state_manager_stalker.h"
#else
	#include "../../motivation_action_manager_stalker.h"
#endif

using namespace StalkerSpace;

class CSE_ALifeSimulator;
class CCharacterPhysicsSupport;
class CWeapon;
class CCoverPoint;
class CCoverEvaluatorCloseToEnemy;
class CCoverEvaluatorFarFromEnemy;
class CCoverEvaluatorBest;
class CCoverEvaluatorAngle;

//#define LOG_PARAMETERS

#ifdef LOG_PARAMETERS
extern FILE	*ST_VF;
#endif

class CAI_Stalker : 
	public CCustomMonster, 
#ifdef OLD_OBJECT_HANDLER
	public CObjectHandler,
#else
	public CObjectHandlerGOAP,
#endif
	public CSightManager,
	public CStalkerAnimations, 
	public CStalkerMovementManager,
#ifdef OLD_DECISION_BLOCK
	public CStateManagerStalker,
#else
	public CMotivationActionManagerStalker,
#endif
	public CScriptBinder,
	public CSetupManager<CSetupAction,CAI_Stalker,u32>
{
private:
	typedef CCustomMonster								inherited;
	typedef CSetupManager<CSetupAction,CAI_Stalker,u32>	CSSetupManager;
	
	u32							m_dwParticularState;
	ALife::ETaskState			m_tTaskState;
	ALife::_GRAPH_ID			m_tDestGraphPointIndex;
	ALife::_TASK_ID				m_tTaskID;
	ALife::OBJECT_VECTOR		m_tpKnownCustomers;
	int							m_r_hand;
	int							m_l_finger1;
	int							m_r_finger2;

public:
	CCharacterPhysicsSupport	*m_pPhysics_support;

	// heritage
								CAI_Stalker						();
	virtual						~CAI_Stalker					();
			void				Init							();
	virtual void				Load							(LPCSTR	section );				
	virtual	void				reinit							();
	virtual void				reload							(LPCSTR	section );				
	
	virtual BOOL				net_Spawn						(LPVOID DC);
	virtual void				net_Export						(NET_Packet& P);
	virtual void				net_Import						(NET_Packet& P);
	virtual void				net_Destroy						();
	
	virtual void				UpdateCL						();
	virtual void				shedule_Update					(u32 dt);
	virtual void				Think							();
	virtual void				SelectAnimation					(const Fvector& _view, const Fvector& _move, float speed );
	virtual BOOL				UsedAI_Locations				();

	virtual void				g_WeaponBones					(int &L, int &R1, int &R2);
	virtual void				g_fireParams					(const CHudItem* pHudItem, Fvector& P, Fvector& D);
	virtual void				HitSignal						(float P,	Fvector& vLocalDir, CObject* who, s16 element);
	virtual float				HitScale						(int element);
	virtual void				Die								();

	virtual void				OnEvent							(NET_Packet& P, u16 type);
	virtual void				feel_touch_new					(CObject* O);

	virtual void				renderable_Render				();
	virtual void				Exec_Look						(float dt);
	virtual void				Hit								(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual	void				PHHit							(Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual BOOL				feel_vision_isRelevant			(CObject* who);
	virtual float				Radius							() const;
	virtual void				OnHUDDraw						(CCustomHUD* hud) {inherited::OnHUDDraw(hud);}
#ifdef DEBUG
	virtual void				OnRender						() {inherited::OnRender();}
#endif

	virtual bool				useful							(const CGameObject *object) const;
	virtual	float				evaluate						(const CGameObject *object) const;
	
	// PDA		
			void				ReceivePdaMessage				(u16 who, EPdaMsg msg, int info_index);

	// scripts
	virtual void				UseObject				(const CObject			*tpObject);
	virtual ALife::EStalkerRank	GetRank					() const;
	virtual CWeapon				*GetCurrentWeapon		() const;
	virtual u32					GetWeaponAmmo			() const;
	virtual CInventoryItem		*GetCurrentEquipment	() const;
	virtual CInventoryItem		*GetMedikit				() const;
	virtual CInventoryItem		*GetFood				() const;
	virtual	bool				bfAssignMovement		(CEntityAction			*tpEntityAction);
	virtual	bool				bfAssignWatch			(CEntityAction			*tpEntityAction);
	virtual	void				ResetScriptData			(void					*P = 0);
	virtual	bool				bfAssignObject			(CEntityAction			*tpEntityAction);
	virtual	bool				bfAssignAnimation		(CEntityAction			*tpEntityAction);
	
	// physics
	virtual u16					PHGetSyncItemsNumber	()			{return inherited ::PHGetSyncItemsNumber();}
	virtual CPHSynchronize*		PHGetSyncItem			(u16 item)	{return inherited ::PHGetSyncItem(item);}
	virtual void				PHUnFreeze				()			{return inherited ::PHUnFreeze();}
	virtual void				PHFreeze				()			{return inherited ::PHFreeze();}

	// miscellanious functions
			void				DropItemSendMessage				(CObject *O);
			bool				bfCheckForNodeVisibility		(u32 dwNodeID, bool bIfRyPick = false);
			bool				bfIfHuman						(const CEntity *tpEntity = 0);

	// states
			void				Death							();
			void				BackStraight					();
			void				BackCover						(bool bFire = true);
			void				ForwardCover					();
			void				ForwardStraight					();
			void				Camp							(bool bWeapon = true);
			void				Panic							();
			void				Hide							();
			void				Detour							();
			void				SearchEnemy						();
			void				ExploreDE						();
			void				ExploreDNE						();
			void				ExploreNDE						();
			void				ExploreNDNE						();
			void				TakeItems						();
	// ALife states			
			void				ALifeUpdate						();
			void				vfChooseTask					();
			void				vfHealthCare					();
			void				vfBuySupplies					();
			void				vfGoToCustomer					();
			void				vfBringToCustomer				();
			void				vfGoToSOS						();
			void				vfSendSOS						();
			void				vfAccomplishTask				();
			void				vfContinueWithALifeGoals		(CAbstractVertexEvaluator *tpNodeEvaluator = 0);
			void				vfSearchObject					();
			bool				bfHealthIsGood					();
			bool				bfItemCanTreat					(CInventoryItem *tpInventoryItem);
			void				vfUseItem						(CInventoryItem *tpInventoryItem);
			bool				bfCanTreat						();
			bool				bfEnoughMoneyToTreat			();
			bool				bfEnoughTimeToTreat				();
			bool				bfEnoughEquipmentToGo			();
			bool				bfDistanceToTraderIsDanger		();
			bool				bfEnoughMoneyToEquip			();
			void				vfChooseHumanTask				();
			bool				bfCheckIfTaskCompleted			();
			bool				bfCheckIfTaskCompleted			(ALife::OBJECT_IT	&I);
			void				vfSetCurrentTask				(ALife::_TASK_ID	&tTaskID);
			bool				bfAssignDestinationNode			();
			void				vfFinishTask					();
	virtual const SRotation		Orientation						() const
	{
		return					(m_head.current);
	};

	virtual bool				use_model_pitch					() const;

	//////////////////////////////////////////////////////////////////////////
	// action/evaluators/motivations suppoort functions
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
	virtual bool				human_being						() const
	{
		return					(true);
	}

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

	virtual	Feel::Sound*		dcast_FeelSound			()			{ return this;	}
};

#include "ai_stalker_inline.h"