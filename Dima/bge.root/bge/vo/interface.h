//////////////////////////////////////////////////////////
//	Module			:	Interface routines				//
//	File			:	interface.h						//
//	Creation date	:	22.01.2001						//
//	Author			:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////
// Exported functions
//////////////////////////////////////////////////////////

extern void vfPlayGame(bool bBlack);
extern void vfGenerateMPCStats();
extern void vfAutoPlay(u8 ucPly, bool bBlack);
extern void vfLoadGame(TBoardCell *taBoard, TBoardCell *tColor, u8 *ucEmpties, char *s);
