////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_console.cpp
//	Created 	: 09.01.2003
//  Modified 	: 09.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation - console commands support
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
void CAI_ALife::vfListObjects()
{
	OBJECT_PAIR_IT	it = m_tObjectRegistry.m_tppMap.begin();
	OBJECT_PAIR_IT	E  = m_tObjectRegistry.m_tppMap.end();
	string64		tString;
	Msg("%s->Listing objects :",cName());
	for (int i=0; it != E; it++, i++) {
		memcpy(tString,&((*it).second->m_tClassID),sizeof((*it).second->m_tClassID));
		tString[sizeof((*it).second->m_tClassID)] = 0;
		Msg("* %4d : %8s[ID=%4d][MDL=%10s][CNT=%3d][GID=%4d][UPD=%d]",i,tString,(*it).first,m_tpSpawnPoints[(*it).second->m_tSpawnID].caModel,(*it).second->m_wCount,(*it).second->m_tGraphID,(*it).second->m_tTimeID);
	}
	Msg("Total %d objects",i);
}

void CAI_ALife::vfListEvents()
{
	EVENT_PAIR_IT	it = m_tEventRegistry.m_tpMap.begin();
	EVENT_PAIR_IT	E  = m_tEventRegistry.m_tpMap.end();
	Msg("%s->Listing events :",cName());
	for (int i=0; it != E; it++, i++)
		Msg("* %4d : [ID=%4d][BR=%1d][GID=%4d][TIME=%d]",i,(*it).first,(*it).second.tBattleResult,(*it).second.tGraphID,(*it).second.tTimeID);
	Msg("Total %d events",i);
}

void CAI_ALife::vfListTasks()
{
	TASK_PAIR_IT	it = m_tTaskRegistry.m_tpMap.begin();
	TASK_PAIR_IT	E  = m_tTaskRegistry.m_tpMap.end();
	Msg("%s->Listing tasks :",cName());
	for (int i=0; it != E; it++, i++)
		Msg("* %4d : [ID=%4d][CID=%1d][TT=][GID=%4d][UPD=%d]",i,(*it).first,(*it).second.tCustomerID,(*it).second.tTaskType,(*it).second.tGraphID,(*it).second.tTimeID);
	Msg("Total %d tasks",i);
}

void CAI_ALife::vfListLocations()
{
	TASK_PAIR_IT	it = m_tTaskRegistry.m_tpMap.begin();
	TASK_PAIR_IT	E  = m_tTaskRegistry.m_tpMap.end();
	Msg("%s->Listing location owners :",cName());
	for (int i=0; it != E; it++, i++)
		Msg("* %4d : [ID=%4d][CID=%1d][TT=][GID=%4d][UPD=%d]",i,(*it).first,(*it).second.tCustomerID,(*it).second.tTaskType,(*it).second.tGraphID,(*it).second.tTimeID);
	Msg("Total %d location owners",i);
}

void CAI_ALife::vfListTerrain()
{
	TASK_PAIR_IT	it = m_tTaskRegistry.m_tpMap.begin();
	TASK_PAIR_IT	E  = m_tTaskRegistry.m_tpMap.end();
	Msg("%s->Listing terrain :",cName());
	for (int i=0; it != E; it++, i++)
		Msg("* %4d : [ID=%4d][CID=%1d][TT=][GID=%4d][UPD=%d]",i,(*it).first,(*it).second.tCustomerID,(*it).second.tTaskType,(*it).second.tGraphID,(*it).second.tTimeID);
	Msg("Total %d terrain",i);
}

void CAI_ALife::vfListSpawnPoints()
{
	TASK_PAIR_IT	it = m_tTaskRegistry.m_tpMap.begin();
	TASK_PAIR_IT	E  = m_tTaskRegistry.m_tpMap.end();
	Msg("%s->Listing spawn points :",cName());
	for (int i=0; it != E; it++, i++)
		Msg("* %4d : [ID=%4d][CID=%1d][TT=][GID=%4d][UPD=%d]",i,(*it).first,(*it).second.tCustomerID,(*it).second.tTaskType,(*it).second.tGraphID,(*it).second.tTimeID);
	Msg("Total %d spawn points",i);
}

