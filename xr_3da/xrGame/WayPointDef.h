//---------------------------------------------------------------------------
#ifndef WayPointTypeH
#define WayPointTypeH

enum EWayType{
    wtPatrolPath=0,
    wtJumpPoint,
    wtTraffic,
    wtCustom,
    wtMaxType,
    force_dword=DWORD(-1)
};

#define WAY_BASE					0x1000
// chunks
#define WAY_PATH_CHUNK				WAY_BASE+wtPatrolPath
#define WAY_JUMP_CHUNK				WAY_BASE+wtJumpPoint
#define WAY_TRAFFIC_CHUNK			WAY_BASE+wtTraffic
#define WAY_CUSTOM_CHUNK			WAY_BASE+wtCustom
//----------------------------------------------------

#define WAYOBJECT_VERSION			0x0012
//----------------------------------------------------
#define WAYOBJECT_CHUNK_VERSION		0x0001
#define WAYOBJECT_CHUNK_POINTS		0x0002
#define WAYOBJECT_CHUNK_LINKS		0x0003
#define WAYOBJECT_CHUNK_TYPE		0x0004
#define WAYOBJECT_CHUNK_NAME		0x0005

//----------------------------------------------------
#define START_WAYPOINT				0x00000001
#define FINISH_WAYPOINT				0x00000002
//----------------------------------------------------
/*
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
//---------------------------------------------------------------------------
#endif
 