////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_console.cpp
//	Created 	: 09.01.2003
//  Modified 	: 09.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation - console commands support
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
using namespace AI;

#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
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
		for (int k=0, l=strlen(S), m=((l - 1)/u) + 1; k<m; k++) {
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
		for (int k=0, l=strlen(S), m=((l - 1)/u) + 1; k<m; k++) {
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

void CAI_ALife::vfListObjects()
{
	OBJECT_PAIR_IT	I = m_tObjectRegistry.begin();
	OBJECT_PAIR_IT	E = m_tObjectRegistry.end();
	string64		tString;
	Msg("%s->Listing objects :",cName());
	for (int i=0; I != E; I++, i++) {
		Memory.mem_copy(tString,&((*I).second->m_tClassID),sizeof((*I).second->m_tClassID));
		tString[sizeof((*I).second->m_tClassID)] = 0;
		Msg("* %4d : %8s[m_tObjectID=%4d][MDL=%10s][CNT=%3d][GID=%4d][UPD=%d]",i,tString,(*I).first,(*I).second->s_name,(*I).second->m_wCount,(*I).second->m_tGraphID,(*I).second->m_tTimeID);
	}
	Msg("Total %d objects",i);
}

void CAI_ALife::vfListEvents()
{
	EVENT_PAIR_IT	I = m_tEventRegistry.begin();
	EVENT_PAIR_IT	E = m_tEventRegistry.end();
	Msg("%s->Listing events :",cName());
	for (int i=0; I != E; I++, i++)
		Msg("* %4d : [m_tObjectID=%4d][BR=%1d][GID=%4d][TIME=%d]",i,(*I).first,(*I).second->m_tBattleResult,(*I).second->m_tGraphID,(*I).second->m_tTimeID);
	Msg("Total %d events",i);
}

void CAI_ALife::vfListTasks()
{
	TASK_PAIR_IT	I = m_tTaskRegistry.begin();
	TASK_PAIR_IT	E = m_tTaskRegistry.end();
	Msg("%s->Listing tasks :",cName());
	for (int i=0; I != E; I++, i++)
		Msg("* %4d : [m_tObjectID=%4d][CID=%1d][TT=][GID=%4d][UPD=%d]",i,(*I).first,(*I).second->m_tCustomerID,(*I).second->m_tTaskType,(*I).second->m_tGraphID,(*I).second->m_tTimeID);
	Msg("Total %d tasks",i);
}

void CAI_ALife::vfListTerrain()
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
				vfPrintLargeString("Terrain location m_tObjectID",S,i,j,105);
		}
		xr_free(S);
		Msg("Total %d terrain locations",i);
	}
}

void CAI_ALife::vfListSpawnPoints()
{
//	SPAWN_P_IT	I = m_tpSpawnPoints.begin();
//	SPAWN_P_IT	E = m_tpSpawnPoints.end();
//	Msg("%s->Listing spawn points :",cName());
//	for (int i=0; I != E; I++, i++) {
//		CALifeCreatureSpawnPoint *tpALifeCreatureSpawnPoint = dynamic_cast<CALifeCreatureSpawnPoint *>(*I);
//		if (tpALifeCreatureSpawnPoint)
//			Msg("* %4d : [MDL=%10s][GID=%d]",i,(*I)->m_caModel,tpALifeCreatureSpawnPoint->m_wGroupID);
//		else
//			Msg("* %4d : [MDL=%10s]",i,(*I)->m_caModel);
//	}
//	Msg("Total %d spawn points",i);
}

