////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_debug.h
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Debug functions
////////////////////////////////////////////////////////////////////////////

#pragma once

extern	Flags32		psAI_Flags;

#define aiDebug		(1<<0)
#define aiBrain		(1<<1)
#define aiMotion	(1<<2)
#define aiFrustum	(1<<3)
#define aiFuncs		(1<<4)
#define aiALife		(1<<5)
#define aiLua		(1<<6)
#define aiGOAP		(1<<7)
#define aiCover		(1<<8)
