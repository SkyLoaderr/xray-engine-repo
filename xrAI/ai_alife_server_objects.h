////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_server_objects.h
//	Created 	: 11.02.2003
//  Modified 	: 11.02.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life server objects
////////////////////////////////////////////////////////////////////////////

#ifndef ai_alife_server_objectsH
#define ai_alife_server_objectsH

#include "ai_alife_interfaces.h"
#include "ai_alife_templates.h"
#include "xrServer_Entity_Custom.h"
#ifdef _EDITOR
#include "PropertiesListHelper.h"
#endif

class CALifeObject : public xrServerEntity {
public:
	typedef xrServerEntity inherited;
	_CLASS_ID						m_tClassID;
	_OBJECT_ID						m_tObjectID;
	_GRAPH_ID						m_tGraphID;
	float							m_fDistance;
	bool							m_bOnline;
	u8								m_ucProbability;
	u32								m_dwSpawnGroup;
	bool							m_bDirectControl;

									CALifeObject()
	{
		m_bOnline					= false;
		m_fDistance					= 0.0f;
		m_tClassID					= _CLASS_ID(-1);
		m_tObjectID					= _OBJECT_ID(-1);
		m_tGraphID					= _GRAPH_ID(-1);
		m_ucProbability				= 1;
		m_dwSpawnGroup				= 0;
		m_bDirectControl			= true;
	};

