////////////////////////////////////////////////////////////////////////////
//	Module 		: mt_config.h
//	Created 	: 22.02.2005
//  Modified 	: 22.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Multithreading configuration options
////////////////////////////////////////////////////////////////////////////

#pragma once

extern Flags32 g_mt_config;

#define mtLevelPath			(1<<0)
#define mtDetailPath		(1<<1)
#define mtObjectHandler		(1<<2)
#define mtSightManager		(1<<3)
