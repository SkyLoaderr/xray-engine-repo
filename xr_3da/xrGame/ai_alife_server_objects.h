////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_server_objects.h
//	Created 	: 11.02.2003
//  Modified 	: 11.02.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life server objects
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_objects.h"

class CALifeMonsterParams : public IPureServerEditorObject {
public:
	int								m_iHealth;
	
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
	}

	virtual void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		tNetPacket.w_s32			(m_iHealth);
	};

	virtual void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		tNetPacket.r_s32			(m_iHealth);
	};
	
	virtual void					Init(LPCSTR caSection)
	{
		m_iHealth					= pSettings->ReadINT(caSection, "health");
	};
};

class CALifeTraderParams : public IPureServerEditorObject {
public:
	float							m_fCumulativeItemMass;
	u32								m_dwMoney;
	EStalkerRank					m_tRank;
	OBJECT_VECTOR					m_tpItemIDs;
	
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
	}

	virtual void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		tNetPacket.w_float			(m_fCumulativeItemMass);
		tNetPacket.w_u32			(m_dwMoney);
		tNetPacket.w_u32			(m_tRank);
		save_base_vector			(m_tpItemIDs,tNetPacket);
	};

	virtual void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		tNetPacket.r_float			(m_fCumulativeItemMass);
		tNetPacket.r_u32			(m_dwMoney);
		u32							dwDummy;
		tNetPacket.r_u32			(dwDummy);
		m_tRank						= EStalkerRank(m_tRank);
		load_base_vector			(m_tpItemIDs,tNetPacket);
	};

	virtual void					Init(LPCSTR caSection)
	{
		m_fCumulativeItemMass		= 0.0f;
		m_dwMoney					= 0;
		if (pSettings->LineExists(caSection, "money"))
			m_dwMoney = pSettings->ReadINT(caSection, "money");
		m_tRank						= EStalkerRank(pSettings->ReadINT(caSection, "rank"));
		m_tpItemIDs.clear			();
	};
};

class CALifeHumanParams : public CALifeMonsterParams, public CALifeTraderParams {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		CALifeMonsterParams::STATE_Write(tNetPacket);
		CALifeTraderParams::STATE_Write(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		CALifeMonsterParams::STATE_Read(tNetPacket, size);
		CALifeTraderParams::STATE_Read(tNetPacket, size);
	}

	virtual void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		CALifeMonsterParams::UPDATE_Write(tNetPacket);
		CALifeTraderParams::UPDATE_Write(tNetPacket);
	};
	
	virtual void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		CALifeMonsterParams::UPDATE_Read(tNetPacket);
		CALifeTraderParams::UPDATE_Read(tNetPacket);
	};
	
	virtual void					Init(LPCSTR caSection)
	{
		CALifeMonsterParams::Init	(caSection);
		CALifeTraderParams::Init	(caSection);
	};
};

class CALifeTraderAbstract : public IPureServerEditorObject {
public:
	PERSONAL_EVENT_P_VECTOR			m_tpEvents;
	TASK_VECTOR						m_tpTaskIDs;
	float							m_fMaxItemMass;

	virtual							~CALifeTraderAbstract()
	{
		free_vector					(m_tpEvents);
	};
	
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		save_vector					(m_tpEvents,tNetPacket);
		save_base_vector			(m_tpTaskIDs,tNetPacket);
		tNetPacket.w_float			(m_fMaxItemMass);
	};

	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		load_vector					(m_tpEvents,tNetPacket);
		load_base_vector			(m_tpTaskIDs,tNetPacket);
		tNetPacket.r_float			(m_fMaxItemMass);
	};

	virtual void					Init(LPCSTR caSection)
	{
		m_tpEvents.clear			();
		m_tpTaskIDs.clear			();
		m_fMaxItemMass				= pSettings->ReadFLOAT(caSection, "max_item_mass");
	};
};

