////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_server_objects.cpp
//	Created 	: 13.02.2003
//  Modified 	: 13.02.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life server objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "xrServer_Entities.h"

// CALifeObject
void CALifeObject::STATE_Write(NET_Packet &tNetPacket)
{
	tNetPacket.w_u8				(m_ucProbability);
	tNetPacket.w_u32			(m_dwSpawnGroup);
	tNetPacket.w				(&m_tGraphID,sizeof(m_tGraphID));
	tNetPacket.w_float			(m_fDistance);
	tNetPacket.w_u32			(m_bDirectControl);
	tNetPacket.w_u32			(m_tNodeID);
}

void CALifeObject::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	if (m_wVersion >= 1) {
		tNetPacket.r_u8			(m_ucProbability);
		tNetPacket.r_u32		(m_dwSpawnGroup);
		if (m_wVersion < 4) {
			u16					wDummy;
			tNetPacket.r_u16	(wDummy);
		}
		tNetPacket.r			(&m_tGraphID,sizeof(m_tGraphID));
		tNetPacket.r_float		(m_fDistance);
		m_tObjectID				= ID;
	}
	if (m_wVersion >= 4) {
		u32						dwDummy;
		tNetPacket.r_u32		(dwDummy);
		m_bDirectControl		= !!dwDummy;
	}
	if (m_wVersion >= 8)
		tNetPacket.r_u32			(m_tNodeID);
}

void CALifeObject::UPDATE_Write(NET_Packet &tNetPacket)
{
}

void CALifeObject::UPDATE_Read(NET_Packet &tNetPacket)
{
};

#ifdef _EDITOR
#include "PropertiesListHelper.h"
void CALifeObject::FillProp	(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref, items);
	PHelper.CreateU8			(items,	PHelper.PrepareKey(pref, "ALife\\Probability"),		&m_ucProbability,	0,100);
	PHelper.CreateU32			(items,	PHelper.PrepareKey(pref, "ALife\\Spawn group ID"),	&m_dwSpawnGroup,	0,0xffffffff);
}
#endif
// CALifeTraderParams
void CALifeTraderParams::STATE_Write(NET_Packet &tNetPacket)
{
}

void CALifeTraderParams::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
}

void CALifeTraderParams::UPDATE_Write(NET_Packet &tNetPacket)
{
	tNetPacket.w_float			(m_fCumulativeItemMass);
	tNetPacket.w_u32			(m_dwMoney);
	tNetPacket.w_u32			(m_tRank);
};

void CALifeTraderParams::UPDATE_Read(NET_Packet &tNetPacket)
{
	tNetPacket.r_float			(m_fCumulativeItemMass);
	tNetPacket.r_u32			(m_dwMoney);
	u32							dwDummy;
	tNetPacket.r_u32			(dwDummy);
	m_tRank						= EStalkerRank(m_tRank);
};

#ifdef _EDITOR
void CALifeTraderParams::FillProp(LPCSTR pref, PropItemVec& items)
{
}
#endif

// CALifeTraderAbstract
void CALifeTraderAbstract::STATE_Write(NET_Packet &tNetPacket)
{
}

void CALifeTraderAbstract::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
}

void CALifeTraderAbstract::UPDATE_Write(NET_Packet &tNetPacket)
{
	save_vector					(m_tpEvents,tNetPacket);
	save_base_vector			(m_tpTaskIDs,tNetPacket);
};

void CALifeTraderAbstract::UPDATE_Read(NET_Packet &tNetPacket)
{
	load_vector					(m_tpEvents,tNetPacket);
	load_base_vector			(m_tpTaskIDs,tNetPacket);
};

// CALifeEventGroup
void CALifeEventGroup::STATE_Write(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeEventGroup::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CALifeEventGroup::UPDATE_Write(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w				(&m_wCountAfter,sizeof(m_wCountAfter));
};

void CALifeEventGroup::UPDATE_Read(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	tNetPacket.r				(&m_wCountAfter,sizeof(m_wCountAfter));
};

// CALifeEvent
void CALifeEvent::STATE_Write(NET_Packet &tNetPacket)
{
}

void CALifeEvent::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
}

