////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects monsters for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALife_MonstersH
#define xrServer_Objects_ALife_MonstersH

#include "xrServer_Objects_ALife.h"

class CSE_ALifeSimulator;

class CSE_ALifeTraderAbstract : virtual public CSE_Abstract {
public:
	float							m_fCumulativeItemMass;
	u32								m_dwMoney;
	EStalkerRank					m_tRank;
	float							m_fMaxItemMass;
	PERSONAL_EVENT_P_VECTOR			m_tpEvents;
	
									CSE_ALifeTraderAbstract(LPCSTR caSection);
	virtual							~CSE_ALifeTraderAbstract();

	virtual void					STATE_Write		(NET_Packet &tNetPacket);
	virtual void					STATE_Read		(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write	(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read		(NET_Packet &tNetPacket);
#ifdef _EDITOR
	virtual void					FillProp		(LPCSTR pref, PropItemVec& items);
#endif
};

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeTrader,CSE_ALifeDynamicObjectVisual,CSE_ALifeTraderAbstract)
	_ORGANIZATION_ID				m_tOrgID;

									CSE_ALifeTrader	(LPCSTR caSection);
	virtual							~CSE_ALifeTrader();

#ifdef _EDITOR
	int 							supplies_count;
    void __fastcall   				OnSuppliesCountChange	(PropValue* sender);
#endif    
    
	ARTEFACT_TRADER_ORDER_VECTOR	m_tpOrderedArtefacts;
	TRADER_SUPPLY_VECTOR			m_tpSupplies;
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeCreatureAbstract,CSE_ALifeDynamicObjectVisual)
	u8								s_team;
	u8								s_squad;
	u8								s_group;
	float							fHealth;

	u32								timestamp;				// server(game) timestamp
	u8								flags;
	float							o_model;				// model yaw
	SRotation						o_torso;				// torso in world coords
	bool							m_bDeathIsProcessed;
									
									CSE_ALifeCreatureAbstract(LPCSTR caSection);
	virtual u8						g_team					();
	virtual u8						g_squad					();
	virtual u8						g_group					();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeMonsterAbstract,CSE_ALifeCreatureAbstract,IPureSchedulableObject)
	_GRAPH_ID						m_tNextGraphID;
	_GRAPH_ID						m_tPrevGraphID;
	float							m_fGoingSpeed;
	float							m_fCurSpeed;
	float							m_fDistanceFromPoint;
	float							m_fDistanceToPoint;
	TERRAIN_VECTOR					m_tpaTerrain;
	
									CSE_ALifeMonsterAbstract(LPCSTR caSection);
#ifdef _EDITOR
									virtual	void					Update					(CSE_ALifeSimulator *tpALifeSimulator){};
#else
#ifdef AI_COMPILER
									virtual	void					Update					(CSE_ALifeSimulator *tpALifeSimulator){};
#else
									virtual	void					Update					(CSE_ALifeSimulator *tpALifeSimulator);
#endif
#endif
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeCreatureActor,CSE_ALifeCreatureAbstract,CSE_ALifeTraderAbstract)
	u16								mstate;
	Fvector							accel;
	Fvector							velocity;
	float							fArmor;
	u8								weapon;
									CSE_ALifeCreatureActor		(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeCreatureCrow,CSE_ALifeCreatureAbstract)
									CSE_ALifeCreatureCrow		(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMonsterRat,CSE_ALifeMonsterAbstract)
	// Personal characteristics:
	float							fEyeFov;
	float							fEyeRange;
	float							fMinSpeed;
	float							fMaxSpeed;
	float							fAttackSpeed;
	float							fMaxPursuitRadius;
	float							fMaxHomeRadius;
	// morale
	float							fMoraleSuccessAttackQuant;
	float							fMoraleDeathQuant;
	float							fMoraleFearQuant;
	float							fMoraleRestoreQuant;
	u16								u16MoraleRestoreTimeInterval;
	float							fMoraleMinValue;
	float							fMoraleMaxValue;
	float							fMoraleNormalValue;
	// attack
	float							fHitPower;
	u16								u16HitInterval;
	float							fAttackDistance;
	float							fAttackAngle;
	float							fAttackSuccessProbability;

									CSE_ALifeMonsterRat	(LPCSTR caSection);				// constructor for variable initialization
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMonsterZombie,CSE_ALifeMonsterAbstract)
	// Personal characteristics:
	float							fEyeFov;
	float							fEyeRange;
	float							fMinSpeed;
	float							fMaxSpeed;
	float							fAttackSpeed;
	float							fMaxPursuitRadius;
	float							fMaxHomeRadius;
	// attack
	float							fHitPower;
	u16								u16HitInterval;
	float							fAttackDistance;
	float							fAttackAngle;

									CSE_ALifeMonsterZombie	(LPCSTR caSection);				// constructor for variable initialization
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMonsterBiting,CSE_ALifeMonsterAbstract)
									CSE_ALifeMonsterBiting	(LPCSTR caSection);				// constructor for variable initialization
