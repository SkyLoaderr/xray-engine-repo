//////////////////////////////////////////////////////////////////////////////////
//	Module			:	Board routines											//
//	File			:	board.h													//
//	Creation date	:	19.12.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////
// Used modules
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// Macros declarations
//////////////////////////////////////////////////////////

#define WHITE        0
#define EMPTY        1
#define BLACK        2
#define BLACK_EMPTY  3
#define WHITE_EMPTY  4
#define BLACK_WHITE  5
#define DUMMY        6

#define BLACK_DISK_CHARACTER    'x'
#define WHITE_DISK_CHARACTER    'o'
#define EMPTY_MOVE_AVAILABLE    '.'
#define EMPTY_MOVE_UNAVAILABLE  '-'
#define BLACK_START_DISK_STRING "##"
#define WHITE_START_DISK_STRING "()"

#define Index(i,j)         (BOARD_START + (i)*BOARD_LINE + (j))

//////////////////////////////////////////////////////////
// Type declarations
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// Exported variables
//////////////////////////////////////////////////////////

extern TBoardCell  *tpaGlobalFlipStack[65536];
extern TBoardCell **tppFlipStack;
extern char         caNotationBoard[BOARD_SIZE];

//////////////////////////////////////////////////////////
// Exported functions
//////////////////////////////////////////////////////////

extern bool bfCheckIfAvailable(TBoardCell tMoveColor, int iIndex);
extern bool bfCheckIfAvailableFast(TBoardCell tMoveColor, int iIndex);
extern bool bfMakeMoveIfAvailable(TBoardCell tMoveColor, int iIndex, char *cpCurrentValue, u32 *dwpFlipCount);//, bool bEvaluate = true);
extern bool bfMakeMoveIfAvailableFast(TBoardCell tMoveColor, int iIndex, char *cpCurrentValue, u32 *dwpFlipCount);//, bool bEvaluate = true);

extern bool bfChangeHashIfAvailable(TBoardCell tMoveColor,int iIndex);
//extern bool bfMakeMoveIfAvailableWOHash(TBoardCell tMoveColor, int iIndex, char *cpCurrentValue, u32 *dwpFlipCount);
//extern bool bfChangeIndexesIfAvailableFast(TBoardCell tMoveColor, int iIndex);
extern bool bfMakeMoveMobilityIfAvailableFast(TBoardCell tMoveColor, int iIndex, char *cpCurrentValue, u32 *dwpFlipCount);//, bool bEvaluate = true);
extern bool bfChangeEFIndexesIfAvailableFast(TBoardCell tMoveColor, int iIndex);

extern bool bfCountIfAvailable(TBoardCell tMoveColor, int iIndex, char *cpCurrentValue);
extern void vfUndo(u32 dwFlipCount, TBoardCell tOpponentColor);
extern void vfUndoAndRestore(u32 dwFlipCount, TBoardCell tOpponentColor);
extern char cfGetDiskDifference();
extern bool bfCheckIfAnyAvailable(TBoardCell tMoveColor);
extern u8	ucfGetEmptiesCount();
extern TBoardCell tfMoveToIndex(char *S);
extern void vfInitBoard(TBoardCell *taBoard);
extern void vfShowBoard(TBoardCell tColor, f64 dBlackTime = 0, f64 dWhiteTime = 0);
extern bool bfMakeMove(TBoardCell *taBoard, TBoardCell tMoveColor, TBoardCell tIndex, char *cpCurrentValue);
extern char *cafTimeToString(f64 dTime);
extern char cfShowDisk(TBoardCell tColor, char cIndex);
