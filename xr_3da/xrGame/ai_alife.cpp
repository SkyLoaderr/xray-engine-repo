////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"

CSE_ALifeSimulator::CSE_ALifeSimulator(xrServer *tpServer)
{
	m_tpServer			= tpServer;
	m_bLoaded			= false;
	m_tpActor			= 0;
	m_caSaveName[0]		= 0;
	shedule_register	();
}

CSE_ALifeSimulator::~CSE_ALifeSimulator()
{
	shedule_unregister	();
}

float CSE_ALifeSimulator::shedule_Scale()
{
	return(.5f); // (schedule_min + schedule_max)*0.5f
}

BOOL CSE_ALifeSimulator::shedule_Ready()
{
	return(TRUE);
}

LPCSTR CSE_ALifeSimulator::cName()
{
	return("ALife Simulator");
}; 

void CSE_ALifeSimulator::vfUpdateDynamicData(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	CSE_ALifeGraphRegistry::Update		(tpALifeDynamicObject);
	CSE_ALifeTraderRegistry::Update		(tpALifeDynamicObject);
	CSE_ALifeScheduleRegistry::Update	(tpALifeDynamicObject);
	
	CSE_ALifeItem						*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(tpALifeDynamicObject);
	if (l_tpALifeItem && l_tpALifeItem->bfAttached()) {
		OBJECT_PAIR_IT					II = m_tObjectRegistry.find(l_tpALifeItem->ID_Parent);
		R_ASSERT2						(II != m_tObjectRegistry.end(),"Parent object doesn't exist!");
		(*II).second->children.push_back(l_tpALifeItem->ID);
	}
}

void CSE_ALifeSimulator::vfUpdateDynamicData()
{
	// initialize
	CSE_ALifeGraphRegistry::Init	();
	CSE_ALifeTraderRegistry::Init	();
	CSE_ALifeScheduleRegistry::Init	();
	// update objects
	{
		OBJECT_PAIR_IT			I = m_tObjectRegistry.begin();
		OBJECT_PAIR_IT			E = m_tObjectRegistry.end();
		for ( ; I != E; I++) {
			vfUpdateDynamicData	((*I).second);
			_OBJECT_ID			l_tObjectID = (*I).second->ID;
			(*I).second->ID		= m_tpServer->PerformIDgen(l_tObjectID);
			R_ASSERT2			(l_tObjectID == (*I).second->ID,"Can't reserve a particular object identifier");
		}
	}
	
	// update events
	{
		EVENT_PAIR_IT			I = m_tEventRegistry.begin();
		EVENT_PAIR_IT			E = m_tEventRegistry.end();
		for ( ; I != E; I++)
			vfAddEventToGraphPoint((*I).second,(*I).second->m_tGraphID);
	}
	
	// setting up the first switched object
	R_ASSERT2					(m_tpCurrentLevel->begin() != m_tpCurrentLevel->end(),"It is impossible, because at least actor must be in the switch objects map");
	m_tNextFirstSwitchObjectID	= (*(m_tpCurrentLevel->begin())).second->ID;
	
	// setting up the first scheduled object
	if (m_tpScheduledObjects.size())
		m_tNextFirstProcessObjectID = (*m_tpScheduledObjects.begin()).second->ID;
	else
		m_tNextFirstProcessObjectID	= _OBJECT_ID(-1);
}

void CSE_ALifeSimulator::Save()
{
	if (!strlen(m_caSaveName))
		Log						("There is no file name specified!");
	else
		Save(m_caSaveName);
}

