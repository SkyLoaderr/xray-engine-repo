////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#pragma once

#define NPC_SPAWN_POINT_VERSION			0x0001

#define NPC_SPAWN_POINT_CHUNK_VERSION	0x0001
#define NPC_SPAWN_POINT_CHUNK_DATA		0x0002
	
#define ALIFE_VERSION					0x0001

#define ALIFE_CHUNK_VERSION				0x0001
#define NPC_CHUNK_DATA					0x0002
#define TASK_CHUNK_DATA					0x0003
#define EVENT_CHUNK_DATA				0x0004
	
class CAI_ALife : public CSheduled {
public:
	typedef CSheduled inherited;
	typedef u32	GOID;									// Game Object ID
	typedef u32	EID;									// Event ID

	enum EPerception {
		ePerceptSee = u32(0),
		ePerceptHear,
		ePerceptNotice,
		ePerceptMeet,
		ePerceptDummy = u32(-1),
	};

	enum EBattleResult {
		eBattleRun1 = u32(0),
		eBattleRun2,
		eBattleRun12,
		eBattleRun1WF,
		eBattleRun2WF,
		eBattleDummy = u32(-1),
	};

	enum ETaskType {
		eTaskTypeSearchForArtefact = u32(0),
		eTaskTypeSearchForItem,
		eTaskTypeDummy = u32(-1),
	};

	typedef struct tagSItem {
		GOID					tItemID;
		u64						qwClassID;
		u16						wSpawnPoint;
		u8						ucCount;
	} SItem;

	typedef struct tagSTask {
		u16						wGraphPoint;
		u8						ucTerrain;
		ETaskType				tTaskType;
		GOID					tCustomerGOID;
	} STask;

	typedef struct tagSMonsterGroup {
		u64						qwClassID;
		GOID					tGOID;
		u8						ucCount;
	} SMonsterGroup;

	typedef struct tagSEvent {
		u64						qwGameTime;
		u16						wGraphPoint;
		SMonsterGroup			tMonsterGroup1;
		SMonsterGroup			tMonsterGroup2;
		u8						ucMonster1CountAfter;
		u8						ucMonster2CountAfter;
		EBattleResult			tBattleResult;
	} SEvent;

	typedef struct tagSPersonalEvent {
		EID						tEventID;
		u64						qwGameTime;
		int						iHealth;
		vector<SItem>			tpItem;
		STask					tTask;
		EPerception				tPerception;
	} SPersonalEvent;

	typedef struct tagSSpawnHeader {
		u32						dwVersion;
		u32						dwCount;
	} SSpawnHeader;

	typedef struct tagSSpawnPoint {
		u16						wNearestGraphPoint;
		string64				caModel;
		u8						ucTeam;
		u8						ucSquad;
		u8						ucGroup;
		u16						wGroupID;
		u16						wCount;
		float					fBirthRadius;
		float					fBirthProbability;
		float					fIncreaseCoefficient;
		float					fAnomalyDeathProbability;
		u8						ucRoutePointCount;
		vector<u16>				wpRouteGraphPoints;
	} SSpawnPoint;

	typedef struct tagSNPCHeader {
		u32						dwVersion;
		u32						dwCount;
	} SNPCHeader;

	typedef struct tagSALifeNPC {
		GOID					tNPCID;
		bool					bCorp;
		u16						wCount;
		u32						dwLastUpdateTime;
		u16						wSpawnPoint;
		u16						wGraphPoint;
		u16						wNextGraphPoint;
		u16						wPrevGraphPoint;
		float					fSpeed;
		float					fDistanceFromPoint;
		float					fDistanceToPoint;
		s32						iHealth;
		u8						ucCurrentTask;
		vector<STask>			tpTask;
		vector<SItem>			tpItem;
		vector<SPersonalEvent>	tpEvent;
	} SALifeNPC;

	u32							m_dwNPCBeingProcessed;
	u64							m_qwMaxProcessTime;
	bool						m_bLoaded;
	
	SSpawnHeader				m_tSpawnHeader;
	SNPCHeader					m_tNPCHeader;

	// auto-generated
	svector<vector<u16>,256>	m_tpTerrain;			// массив списков : по идетнификатору 
														//	местности получить список точек графа
	vector<vector<u16> >		m_tpLocationOwner;		// массив списков : по точке графа получить 
														//	список её владельцев
	// static
	vector<SSpawnPoint>			m_tpSpawnPoint;			// массив spawn-point-ов
	
	// dynamic
	vector<vector<u32> >		m_tpGraphObject;		// по точке графа получить все динамические
	u32							m_dwNPCID;				// идентификатор NPC
	vector<SALifeNPC>			m_tpNPC;				// массив NPC-й
	vector<STask>				m_tpTask;				// задания для сталкеров
	u32							m_dwEventID;			// идентификатор карты событий
	map<u32,SEvent>				m_tpEventRegistry;		// список событий игры

	IC void vfRemoveFromGraphPoint(u32 dwNPCIndex, u16 wGraphPoint)
	{
		for (int i=0; i<(int)m_tpGraphObject[wGraphPoint].size(); i++)
			if (m_tpGraphObject[wGraphPoint][i] == dwNPCIndex) {
				m_tpGraphObject[wGraphPoint].erase(m_tpGraphObject[m_tpNPC[dwNPCIndex].wGraphPoint].begin() + i);
				break;
			}
	}
	
	IC void vfAddToGraphPoint(u32 dwNPCIndex, u16 wNextGraphPoint)
	{
		m_tpGraphObject[wNextGraphPoint].push_back((u16)dwNPCIndex);
	}

	IC void vfChangeGraphPoint(u32 dwNPCIndex, u16 wGraphPoint, u16 wNextGraphPoint)
	{
		vfRemoveFromGraphPoint	(dwNPCIndex,wGraphPoint);
		vfAddToGraphPoint		(dwNPCIndex,wNextGraphPoint);
	}

	void						vfCheckForDeletedEvents	(u32 dwNPCIndex);
	void						vfCheckForTheBattle		(u32 dwNPCIndex);
	void						vfChooseNextRoutePoint	(u32 dwNPCIndex);
	void						vfProcessNPC			(u32 dwNPCIndex);
	void						vfInitTerrain			();
	void						vfInitLocationOwners	();
	void						vfInitGraph				();
	// temporary
	void						vfGenerateSpawnPoints	(u32 dwSpawnCount);
	void						vfSaveSpawnPoints		();
	//
public:
								CAI_ALife				();
	virtual						~CAI_ALife				();
	virtual float				shedule_Scale			()					{return .5f;};
	virtual BOOL				Ready					()					{return TRUE;};
	virtual LPCSTR				cName					()					{return "ALife simulator";}; 
	virtual void				Load					();
	virtual void				Update					(u32 dt);	
			void				Save					();
			void				Generate				();
};