	virtual void					STATE_Write	(NET_Packet &tNetPacket);
	virtual void					STATE_Read	(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read	(NET_Packet &tNetPacket);
#ifdef _EDITOR
    virtual void					FillProp	(LPCSTR pref, PropItemVec &items);
#endif
};

class CALifeMonsterParams : public IPureServerInitObject {
public:
	int								m_iHealth;
	u16								ID;
	
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeTraderParams : public IPureServerInitObject {
public:
	float							m_fCumulativeItemMass;
	u32								m_dwMoney;
	EStalkerRank					m_tRank;
	OBJECT_VECTOR					m_tpItemIDs;
	
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeHumanParams : public CALifeMonsterParams, public CALifeTraderParams {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeTraderAbstract : public IPureServerInitObject {
public:
	PERSONAL_EVENT_P_VECTOR			m_tpEvents;
	TASK_VECTOR						m_tpTaskIDs;
	float							m_fMaxItemMass;

									~CALifeTraderAbstract()
	{
		free_vector					(m_tpEvents);
	};

	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeEventGroup : public CALifeObject {
public:
	typedef	CALifeObject inherited;
	u16								m_wCountBefore;
	u16								m_wCountAfter;
	
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeEvent : public IPureServerObject {
public:
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_GRAPH_ID						m_tGraphID;
	EBattleResult					m_tBattleResult;
	CALifeEventGroup				*m_tpMonsterGroup1;
	CALifeEventGroup				*m_tpMonsterGroup2;

	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifePersonalEvent : public IPureServerObject {
public:
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_TASK_ID						m_tTaskID;
	int								m_iHealth;
	ERelation						m_tRelation;
	OBJECT_VECTOR					m_tpItemIDs;

	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
};

class CALifeTask : public IPureServerObject {
public:
	_TASK_ID						m_tTaskID;
	_TIME_ID						m_tTimeID;
	_OBJECT_ID						m_tCustomerID;
	float							m_fCost;
	ETaskType						m_tTaskType;
	union {
		_CLASS_ID					m_tClassID;
		_OBJECT_ID					m_tObjectID;
	};
	union {
		_LOCATION_ID				m_tLocationID[LOCATION_TYPE_COUNT];
		_GRAPH_ID					m_tGraphID;
	};

	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
};

class CALifePersonalTask : public CALifeTask {
public:
	typedef CALifeTask inherited;
	u32								m_dwTryCount;

	CALifePersonalTask()
	{
		m_dwTryCount = 0;
	};
	
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
};

class CALifeZone : public IPureServerInitObject {
public:
	EAnomalousZoneType				m_tAnomalousZone;

	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeDynamicObject : public CALifeObject {
public:
	typedef	CALifeObject inherited;
	
	_TIME_ID						m_tTimeID;
	u32								m_dwLastSwitchTime;
	
									CALifeDynamicObject()
	{
		m_tTimeID					= 0;
		m_dwLastSwitchTime			= 1;
	}
	
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
};

class CALifeItem : public CALifeDynamicObject {
public:
	typedef	CALifeDynamicObject inherited;
	
	float							m_fMass;
	u32								m_dwCost;
	s32								m_iHealthValue;
	
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
	IC		bool					bfAttached()
	{
		return(ID_Parent < 65534);
	};
};

class CALifeAnomalousZone : public CALifeDynamicObject, public CALifeZone {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeTrader : public CALifeDynamicObject, public CALifeTraderParams, public CALifeTraderAbstract {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeMonsterAbstract : public CALifeDynamicObject {
public:
	typedef	CALifeDynamicObject inherited;
	
	_GRAPH_ID						m_tNextGraphID;
	_GRAPH_ID						m_tPrevGraphID;
	float							m_fGoingSpeed;
	float							m_fCurSpeed;
	float							m_fDistanceFromPoint;
	float							m_fDistanceToPoint;
	
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeDynamicAnomalousZone : public CALifeMonsterAbstract, public CALifeZone {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeMonster : public CALifeMonsterAbstract, public CALifeMonsterParams {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeHumanAbstract : public CALifeMonsterAbstract, public CALifeTraderAbstract {
public:
	DWORD_VECTOR					m_tpaVertices;
	BOOL_VECTOR						m_baVisitedVertices;
	PERSONAL_TASK_P_VECTOR			m_tpTasks;
	ETaskState						m_tTaskState;
	u32								m_dwCurNode;
	u32								m_dwCurTaskLocation;
	u32								m_dwCurTask;
	float							m_fSearchSpeed;

	virtual							~CALifeHumanAbstract()
	{
		free_vector					(m_tpTasks);
	};
	
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeHuman : public CALifeHumanAbstract, public CALifeHumanParams {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	virtual void					Init(LPCSTR caSection);
};

class CALifeAbstractGroup : public IPureServerObject {
public:
	OBJECT_VECTOR				m_tpMembers;
	bool						m_bCreateSpawnPositions;
	u16							m_wCount;

								CALifeAbstractGroup()
	{
		m_tpMembers.clear		();
		m_bCreateSpawnPositions	= true;
		m_wCount				= 1;
	};

	virtual						~CALifeAbstractGroup()
	{
	};
	
	virtual void STATE_Read		(NET_Packet& P, u16 size)
	{
		u32						dwDummy;
		P.r_u32					(dwDummy);
		m_bCreateSpawnPositions = !!dwDummy;
		P.r_u16					(m_wCount);
	};

	virtual void STATE_Write	(NET_Packet& P)
	{
		P.w_u32					(m_bCreateSpawnPositions);
		P.w_u16					(m_wCount);
	};

	virtual void UPDATE_Read	(NET_Packet& P)
	{
		load_base_vector		(m_tpMembers,P);
		u32						dwDummy;
		P.r_u32					(dwDummy);
		m_bCreateSpawnPositions = !!dwDummy;
		P.r_u16					(m_wCount);
	};

	virtual void UPDATE_Write	(NET_Packet& P)
	{
		save_base_vector		(m_tpMembers,P);
		P.w_u32					(m_bCreateSpawnPositions);
		P.w_u16					(m_wCount);
	};

	#ifdef _EDITOR
	virtual void FillProp		(LPCSTR pref, PropItemVec& items)
	{
		PHelper.CreateU16		(items,	PHelper.PrepareKey(pref, "ALife\\Count"),			&m_wCount,			0,0xff);
	};	
	#endif
};

template<class __A> class CALifeGroupTemplate : public __A, public CALifeAbstractGroup {
	typedef __A					inherited1;
	typedef CALifeAbstractGroup inherited2;
public:
	virtual						~CALifeGroupTemplate()
	{
	};
	
	virtual void STATE_Read		(NET_Packet& P, u16 size)
	{
		inherited1::STATE_Read	(P,size);
		inherited2::STATE_Read	(P,size);
	};

	virtual void STATE_Write	(NET_Packet& P)
	{
		inherited1::STATE_Write	(P);
		inherited2::STATE_Write	(P);
	};

	virtual void UPDATE_Read	(NET_Packet& P)
	{
		inherited1::UPDATE_Read	(P);
		inherited2::UPDATE_Read	(P);
	};

	virtual void UPDATE_Write	(NET_Packet& P)
	{
		inherited1::UPDATE_Write(P);
		inherited2::UPDATE_Write(P);
	};

	#ifdef _EDITOR
	virtual void FillProp		(LPCSTR pref, PropItemVec& items)
	{
   		inherited1::FillProp	(pref, items);
   		inherited2::FillProp	(pref, items);
	};	
	#endif
};
#endif