////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_console.cpp
//	Created 	: 09.01.2003
//  Modified 	: 09.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation - console commands support
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_objects.h"

#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
#include "ai_alife.h"
#include "ai_space.h"
using namespace AI;

#define NORMALIZE_VARIABLE(a,b,c,d) a = u32(b % c) + d, b /= c;

IC void vfPrintTime(char *S, _TIME_ID tTimeID)
{
	u32 Milliseconds,Seconds,Minutes,Hours,Days,Week,Months,Years;
	NORMALIZE_VARIABLE(Milliseconds,tTimeID,1000,0);
	NORMALIZE_VARIABLE(Seconds,		tTimeID,  60,0);
	NORMALIZE_VARIABLE(Minutes,		tTimeID,  60,0);
	NORMALIZE_VARIABLE(Hours,		tTimeID,  24,0);
	NORMALIZE_VARIABLE(Days,		tTimeID,   7,1);
	NORMALIZE_VARIABLE(Week,		tTimeID,   4,1);
	NORMALIZE_VARIABLE(Months,		tTimeID,  12,1);
	Years = u32(tTimeID) + 1;
	Msg("%s year %d month %d week %d day %d time %d:%d:%d.%d",S,Years,Months,Week,Days,Hours,Minutes,Seconds,Milliseconds);
}

void vfPrintLargeString(const char *S1, const char *S, const int i, const int j, const u32 u)
{
	string128	S2;
	if (strlen(S))
		for (int k=0, l=(int)strlen(S), m=((l - 1)/u) + 1; k<m; k++) {
			Memory.mem_copy(S2,S + k*u,u*sizeof(char));
			if (k == m - 1)
				S2[l - k*u]=0;
			else
				S2[u] = 0;
			if (!k)
				if (m > 1)
					Msg("* %s %d : %d[\n* %s",S1,i,j,S2);
				else
					Msg("* %s %d : %d[%s]",S1,i,j,S2);
			else
				if (k == m - 1)
					Msg("* %s]",S2);
				else
					Msg("* %s",S2);
		}
	else
		Msg("* %s %d : %d[%s]",S1,i,j,S);
}

void vfPrintLargeString(const char *S1, const char *S, const int j, const u32 u)
{
	string128	S2;
	if (strlen(S))
		for (int k=0, l=(int)strlen(S), m=((l - 1)/u) + 1; k<m; k++) {
			Memory.mem_copy(S2,S + k*u,u*sizeof(char));
			if (k == m - 1)
				S2[l - k*u]=0;
			else
				S2[u] = 0;
			if (!k)
				if (m > 1)
					Msg("* %s[%d] : \n* %s",S1,j,S2);
				else
					Msg("* %s[%d] : %s",S1,j,S2);
			else
				Msg("* %s",S2);
		}
	else
		Msg("* %s[%d] : %s",S1,j,S);
}

void CSE_ALifeSimulator::vfListObjects()
{
	OBJECT_PAIR_IT	I = m_tObjectRegistry.begin();
	OBJECT_PAIR_IT	E = m_tObjectRegistry.end();
	string64		tString;
	Msg("%s->Listing objects :",cName());
	for (int i=0; I != E; I++, i++) {
		Memory.mem_copy(tString,&((*I).second->m_tClassID),sizeof((*I).second->m_tClassID));
		tString[sizeof((*I).second->m_tClassID)] = 0;
		Msg("* %4d : %8s[ID=%4d][MDL=%10s][GID=%4d][UPD=%d]",i,tString,(*I).first,(*I).second->s_name,(*I).second->m_tGraphID,(*I).second->m_tTimeID);
	}
	Msg("Total %d objects",i);
}

void CSE_ALifeSimulator::vfListEvents()
{
	EVENT_PAIR_IT	I = m_tEventRegistry.begin();
	EVENT_PAIR_IT	E = m_tEventRegistry.end();
	Msg("%s->Listing events :",cName());
	for (int i=0; I != E; I++, i++)
		Msg("* %4d : [ID=%4d][BR=%1d][GID=%4d][TIME=%d]",i,(*I).first,(*I).second->m_tBattleResult,(*I).second->m_tGraphID,(*I).second->m_tTimeID);
	Msg("Total %d events",i);
}

