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
#define ALIFE_CHUNK_DATA				0x0002
	
class CAI_ALife : public CSheduled {
public:
	typedef CSheduled inherited;
	
	typedef struct tagSSpawnHeader {
		u32						dwVersion;
		u32						dwCount;
	} SSpawnHeader;

	typedef struct tagSNPCHeader {
		u32						dwVersion;
		u32						dwCount;
	} SNPCHeader;

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

	typedef struct tagSUsefulObject {
		u16						wSpawnPoint;
		u16						wCount;
	} SUsefulObject;
	
	typedef struct tagSALifeNPC {
		u32						dwLastUpdateTime;
		u16						wSpawnPoint;
		u16						wCount;
		u16						wGraphPoint;
		u16						wNextGraphPoint;
		u16						wPrevGraphPoint;
		float					fSpeed;
		float					fDistanceFromPoint;
		s32						iHealth;
		vector<SUsefulObject>	tpUsefulObject;
	} SALifeNPC;

	u32							m_dwNPCBeingProcessed;
	u64							m_qwMaxProcessTime;
	bool						m_bLoaded;
	SSpawnHeader				m_tSpawnHeader;
	SNPCHeader					m_tNPCHeader;

	// static
	vector<SSpawnPoint>			m_tpSpawnPoint;			// массив spawn-point-ов
	svector<vector<u16>,256>	m_tpTerrain;			// массив списков : по идетнификатору 
														// местности получить список точек графа
	vector<vector<u16> >		m_tpLocationOwner;		// массив списков : по точке графа получить 
														// список её владельцев
	// dynamic
	vector<SALifeNPC>			m_tpNPC;				// массив NPC-й
	vector<vector<u16> >		m_tpGraphObject;		// по точке графа получить все динамические

	void						vfCheckForTheBattle		(u32 dwNPCIndex);
	void						vfChooseNextRoutePoint	(u32 dwNPCIndex);
	void						vfProcessNPC			(u32 dwNPCIndex);
	void						vfInitTerrain			();
	void						vfInitLocationOwners	();
	void						vfInitGraph				();
	// temporary
	void						vfGenerateSpawnPoints	(u32 dwSpawnCount);
	void						vfSaveSpawnPoints		();

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