class CALifeEventGroup : public CALifeObject {
public:
	typedef	CALifeObject inherited;
	u16								m_wCountAfter;
	
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		inherited::STATE_Write		(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		inherited::STATE_Read		(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Write		(tNetPacket);
		tNetPacket.w				(&m_wCountAfter,sizeof(m_wCountAfter));
	};

	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Read		(tNetPacket);
		tNetPacket.r				(&m_wCountAfter,sizeof(m_wCountAfter));
	};

	virtual void					Init(LPCSTR caSection)
	{
		m_wCountAfter				= m_wCount;
	};
};

class CALifeEvent : public IPureServerObject {
public:
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_GRAPH_ID						m_tGraphID;
	EBattleResult					m_tBattleResult;
	CALifeEventGroup				*m_tpMonsterGroup1;
	CALifeEventGroup				*m_tpMonsterGroup2;

	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
	}

	virtual void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		tNetPacket.w				(&m_tEventID,		sizeof(m_tEventID));
		tNetPacket.w				(&m_tTimeID,		sizeof(m_tTimeID));
		tNetPacket.w				(&m_tGraphID,		sizeof(m_tGraphID));
		tNetPacket.w				(&m_tBattleResult,	sizeof(m_tBattleResult));
		m_tpMonsterGroup1->UPDATE_Write(tNetPacket);
		m_tpMonsterGroup2->UPDATE_Write(tNetPacket);
	};
	
	virtual void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		tNetPacket.r				(&m_tEventID,		sizeof(m_tEventID));
		tNetPacket.r				(&m_tTimeID,		sizeof(m_tTimeID));
		tNetPacket.r				(&m_tGraphID,		sizeof(m_tGraphID));
		tNetPacket.r				(&m_tBattleResult,	sizeof(m_tBattleResult));
		m_tpMonsterGroup1->UPDATE_Read(tNetPacket);
		m_tpMonsterGroup2->UPDATE_Read(tNetPacket);
	};
};