void CSE_ALifeSimulator::vfListTasks()
{
	TASK_PAIR_IT	I = m_tTaskRegistry.begin();
	TASK_PAIR_IT	E = m_tTaskRegistry.end();
	Msg("%s->Listing tasks :",cName());
	for (int i=0; I != E; I++, i++)
		Msg("* %4d : [ID=%4d][CID=%1d][TT=][GID=%4d][UPD=%d]",i,(*I).first,(*I).second->m_tCustomerID,(*I).second->m_tTaskType,(*I).second->m_tGraphID,(*I).second->m_tTimeID);
	Msg("Total %d tasks",i);
}

void CSE_ALifeSimulator::vfListTerrain()
{
	Msg("%s->Listing terrain locations :",cName());
	char *S = (char *)xr_malloc(128*1024*sizeof(char));
	for (int j=0; j<LOCATION_TYPE_COUNT; j++) {
		GRAPH_VECTOR_IT	I = m_tpTerrain[j].begin();
		GRAPH_VECTOR_IT	E = m_tpTerrain[j].end();
		for (int i=0; I != E; I++, i++) {
			GRAPH_IT	it1 = (*I).begin();
			GRAPH_IT	E1  = (*I).end();
			int			j;
			S[0]		= 0;
			string16	S1;
			for ( j=0; it1 != E1; it1++, j++) {
				if (j)
					strcat(S,",");
				strcat(S,itoa(*it1,S1,10));
			}

			if (j)
				vfPrintLargeString("Terrain location ID",S,i,j,105);
		}
		xr_free(S);
		Msg("Total %d terrain locations",i);
	}
}

void CSE_ALifeSimulator::vfListSpawnPoints()
{
//	SPAWN_P_IT	I = m_tpSpawnPoints.begin();
//	SPAWN_P_IT	E = m_tpSpawnPoints.end();
//	Msg("%s->Listing spawn points :",cName());
//	for (int i=0; I != E; I++, i++) {
//		CSE_ALifeCreatureSpawnPoint *tpALifeCreatureSpawnPoint = dynamic_cast<CSE_ALifeCreatureSpawnPoint *>(*I);
//		if (tpALifeCreatureSpawnPoint)
//			Msg("* %4d : [MDL=%10s][GID=%d]",i,(*I)->m_caModel,tpALifeCreatureSpawnPoint->m_wGroupID);
//		else
//			Msg("* %4d : [MDL=%10s]",i,(*I)->m_caModel);
//	}
//	Msg("Total %d spawn points",i);
}

