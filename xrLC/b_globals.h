#pragma once

const	u32								c_LMAP_size			= 1024;			// pixels
const	u32								c_VB_maxSize		= 4096*1024;	// bytes
const	u32								c_VB_maxVertices	= 65535;		// count
const	u32								c_vCacheSize		= 24;			// entries
const	u32								c_SS_LowVertLimit	= 64;			// polys
const	u32								c_SS_HighVertLimit	= 3*1024;		// polys
const	u32								c_SS_maxsize		= 40;			// meters
const	u32								c_PM_LowVertLimit	= 128;			// vertices

BOOL	exact_normalize					(Fvector3&	a);
BOOL	exact_normalize					(float*		a);

#include "xrPool.h"
#include "xrFace.h"
#include "xrCDB.h"
#include "xrDeflector.h"
#include "vbm.h"
#include "OGF_Face.h"

typedef xr_vector<vecFace*>				vec2Face;
typedef vec2Face::iterator				splitIt;

typedef xr_vector<CDeflector*>			vecDefl;
typedef vecDefl::iterator				vecDeflIt;

typedef std::pair<Vertex*, Vertex *>	PAIR_VV;
typedef xr_map<Vertex*,Vertex*>			map_v2v;	// vertex to vertex translation
typedef map_v2v::iterator				map_v2v_it;

// Globals
extern SWIContainer				g_SWI;
extern vecVertex				g_vertices;
extern vecFace					g_faces;
extern vec2Face					g_XSplit;
extern vecDefl					g_deflectors;
extern CDB::MODEL*				RCAST_Model;
extern CDeflector*				Deflector;
extern VBContainer				g_VB;
extern IBContainer				g_IB;
extern OGF_Base*				g_TREE_ROOT;
extern b_params					g_params;
extern const int				edge2idx	[3][2];
extern const int				edge2idx3	[3][3];
extern const int				idx2edge	[3][3];
extern BOOL						b_R2;
extern BOOL						b_radiosity;
extern BOOL						b_noise;
