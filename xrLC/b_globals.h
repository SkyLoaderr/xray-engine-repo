#pragma once

const	int						lmap_size	= 1024;

#include "xrPool.h"
#include "xrFace.h"
#include "xrCDB.h"
#include "xrDeflector.h"
#include "vbm.h"
#include "OGF_Face.h"
#include "xr_list.h"

typedef vector<vecFace*>		vec2Face;
typedef vec2Face::iterator		splitIt;

typedef vector<CDeflector*>		vecDefl;
typedef vecDefl::iterator		vecDeflIt;

typedef pair<Vertex*, Vertex *> PAIR_VV;
typedef map<Vertex*,Vertex*>	map_v2v;	// vertex to vertex translation
typedef map_v2v::iterator		map_v2v_it;

// Globals
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