void CSE_ALifeSimulator::vfObjectInfo(_OBJECT_ID	&tObjectID)
{
//	string4096		S;
//	string16		S1;
//	OBJECT_PAIR_IT	I = m_tObjectRegistry.find(tObjectID);
//	Msg("%s->Object information :",cName());
//	if (I == m_tObjectRegistry.end()) {
//		Msg("Object not found! (ID = %d)",tObjectID);
//		return;
//	}
//	
//	CSE_ALifeDynamicObject	*tpALifeDynamicObject = (*I).second;
//	string64		tString;
//	Memory.mem_copy	(tString,&(tpALifeDynamicObject->m_caSection),sizeof(tpALifeDynamicObject->m_caSection));
//	tString[sizeof(tpALifeDynamicObject->m_caSection)] = 0;
//	Msg("* Class ID      : %s[%I64u]",tString,tpALifeDynamicObject->m_caSection);
//	Msg("* ObjectID      : %d",tpALifeDynamicObject->ID);
//	Msg("* Spawn ID      : %d",tpALifeDynamicObject->m_tSpawnID);
//	Msg("* Graph ID      : %d",tpALifeDynamicObject->m_tGraphID);
//	Msg("* Count         : %d",tpALifeDynamicObject->m_wCount);
//	vfPrintTime("* Update        :",tpALifeDynamicObject->m_tTimeID);
//	//Msg("* Update        : %d",tpALifeDynamicObject->m_tTimeID);
//	
//	CSE_ALifeItem *tpALifeItem = dynamic_cast<CSE_ALifeItem *>(tpALifeDynamicObject);
//	if (tpALifeItem) {
//		Msg("* Mass          : %7.2f",tpALifeItem->m_fMass);
//		Msg("* Price         : %d",tpALifeItem->m_dwCost);
//		return;
//	}
//	
//	CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract *>(tpALifeDynamicObject);
//	if (tpALifeMonsterAbstract) {
//		Msg("* Graph ID next : %d",tpALifeMonsterAbstract->m_tNextGraphID);
//		Msg("* Graph ID prev : %d",tpALifeMonsterAbstract->m_tPrevGraphID);
//		Msg("* Current speed : %7.2f",tpALifeMonsterAbstract->m_fCurSpeed);
//		Msg("* Going speed   : %7.2f",tpALifeMonsterAbstract->m_fGoingSpeed);
//		Msg("* Distance from : %7.2f",tpALifeMonsterAbstract->m_fDistanceFromPoint);
//		Msg("* Distance to   : %7.2f",tpALifeMonsterAbstract->m_fDistanceToPoint);
//	}
//	
//	CSE_ALifeMonsterParams *tpALifeMonsterParams = dynamic_cast<CSE_ALifeMonsterParams *>(tpALifeDynamicObject);
//	if (tpALifeMonsterParams) {
//		Msg("* Health        : %d",tpALifeMonsterParams->m_iHealth);
//	}
//		
//	CSE_ALifeMonsterGroup *tpALifeMonsterGroup = dynamic_cast<CSE_ALifeMonsterGroup *>(tpALifeDynamicObject);
//	if (tpALifeMonsterGroup) {
//		MONSTER_PARAMS_P_IT		I = tpALifeMonsterGroup->m_tpMembers.begin();
//		MONSTER_PARAMS_P_IT		E = tpALifeMonsterGroup->m_tpMembers.end();
//		for (int i=0; I != E; I++, i++)
//			Msg("* Member ID [%d]: %d",i,(*I)->m_iHealth);
//		return;
//	}
//	
//	CSE_ALifeTraderParams *tpALifeTraderParams = dynamic_cast<CSE_ALifeTraderParams *>(tpALifeDynamicObject);
//	if (tpALifeTraderParams) {
//		Msg("* Rank          : %d",tpALifeTraderParams->m_tRank);
//		Msg("* Money         : %d",tpALifeTraderParams->m_dwMoney);
//		Msg("* Item mass     : %7.2f",tpALifeTraderParams->m_fCumulativeItemMass);
//		{
//			OBJECT_IT I			= tpALifeTraderParams->m_tpItemIDs.begin();
//			OBJECT_IT E			= tpALifeTraderParams->m_tpItemIDs.end();
//			S[0] = 0;
//			for (int j=0; I != E; I++, j++) {
//				if (j)
//					strcat(S,",");
//				strcat(S,itoa(*I,S1,10));
//			}
//			vfPrintLargeString("* Items",S,E - tpALifeTraderParams->m_tpItemIDs.begin(),105);
//		}
//	}
//		
//	CSE_ALifeTraderAbstract *tpALifeTraderAbstract = dynamic_cast<CSE_ALifeTraderAbstract *>(tpALifeDynamicObject);
//	if (tpALifeTraderAbstract) {
//		{
//			PERSONAL_EVENT_P_IT	I = tpALifeTraderAbstract->m_tpEvents.begin();
//			PERSONAL_EVENT_P_IT	E = tpALifeTraderAbstract->m_tpEvents.end();
//			S[0] = 0;
//			for (int j=0; I != E; I++, j++) {
//				if (j)
//					strcat(S,",");
//				strcat(S,itoa((*I)->m_tEventID,S1,10));
//			}
//			vfPrintLargeString("* Events",S,E - tpALifeTraderAbstract->m_tpEvents.begin(),105);
//		}
//		{
//			TASK_IT				I = tpALifeTraderAbstract->m_tpTaskIDs.begin();
//			TASK_IT				E = tpALifeTraderAbstract->m_tpTaskIDs.end();
//			S[0] = 0;
//			for (int j=0; I != E; I++, j++) {
//				if (j)
//					strcat(S,",");
//				strcat(S,itoa((*I),S1,10));
//			}
//			vfPrintLargeString("* Tasks",S,E - tpALifeTraderAbstract->m_tpTaskIDs.begin(),105);
//		}
//		//
//		Msg("* Max item mass : %7.2f",tpALifeTraderAbstract->m_fMaxItemMass);
//	}
//		
//	CSE_ALifeHumanAbstract *tpALifeHumanAbstract = dynamic_cast<CSE_ALifeHumanAbstract *>(tpALifeDynamicObject);
//	if (tpALifeHumanAbstract) {
//		Msg("* Search speed  : %7.2f",tpALifeHumanAbstract->m_fSearchSpeed);
//		Msg("* Task state    : %7.2f",tpALifeHumanAbstract->m_tTaskState);
//		Msg("* Current node  : %7.2f",tpALifeHumanAbstract->m_dwCurNode);
//		Msg("* Current point : %7.2f",tpALifeHumanAbstract->m_dwCurTaskLocation);
//		Msg("* Current task  :");
//		if (tpALifeHumanAbstract->m_tpTaskIDs.size()) {
//			CSE_ALifePersonalTask	&tTask = *(tpALifeHumanAbstract->m_tpTasks[tpALifeHumanAbstract->m_dwCurTask]);
//			Msg("* Task  ID    : %d",tTask.m_tTaskID);
//			vfPrintTime("* Time  ID  :",tTask.m_tTimeID);
//			Msg("* Customer ID : %d",tTask.m_tCustomerID);
//			Msg("* Task type   : %d",tTask.m_tTaskType);
//			if (tTask.m_tTaskType == eTaskTypeSearchForItemOG) {
//				Msg("* Graph ID    : %d",tTask.m_tGraphID);
//				Msg("* Object ID   : %d",tTask.ID);
//			}
//			else if (tTask.m_tTaskType == eTaskTypeSearchForItemOL) {
//				Msg("* Location ID : %d",tTask.m_tLocationID);
//				Msg("* Object ID   : %d",tTask.ID);
//			}
//			else if (tTask.m_tTaskType == eTaskTypeSearchForItemCG) {
//				string64 tString;
//				Memory.mem_copy(tString,&(tTask.m_caSection),sizeof(tTask.m_caSection));
//				tString[sizeof(tTask.m_caSection)] = 0;
//				Msg("* Graph ID    : %d",tTask.m_tGraphID);
//				Msg("* Class ID    : %d (%s)",tTask.m_caSection,tString);
//			}
//			else if (tTask.m_tTaskType == eTaskTypeSearchForItemCL) {
//				string64 tString;
//				Memory.mem_copy(tString,&(tTask.m_caSection),sizeof(tTask.m_caSection));
//				tString[sizeof(tTask.m_caSection)] = 0;
//				Msg("* Location ID : %d",tTask.m_tLocationID);
//				Msg("* Class ID    : %d (%s)",tTask.m_caSection,tString);
//			}
//		}
//		{
//			PERSONAL_TASK_P_IT	I = tpALifeHumanAbstract->m_tpTasks.begin();
//			PERSONAL_TASK_P_IT	E = tpALifeHumanAbstract->m_tpTasks.end();
//			for (int j=0; I != E; I++, j++) {
//				Msg("* Task  ID    : %d",(*I)->m_tTaskID);
//				vfPrintTime(S,(*I)->m_tTimeID);
//				Msg("* Cost hope   : %7.2f",(*I)->m_fCost);
//				Msg("* Try count   : %d",(*I)->m_dwTryCount);
//			}
//		}
//	}
//	CSE_ALifeHumanAbstract *tpALifeHuman = dynamic_cast<CSE_ALifeHumanAbstract *>(tpALifeDynamicObject);
//	if (tpALifeHuman) {
//		{
//			TASK_IT				I = tpALifeHuman->m_tpTaskIDs.begin();
//			TASK_IT				E = tpALifeHuman->m_tpTaskIDs.end();
//			S[0] = 0;
//			for (int j=0; I != E; I++, j++) {
//				if (j)
//					strcat(S,",");
//				strcat(S,itoa(*I,S1,10));
//			}
//			vfPrintLargeString("Tasks",S,E - tpALifeHuman->m_tpTaskIDs.begin(),105);
//		}
//		return;
//	}
//	
//	CSE_ALifeHumanGroup *tpALifeHumanGroup = dynamic_cast<CSE_ALifeHumanGroup *>(tpALifeDynamicObject);
//	if (tpALifeHumanGroup) {
//		Msg("* Members       :");
//		{
//			HUMAN_PARAMS_P_IT		I = tpALifeHumanGroup->m_tpMembers.begin();
//			HUMAN_PARAMS_P_IT		E = tpALifeHumanGroup->m_tpMembers.begin();
//			for (int i=0; I != E; I++, i++) {
//				Msg("* Member ID [%d]: %d",i,*I);
//				Msg("* Health        : %d",(*I)->m_iHealth);
//				Msg("* Money         : %d",(*I)->m_dwMoney);
//				Msg("* Item mass     : %7.2f",(*I)->m_fCumulativeItemMass);
//				{
//					OBJECT_IT I			= tpALifeHuman->m_tpItemIDs.begin();
//					OBJECT_IT E			= tpALifeHuman->m_tpItemIDs.end();
//					S[0] = 0;
//					for (int j=0; I != E; I++, j++) {
//						if (j)
//							strcat(S,",");
//						strcat(S,itoa(*I,S1,10));
//					}
//					vfPrintLargeString("Items",S,E - tpALifeHuman->m_tpItemIDs.begin(),105);
//				}
//			}
//		}
//	}
}