void CSE_ALifeSimulator::Save(LPCSTR caSaveName)
{
	strconcat					(m_caSaveName,caSaveName,SAVE_EXTENSION);
	CMemoryWriter				tStream;
	CSE_ALifeHeader::Save		(tStream);
	CSE_ALifeGameTime::Save		(tStream);
	CSE_ALifeObjectRegistry::Save(tStream);
	CSE_ALifeEventRegistry::Save(tStream);
	CSE_ALifeTaskRegistry::Save	(tStream);
	CSE_ALifeAnomalyRegistry::Save(tStream);
	CSE_ALifeOrganizationRegistry::Save(tStream);
	string256					S;
	FS.update_path				(S,"$game_saves$",m_caSaveName);
	tStream.save_to				(S);
	Msg							("* Game is successfully saved to file '%s'",m_caSaveName);
}

void CSE_ALifeSimulator::Load	(LPCSTR caSaveName)
{
	// initializing
	Memory.mem_compact			();
	u32							dwMemUsage = Memory.mem_usage();
	pApp->LoadTitle				("SERVER: Loading a-life simulator...");
	m_tALifeVersion				= ALIFE_VERSION;
	m_tpActor					= 0;
	m_tGameTime					= 0;
	m_bActorEnabled				= true;
	strconcat					(m_caSaveName,caSaveName,SAVE_EXTENSION);

	// loading default settings from 'system.ltx'
	Log							("* Loading parameters...");
	shedule.t_min				= pSettings->r_s32	("alife","schedule_min");
	shedule.t_max				= pSettings->r_s32	("alife","schedule_max");
	m_qwMaxProcessTime			= pSettings->r_s32	("alife","process_time")*CPU::cycles_per_microsec;
	m_fOnlineDistance			= pSettings->r_float("alife","online_distance");
	m_dwSwitchDelay				= pSettings->r_s32	("alife","switch_delay");
	m_fTimeFactor				= pSettings->r_float("alife","time_factor");

	string256					caFileName;
	IReader						*tpStream;

	// loading spawn registry
	R_ASSERT2					(FS.exist(caFileName, "$game_data$", SPAWN_NAME),"Can't find file 'game.spawn'");
	tpStream					= FS.r_open(caFileName);
	Log							("* Loading spawn registry");
	CSE_ALifeSpawnRegistry::Load(*tpStream);
	FS.r_close					(tpStream);

	// loading game
	Log							("* Loading simulator...");
	if (!FS.exist(caFileName,"$game_saves$",m_caSaveName)) {
		// creating new game
		Log						("* Creating new game...");
		vfNewGame				(caSaveName);
		strcpy					(m_caSaveName,caSaveName);
	}
	else {
		strcpy					(m_caSaveName,caSaveName);
		tpStream				= FS.r_open(caFileName);
		if (!tpStream) {
			string4096			S;
			sprintf				(S,"Can't open saved game file '%s'",m_caSaveName);
			R_ASSERT2			(tpStream,S);
		}
		CSE_ALifeHeader::Load	(*tpStream);
		CSE_ALifeGameTime::Load	(*tpStream);
		Log						("* Loading objects...");
		CSE_ALifeObjectRegistry::Load(*tpStream);
		Log						("* Loading events...");
		CSE_ALifeEventRegistry::Load(*tpStream);
		Log						("* Loading tasks...");
		CSE_ALifeTaskRegistry::Load	(*tpStream);
		Log						("* Loading anomly map...");
		CSE_ALifeAnomalyRegistry::Load(*tpStream);
		Log						("* Loading organizations and discoveries...");
		CSE_ALifeOrganizationRegistry::Load(*tpStream);
		Log						("* Building dynamic objects...");
		vfUpdateDynamicData		();
	}
	
	// updating server options information for loading correct level
	{
		R_ASSERT2				(m_tpActor,"There is no actor spawn point!");
		string64				S;
		strcpy					(S,*m_cppServerOptions);
		LPSTR					l_cpPointer = strchr(S,'/');
		R_ASSERT2				(l_cpPointer,"Invalid server options!");
		u32						l_dwLevelID = getAI().m_tpaGraph[m_tpActor->m_tGraphID].tLevelID;
		bool					bOk = false;
		xr_vector<CSE_ALifeGraph::SLevel>::const_iterator	I = getAI().GraphHeader().tpLevels.begin();
		xr_vector<CSE_ALifeGraph::SLevel>::const_iterator	E = getAI().GraphHeader().tpLevels.end();
		for ( ; I != E; I++)
			if ((*I).dwLevelID == l_dwLevelID) {
				strconcat		(*m_cppServerOptions,(*I).caLevelName,l_cpPointer);
				bOk				= true;
				break;
			}
		R_ASSERT2				(bOk,"Graph point level ID not found!");
	}

	// finalizing
	m_bLoaded					= true;
	Msg							("* Loading ALife Simulator is successfully completed (%7.3f Mb)",float(Memory.mem_usage() - dwMemUsage)/1048576.0);
	pApp->LoadTitle				("SERVER: Connecting...");
}