void CAI_ALife::vfObjectInfo(_OBJECT_ID	&tObjectID)
{
//	string4096		S;
//	string16		S1;
//	OBJECT_PAIR_IT	I = m_tObjectRegistry.find(tObjectID);
//	Msg("%s->Object information :",cName());
//	if (I == m_tObjectRegistry.end()) {
//		Msg("Object not found! (m_tObjectID = %d)",tObjectID);
//		return;
//	}
//	
//	CALifeDynamicObject	*tpALifeDynamicObject = (*I).second;
//	string64		tString;
//	Memory.mem_copy	(tString,&(tpALifeDynamicObject->m_tClassID),sizeof(tpALifeDynamicObject->m_tClassID));
//	tString[sizeof(tpALifeDynamicObject->m_tClassID)] = 0;
//	Msg("* Class m_tObjectID      : %s[%I64u]",tString,tpALifeDynamicObject->m_tClassID);
//	Msg("* ObjectID      : %d",tpALifeDynamicObject->m_tObjectID);
//	Msg("* Spawn m_tObjectID      : %d",tpALifeDynamicObject->m_tSpawnID);
//	Msg("* Graph m_tObjectID      : %d",tpALifeDynamicObject->m_tGraphID);
//	Msg("* Count         : %d",tpALifeDynamicObject->m_wCount);
//	vfPrintTime("* Update        :",tpALifeDynamicObject->m_tTimeID);
//	//Msg("* Update        : %d",tpALifeDynamicObject->m_tTimeID);
//	
//	CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(tpALifeDynamicObject);
//	if (tpALifeItem) {
//		Msg("* Mass          : %7.2f",tpALifeItem->m_fMass);
//		Msg("* Price         : %d",tpALifeItem->m_dwCost);
//		return;
//	}
//	
//	CALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CALifeMonsterAbstract *>(tpALifeDynamicObject);
//	if (tpALifeMonsterAbstract) {
//		Msg("* Graph m_tObjectID next : %d",tpALifeMonsterAbstract->m_tNextGraphID);
//		Msg("* Graph m_tObjectID prev : %d",tpALifeMonsterAbstract->m_tPrevGraphID);
//		Msg("* Current speed : %7.2f",tpALifeMonsterAbstract->m_fCurSpeed);
//		Msg("* Going speed   : %7.2f",tpALifeMonsterAbstract->m_fGoingSpeed);
//		Msg("* Distance from : %7.2f",tpALifeMonsterAbstract->m_fDistanceFromPoint);
//		Msg("* Distance to   : %7.2f",tpALifeMonsterAbstract->m_fDistanceToPoint);
//	}
//	
//	CALifeMonsterParams *tpALifeMonsterParams = dynamic_cast<CALifeMonsterParams *>(tpALifeDynamicObject);
//	if (tpALifeMonsterParams) {
//		Msg("* Health        : %d",tpALifeMonsterParams->m_iHealth);
//	}
//		
//	CALifeMonsterGroup *tpALifeMonsterGroup = dynamic_cast<CALifeMonsterGroup *>(tpALifeDynamicObject);
//	if (tpALifeMonsterGroup) {
//		MONSTER_PARAMS_P_IT		I = tpALifeMonsterGroup->m_tpMembers.begin();
//		MONSTER_PARAMS_P_IT		E = tpALifeMonsterGroup->m_tpMembers.end();
//		for (int i=0; I != E; I++, i++)
//			Msg("* Member m_tObjectID [%d]: %d",i,(*I)->m_iHealth);
//		return;
//	}
//	
//	CALifeTraderParams *tpALifeTraderParams = dynamic_cast<CALifeTraderParams *>(tpALifeDynamicObject);
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
//	CALifeTraderAbstract *tpALifeTraderAbstract = dynamic_cast<CALifeTraderAbstract *>(tpALifeDynamicObject);
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
//	CALifeHumanAbstract *tpALifeHumanAbstract = dynamic_cast<CALifeHumanAbstract *>(tpALifeDynamicObject);
//	if (tpALifeHumanAbstract) {
//		Msg("* Search speed  : %7.2f",tpALifeHumanAbstract->m_fSearchSpeed);
//		Msg("* Task state    : %7.2f",tpALifeHumanAbstract->m_tTaskState);
//		Msg("* Current node  : %7.2f",tpALifeHumanAbstract->m_dwCurNode);
//		Msg("* Current point : %7.2f",tpALifeHumanAbstract->m_dwCurTaskLocation);
//		Msg("* Current task  :");
//		if (tpALifeHumanAbstract->m_tpTaskIDs.size()) {
//			CALifePersonalTask	&tTask = *(tpALifeHumanAbstract->m_tpTasks[tpALifeHumanAbstract->m_dwCurTask]);
//			Msg("* Task  m_tObjectID    : %d",tTask.m_tTaskID);
//			vfPrintTime("* Time  m_tObjectID  :",tTask.m_tTimeID);
//			Msg("* Customer m_tObjectID : %d",tTask.m_tCustomerID);
//			Msg("* Task type   : %d",tTask.m_tTaskType);
//			if (tTask.m_tTaskType == eTaskTypeSearchForItemOG) {
//				Msg("* Graph m_tObjectID    : %d",tTask.m_tGraphID);
//				Msg("* Object m_tObjectID   : %d",tTask.m_tObjectID);
//			}
//			else if (tTask.m_tTaskType == eTaskTypeSearchForItemOL) {
//				Msg("* Location m_tObjectID : %d",tTask.m_tLocationID);
//				Msg("* Object m_tObjectID   : %d",tTask.m_tObjectID);
//			}
//			else if (tTask.m_tTaskType == eTaskTypeSearchForItemCG) {
//				string64 tString;
//				Memory.mem_copy(tString,&(tTask.m_tClassID),sizeof(tTask.m_tClassID));
//				tString[sizeof(tTask.m_tClassID)] = 0;
//				Msg("* Graph m_tObjectID    : %d",tTask.m_tGraphID);
//				Msg("* Class m_tObjectID    : %d (%s)",tTask.m_tClassID,tString);
//			}
//			else if (tTask.m_tTaskType == eTaskTypeSearchForItemCL) {
//				string64 tString;
//				Memory.mem_copy(tString,&(tTask.m_tClassID),sizeof(tTask.m_tClassID));
//				tString[sizeof(tTask.m_tClassID)] = 0;
//				Msg("* Location m_tObjectID : %d",tTask.m_tLocationID);
//				Msg("* Class m_tObjectID    : %d (%s)",tTask.m_tClassID,tString);
//			}
//		}
//		{
//			PERSONAL_TASK_P_IT	I = tpALifeHumanAbstract->m_tpTasks.begin();
//			PERSONAL_TASK_P_IT	E = tpALifeHumanAbstract->m_tpTasks.end();
//			for (int j=0; I != E; I++, j++) {
//				Msg("* Task  m_tObjectID    : %d",(*I)->m_tTaskID);
//				vfPrintTime(S,(*I)->m_tTimeID);
//				Msg("* Cost hope   : %7.2f",(*I)->m_fCost);
//				Msg("* Try count   : %d",(*I)->m_dwTryCount);
//			}
//		}
//	}
//	CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeDynamicObject);
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
//	CALifeHumanGroup *tpALifeHumanGroup = dynamic_cast<CALifeHumanGroup *>(tpALifeDynamicObject);
//	if (tpALifeHumanGroup) {
//		Msg("* Members       :");
//		{
//			HUMAN_PARAMS_P_IT		I = tpALifeHumanGroup->m_tpMembers.begin();
//			HUMAN_PARAMS_P_IT		E = tpALifeHumanGroup->m_tpMembers.begin();
//			for (int i=0; I != E; I++, i++) {
//				Msg("* Member m_tObjectID [%d]: %d",i,*I);
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

void CAI_ALife::vfEventInfo(_EVENT_ID &tEventID)
{
	EVENT_PAIR_IT I = m_tEventRegistry.find(tEventID);
	if (I == m_tEventRegistry.end()) {
		Msg("* Invalid event m_tObjectID! (%d)",tEventID);
		return;
	}
	CALifeEvent &tEvent = *((*I).second);
	Msg("* Event information");
	Msg("* Event m_tObjectID  : %d",tEvent.m_tEventID);
	Msg("* Graph m_tObjectID  : %d",tEvent.m_tGraphID);
	vfPrintTime("* Time  m_tObjectID  :",tEvent.m_tTimeID);
	Msg("* Battle    : %d",tEvent.m_tBattleResult);
	Msg("* Monster 1 :");
	CALifeEventGroup *tpMG = tEvent.m_tpMonsterGroup1;
	Msg("*     Class  m_tObjectID    : %d",tpMG->m_tClassID);
	Msg("*     Object m_tObjectID    : %d",tpMG->m_tObjectID);
//	Msg("*     Spawn  m_tObjectID    : %d",tpMG->m_tSpawnID);
	Msg("*     Count before : %d",tpMG->m_wCount);
	Msg("*     Count after  : %d",tpMG->m_wCountAfter);
	Msg("* Monster 2 :");
	tpMG = tEvent.m_tpMonsterGroup2;
	Msg("*     Class  m_tObjectID    : %d",tpMG->m_tClassID);
	Msg("*     Object m_tObjectID    : %d",tpMG->m_tObjectID);
//	Msg("*     Spawn  m_tObjectID    : %d",tpMG->m_tSpawnID);
	Msg("*     Count before : %d",tpMG->m_wCount);
	Msg("*     Count after  : %d",tpMG->m_wCountAfter);
	{
		string4096	S;
		string16	S1;
		S[0] = 0;
		OBJECT_PAIR_IT I = m_tObjectRegistry.begin();
		OBJECT_PAIR_IT E = m_tObjectRegistry.end();
		for (int j=0; I != E; I++) {
			CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>((*I).second);
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

void CAI_ALife::vfTaskInfo(_TASK_ID &tTaskID)
{
	TASK_PAIR_IT I = m_tTaskRegistry.find(tTaskID);
	if (I == m_tTaskRegistry.end()) {
		Msg("* Invalid task m_tObjectID! (%d)",tTaskID);
		return;
	}
	CALifeTask &tTask = *((*I).second);
	Msg("* Task information");
	Msg("* Task  m_tObjectID    : %d",tTask.m_tTaskID);
	vfPrintTime("* Time  m_tObjectID  :",tTask.m_tTimeID);
//	Msg("* Time  m_tObjectID    : %d",tTask.m_tTimeID);
	Msg("* Customer m_tObjectID : %d",tTask.m_tCustomerID);
	Msg("* Task type   : %d",tTask.m_tTaskType);
	if (tTask.m_tTaskType == eTaskTypeSearchForItemOG) {
		Msg("* Graph m_tObjectID    : %d",tTask.m_tGraphID);
		Msg("* Object m_tObjectID   : %d",tTask.m_tObjectID);
	}
	else if (tTask.m_tTaskType == eTaskTypeSearchForItemOL) {
		Msg("* Location m_tObjectID : %d",tTask.m_tLocationID);
		Msg("* Object m_tObjectID   : %d",tTask.m_tObjectID);
	}
	else if (tTask.m_tTaskType == eTaskTypeSearchForItemCG) {
		string64 tString;
		Memory.mem_copy(tString,&(tTask.m_tClassID),sizeof(tTask.m_tClassID));
		tString[sizeof(tTask.m_tClassID)] = 0;
		Msg("* Graph m_tObjectID    : %d",tTask.m_tGraphID);
		Msg("* Class m_tObjectID    : %d (%s)",tTask.m_tClassID,tString);
	}
	else if (tTask.m_tTaskType == eTaskTypeSearchForItemCL) {
		string64 tString;
		Memory.mem_copy(tString,&(tTask.m_tClassID),sizeof(tTask.m_tClassID));
		tString[sizeof(tTask.m_tClassID)] = 0;
		Msg("* Location m_tObjectID : %d",tTask.m_tLocationID);
		Msg("* Class m_tObjectID    : %d (%s)",tTask.m_tClassID,tString);
	}
	string4096	S;
	string16	S1;
	S[0] = 0;
	{
		OBJECT_PAIR_IT I = m_tObjectRegistry.begin();
		OBJECT_PAIR_IT E = m_tObjectRegistry.end();
		for (int j=0; I != E; I++) {
			CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>((*I).second);
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

void CAI_ALife::vfSpawnPointInfo(_SPAWN_ID &tSpawnID)
{
//	CALifeSpawnPoint &tSpawnPoint = *(m_tpSpawnPoints[tSpawnID]);
//	Msg("%s->Spawn-point information :",cName());
//	Msg("* Model                   : %s",tSpawnPoint.m_caModel);
//	Msg("* GraphID                 : %s",tSpawnPoint.m_tNearestGraphPointID);
//	Msg("* Position                : [%7.2f][%7.2f][%7.2f]",tSpawnPoint.m_tPosition.x,tSpawnPoint.m_tPosition.y,tSpawnPoint.m_tPosition.z);
//	
//	CALifeZoneSpawnPoint *tpZoneSpawnPoint = dynamic_cast<CALifeZoneSpawnPoint *>(&tSpawnPoint);
//	if (tpZoneSpawnPoint) {
//		Msg("* Zone type               : %d",tpZoneSpawnPoint->m_tAnomalousZoneType);
//	}
//	CALifeCreatureSpawnPoint *tpCreatureSpawnPoint = dynamic_cast<CALifeCreatureSpawnPoint *>(&tSpawnPoint);
//	if (tpCreatureSpawnPoint) {
//		Msg("* Team                    : %d",tpCreatureSpawnPoint->m_ucTeam);
//		Msg("* Squad                   : %d",tpCreatureSpawnPoint->m_ucSquad);
//		Msg("* Group                   : %d",tpCreatureSpawnPoint->m_ucGroup);
//		Msg("* Group m_tObjectID	               : %d",tpCreatureSpawnPoint->m_wGroupID);
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

void CAI_ALife::vfGraphVertexInfo(_GRAPH_ID &tGraphID)
{
	SGraphVertex &tGraphVertex = m_tpaGraph[tGraphID];
	Msg("%s->Graph vertex information :",cName());
	Msg("* Level point                : [%7.2f][%7.2f][%7.2f]",tGraphVertex.tGlobalPoint.x,tGraphVertex.tGlobalPoint.y,tGraphVertex.tGlobalPoint.z);
//	Msg("* Location m_tObjectID                : %d",tGraphVertex.tVertexType);
	Msg("* Neighbours                 :");
	SGraphEdge	*tpaEdges = (SGraphEdge *)((BYTE *)m_tpaGraph + tGraphVertex.dwEdgeOffset);
	for (int i=0; i<(int)tGraphVertex.tNeighbourCount; i++)
		Msg("*   Vertex %d -> distance %7.2f",tpaEdges[i].dwVertexNumber, tpaEdges[i].fPathDistance);
}
#endif