void CSE_ALifeSimulator::vfEventInfo(_EVENT_ID &tEventID)
{
	EVENT_PAIR_IT I = m_tEventRegistry.find(tEventID);
	if (I == m_tEventRegistry.end()) {
		Msg("* Invalid event ID! (%d)",tEventID);
		return;
	}
	CSE_ALifeEvent &tEvent = *((*I).second);
	Msg("* Event information");
	Msg("* Event ID  : %d",tEvent.m_tEventID);
	Msg("* Graph ID  : %d",tEvent.m_tGraphID);
	vfPrintTime("* Time  ID  :",tEvent.m_tTimeID);
	Msg("* Battle    : %d",tEvent.m_tBattleResult);
	Msg("* Monster 1 :");
	CSE_ALifeEventGroup *tpMG = tEvent.m_tpMonsterGroup1;
	Msg("*     Class  ID    : %d",tpMG->m_tClassID);
	Msg("*     Object ID    : %d",tpMG->ID);
//	Msg("*     Spawn  ID    : %d",tpMG->m_tSpawnID);
	Msg("*     Count before : %d",tpMG->m_wCountBefore);
	Msg("*     Count after  : %d",tpMG->m_wCountAfter);
	Msg("* Monster 2 :");
	tpMG = tEvent.m_tpMonsterGroup2;
	Msg("*     Class  ID    : %d",tpMG->m_tClassID);
	Msg("*     Object ID    : %d",tpMG->ID);
//	Msg("*     Spawn  ID    : %d",tpMG->m_tSpawnID);
	Msg("*     Count before : %d",tpMG->m_wCountBefore);
	Msg("*     Count after  : %d",tpMG->m_wCountAfter);
	{
		string4096	S;
		string16	S1;
		S[0] = 0;
		OBJECT_PAIR_IT I = m_tObjectRegistry.begin();
		OBJECT_PAIR_IT E = m_tObjectRegistry.end();
		for (int j=0; I != E; I++) {
			CSE_ALifeHumanAbstract *tpALifeHuman = dynamic_cast<CSE_ALifeHumanAbstract *>((*I).second);
			if (!tpALifeHuman)
				continue;
			PERSONAL_EVENT_P_IT i = tpALifeHuman->m_tpEvents.begin();
			PERSONAL_EVENT_P_IT e = tpALifeHuman->m_tpEvents.end();
			for ( ; i != e; i++)
				if ((*i)->m_tEventID == tEventID) {
					if (j++)
						strcat(S,",");
					strcat(S,itoa((*I).first,S1,10));
					break;
				}
		}
		vfPrintLargeString(S,"Monsters, who know about the event",j,105);
	}
}

