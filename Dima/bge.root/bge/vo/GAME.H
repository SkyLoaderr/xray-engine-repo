//////////////////////////////////////////////////////////////////////////////////
//	Module			:	Game routines											//
//	File			:	game.cpp												//
//	Creation date	:	14.02.2002												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "moves.h"

//////////////////////////////////////////////////////////
// Macros declarations
//////////////////////////////////////////////////////////

#define EXTENDED_INTERFACE
#define DEFAULT_TIME 60.0
//#define AUTO_PLAY

//////////////////////////////////////////////////////////
// Type declarations
//////////////////////////////////////////////////////////

typedef struct tagTGame {
	TBoardCell	taBoard[BOARD_SIZE];
	TBoardCell	tColor;
	u8			ucPly;
	SFeature	tLastBest;
} TGame;

typedef struct tagTSearchGameInfo {
	TBoardCell taBoard[BOARD_SIZE];
	SHashValue tHashValue;
} TSearchGameInfo;

//////////////////////////////////////////////////////////
// Exported variables
//////////////////////////////////////////////////////////

extern u32				dwChangeColor;
extern f64				dStartTime;
extern f64				dTimeLeft;
extern TGame			tGame;
extern TBoardCell		tLastBest;
extern u8				ucLastDepth;
extern SFeature			tLastValue;
extern bool				bTotalWLD;
extern u64				qwStart, qwFinish, qwMoveStart, qwMoveFinish, qwStartThink, qwStopThink;
extern TSearchGameInfo	tGlobalSearchParameters;
extern HANDLE			hThreadHandle;
extern char				cCurMoveCount;
extern u8				ucBugLastDepth;

//////////////////////////////////////////////////////////
// Exported functions
//////////////////////////////////////////////////////////

extern void vfPrintPrincipalVariation(TBoardCell tBestMove, u8 ucDepth, TBoardCell tColor, SFeature tValue, bool bWLD, char cCurrentMove, bool bOverTime = false);
extern void vfGetBestMove(u8 ucEmpties, f64 &dTimeLeft, int *iRow, int *iColumn, f64 *dEval, f64 *dTime);
extern void vfInitAll();
extern void vfPrepareForGame();
extern void vfPrepareForMove(TBoardCell *taBoard, TBoardCell tColor, char *cCurrentValue, u8 *ucEmpties);
extern void vfFinishThread();
extern void vfSolveGames(char *caFileName);
