//////////////////////////////////////////////////////////////////////////////////
//	Module			:	Move routines											//
//	File			:	misc.h													//
//	Creation date	:	14.02.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

extern FILE *fVOOutput;
extern bool bfFileExists(char *caFileName);
extern void __cdecl vfDualPrintF(const char *caFormat, ... );