void CSE_ALifeSimulator::vfTaskInfo(_TASK_ID &tTaskID)
{
	TASK_PAIR_IT I = m_tTaskRegistry.find(tTaskID);
	if (I == m_tTaskRegistry.end()) {
		Msg("* Invalid task ID! (%d)",tTaskID);
		return;
	}
	CSE_ALifeTask &tTask = *((*I).second);
	Msg("* Task information");
	Msg("* Task  ID    : %d",tTask.m_tTaskID);
	vfPrintTime("* Time  ID  :",tTask.m_tTimeID);
//	Msg("* Time  ID    : %d",tTask.m_tTimeID);
	Msg("* Customer ID : %d",tTask.m_tCustomerID);
	Msg("* Task type   : %d",tTask.m_tTaskType);
	if (tTask.m_tTaskType == eTaskTypeSearchForItemOG) {
		Msg("* Graph ID    : %d",tTask.m_tGraphID);
		Msg("* Object ID   : %d",tTask.m_tObjectID);
	}
	else if (tTask.m_tTaskType == eTaskTypeSearchForItemOL) {
		Msg("* Location ID : %d",tTask.m_tLocationID);
		Msg("* Object ID   : %d",tTask.m_tObjectID);
	}
	else if (tTask.m_tTaskType == eTaskTypeSearchForItemCG) {
		string64 tString;
		Memory.mem_copy(tString,&(tTask.m_caSection),sizeof(tTask.m_caSection));
		tString[sizeof(tTask.m_caSection)] = 0;
		Msg("* Graph ID    : %d",tTask.m_tGraphID);
		Msg("* Class ID    : %d (%s)",tTask.m_caSection,tString);
	}
	else if (tTask.m_tTaskType == eTaskTypeSearchForItemCL) {
		string64 tString;
		Memory.mem_copy(tString,&(tTask.m_caSection),sizeof(tTask.m_caSection));
		tString[sizeof(tTask.m_caSection)] = 0;
		Msg("* Location ID : %d",tTask.m_tLocationID);
		Msg("* Class ID    : %d (%s)",tTask.m_caSection,tString);
	}
	string4096	S;
	string16	S1;
	S[0] = 0;
	{
		OBJECT_PAIR_IT I = m_tObjectRegistry.begin();
		OBJECT_PAIR_IT E = m_tObjectRegistry.end();
		for (int j=0; I != E; I++) {
			CSE_ALifeHumanAbstract *tpALifeHuman = dynamic_cast<CSE_ALifeHumanAbstract *>((*I).second);
			if (!tpALifeHuman)
				continue;
			TASK_IT i = tpALifeHuman->m_tpTaskIDs.begin();
			TASK_IT e = tpALifeHuman->m_tpTaskIDs.end();
			for ( ; i != e; i++)
				if ((*i) == tTaskID) {
					if (j++)
						strcat(S,",");
					strcat(S,itoa((*I).first,S1,10));
					break;
				}
		}
		vfPrintLargeString(S,"Monsters, who know about the task",j,105);
	}
}

