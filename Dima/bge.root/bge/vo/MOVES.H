//////////////////////////////////////////////////////////////////////////////////
//	Module			:	Move routines											//
//	File			:	moves.h													//
//	Creation date	:	22.01.2002												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////
// Used modules
//////////////////////////////////////////////////////////

#include "tt.h"

//////////////////////////////////////////////////////////
// Macros declarations
//////////////////////////////////////////////////////////

#define MOVE_PASS      90
#define MAX_MOVE_COUNT 64

//////////////////////////////////////////////////////////
// Type declarations
//////////////////////////////////////////////////////////

typedef struct tagSSavedMove {
	TBoardCell  tMove;
	char		cCurrentValue;
	u8			ucFlipCount;
	u8			ucFastestFirstValue;
	SHashValue	tHashValue;
	TBoardCell	**tppFlipStack;
} SSavedMove;

//////////////////////////////////////////////////////////
// Exported variables
//////////////////////////////////////////////////////////

extern TBoardCell   taGlobalMoveStack[60*MAX_MOVE_COUNT];
extern TBoardCell	*tpMoveStack;
extern const u8		ucaW2B[64];

//////////////////////////////////////////////////////////
// Exported functions
//////////////////////////////////////////////////////////

extern void			vfInitializeMoveList();
extern TBoardCell	*tpfGetAvailableMoves();
extern TBoardCell	*tpfGetAvailableMoves(u8 ucEmpties);
extern TBoardCell	*tpfGetAvailableMovesAndFastestFirst(u8 ucBestMove, u8 ucEmpties, u8 *ucMoveCount, TBoardCell tColor, TBoardCell tOpponentColor, bool *bpETCFound, u8 *ucpBestMove, int *ipMoveIndex, SFeature tBeta);
extern void			vfGetAvailableMovesAndFastestFirst(u8 ucBestMove, u8 ucEmpties, u8 *ucMoveCount, TBoardCell tColor, TBoardCell tOpponentColor, bool *bpETCFound, u8 *ucpBestMove, int *ipMoveIndex, SFeature tBeta, char cCurrentValue, SSavedMove *taSavedMoves);
//extern TBoardCell	*tpfGetAvailableMovesAndFastestFirst(u8 ucBestMove, u8 ucEmpties, u8 *ucMoveCount, TBoardCell tColor, TBoardCell tOpponentColor, bool *bpETCFound, u8 *ucpBestMove, int *ipMoveIndex, SFeature tBeta, char cCurrentValue, SSavedMove *taSavedMoves);
extern TBoardCell	*tpfGetAvailableMovesAndFastestFirstX(u8 ucBestMove, u8 ucEmpties, u8 *ucMoveCount, TBoardCell tColor, TBoardCell tOpponentColor, bool *bpETCFound, u8 *ucpBestMove, int *ipMoveIndex, SFeature tBeta);

extern void			vfValidateMoveList(u8 ucEmpties);
extern void			vfMoveToFrontByValue(TBoardCell *taStack, TBoardCell tMove);
extern void			vfMoveToFrontByIndex(u32 ucIndex, TBoardCell *taStack, TBoardCell tMove);
extern void			vfMoveToBackByValue(TBoardCell tMove, u8 ucEmpties);
extern char			cfGetMoveCount(TBoardCell tMoveColor);
extern void			vfPrepareMoveListForBoard();
