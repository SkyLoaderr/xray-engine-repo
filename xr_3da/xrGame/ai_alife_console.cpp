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
void vfPrintLargeString(const char *S1, const char *S, const int i, const int j, const u32 u)
{
	string128	S2;
	for (int k=0, l=strlen(S), m=((l - 1)/u) + 1; k<m; k++) {
		memcpy(S2,S + k*u,u*sizeof(char));
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
}

void vfPrintLargeString(const char *S1, const char *S, const int j, const u32 u)
{
	string128	S2;
	for (int k=0, l=strlen(S), m=((l - 1)/u) + 1; k<m; k++) {
		memcpy(S2,S + k*u,u*sizeof(char));
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
}

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
	OBJECT_VECTOR_IT	it = m_tpLocationOwners.begin();
	OBJECT_VECTOR_IT	E  = m_tpLocationOwners.end();
	Msg("%s->Listing location owners :",cName());
	string4096			S;
	for (int i=0; it != E; it++, i++) {
		OBJECT_IT   it1 = (*it).begin();
		OBJECT_IT   E1  = (*it).end();
		S[0] = 0;
		string16		S1;
		for (int j=0; it1 != E1; it1++, j++) {
			if (j)
				strcat(S,",");
			strcat(S,itoa(*it1,S1,10));
		}

		if (j)
			vfPrintLargeString("Graph vertex ID",S,i,j,105);
	}
	Msg("Total %d graph vertexes",i);
}

void CAI_ALife::vfListTerrain()
{
	GRAPH_VECTOR_IT	it = m_tpTerrain.begin();
	GRAPH_VECTOR_IT	E  = m_tpTerrain.end();
	Msg("%s->Listing terrain locations :",cName());
	char *S = (char *)xr_malloc(128*1024*sizeof(char));
	for (int i=0; it != E; it++, i++) {
		GRAPH_IT   it1 = (*it).begin();
		GRAPH_IT   E1  = (*it).end();
		S[0] = 0;
		string16		S1;
		for (int j=0; it1 != E1; it1++, j++) {
			if (j)
				strcat(S,",");
			strcat(S,itoa(*it1,S1,10));
		}

		if (j)
			vfPrintLargeString("Terrain location ID",S,i,j,105);
	}
	_FREE(S);
	Msg("Total %d terrain locations",i);
}

void CAI_ALife::vfListSpawnPoints()
{
	SPAWN_IT	it = m_tpSpawnPoints.begin();
	SPAWN_IT	E  = m_tpSpawnPoints.end();
	Msg("%s->Listing spawn points :",cName());
	for (int i=0; it != E; it++, i++)
		Msg("* %4d : [MDL=%10s][GID=%d]",i,(*it).caModel,(*it).wGroupID);
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
	//Msg("* Health        : %d",tpALifeMonster->m_iHealth);

	CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeMonster);
	if (tpALifeHuman) {
		Msg("* Item mass     : %7.2f",tpALifeHuman->m_tHumanParams.m_fCumulativeItemMass);
		//Msg("* Max item mass : %7.2f",tpALifeHuman->m_fMaxItemMass);
		Msg("* Items         :");
		{
			OBJECT_IT it			= tpALifeHuman->m_tHumanParams.m_tpItemIDs.begin();
			OBJECT_IT E				= tpALifeHuman->m_tHumanParams.m_tpItemIDs.end();
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
	EVENT_PAIR_IT it = m_tEventRegistry.m_tpMap.find(tEventID);
	if (it == m_tEventRegistry.m_tpMap.end()) {
		Msg("* Invalid event ID! (%d)",tEventID);
		return;
	}
	SEvent &tEvent = (*it).second;
	Msg("* Event information");
	Msg("* Event ID  : %d",tEvent.tEventID);
	Msg("* Graph ID  : %d",tEvent.tGraphID);
	Msg("* Time  ID  : %d",tEvent.tTimeID);
	Msg("* Battle    : %d",tEvent.tBattleResult);
	Msg("* Monster 1 :");
	CALifeEventGroup *tpMG = tEvent.tpMonsterGroup1;
	Msg("*     Class  ID    : %d",tpMG->m_tClassID);
	Msg("*     Object ID    : %d",tpMG->m_tObjectID);
	Msg("*     Spawn  ID    : %d",tpMG->m_tSpawnID);
	Msg("*     Count before : %d",tpMG->m_wCount);
	Msg("*     Count after  : %d",tpMG->m_wCountAfter);
	Msg("* Monster 2 :");
	tpMG = tEvent.tpMonsterGroup2;
	Msg("*     Class  ID    : %d",tpMG->m_tClassID);
	Msg("*     Object ID    : %d",tpMG->m_tObjectID);
	Msg("*     Spawn  ID    : %d",tpMG->m_tSpawnID);
	Msg("*     Count before : %d",tpMG->m_wCount);
	Msg("*     Count after  : %d",tpMG->m_wCountAfter);
	string4096	S;
	string16	S1;
	S[0] = 0;
	OBJECT_PAIR_IT I = m_tObjectRegistry.m_tppMap.begin();
	OBJECT_PAIR_IT E = m_tObjectRegistry.m_tppMap.end();
	for (int j=0; I != E; I++) {
		CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>((*I).second);
		if (!tpALifeHuman)
			continue;
		PERSONAL_EVENT_IT i = tpALifeHuman->m_tpEvents.begin();
		PERSONAL_EVENT_IT e = tpALifeHuman->m_tpEvents.end();
		for ( ; i != e; i++)
			if ((*i).tEventID == tEventID) {
				if (j++)
					strcat(S,",");
				strcat(S,itoa((*I).first,S1,10));
				break;
			}
	}
	vfPrintLargeString(S,"Monsters, who know about the event",j,105);
}

void CAI_ALife::vfTaskInfo(_TASK_ID &tTaskID)
{
	TASK_PAIR_IT it = m_tTaskRegistry.m_tpMap.find(tTaskID);
	if (it == m_tTaskRegistry.m_tpMap.end()) {
		Msg("* Invalid task ID! (%d)",tTaskID);
		return;
	}
	STask &tTask = (*it).second;
	Msg("* Task information");
	Msg("* Task  ID    : %d",tTask.tTaskID);
	Msg("* Graph ID    : %d",tTask.tGraphID);
	Msg("* Time  ID    : %d",tTask.tTimeID);
	Msg("* Customer ID : %d",tTask.tCustomerID);
	Msg("* Task type   : %d",tTask.tTaskType);
	string4096	S;
	string16	S1;
	S[0] = 0;
	OBJECT_PAIR_IT I = m_tObjectRegistry.m_tppMap.begin();
	OBJECT_PAIR_IT E = m_tObjectRegistry.m_tppMap.end();
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

void CAI_ALife::vfSpawnPointInfo(_SPAWN_ID &tSpawnID)
{
	SSpawnPoint &tSpawnPoint = m_tpSpawnPoints[tSpawnID];
	Msg("%s->Spawn-point information :",cName());
	Msg("* Model                   : %s",tSpawnPoint.caModel);
	Msg("* Team                    : %d",tSpawnPoint.ucTeam);
	Msg("* Squad                   : %d",tSpawnPoint.ucSquad);
	Msg("* Group                   : %d",tSpawnPoint.ucGroup);
	Msg("* Group ID	               : %d",tSpawnPoint.wGroupID);
	Msg("* Count	               : %d",tSpawnPoint.wCount);
	Msg("* BirthRadius	           : %6.2f",tSpawnPoint.fBirthRadius);
	Msg("* BirthProbability	       : %6.2f",tSpawnPoint.fBirthProbability);
	Msg("* IncreaseCoefficient     : %6.2f",tSpawnPoint.fIncreaseCoefficient);
	Msg("* AnomalyDeathProbability : %6.2f",tSpawnPoint.fAnomalyDeathProbability);
	
	GRAPH_IT	it = tSpawnPoint.tpRouteGraphPoints.begin();
	GRAPH_IT	E  = tSpawnPoint.tpRouteGraphPoints.end();
	string4096		S;
	string16		S1;
	S[0] = 0;
	for (int j=0; it != E; it++, j++) {
		if (j)
			strcat(S,",");
		strcat(S,itoa(*it,S1,10));
	}
	vfPrintLargeString(S,"RouteGraphPoints",tSpawnPoint.ucRoutePointCount,105);
}
#endif