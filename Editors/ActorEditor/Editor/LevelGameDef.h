//---------------------------------------------------------------------------
#ifndef LevelGameDefH
#define LevelGameDefH

#define RPOINT_CHOOSE_NAME 		"$rpoint"
#define AIPOINT_CHOOSE_NAME		"$aipoint"
#define NPCPOINT_CHOOSE_NAME 	"$npcpoint"

enum EPointType{
    ptRPoint=0,
    ptAIPoint,
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
#define AIPOINT_CHUNK				POINT_BASE+ptAIPoint

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
        vector	(PPosition);
        vector	(PRotation);
        dword	(m_dwTeamId);
    ...
    - chunk #n
    	
- chunk AIPOINT_CHUNK
	- chunk #0
        vector	(PPosition);
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
	virtual void Save		(CStream&)							= 0;
	virtual void Load		(CFS_Base&)							= 0;
#ifdef _EDITOR
	virtual void FillProp	(LPCSTR pref, PropValueVec& values)	= 0;
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
	virtual void Save		(CFS_Base &fs)
	{
		// version chunk
		fs.open_chunk		(NPC_POINT_CHUNK_VERSION);
		fs.Wdword			(NPC_POINT_VERSION);
		fs.close_chunk		();
		
		// data chunk
		fs.open_chunk		(NPC_POINT_CHUNK_DATA);
		fs.Wstring			(caModel);
		fs.Wbyte			(ucTeam);
		fs.Wbyte			(ucSquad);
		fs.Wbyte			(ucGroup);
		fs.Wword			(wGroupID);
		fs.Wword			(wCount);
		fs.Wfloat			(fBirthRadius);
		fs.Wfloat			(fBirthProbability);
		fs.Wfloat			(fIncreaseCoefficient);
		fs.Wfloat			(fAnomalyDeathProbability);
		fs.Wstring			(caRouteGraphPoints);
		fs.close_chunk		();
	}
	
	virtual void Load		(CStream &fs)
	{
		R_ASSERT(fs.FindChunk(NPC_POINT_CHUNK_VERSION));
		u32 dwVersion = fs.Rdword();
		if (dwVersion != NPC_POINT_VERSION)
			THROW;
		
		R_ASSERT(fs.FindChunk(NPC_POINT_CHUNK_DATA));
		fs.Rstring					(caModel);
		ucTeam						= fs.Rbyte();
		ucSquad						= fs.Rbyte();
		ucGroup						= fs.Rbyte();
		wGroupID					= fs.Rword();
		wCount						= fs.Rword();
		fBirthRadius				= fs.Rfloat();
		fBirthProbability			= fs.Rfloat();
		fIncreaseCoefficient		= fs.Rfloat();
		fAnomalyDeathProbability	= fs.Rfloat();
		fs.Rstring					(caRouteGraphPoints);
	}
#ifdef _EDITOR
	virtual void FillProp	(LPCSTR pref, PropValueVec& values);
#endif
};

//---------------------------------------------------------------------------
#endif //LevelGameDefH
 