void CSE_ALifeSimulator::vfSpawnPointInfo(_SPAWN_ID &tSpawnID)
{
//	CSE_ALifeSpawnPoint &tSpawnPoint = *(m_tpSpawnPoints[tSpawnID]);
//	Msg("%s->Spawn-point information :",cName());
//	Msg("* Model                   : %s",tSpawnPoint.m_caModel);
//	Msg("* GraphID                 : %s",tSpawnPoint.m_tNearestGraphPointID);
//	Msg("* Position                : [%7.2f][%7.2f][%7.2f]",tSpawnPoint.m_tPosition.x,tSpawnPoint.m_tPosition.y,tSpawnPoint.m_tPosition.z);
//	
//	CSE_ALifeZoneSpawnPoint *tpZoneSpawnPoint = dynamic_cast<CSE_ALifeZoneSpawnPoint *>(&tSpawnPoint);
//	if (tpZoneSpawnPoint) {
//		Msg("* Zone type               : %d",tpZoneSpawnPoint->m_tAnomalousZoneType);
//	}
//	CSE_ALifeCreatureSpawnPoint *tpCreatureSpawnPoint = dynamic_cast<CSE_ALifeCreatureSpawnPoint *>(&tSpawnPoint);
//	if (tpCreatureSpawnPoint) {
//		Msg("* Team                    : %d",tpCreatureSpawnPoint->m_ucTeam);
//		Msg("* Squad                   : %d",tpCreatureSpawnPoint->m_ucSquad);
//		Msg("* Group                   : %d",tpCreatureSpawnPoint->m_ucGroup);
//		Msg("* Group ID	               : %d",tpCreatureSpawnPoint->m_wGroupID);
//		Msg("* Count	               : %d",tpCreatureSpawnPoint->m_wCount);
//		Msg("* BirthRadius	           : %6.2f",tpCreatureSpawnPoint->m_fBirthRadius);
//		Msg("* BirthProbability	       : %6.2f",tpCreatureSpawnPoint->m_fBirthProbability);
//		Msg("* IncreaseCoefficient     : %6.2f",tpCreatureSpawnPoint->m_fIncreaseCoefficient);
//		
//		GRAPH_IT	I = tpCreatureSpawnPoint->m_tpRouteGraphPoints.begin();
//		GRAPH_IT	E = tpCreatureSpawnPoint->m_tpRouteGraphPoints.end();
//		string4096		S;
//		string16		S1;
//		S[0] = 0;
//		for (int j=0; I != E; I++, j++) {
//			if (j)
//				strcat(S,",");
//			strcat(S,itoa(*I,S1,10));
//		}
//		vfPrintLargeString(S,"RouteGraphPoints",tpCreatureSpawnPoint->m_tpRouteGraphPoints.size(),105);
//	}
}