void CSE_ALifeSimulator::vfNewGame(LPCSTR caSaveName)
{
	CSE_ALifeGraphRegistry::Init	();
	CSE_ALifeTraderRegistry::Init	();
	CSE_ALifeScheduleRegistry::Init	();

	ALIFE_ENTITY_P_IT			B = m_tpSpawnPoints.begin();
	ALIFE_ENTITY_P_IT			E = m_tpSpawnPoints.end();
	for (ALIFE_ENTITY_P_IT I = B ; I != E; ) {
		u32						l_dwGroupID	= (*I)->m_dwSpawnGroup;
		for (ALIFE_ENTITY_P_IT m = I + 1, j = I; (m != E) && ((*m)->m_dwSpawnGroup == l_dwGroupID); m++) ;

		CSE_Abstract			*tpSE_Abstract = F_entity_Create((*I)->s_name);
		R_ASSERT2				(tpSE_Abstract,"Can't create entity.");

		CSE_ALifeDynamicObject	*i = dynamic_cast<CSE_ALifeDynamicObject*>(tpSE_Abstract);
		R_ASSERT2				(i,"Non-ALife object in the 'game.spawn'");

		CSE_ALifeAnomalousZone	*tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(i);
		if (tpALifeAnomalousZone) {
			for ( ; j != m; j++) {
				CSE_ALifeAnomalousZone	*tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(*j);
				R_ASSERT2				(tpALifeAnomalousZone,"Anomalous zones are grouped with incompatible objects!");
				vfCreateObjectFromSpawnPoint(i,*j,_SPAWN_ID(j - B));
			}
		}
		xr_delete				(tpSE_Abstract);
		I = m;
	}

	m_tGameTime					= u64(m_dwStartTime = Device.TimerAsync());
	m_tZoneState				= eZoneStateSurge;
	_TIME_ID					l_tFinishTime = m_tGameTime + 0*120000;//3*7*24*3600*1000; // 3 weeks in milliseconds
	float						l_fTimeFactor = m_fTimeFactor;
	m_fTimeFactor				= 1;

	// simulating ALife during the given time period
	do {
		switch (m_tZoneState) {
			case eZoneStateSurge : {
				vfPerformSurge		();
				m_tLastSurgeTime	= tfGetGameTime();
				m_tNextSurgeTime	= m_tLastSurgeTime + 7*24*3600*1000; // a week in milliseconds
				m_tZoneState		= eZoneStateAfterSurge;
				break;
			}
			case eZoneStateAfterSurge : {
				if (tfGetGameTime() > m_tNextSurgeTime) {
					m_tZoneState	= eZoneStateSurge;
					break;
				}

				ALIFE_MONSTER_P_PAIR_IT	I = m_tpScheduledObjects.begin();
				ALIFE_MONSTER_P_PAIR_IT	E = m_tpScheduledObjects.end();
				for ( ; I != E; I++)
					vfProcessNPC	((*I).second);

				break;
			}
			default : NODEFAULT;
		}
	}
	while (tfGetGameTime() < l_tFinishTime);

	m_fTimeFactor				= l_fTimeFactor;
	Save						(caSaveName);
}