class CALifePersonalEvent : public IPureServerObject {
public:
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_TASK_ID						m_tTaskID;
	int								m_iHealth;
	ERelation						m_tRelation;
	OBJECT_VECTOR					m_tpItemIDs;

	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
	}

	virtual void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		tNetPacket.w				(&m_tEventID,			sizeof(m_tEventID));
		tNetPacket.w				(&m_tTimeID,			sizeof(m_tTimeID));
		tNetPacket.w				(&m_tTaskID,			sizeof(m_tTaskID));
		tNetPacket.w_u32			(m_iHealth);
		tNetPacket.w				(&m_tRelation,			sizeof(m_tRelation));
		save_base_vector			(m_tpItemIDs, tNetPacket);
	};
	
	virtual void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		tNetPacket.r				(&m_tEventID,		sizeof(m_tEventID));
		tNetPacket.r				(&m_tTimeID,		sizeof(m_tTimeID));
		tNetPacket.r				(&m_tTaskID,		sizeof(m_tTaskID));
		tNetPacket.r_s32			(m_iHealth);
		tNetPacket.r				(&m_tRelation,		sizeof(m_tRelation));
		load_base_vector			(m_tpItemIDs, tNetPacket);
	};
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
		_LOCATION_ID				m_tLocationID;
		_GRAPH_ID					m_tGraphID;
	};

	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
	}

	virtual void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		tNetPacket.w				(&m_tTaskID,	sizeof(m_tTaskID));
		tNetPacket.w				(&m_tTimeID,	sizeof(m_tTimeID));
		tNetPacket.w				(&m_tCustomerID,sizeof(m_tCustomerID));
		tNetPacket.w_float			(m_fCost);
		tNetPacket.w				(&m_tTaskType,	sizeof(m_tTaskType));
		switch (m_tTaskType) {
			case eTaskTypeSearchForItemCL : {
				tNetPacket.w		(&m_tClassID,	sizeof(m_tClassID));
				tNetPacket.w		(&m_tLocationID,sizeof(m_tLocationID));
				break;
			}
			case eTaskTypeSearchForItemCG : {
				tNetPacket.w		(&m_tClassID,	sizeof(m_tClassID));
				tNetPacket.w		(&m_tGraphID,	sizeof(m_tGraphID));
				break;
			}
			case eTaskTypeSearchForItemOL : {
				tNetPacket.w		(&m_tObjectID,	sizeof(m_tObjectID));
				tNetPacket.w		(&m_tLocationID,sizeof(m_tLocationID));
				break;
			}
			case eTaskTypeSearchForItemOG : {
				tNetPacket.w		(&m_tObjectID,	sizeof(m_tObjectID));
				tNetPacket.w		(&m_tGraphID,	sizeof(m_tGraphID));
				break;
			}
			default : {
				break;
			}
		};
	};

	virtual void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		tNetPacket.r				(&m_tTaskID,	sizeof(m_tTaskID));
		tNetPacket.r				(&m_tTimeID,	sizeof(m_tTimeID));
		tNetPacket.r				(&m_tCustomerID,sizeof(m_tCustomerID));
		tNetPacket.r_float			(m_fCost);
		tNetPacket.r				(&m_tTaskType,	sizeof(m_tTaskType));
		switch (m_tTaskType) {
			case eTaskTypeSearchForItemCL : {
				tNetPacket.r		(&m_tClassID,	sizeof(m_tClassID));
				tNetPacket.r		(&m_tLocationID,sizeof(m_tLocationID));
				break;
			}
			case eTaskTypeSearchForItemCG : {
				tNetPacket.r		(&m_tClassID,	sizeof(m_tClassID));
				tNetPacket.r		(&m_tGraphID,	sizeof(m_tGraphID));
				break;
			}
			case eTaskTypeSearchForItemOL : {
				tNetPacket.r		(&m_tObjectID,	sizeof(m_tObjectID));
				tNetPacket.r		(&m_tLocationID,sizeof(m_tLocationID));
				break;
			}
			case eTaskTypeSearchForItemOG : {
				tNetPacket.r		(&m_tObjectID,	sizeof(m_tObjectID));
				tNetPacket.r		(&m_tGraphID,	sizeof(m_tGraphID));
				break;
			}
			default : {
				break;
			}
		};
	}
};

class CALifePersonalTask : public CALifeTask {
public:
	typedef CALifeTask inherited;
	u32								m_dwTryCount;

	CALifePersonalTask()
	{
		m_dwTryCount = 0;
	};
	
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		inherited::STATE_Write		(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		inherited::STATE_Read		(tNetPacket,size);
	}

	virtual void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Write		(tNetPacket);
		tNetPacket.w_u32			(m_dwTryCount);
	};

	virtual void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Read		(tNetPacket);
		tNetPacket.r_u32			(m_dwTryCount);
	};
};

class CALifeZone : public IPureServerEditorObject {
public:
	EAnomalousZoneType				m_tAnomalousZone;

	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
	}

	virtual void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		tNetPacket.w				(&m_tAnomalousZone,sizeof(m_tAnomalousZone));
	};
	
	virtual void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		tNetPacket.r				(&m_tAnomalousZone,sizeof(m_tAnomalousZone));
	};

	virtual void					Init(LPCSTR caSection)
	{
		m_tAnomalousZone			= EAnomalousZoneType(pSettings->ReadINT(caSection, "anomaly_type"));
	};

};

class CALifeDynamicObject : public CALifeObject {
public:
	typedef	CALifeObject inherited;
	
	_TIME_ID						m_tTimeID;
	
									CALifeDynamicObject()
	{
		m_tTimeID					= 0;
	}
	
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		inherited::STATE_Write		(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		inherited::STATE_Read		(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Write		(tNetPacket);
		tNetPacket.w				(&m_tTimeID,	sizeof(m_tTimeID));
	};
	
	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Read		(tNetPacket);
		tNetPacket.r				(&m_tTimeID,	sizeof(m_tTimeID));
	};
};

