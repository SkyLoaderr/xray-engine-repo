#pragma once

#include "cl_rapid.h"
#include "xrLevel.h"

struct Params
{
	float		fPatchSize;			// patch size
	float		fTestHeight;		// test height (center of the "tester")
	float		fCanUP;				// can reach point in up (dist)
	float		fCanDOWN;			// can reach point down  (dist)
	
	void		Init	()
	{
		fPatchSize		= 0.75f;
		fTestHeight		= 1.0f;
		fCanUP			= 1.5f;
		fCanDOWN		= 4.0f;
	}
};


// base patch used all the time up to merging
const DWORD InvalidNode		= (1<<24)-1;
const DWORD UnkonnectedNode	= 0xfffffff0;
const WORD	InvalidSector	= 0xff;

struct Node					// definition of "patch" or "node"
{
	union	{
		struct {
			DWORD n1,n2,n3,n4;	// neighbourh patches (Left,Forward,Right,Backward)
		};
		DWORD	n[4];
	};
	Fplane	Plane;			// plane of patch										
	Fvector	Pos;			// position of patch center								
	WORD	Sector;			//														
	
	DWORD	Group;			
	
	float	LightLevel;
	
	float	cover[4];		// cover in four directions

	Node() {
		n1=n2=n3=n4	= UnkonnectedNode;
		Sector		= InvalidSector;
		Group		= 0;
	}
	DWORD	nLeft()			{return n1;}
	DWORD	nForward()		{return n2;}
	DWORD	nRight()		{return n3;}
	DWORD	nBack()			{return n4;}
	
	void	PointLF(Fvector& D);
	void	PointFR(Fvector& D);
	void	PointRB(Fvector& D);
	void	PointBL(Fvector& D);
};

DEF_VECTOR(DWORDs,DWORD);
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
    DWORD           type;				// Type of light source		
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
DEF_VECTOR(Marks,bool			);
DEF_VECTOR(Lights,R_Light		);

// data
extern	Nodes			g_nodes;
extern	Merged			g_merged;
extern	Lights			g_lights;
extern	Params			g_params;
extern	RAPID::Model	Level;
extern	RAPID::Model	LevelLight;
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
const int	RCAST_MaxTris	= 1024;
const int	RCAST_Count		= 6;
const int	RCAST_Total		= (2*RCAST_Count+1)*(2*RCAST_Count+1);
const float	RCAST_Depth		= 1.f;

const float	cover_distance	= 30.f;
const float cover_height	= 1.f;
const float cover_sqr_dist	= cover_distance*cover_distance;

const float	sim_angle		= 20.f;
const float	sim_dist		= 0.15f;
const int	sim_light		= 32;
const float	sim_cover		= 48;
