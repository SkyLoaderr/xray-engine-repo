//////////////////////////////////////////////////////////////////////////////////
//	Module			:	midgame solver											//
//	File			:	mg_pvs.h												//
//	Creation date	:	14.02.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////
// Exported variables
//////////////////////////////////////////////////////////

extern u64 qwCount;

//////////////////////////////////////////////////////////
// Exported functions
//////////////////////////////////////////////////////////

extern int tfSearchGameRoot(u8 ucDepth, u8 ucEmpties, int tAlpha, int tBeta, TBoardCell tColor, char cCurrentValue, char cGameStage, char cUseProbCut);
extern int tfSearchSelectiveEndgameRoot(u8 ucDepth, u8 ucEmpties, int tAlpha, int tBeta, TBoardCell tColor, char cCurrentValue, char cGameStage, char cUseProbCut);
extern int mini_max(u8 ucDepth, u8 ucEmpties, TBoardCell tColor, char cCurrentValue);