class CALifeItem : public CALifeDynamicObject {
public:
	typedef	CALifeDynamicObject inherited;
	
	float							m_fMass;
	u32								m_dwCost;
	s32								m_iHealthValue;
	bool							m_bAttached;
	
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		inherited::STATE_Write		(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		inherited::STATE_Read		(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Write		(tNetPacket);
		tNetPacket.w_float			(m_fMass);
		tNetPacket.w_u32			(m_dwCost);
		tNetPacket.w_u32			(m_iHealthValue);
		tNetPacket.w_u32			(m_bAttached);
	};
	
	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Read		(tNetPacket);
		tNetPacket.r_float			(m_fMass);
		tNetPacket.r_u32			(m_dwCost);
		tNetPacket.r_s32			(m_iHealthValue);
		u32							dwDummy;
		tNetPacket.r_u32			(dwDummy);
		m_bAttached					= !!dwDummy;
	};

	virtual void					Init(LPCSTR caSection)
	{
		inherited::Init				(caSection);
		m_fMass						= pSettings->ReadFLOAT(caSection, "ph_mass");
		m_dwCost					= pSettings->ReadINT(caSection, "cost");
		if (pSettings->LineExists	(caSection, "health_value"))
			m_iHealthValue			= pSettings->ReadINT(caSection, "health_value");
		else
			m_iHealthValue			= 0;
		m_bAttached					= false;
	};
};

class CALifeAnomalousZone : public CALifeDynamicObject, public CALifeZone {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		CALifeDynamicObject::STATE_Write(tNetPacket);
		CALifeZone::STATE_Write		(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		CALifeDynamicObject::STATE_Read(tNetPacket, size);
		CALifeZone::STATE_Read		(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		CALifeDynamicObject::UPDATE_Write(tNetPacket);
		CALifeZone::UPDATE_Write	(tNetPacket);
	};
	
	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		CALifeDynamicObject::UPDATE_Read(tNetPacket);
		CALifeZone::UPDATE_Read	(tNetPacket);
	};

	virtual void					Init(LPCSTR caSection)
	{
		CALifeDynamicObject::Init	(caSection);
		CALifeZone::Init			(caSection);
	};
};

class CALifeTrader : public CALifeDynamicObject, public CALifeTraderParams, public CALifeTraderAbstract {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		CALifeDynamicObject::STATE_Write(tNetPacket);
		CALifeTraderParams::STATE_Write(tNetPacket);
		CALifeTraderAbstract::STATE_Write(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		CALifeDynamicObject::STATE_Read(tNetPacket, size);
		CALifeTraderParams::STATE_Read(tNetPacket, size);
		CALifeTraderAbstract::STATE_Read(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		CALifeDynamicObject::UPDATE_Write(tNetPacket);
		CALifeTraderParams::UPDATE_Write(tNetPacket);
		CALifeTraderAbstract::UPDATE_Write(tNetPacket);
	};
	
	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		CALifeDynamicObject::UPDATE_Read(tNetPacket);
		CALifeTraderParams::UPDATE_Read(tNetPacket);
		CALifeTraderAbstract::UPDATE_Read(tNetPacket);
	};

