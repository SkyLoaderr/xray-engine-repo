////////////////////////////////////////////////////////////////////////////
//	Module 		: xr_spawn_merge.h
//	Created 	: 29.01.2003
//  Modified 	: 29.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Merging level spawns for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Entities.h"

typedef struct tagSSPawnHeader {
	u32		dwVersion;
	u32		dwSpawnCount;
	u32		dwLevelCount;
} SSpawnHeader;

extern void xrMergeSpawns();
