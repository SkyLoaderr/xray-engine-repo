//---------------------------------------------------------------------------
#ifndef LevelGameDefH
#define LevelGameDefH

#define RPOINT_CHOOSE_NAME 		"$rpoint"
#define NPCPOINT_CHOOSE_NAME 	"$npcpoint"

enum EPointType{
    ptRPoint=0,
    ptReserved,
    ptSpawnPoint,
    ptMaxType,
    pt_force_dword=u32(-1)
};

enum EWayType{
    wtPatrolPath=0,
    wtMaxType,
    wt_force_dword=u32(-1)
};

// BASE offset
#define WAY_BASE					0x1000
#define POINT_BASE					0x2000

// POINT chunks
#define RPOINT_CHUNK				POINT_BASE+ptRPoint

// WAY chunks
#define WAY_PATROLPATH_CHUNK		WAY_BASE+wtPatrolPath
//----------------------------------------------------

#define WAYOBJECT_VERSION			0x0012
//----------------------------------------------------
#define WAYOBJECT_CHUNK_VERSION		0x0001
#define WAYOBJECT_CHUNK_POINTS		0x0002
#define WAYOBJECT_CHUNK_LINKS		0x0003
#define WAYOBJECT_CHUNK_TYPE		0x0004
#define WAYOBJECT_CHUNK_NAME		0x0005

#define NPC_POINT_VERSION			0x0001
//----------------------------------------------------
#define NPC_POINT_CHUNK_VERSION		0x0001
#define NPC_POINT_CHUNK_DATA		0x0002
//----------------------------------------------------
/*
- chunk RPOINT_CHUNK
	- chunk #0
        vector3	(PPosition);
        vector3	(PRotation);
        dword	(m_dwTeamId);
    ...
    - chunk #n

- chunk WAY_PATH_CHUNK
	- chunk #0
    	chunk WAYOBJECT_CHUNK_VERSION
        	word (version)
		chunk WAYOBJECT_CHUNK_NAME
        	stringZ (Name)
        chunk WAY_CHUNK_TYPE
        	dword EWayType (type)
        chunk WAY_CHUNK_POINTS
            word (count)
            for (i=0; i<count; i++){
            	Fvector (pos)
                dword	(flags)
            }
        chunk WAY_CHUNK_LINKS
            word (count)
            for (i=0; i<count; i++){
            	word 	(from)
				word 	(to)
            }
    ...
    - chunk #n
- chunk WAY_JUMP_CHUNK
	-//-
- chunk WAY_TRAFFIC_CHUNK
	-//-
- chunk WAY_CUSTOM_CHUNK
	-//-
*/
class CCustomGamePoint {
public:
	virtual void Save		(IReader&)							= 0;
	virtual void Load		(IWriter&)							= 0;
#ifdef _EDITOR
	virtual void FillProp	(LPCSTR pref, PropItemVec& values)	= 0;
#endif
};

class CNPC_Point : public CCustomGamePoint {
private:
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
	string512				caRouteGraphPoints;
public:
	virtual void Save		(IWriter &fs)
	{
		// version chunk
		fs.open_chunk		(NPC_POINT_CHUNK_VERSION);
		fs.w_u32			(NPC_POINT_VERSION);
		fs.close_chunk		();

		// data chunk
		fs.open_chunk		(NPC_POINT_CHUNK_DATA);
		fs.w_string			(caModel);
		fs.w_u8				(ucTeam);
		fs.w_u8				(ucSquad);
		fs.w_u8				(ucGroup);
		fs.w_u16			(wGroupID);
		fs.w_u16			(wCount);
		fs.w_float			(fBirthRadius);
		fs.w_float			(fBirthProbability);
		fs.w_float			(fIncreaseCoefficient);
		fs.w_float			(fAnomalyDeathProbability);
		fs.w_string			(caRouteGraphPoints);
		fs.close_chunk		();
	}

	virtual void Load		(IReader &fs)
	{
		R_ASSERT(fs.find_chunk(NPC_POINT_CHUNK_VERSION));
		u32 dwVersion = fs.r_u32();
		R_ASSERT(dwVersion != NPC_POINT_VERSION);

		R_ASSERT(fs.find_chunk(NPC_POINT_CHUNK_DATA));
		fs.r_string					(caModel);
		ucTeam						= fs.r_u8();
		ucSquad						= fs.r_u8();
		ucGroup						= fs.r_u8();
		wGroupID					= fs.r_u16();
		wCount						= fs.r_u16();
		fBirthRadius				= fs.r_float();
		fBirthProbability			= fs.r_float();
		fIncreaseCoefficient		= fs.r_float();
		fAnomalyDeathProbability	= fs.r_float();
		fs.r_string					(caRouteGraphPoints);
	}
#ifdef _EDITOR
	virtual void FillProp	(LPCSTR pref, PropItemVec& values);
#endif
};

//---------------------------------------------------------------------------
#endif //LevelGameDefH