	virtual void					Init(LPCSTR caSection)
	{
		CALifeDynamicObject::Init	(caSection);
		CALifeTraderParams::Init	(caSection);
		CALifeTraderAbstract::Init	(caSection);
	};
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
	
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		inherited::STATE_Write		(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		inherited::STATE_Read		(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Write		(tNetPacket);
		tNetPacket.w				(&m_tNextGraphID,			sizeof(m_tNextGraphID));
		tNetPacket.w				(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
		tNetPacket.w				(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
		tNetPacket.w				(&m_fCurSpeed,				sizeof(m_fCurSpeed));
		tNetPacket.w				(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
		tNetPacket.w				(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
	};
	
	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Read		(tNetPacket);
		tNetPacket.r				(&m_tNextGraphID,			sizeof(m_tNextGraphID));
		tNetPacket.r				(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
		tNetPacket.r				(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
		tNetPacket.r				(&m_fCurSpeed,				sizeof(m_fCurSpeed));
		tNetPacket.r				(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
		tNetPacket.r				(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
	};

	virtual void					Init(LPCSTR caSection)
	{
		inherited::Init				(caSection);
		m_tNextGraphID				= m_tGraphID;
		m_tPrevGraphID				= m_tGraphID;
		m_fGoingSpeed				= pSettings->ReadFLOAT	(caSection, "going_speed");
		m_fCurSpeed					= 0.0f;
		m_fDistanceFromPoint		= 0.0f;
		m_fDistanceToPoint			= 0.0f;
	};
};

class CALifeDynamicAnomalousZone : public CALifeMonsterAbstract, public CALifeZone {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		CALifeMonsterAbstract::STATE_Write(tNetPacket);
		CALifeZone::STATE_Write		(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		CALifeMonsterAbstract::STATE_Read(tNetPacket, size);
		CALifeZone::STATE_Read		(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		CALifeMonsterAbstract::UPDATE_Write(tNetPacket);
		CALifeZone::UPDATE_Write	(tNetPacket);
	};
	
	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		CALifeMonsterAbstract::UPDATE_Read(tNetPacket);
		CALifeZone::UPDATE_Read		(tNetPacket);
	};

	virtual void					Init(LPCSTR caSection)
	{
		CALifeMonsterAbstract::Init	(caSection);
		CALifeZone::Init			(caSection);
	};
};

class CALifeMonster : public CALifeMonsterAbstract, public CALifeMonsterParams {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		CALifeMonsterAbstract::STATE_Write(tNetPacket);
		CALifeMonsterParams::STATE_Write(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		CALifeMonsterAbstract::STATE_Read(tNetPacket, size);
		CALifeMonsterParams::STATE_Read(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		CALifeMonsterAbstract::UPDATE_Write(tNetPacket);
		CALifeMonsterParams::UPDATE_Write(tNetPacket);
	};
	
	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		CALifeMonsterAbstract::UPDATE_Read(tNetPacket);
		CALifeMonsterParams::UPDATE_Read(tNetPacket);
	};

	virtual void					Init(LPCSTR caSection)
	{
		CALifeMonsterAbstract::Init	(caSection);
		CALifeMonsterParams::Init	(caSection);
	};
};

class CALifeMonsterGroup : public CALifeMonsterAbstract {
public:
	typedef	CALifeMonsterAbstract inherited;
	
	MONSTER_PARAMS_P_VECTOR			m_tpMembers;

	virtual							~CALifeMonsterGroup()
	{
		free_vector					(m_tpMembers);
	};
	
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		inherited::STATE_Write		(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		inherited::STATE_Read		(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Write		(tNetPacket);
		save_vector					(m_tpMembers,tNetPacket);
	};
	
	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Read		(tNetPacket);
		load_vector					(m_tpMembers,tNetPacket);
	};

	virtual void					Init(LPCSTR caSection)
	{
		inherited::Init				(caSection);
		m_tpMembers.resize			(m_wCount);
		init_vector					(m_tpMembers,caSection);
	};
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
	
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		CALifeMonsterAbstract::STATE_Write(tNetPacket);
		CALifeTraderAbstract::STATE_Write(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		CALifeMonsterAbstract::STATE_Read(tNetPacket, size);
		CALifeTraderAbstract::STATE_Read(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		// calling inherited
		CALifeMonsterAbstract::UPDATE_Write(tNetPacket);
		CALifeTraderAbstract::UPDATE_Write(tNetPacket);
		save_base_vector			(m_tpaVertices,tNetPacket);
		save_bool_vector			(m_baVisitedVertices,tNetPacket);
		save_vector					(m_tpTasks,tNetPacket);
		tNetPacket.w				(&m_tTaskState,sizeof(m_tTaskState));
		tNetPacket.w_u32			(m_dwCurNode);
		tNetPacket.w_u32			(m_dwCurTaskLocation);
		tNetPacket.w_u32			(m_dwCurTask);
		tNetPacket.w_float			(m_fSearchSpeed);
	};

	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		// calling inherited
		CALifeMonsterAbstract::UPDATE_Read	(tNetPacket);
		CALifeTraderAbstract::UPDATE_Read	(tNetPacket);
		load_base_vector			(m_tpaVertices,tNetPacket);
		load_bool_vector			(m_baVisitedVertices,tNetPacket);
		load_vector					(m_tpTasks,tNetPacket);
		tNetPacket.r				(&m_tTaskState,sizeof(m_tTaskState));
		tNetPacket.r_u32			(m_dwCurNode);
		tNetPacket.r_u32			(m_dwCurTaskLocation);
		tNetPacket.r_u32			(m_dwCurTask);
		tNetPacket.r_float			(m_fSearchSpeed);
	};

	virtual void					Init(LPCSTR caSection)
	{
		CALifeMonsterAbstract::Init	(caSection);
		CALifeTraderAbstract::Init	(caSection);
		m_tpaVertices.clear			();
		m_baVisitedVertices.clear	();
		m_tpTasks.clear				();
		m_dwCurTask					= u32(-1);
		m_tTaskState				= eTaskStateNoTask;
		m_dwCurNode					= u32(-1);
		m_dwCurTaskLocation			= u32(-1);
		m_fSearchSpeed				= pSettings->ReadFLOAT				(caSection, "search_speed");
	};
};

class CALifeHuman : public CALifeHumanAbstract, public CALifeHumanParams {
public:
	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		CALifeHumanAbstract::STATE_Write(tNetPacket);
		CALifeHumanParams::STATE_Write(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		CALifeHumanAbstract::STATE_Read(tNetPacket, size);
		CALifeHumanParams::STATE_Read(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		CALifeHumanAbstract::UPDATE_Write(tNetPacket);
		CALifeHumanParams::UPDATE_Write(tNetPacket);
	};
	
	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		CALifeHumanAbstract::UPDATE_Read(tNetPacket);
		CALifeHumanParams::UPDATE_Read(tNetPacket);
	};

	virtual void					Init(LPCSTR caSection)
	{
		CALifeHumanAbstract::Init	(caSection);
		CALifeHumanParams::Init		(caSection);
	};
};

class CALifeHumanGroup : public CALifeHumanAbstract {
public:
	typedef	CALifeHumanAbstract inherited;

	HUMAN_PARAMS_P_VECTOR			m_tpMembers;

	virtual							~CALifeHumanGroup()
	{
		free_vector					(m_tpMembers);
	};

	virtual void					STATE_Write(NET_Packet &tNetPacket)
	{
		inherited::STATE_Write		(tNetPacket);
	}

	virtual void					STATE_Read(NET_Packet &tNetPacket, u16 size)
	{
		inherited::STATE_Read		(tNetPacket, size);
	}

	virtual	void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Write		(tNetPacket);
		save_vector					(m_tpMembers,tNetPacket);
	};
	
	virtual	void					UPDATE_Read(NET_Packet &tNetPacket)
	{
		inherited::UPDATE_Read		(tNetPacket);
		load_vector					(m_tpMembers,tNetPacket);
	};

	virtual void					Init(LPCSTR caSection)
	{
		inherited::Init				(caSection);
		m_tpMembers.resize			(m_wCount);
		init_vector					(m_tpMembers,caSection);
	};
};
