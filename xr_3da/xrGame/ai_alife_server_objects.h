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

class CALifeObject : virtual public xrServerEntity {
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
	u32								m_tNodeID;

									CALifeObject(LPCSTR caSection) : xrServerEntity(caSection)
	{
		m_bOnline					= false;
		m_fDistance					= 0.0f;
		m_tClassID					= _CLASS_ID(-1);
		m_tObjectID					= _OBJECT_ID(-1);
		m_tGraphID					= _GRAPH_ID(-1);
		m_ucProbability				= 1;
		m_dwSpawnGroup				= 0;
		m_bDirectControl			= true;
		m_tNodeID					= u32(-1);
	};

	virtual void					STATE_Write	(NET_Packet &tNetPacket);
	virtual void					STATE_Read	(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read	(NET_Packet &tNetPacket);
#ifdef _EDITOR
    virtual void					FillProp	(LPCSTR pref, PropItemVec &items);
#endif
};

class CALifeTraderParams : public IPureServerObject {
public:
	float							m_fCumulativeItemMass;
	u32								m_dwMoney;
	EStalkerRank					m_tRank;
	float							m_fMaxItemMass;
	
									CALifeTraderParams(LPCSTR caSection)
	{
		m_fCumulativeItemMass		= 0.0f;
		m_dwMoney					= 0;
		if (pSettings->line_exist(caSection, "money"))
			m_dwMoney 				= pSettings->r_u32(caSection, "money");
		m_tRank						= EStalkerRank(pSettings->r_u32(caSection, "rank"));
		m_fMaxItemMass				= pSettings->r_float(caSection, "max_item_mass");
	};

	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
#ifdef _EDITOR
	virtual void					FillProp(LPCSTR pref, PropItemVec& items);
#endif
};

class CALifeTraderAbstract : public CALifeTraderParams, virtual public xrServerEntity {
public:
	typedef CALifeTraderParams inherited;
	PERSONAL_EVENT_P_VECTOR			m_tpEvents;
	TASK_VECTOR						m_tpTaskIDs;

									CALifeTraderAbstract(LPCSTR caSection) : CALifeTraderParams(caSection), xrServerEntity(caSection)
	{
		m_tpEvents.clear			();
		m_tpTaskIDs.clear			();
	};

	virtual							~CALifeTraderAbstract()
	{
		free_vector					(m_tpEvents);
	};

	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
};

class CALifeEventGroup : public CALifeObject {
public:
	typedef	CALifeObject inherited;
	u16								m_wCountBefore;
	u16								m_wCountAfter;
	
									CALifeEventGroup(LPCSTR caSection) : CALifeObject(caSection), xrServerEntity(caSection)
	{
		m_wCountAfter				= m_wCountBefore;
	};

	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
};

class CALifeEvent : public IPureServerObject {
public:
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_GRAPH_ID						m_tGraphID;
	EBattleResult					m_tBattleResult;
	CALifeEventGroup				*m_tpMonsterGroup1;
	CALifeEventGroup				*m_tpMonsterGroup2;

									CALifeEvent(LPCSTR caSection);
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
};

class CALifePersonalEvent : public IPureServerObject {
public:
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_TASK_ID						m_tTaskID;
	int								m_iHealth;
	ERelation						m_tRelation;
	OBJECT_VECTOR					m_tpItemIDs;

									CALifePersonalEvent()
	{
	};

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

									CALifeTask()
	{
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

class CALifeZone : public IPureServerObject {
public:
	EAnomalousZoneType				m_tAnomalousZone;

									CALifeZone(LPCSTR caSection)
	{
		m_tAnomalousZone			= EAnomalousZoneType(pSettings->r_u32(caSection, "anomaly_type"));
	};
	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
};

class CALifeDynamicObject : public CALifeObject {
public:
	typedef	CALifeObject inherited;
	
	_TIME_ID						m_tTimeID;
	u32								m_dwLastSwitchTime;
	
									CALifeDynamicObject(LPCSTR caSection) : CALifeObject(caSection), xrServerEntity(caSection)
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
	
									CALifeItem(LPCSTR caSection) : CALifeDynamicObject(caSection), xrServerEntity(caSection)
	{
		m_fMass						= pSettings->r_float(caSection, "inv_weight");
		m_dwCost					= pSettings->r_u32(caSection, "cost");
		if (pSettings->line_exist	(caSection, "health_value"))
			m_iHealthValue			= pSettings->r_s32(caSection, "health_value");
		else
			m_iHealthValue			= 0;
	};

	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
	IC		bool					bfAttached()
	{
		return(ID_Parent < 65534);
	};
};

class CALifeAnomalousZone : public CALifeDynamicObject, public CALifeZone {
public:
									CALifeAnomalousZone(LPCSTR caSection) : CALifeDynamicObject(caSection), CALifeZone(caSection), xrServerEntity(caSection)
	{
	};

	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
};

class CALifeTrader : public CALifeDynamicObject, public CALifeTraderAbstract {
public:
									CALifeTrader(LPCSTR caSection) : CALifeDynamicObject(caSection), CALifeTraderAbstract(caSection), xrServerEntity(caSection)
	{
	};

	virtual void					STATE_Write(NET_Packet &tNetPacket);
	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read(NET_Packet &tNetPacket);
};

class CALifeAbstractGroup : virtual public xrServerEntity {
public:
	OBJECT_VECTOR				m_tpMembers;
	bool						m_bCreateSpawnPositions;
	u16							m_wCount;

								CALifeAbstractGroup(LPCSTR caSection) : xrServerEntity(caSection)
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
		if (m_wVersion > 19)
			load_base_vector	(m_tpMembers,P);
	};

	virtual void STATE_Write	(NET_Packet& P)
	{
		P.w_u32					(m_bCreateSpawnPositions);
		P.w_u16					(m_wCount);
		save_base_vector		(m_tpMembers,P);
	};

	virtual void UPDATE_Read	(NET_Packet& P)
	{
		u32						dwDummy;
		P.r_u32					(dwDummy);
		m_bCreateSpawnPositions = !!dwDummy;
	};

	virtual void UPDATE_Write	(NET_Packet& P)
	{
		P.w_u32					(m_bCreateSpawnPositions);
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
								CALifeGroupTemplate(LPCSTR caSection) : __A(caSection), CALifeAbstractGroup(caSection), xrServerEntity(caSection)
	{
	};
	
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