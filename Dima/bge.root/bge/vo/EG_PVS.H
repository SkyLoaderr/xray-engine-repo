//////////////////////////////////////////////////////////////////////////////////
//	Module			:	endgame solver											//
//	File			:	eg_pvs.h												//
//	Creation date	:	14.02.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "eval.h"

//////////////////////////////////////////////////////////
// Function declarations
//////////////////////////////////////////////////////////

extern SFeature tfSolveGameRoot(u8 ucEmpties, SFeature tAlpha, SFeature tBeta, TBoardCell tColor, char cCurrentValue);
extern SFeature tfSolveForMate(u8 ucEmpties, SFeature tAlpha, SFeature tBeta, TBoardCell tColor, char cCurrentValue, bool bRoot = true);
