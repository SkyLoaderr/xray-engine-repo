////////////////////////////////////////////////////////////////////////////
//	Module 		: xrCrossTable.h
//	Created 	: 25.01.2003
//  Modified 	: 25.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Building cross table for AI nodes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrLevel.h"
#include "ai_alife_space.h"
#include "game_level_cross_table.h"
#include "game_graph.h"
#include "level_graph.h"

#define CROSS_TABLE_NAME_RAW	"level.gct.raw"

typedef	char FILE_NAME[_MAX_PATH];

void xrBuildCrossTable	(LPCSTR	caProjectName);
void vfRecurseMark		(const CLevelGraph &tMap, xr_vector<bool> &tMarks, u32 dwStartNodeID);