void CAI_ALife::vfObjectInfo(_OBJECT_ID	&tObjectID)
{
	OBJECT_PAIR_IT	it = m_tObjectRegistry.m_tppMap.find(tObjectID);
	Msg("%s->Object information :",cName());
	if (it == m_tObjectRegistry.m_tppMap.end()) {
		Msg("Object not found! (ID = %d)",tObjectID);
		return;
	}
	CALifeDynamicObject	*tpALifeDynamicObject = (*it).second;
	string64		tString;
	memcpy			(tString,&(tpALifeDynamicObject->m_tClassID),sizeof(tpALifeDynamicObject->m_tClassID));
	tString[sizeof(tpALifeDynamicObject->m_tClassID)] = 0;
	Msg("* Class ID      : %s[%I64u]",tString,tpALifeDynamicObject->m_tClassID);
	Msg("* ObjectID      : %d",tpALifeDynamicObject->m_tObjectID);
	Msg("* Spawn ID      : %d",tpALifeDynamicObject->m_tSpawnID);
	Msg("* Graph ID      : %d",tpALifeDynamicObject->m_tGraphID);
	Msg("* Count         : %d",tpALifeDynamicObject->m_wCount);
	Msg("* Update        : %d",tpALifeDynamicObject->m_tTimeID);
	
	CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(tpALifeDynamicObject);
	if (tpALifeItem) {
		Msg("* Mass          : %7.2f",tpALifeItem->m_fMass);
		Msg("* Price         : %7.2f",tpALifeItem->m_fPrice);
		return;
	}
	
	CALifeMonster *tpALifeMonster = dynamic_cast<CALifeMonster *>(tpALifeDynamicObject);
	VERIFY(tpALifeMonster);
	Msg("* Graph ID next : %d",tpALifeMonster->m_tNextGraphID);
	Msg("* Graph ID prev : %d",tpALifeMonster->m_tPrevGraphID);
	Msg("* Current speed : %7.2f",tpALifeMonster->m_fCurSpeed);
	Msg("* Minimum speed : %7.2f",tpALifeMonster->m_fMinSpeed);
	Msg("* Maximum speed : %7.2f",tpALifeMonster->m_fMaxSpeed);
	Msg("* Distance from : %7.2f",tpALifeMonster->m_fDistanceFromPoint);
	Msg("* Distance to   : %7.2f",tpALifeMonster->m_fDistanceToPoint);
	Msg("* Health        : %d",tpALifeMonster->m_iHealth);

	CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeMonster);
	if (tpALifeHuman) {
		Msg("* Item mass     : %7.2f",tpALifeHuman->m_fItemMass);
		Msg("* Max item mass : %7.2f",tpALifeHuman->m_fMaxItemMass);
		Msg("* Items         :");
		{
			OBJECT_IT it			= tpALifeHuman->m_tpItemIDs.begin();
			OBJECT_IT E				= tpALifeHuman->m_tpItemIDs.end();
			for (int i=0; it != E; it++, i++)
				Msg("* Item ID [%d]  : %d",i,*it);
		}
		Msg("* Events        :");
		{
			PERSONAL_EVENT_IT it	= tpALifeHuman->m_tpEvents.begin();
			PERSONAL_EVENT_IT E		= tpALifeHuman->m_tpEvents.end();
			for (int i=0; it != E; it++, i++)
				Msg("* Event ID [%d] : %d",i,*it);
		}
		Msg("* Tasks        :");
		{
			TASK_IT it				= tpALifeHuman->m_tpTaskIDs.begin();
			TASK_IT E				= tpALifeHuman->m_tpTaskIDs.end();
			for (int i=0; it != E; it++, i++)
				Msg("* Task ID [%d] : %d",i,*it);
		}
	}
}

void CAI_ALife::vfEventInfo(_EVENT_ID &tEventID)
{
}

void CAI_ALife::vfTaskInfo(_TASK_ID &tTaskID)
{
}
#endif