void CSE_ALifeSimulator::vfGraphVertexInfo(_GRAPH_ID &tGraphID)
{
	CSE_ALifeGraph::SGraphVertex &tGraphVertex = getAI().m_tpaGraph[tGraphID];
	Msg("%s->Graph vertex information :",cName());
	Msg("* Level point                : [%7.2f][%7.2f][%7.2f]",tGraphVertex.tGlobalPoint.x,tGraphVertex.tGlobalPoint.y,tGraphVertex.tGlobalPoint.z);
//	Msg("* Location ID                : %d",tGraphVertex.tVertexType);
	Msg("* Neighbours                 :");
	CSE_ALifeGraph::SGraphEdge	*tpaEdges = (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + tGraphVertex.dwEdgeOffset);
	for (int i=0; i<(int)tGraphVertex.tNeighbourCount; i++)
		Msg("*   Vertex %d -> distance %7.2f",tpaEdges[i].dwVertexNumber, tpaEdges[i].fPathDistance);
}

void CSE_ALifeSimulator::vfSetOnlineDistance			(float	fNewDistance)
{
	m_fOnlineDistance = fNewDistance;
}

void CSE_ALifeSimulator::vfSetProcessTime			(int	iMicroSeconds)
{
	m_qwMaxProcessTime = iMicroSeconds*CPU::cycles_per_microsec;
}

void CSE_ALifeSimulator::vfSetSwitchDelay			(int	iMilliSeconds)
{
	m_dwSwitchDelay = (u32)iMilliSeconds;
}

void CSE_ALifeSimulator::vfSetScheduleMin			(int	iMilliSeconds)
{
	shedule.t_min = iMilliSeconds;
}

void CSE_ALifeSimulator::vfSetScheduleMax			(int	iMilliSeconds)
{
	shedule.t_max = iMilliSeconds;
}
#endif