SERVER_ENTITY_DECLARE_END

//-- server object for chimera --
SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMonsterChimera,CSE_ALifeMonsterAbstract)
									CSE_ALifeMonsterChimera	(LPCSTR caSection);				// constructor for variable initialization
SERVER_ENTITY_DECLARE_END
//-------------------------------


SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeHumanAbstract,CSE_ALifeTraderAbstract,CSE_ALifeMonsterAbstract)
	DWORD_VECTOR					m_tpPath;
	u32								m_dwCurNode;
	_GRAPH_ID						m_tDestGraphPointIndex;								
	xr_vector<bool>					m_baVisitedVertices;
	ETaskState						m_tTaskState;
	u32								m_dwCurTaskLocation;
	u32								m_dwCurTaskID;
	float							m_fSearchSpeed;
	string128						m_caKnownCustomers;
	OBJECT_VECTOR					m_tpKnownCustomers;

									CSE_ALifeHumanAbstract	(LPCSTR caSection);
	virtual							~CSE_ALifeHumanAbstract	();
#ifndef _EDITOR
#ifndef AI_COMPILER
	virtual	void					Update					(CSE_ALifeSimulator	*tpALife);
			// FSM
			void					vfChooseTask			(CSE_ALifeSimulator	*tpALife);
			void					vfHealthCare			(CSE_ALifeSimulator	*tpALife);
			void					vfBuySupplies			(CSE_ALifeSimulator	*tpALife);
			void					vfGoToCustomer			(CSE_ALifeSimulator	*tpALife);
			void					vfBringToCustomer		(CSE_ALifeSimulator	*tpALife);
			void					vfGoToSOS				(CSE_ALifeSimulator	*tpALife);
			void					vfSendSOS				(CSE_ALifeSimulator	*tpALife);
			void					vfAccomplishTask		(CSE_ALifeSimulator	*tpALife);
			void					vfSearchObject			(CSE_ALifeSimulator	*tpALife);
			// FSM miscellanious
			void					vfChooseHumanTask		(CSE_ALifeSimulator	*tpALife);
			bool					HealthIsGood			();
			bool					CanTreat				();
			bool					EnoughMoneyToTreat		();
			bool					EnoughTimeToTreat		();
			bool					EnoughEquipmentToGo		();
			bool					DistanceToTraderIsDanger();
			bool					EnoughMoneyToEquip		();
			// miscellanious
			bool					bfCheckIfTaskCompleted	(CSE_ALifeSimulator	*tpALife, OBJECT_IT &I);
			bool					bfCheckIfTaskCompleted	(CSE_ALifeSimulator	*tpALife);
			bool					bfProcessItems			(CSE_ALifeSimulator	*tpALife);
			void					vfCheckForDeletedEvents	(CSE_ALifeSimulator	*tpALife);
			bool					bfChooseNextRoutePoint	(CSE_ALifeSimulator	*tpALife);
			void					vfSetCurrentTask		(CSE_ALifeSimulator	*tpALife, _TASK_ID &tTaskID);
#endif
#endif
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeHumanStalker,CSE_ALifeHumanAbstract)
									CSE_ALifeHumanStalker	(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectIdol,CSE_ALifeHumanAbstract)
	string256						m_caAnimations;
	u32								m_dwAniPlayType;
									CSE_ALifeObjectIdol		(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

#endif