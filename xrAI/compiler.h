#pragma once

#include "xrCDB.h"
#include "xrLevel.h"
#include "AIMapExport.h"

// base patch used all the time up to merging
const u32 InvalidNode		= (1<<24)-1;
const u32 UnkonnectedNode	= 0xfffffff0;
const WORD	InvalidSector	= 0xff;

struct Node					// definition of "patch" or "node"
{
	union	{
		struct {
			u32 n1,n2,n3,n4;	// neighbourh patches (Left,Forward,Right,Backward)
		};
		u32	n[4];
	};
	Fplane	Plane;			// plane of patch										
	Fvector	Pos;			// position of patch center								
	WORD	Sector;			//														
	
	u32	Group;			
	
	float	LightLevel;
	
	float	cover[4];		// cover in four directions

	Node() {
		n1=n2=n3=n4	= UnkonnectedNode;
		Sector		= InvalidSector;
		Group		= 0;
	}
	u32	nLeft()			{return n1;}
	u32	nForward()		{return n2;}
	u32	nRight()		{return n3;}
	u32	nBack()			{return n4;}
	
	void	PointLF(Fvector& D);
	void	PointFR(Fvector& D);
	void	PointRB(Fvector& D);
	void	PointBL(Fvector& D);
};

DEF_VECTOR(DWORDs,u32);
struct NodeMerged
{
	DWORDs		neighbours;	// list of neighbours
	DWORDs		contains;	// while merging - contains list of elementar nodes
	Fplane		plane;
	Fvector		P0,P1;		// min/max
	WORD		sector;
	BYTE		light;
	float		cover[4];
};

void	Compress	(NodeCompressed& Dest, Node& Src);

#define LT_DIRECT		0
#define LT_POINT		1
#define LT_SECONDARY	2

struct R_Light
{
    u32           type;				// Type of light source		
    float			amount;				// Diffuse color of light	
    Fvector         position;			// Position in world space	
    Fvector         direction;			// Direction in world space	
    float		    range;				// Cutoff range
	float			range2;				// ^2
    float	        attenuation0;		// Constant attenuation		
    float	        attenuation1;		// Linear attenuation		
    float	        attenuation2;		// Quadratic attenuation	
	
	Fvector			tri[3];				// Cached triangle for ray-testing
};

DEF_VECTOR(Nodes,Node			);
DEF_VECTOR(Merged,NodeMerged	);
DEF_VECTOR(Vectors,Fvector		);
DEF_VECTOR(Marks,BYTE			);
DEF_VECTOR(Lights,R_Light		);

// data
extern	Nodes			g_nodes;
extern	Merged			g_merged;
extern	Lights			g_lights;
extern	SAIParams		g_params;
extern	CDB::MODEL		Level;
extern	CDB::MODEL		LevelLight;
extern	CDB::COLLIDER	XRC;
extern	Fbox			LevelBB;
extern	Vectors			Emitters;

// phases
void	xrLoad			(LPCSTR name);
void	xrBuildNodes	();
void	xrSmoothNodes	();
void	xrLight			();
void	xrCover			();
void	xrMerge			();
void	xrConvertAndLink();
void	xrDisplay		();
void	xrSaveNodes		(LPCSTR name);

// constants
const int	RCAST_MaxTris	= (2*1024);
const int	RCAST_Count		= 6;
const int	RCAST_Total		= (2*RCAST_Count+1)*(2*RCAST_Count+1);
const float	RCAST_Depth		= 1.f;

const float	cover_distance	= 17.f;
const float cover_height	= 1.f;
const float cover_sqr_dist	= cover_distance*cover_distance;

const float	sim_angle		= 20.f;
const float	sim_dist		= 0.15f;
const int	sim_light		= 32;
const float	sim_cover		= 48;