void CALifeEvent::UPDATE_Write(NET_Packet &tNetPacket)
{
	tNetPacket.w				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.w				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.w				(&m_tGraphID,		sizeof(m_tGraphID));
	tNetPacket.w				(&m_tBattleResult,	sizeof(m_tBattleResult));
	m_tpMonsterGroup1->UPDATE_Write(tNetPacket);
	m_tpMonsterGroup2->UPDATE_Write(tNetPacket);
};

void CALifeEvent::UPDATE_Read(NET_Packet &tNetPacket)
{
	tNetPacket.r				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.r				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.r				(&m_tGraphID,		sizeof(m_tGraphID));
	tNetPacket.r				(&m_tBattleResult,	sizeof(m_tBattleResult));
	m_tpMonsterGroup1->UPDATE_Read(tNetPacket);
	m_tpMonsterGroup2->UPDATE_Read(tNetPacket);
};

// CALifePersonalEvent
void CALifePersonalEvent::STATE_Write(NET_Packet &tNetPacket)
{
}

void CALifePersonalEvent::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
}

void CALifePersonalEvent::UPDATE_Write(NET_Packet &tNetPacket)
{
	tNetPacket.w				(&m_tEventID,			sizeof(m_tEventID));
	tNetPacket.w				(&m_tTimeID,			sizeof(m_tTimeID));
	tNetPacket.w				(&m_tTaskID,			sizeof(m_tTaskID));
	tNetPacket.w_u32			(m_iHealth);
	tNetPacket.w				(&m_tRelation,			sizeof(m_tRelation));
	save_base_vector			(m_tpItemIDs, tNetPacket);
};

void CALifePersonalEvent::UPDATE_Read(NET_Packet &tNetPacket)
{
	tNetPacket.r				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.r				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.r				(&m_tTaskID,		sizeof(m_tTaskID));
	tNetPacket.r_s32			(m_iHealth);
	tNetPacket.r				(&m_tRelation,		sizeof(m_tRelation));
	load_base_vector			(m_tpItemIDs, tNetPacket);
};

// CALifeTask
void CALifeTask::STATE_Write(NET_Packet &tNetPacket)
{
}

void CALifeTask::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
}

void CALifeTask::UPDATE_Write(NET_Packet &tNetPacket)
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

void CALifeTask::UPDATE_Read(NET_Packet &tNetPacket)
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

// CALifePersonalTask
void CALifePersonalTask::STATE_Write(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifePersonalTask::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CALifePersonalTask::UPDATE_Write(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w_u32			(m_dwTryCount);
};

void CALifePersonalTask::UPDATE_Read(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	tNetPacket.r_u32			(m_dwTryCount);
};

// CALifeZone
void CALifeZone::STATE_Write(NET_Packet &tNetPacket)
{
}

void CALifeZone::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
}

void CALifeZone::UPDATE_Write(NET_Packet &tNetPacket)
{
	tNetPacket.w				(&m_tAnomalousZone,sizeof(m_tAnomalousZone));
};

void CALifeZone::UPDATE_Read(NET_Packet &tNetPacket)
{
	tNetPacket.r				(&m_tAnomalousZone,sizeof(m_tAnomalousZone));
};

// CALifeDynamicObject
void CALifeDynamicObject::STATE_Write(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeDynamicObject::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CALifeDynamicObject::UPDATE_Write(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
};

void CALifeDynamicObject::UPDATE_Read(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
};

// CALifeItem
void CALifeItem::STATE_Write(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeItem::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CALifeItem::UPDATE_Write(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
};

void CALifeItem::UPDATE_Read(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
};

// CALifeAnomalousZone
void CALifeAnomalousZone::STATE_Write(NET_Packet &tNetPacket)
{
	CALifeDynamicObject::STATE_Write(tNetPacket);
	CALifeZone::STATE_Write		(tNetPacket);
}

void CALifeAnomalousZone::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	CALifeDynamicObject::STATE_Read(tNetPacket, size);
	CALifeZone::STATE_Read		(tNetPacket, size);
}

void CALifeAnomalousZone::UPDATE_Write(NET_Packet &tNetPacket)
{
	CALifeDynamicObject::UPDATE_Write(tNetPacket);
	CALifeZone::UPDATE_Write	(tNetPacket);
};

void CALifeAnomalousZone::UPDATE_Read(NET_Packet &tNetPacket)
{
	CALifeDynamicObject::UPDATE_Read(tNetPacket);
	CALifeZone::UPDATE_Read	(tNetPacket);
};

// CALifeTrader
void CALifeTrader::STATE_Write(NET_Packet &tNetPacket)
{
	CALifeDynamicObject::STATE_Write(tNetPacket);
	CALifeTraderAbstract::STATE_Write(tNetPacket);
}

void CALifeTrader::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	CALifeDynamicObject::STATE_Read(tNetPacket, size);
	CALifeTraderAbstract::STATE_Read(tNetPacket, size);
}

