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
#include "../../inventory.h"
#include "../../weapon_manager.h"
#include "../../sight_manager.h"
#include "../../state_manager_stalker.h"
#include "../../state_internal.h"
#include "ai_stalker_animations.h"
#include "ai_stalker_space.h"

using namespace StalkerSpace;

class CSE_ALifeSimulator;
class CCharacterPhysicsSupport;
class CWeapon;

//#define LOG_PARAMETERS

#ifdef LOG_PARAMETERS
extern FILE	*ST_VF;
#endif


class CAI_Stalker : 
	public CCustomMonster, 
	public CInventoryOwner,
	public CWeaponManager,
	public CSightManager,
	public CStalkerAnimations, 
	public CStalkerMovementManager,
	public CStateManagerStalker,
	public CStateInternal
{
private:
	typedef CCustomMonster inherited;
	
	EActionState				m_tActionState;
	u32							m_dwActionStartTime;
	bool						_A,_B,_C,_D,_E,_F,_G,_H,_I,_J,_K,_L,_M;
	bool						A,B,C,D,E,F,G,H,I,J,K,L,M;
	u32							m_dwRandomFactor;
	u32							m_dwInertion;
	u32							m_dwParticularState;
	u32							m_dwLastSoundUpdate;
	
	u32							m_dwLastUpdate;
	u32							m_current_update;
	u32							m_dwUpdateCount;
	bool						m_bStateChanged;
	
	ETaskState					m_tTaskState;
	_GRAPH_ID					m_tDestGraphPointIndex;
	_TASK_ID					m_tTaskID;
	OBJECT_VECTOR				m_tpKnownCustomers;


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
	virtual void				g_fireParams					(Fvector& P, Fvector& D);
	virtual void				HitSignal						(float P,	Fvector& vLocalDir, CObject* who, s16 element);
	virtual float				HitScale						(int element);
	virtual void				Die								();

	virtual void				OnEvent							(NET_Packet& P, u16 type);
	virtual void				feel_touch_new					(CObject* O);

	virtual void				renderable_Render				();
	virtual void				Exec_Look						(float dt);
	virtual void				Hit								(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type = eHitTypeWound);
	virtual BOOL				feel_vision_isRelevant			(CObject* who);
	virtual float				Radius							() const;
	virtual void				OnHUDDraw						(CCustomHUD* hud) {inherited::OnHUDDraw(hud);}
	virtual void				OnRender						() {inherited::OnRender();}

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

//protected:
	// movement and look setup functions
			void				vfSetParameters					(
				PathManagers::CAbstractVertexEvaluator *tpPathEvaluator, 
				Fvector *tpDesiredPosition, 
				PathManagers::CAbstractVertexEvaluator *tpNodeEvaluator, 
				EObjectAction tWeaponState, 
				EPathType		tGlobalPathType,
				EDetailPathType	tPathType, 
				EBodyState tBodyState, 
				EMovementType tMovementType, 
				EMentalState tMentalState, 
				ELookType tLookType
			);
			void				vfSetParameters					(
				PathManagers::CAbstractVertexEvaluator *tpPathEvaluator, 
				Fvector *tpDesiredPosition, 
				PathManagers::CAbstractVertexEvaluator *tpNodeEvaluator, 
				EObjectAction tWeaponState, 
				EPathType		tGlobalPathType,
				EDetailPathType	tPathType, 
				EBodyState tBodyState, 
				EMovementType tMovementType, 
				EMentalState tMentalState, 
				ELookType tLookType, 
				const Fvector &tPointToLook, 
				u32 dwLookOverDelay = 2000
			);
			
	// miscellanious functions
			void				DropItemSendMessage				(CObject *O);
			bool				bfCheckForNodeVisibility		(u32 dwNodeID, bool bIfRyPick = false);
			void				vfUpdateParameters				(bool &A, bool &B, bool &C, bool &D, bool &E, bool &F, bool &G, bool &H, bool &I, bool &J, bool &K, bool &L, bool &M);
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
			void				vfContinueWithALifeGoals		(PathManagers::CAbstractVertexEvaluator *tpNodeEvaluator = 0);
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
			bool				bfCheckIfTaskCompleted			(OBJECT_IT		&I);
			void				vfSetCurrentTask				(_TASK_ID		&tTaskID);
			bool				bfAssignDestinationNode			();
			void				vfFinishTask					();
	virtual const SRotation		Orientation				() const
	{
		return					(m_head.current);
	};
};

#include "ai_stalker_inline.h"