void CALifeTrader::UPDATE_Write(NET_Packet &tNetPacket)
{
	CALifeDynamicObject::UPDATE_Write(tNetPacket);
	CALifeTraderAbstract::UPDATE_Write(tNetPacket);
};

void CALifeTrader::UPDATE_Read(NET_Packet &tNetPacket)
{
	CALifeDynamicObject::UPDATE_Read(tNetPacket);
	CALifeTraderAbstract::UPDATE_Read(tNetPacket);
};

// CALifeDynamicAnomalousZone
void CALifeDynamicAnomalousZone::STATE_Write(NET_Packet &tNetPacket)
{
	CALifeMonsterAbstract::STATE_Write(tNetPacket);
	CALifeZone::STATE_Write		(tNetPacket);
}

void CALifeDynamicAnomalousZone::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	CALifeMonsterAbstract::STATE_Read(tNetPacket, size);
	CALifeZone::STATE_Read		(tNetPacket, size);
}

void CALifeDynamicAnomalousZone::UPDATE_Write(NET_Packet &tNetPacket)
{
	CALifeMonsterAbstract::UPDATE_Write(tNetPacket);
	CALifeZone::UPDATE_Write	(tNetPacket);
};

void CALifeDynamicAnomalousZone::UPDATE_Read(NET_Packet &tNetPacket)
{
	CALifeMonsterAbstract::UPDATE_Read(tNetPacket);
	CALifeZone::UPDATE_Read		(tNetPacket);
};

// CALifeMonster
void CALifeMonster::STATE_Write(NET_Packet &tNetPacket)
{
	CALifeMonsterAbstract::STATE_Write(tNetPacket);
}

void CALifeMonster::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	CALifeMonsterAbstract::STATE_Read(tNetPacket, size);
}

void CALifeMonster::UPDATE_Write(NET_Packet &tNetPacket)
{
	CALifeMonsterAbstract::UPDATE_Write(tNetPacket);
};

void CALifeMonster::UPDATE_Read(NET_Packet &tNetPacket)
{
	CALifeMonsterAbstract::UPDATE_Read(tNetPacket);
};

// CALifeHumanAbstract
void CALifeHumanAbstract::STATE_Write(NET_Packet &tNetPacket)
{
	CALifeMonsterAbstract::STATE_Write(tNetPacket);
	CALifeTraderAbstract::STATE_Write(tNetPacket);
}

void CALifeHumanAbstract::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	CALifeMonsterAbstract::STATE_Read(tNetPacket, size);
	CALifeTraderAbstract::STATE_Read(tNetPacket, size);
}

void CALifeHumanAbstract::UPDATE_Write(NET_Packet &tNetPacket)
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

void CALifeHumanAbstract::UPDATE_Read(NET_Packet &tNetPacket)
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

// CALifeHuman
void CALifeHuman::STATE_Write(NET_Packet &tNetPacket)
{
	CALifeHumanAbstract::STATE_Write(tNetPacket);
}

void CALifeHuman::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	CALifeHumanAbstract::STATE_Read(tNetPacket, size);
}

void CALifeHuman::UPDATE_Write(NET_Packet &tNetPacket)
{
	CALifeHumanAbstract::UPDATE_Write(tNetPacket);
};

void CALifeHuman::UPDATE_Read(NET_Packet &tNetPacket)
{
	CALifeHumanAbstract::UPDATE_Read(tNetPacket);
};